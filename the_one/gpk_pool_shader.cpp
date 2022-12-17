#include "gpk_pool_shader.h"
#include "gpk_engine_shader.h"
#include "gpk_raster_lh.h"

static constexpr float							LIGHT_FACTOR_AMBIENT			= .025f;
static constexpr float							LIGHT_FACTOR_SPECULAR_POWER		= 30.0f;
static constexpr ::gpk::SColorBGRA				PIXEL_BLACK_NUMBER				= ::gpk::SColorBGRA{0, 0, 0, 255};


static	::gpk::error_t							transformTrianglesBall					
	( ::gpk::SVSOutput									& output
	, ::gpk::view_array<const uint16_t>					indices			
	, ::gpk::view_array<const ::gpk::SCoord3<float>>	positions	
	, ::gpk::view_array<const ::gpk::SCoord3<float>>	normals		
	, ::gpk::view_array<const ::gpk::SCoord2<float>>	uv			
	, const ::gpk::SMatrix4<float>						& projection		
	, const ::gpk::SMatrix4<float>						& worldTransform	
	, const ::gpk::SCoord3<float>						& cameraFront
)	{ 
	::gpk::view_array<const ::gpk::STriangle<uint16_t>>		view_indices		= {(const ::gpk::STriangle<uint16_t>*)indices.begin(), indices.size() / 3};

	const ::gpk::SMatrix4<float>							mWVPS				= worldTransform * projection;
	for(uint32_t iTriangle = 0; iTriangle < view_indices.size(); ++iTriangle) {
		const ::gpk::STriangle<uint16_t>											vertexIndices								= view_indices[iTriangle];
		::gpk::STriangle3<float>													transformedNormals							= {normals[vertexIndices.A], normals[vertexIndices.B], normals[vertexIndices.C]};
		::gpk::transformDirection(transformedNormals, worldTransform);
		transformedNormals.A.Normalize();
		transformedNormals.B.Normalize();
		transformedNormals.C.Normalize();
		double																		directionFactorA							= transformedNormals.A.Dot(cameraFront);
		double																		directionFactorB							= transformedNormals.B.Dot(cameraFront);
		double																		directionFactorC							= transformedNormals.C.Dot(cameraFront);
		if(directionFactorA > .35 && directionFactorB > .35 && directionFactorC > .35)
			continue;

		output.Normals.push_back(transformedNormals);

		::gpk::STriangle3<float>													transformedPositions						= {positions[vertexIndices.A], positions[vertexIndices.B], positions[vertexIndices.C]};
		::gpk::transform(transformedPositions, worldTransform);
		output.PositionsWorld.push_back(transformedPositions);
		
		::gpk::transform(transformedPositions, projection);
		output.PositionsScreen.push_back(transformedPositions);
		
		::gpk::STriangle2<float>													transformedUVs								= {uv[vertexIndices.A], uv[vertexIndices.B], uv[vertexIndices.C]};
		if( transformedUVs.A.x > 1.0f
		 || transformedUVs.A.y > 1.0f
		 || transformedUVs.B.x > 1.0f
		 || transformedUVs.B.y > 1.0f
		 || transformedUVs.C.x > 1.0f
		 || transformedUVs.C.y > 1.0f
		) 
			_CrtDbgBreak();
		output.UVs.push_back(transformedUVs);
	}
	return 0; 
}

namespace gpk
{
	struct SPSIn {
		::gpk::SCoord3<float>							WeightedPosition	;
		::gpk::SCoord3<float>							WeightedNormal		;
		::gpk::SCoord2<float>							WeightedUV			;
		::gpk::SRenderMaterial							Material			;
		::gpk::view_grid<const ::gpk::SColorBGRA>		Surface				; 
	};

	typedef ::gpk::error_t								(TFuncPixelShader)
		( const ::gpk::SEngineSceneConstants					& constants
		, const ::gpk::SPSIn									& inPS
		, ::gpk::SColorBGRA										& outputPixel
		);
}

static	::gpk::error_t								psTableCloth
	( const ::gpk::SEngineSceneConstants					& constants
	, const ::gpk::SPSIn									& inPS
	, ::gpk::SColorBGRA										& outputPixel
	) { 
	const ::gpk::SCoord3<float>							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::SColorFloat							diffuse						= ::gpk::lightCalcDiffuse(::gpk::DARKGREEN, inPS.WeightedNormal, lightVecW);
	outputPixel										= ::gpk::SColorFloat(diffuse).Clamp();
	return 0; 
}

static	::gpk::error_t								psPocket
	( const ::gpk::SEngineSceneConstants					& constants
	, const ::gpk::SPSIn									& inPS
	, ::gpk::SColorBGRA										& outputPixel
	) { 
	const ::gpk::SCoord3<float>							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::SColorFloat							diffuse						= ::gpk::lightCalcDiffuse(::gpk::DARKGRAY, inPS.WeightedNormal, lightVecW);
	outputPixel										= ::gpk::SColorFloat(diffuse).Clamp();
	return 0; 
}

static	::gpk::error_t								psStick
	( const ::gpk::SEngineSceneConstants					& constants
	, const ::gpk::SPSIn									& inPS
	, ::gpk::SColorBGRA										& outputPixel
	) { 
	::gpk::SColorFloat									materialcolor				= ::gpk::BROWN + (::gpk::ORANGE * .5f);
	const ::gpk::SCoord3<float>							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::SColorFloat							diffuse						= ::gpk::lightCalcDiffuse(materialcolor, inPS.WeightedNormal, lightVecW);
	const ::gpk::SColorFloat							ambient						= materialcolor * ::LIGHT_FACTOR_AMBIENT;

	outputPixel										= ::gpk::SColorFloat(ambient + diffuse).Clamp();
	return 0; 
}


static	::gpk::error_t								psBallCue				
	( const ::gpk::SEngineSceneConstants					& constants
	, const ::gpk::SPSIn									& inPS
	, ::gpk::SColorBGRA										& outputPixel
	) { 
	::gpk::SCoord2<float>								relativeToCenter			= ::gpk::SCoord2<float>{inPS.WeightedUV.x, inPS.WeightedUV.y} - ::gpk::SCoord2<float>{.5f, .5f};
	relativeToCenter.x								*= 2;

	const ::gpk::SColorBGRA								surfacecolor				
		= ((::gpk::SCoord2<float>{ 0.0f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: ((::gpk::SCoord2<float>{ 1.0f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: ((::gpk::SCoord2<float>{-1.0f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: ((::gpk::SCoord2<float>{ 0.5f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: ((::gpk::SCoord2<float>{-0.5f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: (( 0.5f - relativeToCenter.y) <  .05f) ? ::gpk::RED 
		: ((-0.5f - relativeToCenter.y) > -.05f) ? ::gpk::RED 
		: ::gpk::WHITE;
	::gpk::SColorFloat									materialcolor				= surfacecolor;		
	const ::gpk::SCoord3<float>							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::SColorFloat							specular					= ::gpk::lightCalcSpecular(constants.CameraPosition, ::LIGHT_FACTOR_SPECULAR_POWER, gpk::WHITE, ::gpk::WHITE, inPS.WeightedPosition, inPS.WeightedNormal, lightVecW);
	const ::gpk::SColorFloat							diffuse						= ::gpk::lightCalcDiffuse(materialcolor, inPS.WeightedNormal, lightVecW);
	const ::gpk::SColorFloat							ambient						= materialcolor * ::LIGHT_FACTOR_AMBIENT;

	outputPixel										= (0 == surfacecolor.g) ? ::gpk::RED : ::gpk::SColorFloat(ambient + diffuse + specular).Clamp();
	return 0; 
}

static	::gpk::error_t								psBallSolid				
	( const ::gpk::SEngineSceneConstants					& constants
	, const ::gpk::SPSIn									& inPS
	, ::gpk::SColorBGRA										& outputPixel
	) { 
	const ::gpk::SCoord2<uint32_t>							& surfaceSize							= inPS.Surface.metrics();
	::gpk::SCoord2<float>								relativeToCenter			= ::gpk::SCoord2<float>{inPS.WeightedUV.x, inPS.WeightedUV.y} - ::gpk::SCoord2<float>{.5f, .5f};
	relativeToCenter.x								*= 2;

	::gpk::SColorFloat									materialColor;
	bool												shade						= false;
	float												ambientFactor				= ::LIGHT_FACTOR_AMBIENT;
	if((::gpk::SCoord2<float>{0.0f, 0.0f} - relativeToCenter).LengthSquared() >= .0225f) {
		materialColor									= inPS.Material.Color.Diffuse;
		shade											= true;
	}
	else {
		const ::gpk::SCoord2<uint32_t>							fetchCoord					= 
			{ (uint32_t)(relativeToCenter.x * 2.f * surfaceSize.x + surfaceSize.x / 2)
			, (uint32_t)(relativeToCenter.y * 4.f * surfaceSize.y + surfaceSize.y / 2)
			};
		const ::gpk::SColorBGRA									surfacecolor				= inPS.Surface
			[fetchCoord.y % surfaceSize.y]
			[fetchCoord.x % surfaceSize.x]
			;
		if(surfacecolor != PIXEL_BLACK_NUMBER) {
			materialColor									= ::gpk::WHITE;
			shade											= rand() % 2;
			ambientFactor									= .65f;
		}
		else {
			materialColor									= gpk::BLACK;
			shade											= true;
		}
	}

	if(shade) {
		const ::gpk::SCoord3<float>								lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
		double													diffuseFactor				= inPS.WeightedNormal.Dot(lightVecW);
		if(diffuseFactor < 0) {
			ambientFactor									+= (rand() % 256) / 255.0f * float(diffuseFactor) * -.25f;
			materialColor									= (materialColor * ambientFactor).Clamp();
		}
		else {
			const ::gpk::SColorFloat								specular					= ::gpk::lightCalcSpecular(constants.CameraPosition, ::LIGHT_FACTOR_SPECULAR_POWER, gpk::WHITE, ::gpk::WHITE, inPS.WeightedPosition, inPS.WeightedNormal, lightVecW);
			const ::gpk::SColorFloat								diffuse						= materialColor * ::gpk::max(0.0, diffuseFactor);
			const ::gpk::SColorFloat								ambient						= materialColor * ambientFactor;
			materialColor									= ::gpk::SColorFloat(ambient + diffuse + specular).Clamp();
		}
	}
	outputPixel										= materialColor;
	return 0; 
}

static	::gpk::error_t								psBallStripped			
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::SColorBGRA						& outputPixel
	) { 
	const ::gpk::SCoord2<uint32_t>						& surfaceSize							= inPS.Surface.metrics();
	::gpk::SCoord2<float>								relativeToCenter			= ::gpk::SCoord2<float>{inPS.WeightedUV.x, inPS.WeightedUV.y} - ::gpk::SCoord2<float>{.5f, .5f};
	relativeToCenter.x								*= 2;

	::gpk::SColorFloat									materialColor;
	bool												shade						= false;
	float												ambientFactor				= ::LIGHT_FACTOR_AMBIENT;
	if((::gpk::SCoord2<float>{0.0f, 0.0f} - relativeToCenter).LengthSquared() >= .0225f) {
		materialColor									
			= (relativeToCenter.y >  .20f) ? ::gpk::WHITE
			: (relativeToCenter.y < -.20f) ? ::gpk::WHITE
			: inPS.Material.Color.Diffuse
			;
		shade											= true;
	}
	else {
		const ::gpk::SCoord2<uint32_t>							fetchCoord					= 
			{ (uint32_t)(relativeToCenter.x * 2.f * surfaceSize.x + surfaceSize.x / 2)
			, (uint32_t)(relativeToCenter.y * 4.f * surfaceSize.y + surfaceSize.y / 2)
			};
		const ::gpk::SColorBGRA									surfacecolor				= inPS.Surface
			[fetchCoord.y % surfaceSize.y]
			[fetchCoord.x % surfaceSize.x]
			;

		if(surfacecolor != PIXEL_BLACK_NUMBER) {
			materialColor										= ::gpk::WHITE;
			shade												= rand() % 2;
			ambientFactor										= .65f;
		}
		else {
			materialColor										= ::gpk::BLACK;
			shade												= true;
		} 
	}
	if(shade) {
		const ::gpk::SCoord3<float>								lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
		double													diffuseFactor				= inPS.WeightedNormal.Dot(lightVecW);
		if(diffuseFactor < 0) {
			ambientFactor									+= (rand() % 256) * (1.0f / 255) * float(diffuseFactor) * -.25f;
			materialColor									= (materialColor * ambientFactor).Clamp();
		}
		else {
			const ::gpk::SColorFloat								specular					= ::gpk::lightCalcSpecular(constants.CameraPosition, ::LIGHT_FACTOR_SPECULAR_POWER, gpk::WHITE, ::gpk::WHITE, inPS.WeightedPosition, inPS.WeightedNormal, lightVecW);
			const ::gpk::SColorFloat								diffuse						= materialColor * ::gpk::max(0.0, diffuseFactor);
			const ::gpk::SColorFloat								ambient						= materialColor * ambientFactor;
			materialColor									= ::gpk::SColorFloat(ambient + diffuse + specular).Clamp();
		}
	}
	outputPixel											= materialColor;
	return 0; 
}

static	::gpk::error_t								drawBuffersBall							
	( ::gpk::view_grid<::gpk::SColorBGRA>					& backBufferColors
	, ::gpk::view_grid<uint32_t>							& backBufferDepth
	, ::gpk::SVSOutput										& outVS
	, ::gpk::SVSCache										& cacheVS
	, ::gpk::view_array<const uint16_t>						indices		
	, ::gpk::view_array<const ::gpk::SCoord3<float>>		positions	
	, ::gpk::view_array<const ::gpk::SCoord3<float>>		normals		
	, ::gpk::view_array<const ::gpk::SCoord2<float>>		uv			
	, const ::gpk::SRenderMaterial							& material
	, ::gpk::view_grid<const ::gpk::SColorBGRA>				surface
	, const ::gpk::SMatrix4<float>							& worldTransform	 
	, const ::gpk::SEngineSceneConstants					& constants
	, ::gpk::TFuncPixelShader								& ps
	) {	// 
	(void)uv;(void)surface;
	outVS												= {};
	::transformTrianglesBall(outVS, indices, positions, normals, uv, constants.Projection, worldTransform, constants.CameraFront);
	::gpk::array_pod<::gpk::STriangle<float>>					& triangleWeights			= cacheVS.TriangleWeights		;
	::gpk::array_pod<::gpk::SCoord2<int16_t>>					& trianglePixelCoords		= cacheVS.SolidPixelCoords		;
	const ::gpk::SCoord2<uint16_t>								offscreenMetrics			= backBufferColors.metrics().Cast<uint16_t>();
	const ::gpk::SCoord3<float>									lightDirectionNormalized	= ::gpk::SCoord3<float>{constants.LightDirection}.Normalize();
	::gpk::SPSIn												inPS						= {};
	inPS.Surface											= surface;
	inPS.Material											= material;

	for(uint32_t iTriangle = 0; iTriangle < outVS.PositionsScreen.size(); ++iTriangle) {
		const ::gpk::STriangle3<float>								& triPositions				= outVS.PositionsScreen	[iTriangle];
		if( (triPositions.CulledZ({0, 0xFFFFFF}))
		 || (triPositions.CulledX({0, (float)offscreenMetrics.x}))
		 || (triPositions.CulledY({0, (float)offscreenMetrics.y}))
		)
			continue;

		const ::gpk::STriangle3<float>								& triPositionsWorld			= outVS.PositionsWorld	[iTriangle];
		const ::gpk::STriangle3<float>								& triNormals				= outVS.Normals			[iTriangle];
		const ::gpk::STriangle2<float>								& triUVs					= outVS.UVs				[iTriangle];

		trianglePixelCoords.clear();
		triangleWeights.clear();
		gerror_if(errored(::gpk::drawTriangle(offscreenMetrics.Cast<uint32_t>(), triPositions, trianglePixelCoords, triangleWeights, backBufferDepth)), "Not sure if these functions could ever fail");
		//const bool													stripped					= surface[0][0] == ::gpk::SColorBGRA{gpk::WHITE};
		for(uint32_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord) {
			const ::gpk::STriangle<float>								& vertexWeights				= triangleWeights[iCoord];
			inPS.WeightedPosition									= triPositionsWorld.A * vertexWeights.A + triPositionsWorld.B * vertexWeights.B + triPositionsWorld.C * vertexWeights.C;
			inPS.WeightedNormal										= (triNormals.A * vertexWeights.A + triNormals.B * vertexWeights.B + triNormals.C * vertexWeights.C).Normalize();
			inPS.WeightedUV											= triUVs.A * vertexWeights.A + triUVs.B * vertexWeights.B + triUVs.C * vertexWeights.C;
			const ::gpk::SCoord2<uint16_t>								coord						= trianglePixelCoords[iCoord].Cast<uint16_t>();
			ps(constants, inPS, backBufferColors[coord.y][coord.x]);
		}
	}
	return 0;
}

int32_t												the1::shaderBall				
	( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
	, ::gpk::view_grid<uint32_t>			backBufferDepth
	, ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
	, int32_t								iRenderNode
	) {
	const ::gpk::SRenderNode								& renderNode			= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
	const ::gpk::SRenderMesh								& mesh					= *scene.Graphics->Meshes[renderNode.Mesh];
	::gpk::vcc												meshName				= scene.Graphics->Meshes.Names[renderNode.Mesh];

	info_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iRenderNode, renderNode.Mesh, renderNode.Slice, meshName.begin());

	const ::gpk::SRenderNodeTransforms						& transforms				= scene.ManagedRenderNodes.Transforms[iRenderNode];
	const ::gpk::SMatrix4<float>							& worldTransform			= transforms.World;
	const ::gpk::view_array<const uint16_t>					indices						= (mesh.GeometryBuffers.size() > 0) ? ::gpk::view_array<const uint16_t>					{(const uint16_t				*)scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.size() / sizeof(const uint16_t)}				: ::gpk::view_array<const uint16_t>					{};
	const ::gpk::view_array<const ::gpk::SCoord3<float>>	positions					= (mesh.GeometryBuffers.size() > 1) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
	const ::gpk::view_array<const ::gpk::SCoord3<float>>	normals						= (mesh.GeometryBuffers.size() > 2) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
	const ::gpk::view_array<const ::gpk::SCoord2<float>>	uv							= (mesh.GeometryBuffers.size() > 3) ? ::gpk::view_array<const ::gpk::SCoord2<float>>	{(const ::gpk::SCoord2<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[3]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[3]]->Data.size() / sizeof(const ::gpk::SCoord2<float>)}	: ::gpk::view_array<const ::gpk::SCoord2<float>>	{};
	const ::gpk::SSkin										& skin						= *scene.Graphics->Skins.Elements[renderNode.Skin];
	const ::gpk::SRenderMaterial							& material					= skin.Material;
	const uint32_t											tex							= skin.Textures[0];
	const ::gpk::SSurface									& surface					= *scene.Graphics->Surfaces[tex];

	const ::gpk::SGeometrySlice								slice						= (renderNode.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[renderNode.Slice] : ::gpk::SGeometrySlice{{0, indices.size() / 3}};

	::gpk::TFuncPixelShader									& ps						
		= (0 == iRenderNode) ? psBallCue
		: (8 >= iRenderNode) ? psBallSolid
		: psBallStripped
		;
	drawBuffersBall(backBufferColors, backBufferDepth, renderCache.OutputVertexShader, renderCache.CacheVertexShader
		, {&indices[slice.Slice.Offset], slice.Slice.Count}, positions, normals, uv, material, {(const ::gpk::SColorBGRA*)surface.Data.begin(), surface.Desc.Dimensions.Cast<uint32_t>()}
		, worldTransform, constants, ps
	);

	return 0;
}


int32_t												the1::shaderHole
	( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
	, ::gpk::view_grid<uint32_t>			backBufferDepth
	, ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
	, int32_t								iRenderNode
	) {
	const ::gpk::SRenderNode								& renderNode			= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
	const ::gpk::SRenderMesh								& mesh					= *scene.Graphics->Meshes[renderNode.Mesh];
	::gpk::vcc												meshName				= scene.Graphics->Meshes.Names[renderNode.Mesh];

	info_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iRenderNode, renderNode.Mesh, renderNode.Slice, meshName.begin());

	const ::gpk::SRenderNodeTransforms						& transforms				= scene.ManagedRenderNodes.Transforms[iRenderNode];
	const ::gpk::SMatrix4<float>							& worldTransform			= transforms.World;
	const ::gpk::view_array<const uint16_t>					indices						= (mesh.GeometryBuffers.size() > 0) ? ::gpk::view_array<const uint16_t>					{(const uint16_t				*)scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.size() / sizeof(const uint16_t)}					: ::gpk::view_array<const uint16_t>					{};
	const ::gpk::view_array<const ::gpk::SCoord3<float>>	positions					= (mesh.GeometryBuffers.size() > 1) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
	const ::gpk::view_array<const ::gpk::SCoord3<float>>	normals						= (mesh.GeometryBuffers.size() > 2) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
	const ::gpk::view_array<const ::gpk::SCoord2<float>>	uv							= (mesh.GeometryBuffers.size() > 3) ? ::gpk::view_array<const ::gpk::SCoord2<float>>	{(const ::gpk::SCoord2<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[3]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[3]]->Data.size() / sizeof(const ::gpk::SCoord2<float>)}	: ::gpk::view_array<const ::gpk::SCoord2<float>>	{};
	const ::gpk::SSkin										& skin						= *scene.Graphics->Skins.Elements[renderNode.Skin];
	const ::gpk::SRenderMaterial							& material					= skin.Material;
	const uint32_t											tex							= skin.Textures[0];
	const ::gpk::SSurface									& surface					= *scene.Graphics->Surfaces[tex];

	const ::gpk::SGeometrySlice								slice						= (renderNode.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[renderNode.Slice] : ::gpk::SGeometrySlice{{0, indices.size() / 3}};

	drawBuffersBall(backBufferColors, backBufferDepth, renderCache.OutputVertexShader, renderCache.CacheVertexShader
		, {&indices[slice.Slice.Offset], slice.Slice.Count}, positions, normals, uv, material, {(const ::gpk::SColorBGRA*)surface.Data.begin(), surface.Desc.Dimensions.Cast<uint32_t>()}
		, worldTransform, constants, ::psPocket
	);

	return 0;
}


int32_t												the1::shaderStick
	( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
	, ::gpk::view_grid<uint32_t>			backBufferDepth
	, ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
	, int32_t								iRenderNode
	) {
	const ::gpk::SRenderNode								& renderNode			= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
	const ::gpk::SRenderMesh								& mesh					= *scene.Graphics->Meshes[renderNode.Mesh];
	::gpk::vcc												meshName				= scene.Graphics->Meshes.Names[renderNode.Mesh];

	info_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iRenderNode, renderNode.Mesh, renderNode.Slice, meshName.begin());

	const ::gpk::SRenderNodeTransforms						& transforms				= scene.ManagedRenderNodes.Transforms[iRenderNode];
	const ::gpk::SMatrix4<float>							& worldTransform			= transforms.World;
	const ::gpk::view_array<const uint16_t>					indices						= (mesh.GeometryBuffers.size() > 0) ? ::gpk::view_array<const uint16_t>					{(const uint16_t				*)scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.size() / sizeof(const uint16_t)}					: ::gpk::view_array<const uint16_t>					{};
	const ::gpk::view_array<const ::gpk::SCoord3<float>>	positions					= (mesh.GeometryBuffers.size() > 1) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
	const ::gpk::view_array<const ::gpk::SCoord3<float>>	normals						= (mesh.GeometryBuffers.size() > 2) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
	const ::gpk::view_array<const ::gpk::SCoord2<float>>	uv							= (mesh.GeometryBuffers.size() > 3) ? ::gpk::view_array<const ::gpk::SCoord2<float>>	{(const ::gpk::SCoord2<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[3]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[3]]->Data.size() / sizeof(const ::gpk::SCoord2<float>)}	: ::gpk::view_array<const ::gpk::SCoord2<float>>	{};
	const ::gpk::SSkin										& skin						= *scene.Graphics->Skins.Elements[renderNode.Skin];
	const ::gpk::SRenderMaterial							& material					= skin.Material;
	const uint32_t											tex							= skin.Textures[0];
	const ::gpk::SSurface									& surface					= *scene.Graphics->Surfaces[tex];

	const ::gpk::SGeometrySlice								slice						= (renderNode.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[renderNode.Slice] : ::gpk::SGeometrySlice{{0, indices.size() / 3}};

	drawBuffersBall(backBufferColors, backBufferDepth, renderCache.OutputVertexShader, renderCache.CacheVertexShader
		, {&indices[slice.Slice.Offset], slice.Slice.Count}, positions, normals, uv, material, {(const ::gpk::SColorBGRA*)surface.Data.begin(), surface.Desc.Dimensions.Cast<uint32_t>()}
		, worldTransform, constants, ::psStick
	);

	return 0;
}

int32_t												the1::shaderCloth
	( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
	, ::gpk::view_grid<uint32_t>			backBufferDepth
	, ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
	, int32_t								iRenderNode
	) {
	const ::gpk::SRenderNode								& renderNode			= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
	const ::gpk::SRenderMesh								& mesh					= *scene.Graphics->Meshes[renderNode.Mesh];
	::gpk::vcc												meshName				= scene.Graphics->Meshes.Names[renderNode.Mesh];

	info_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iRenderNode, renderNode.Mesh, renderNode.Slice, meshName.begin());

	const ::gpk::SRenderNodeTransforms						& transforms				= scene.ManagedRenderNodes.Transforms[iRenderNode];
	const ::gpk::SMatrix4<float>							& worldTransform			= transforms.World;
	const ::gpk::view_array<const uint16_t>					indices						= (mesh.GeometryBuffers.size() > 0) ? ::gpk::view_array<const uint16_t>					{(const uint16_t				*)scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.size() / sizeof(const uint16_t)}				: ::gpk::view_array<const uint16_t>					{};
	const ::gpk::view_array<const ::gpk::SCoord3<float>>	positions					= (mesh.GeometryBuffers.size() > 1) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
	const ::gpk::view_array<const ::gpk::SCoord3<float>>	normals						= (mesh.GeometryBuffers.size() > 2) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
	const ::gpk::view_array<const ::gpk::SCoord2<float>>	uv							= (mesh.GeometryBuffers.size() > 3) ? ::gpk::view_array<const ::gpk::SCoord2<float>>	{(const ::gpk::SCoord2<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[3]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[3]]->Data.size() / sizeof(const ::gpk::SCoord2<float>)}	: ::gpk::view_array<const ::gpk::SCoord2<float>>	{};
	const ::gpk::SSkin										& skin						= *scene.Graphics->Skins.Elements[renderNode.Skin];
	const ::gpk::SRenderMaterial							& material					= skin.Material;
	const uint32_t											tex							= skin.Textures[0];
	const ::gpk::SSurface									& surface					= *scene.Graphics->Surfaces[tex];

	const ::gpk::SGeometrySlice								slice						= (renderNode.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[renderNode.Slice] : ::gpk::SGeometrySlice{{0, indices.size() / 3}};

	drawBuffersBall(backBufferColors, backBufferDepth, renderCache.OutputVertexShader, renderCache.CacheVertexShader
		, {&indices[slice.Slice.Offset], slice.Slice.Count}, positions, normals, uv, material, {(const ::gpk::SColorBGRA*)surface.Data.begin(), surface.Desc.Dimensions.Cast<uint32_t>()}
		, worldTransform, constants, ::psTableCloth
	);

	return 0;
}
