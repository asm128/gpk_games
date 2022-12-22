#include "gpk_pool_shader.h"
#include "gpk_engine_shader.h"
#include "gpk_raster_lh.h"

static constexpr float							LIGHT_FACTOR_AMBIENT			= .025f;
static constexpr float							LIGHT_FACTOR_SPECULAR_POWER		= 30.0f;
static constexpr ::gpk::SColorBGRA				PIXEL_BLACK_NUMBER				= ::gpk::SColorBGRA{0, 0, 0, 255};

::gpk::error_t										the1::psTableCloth
	( const ::gpk::SEngineSceneConstants					& constants
	, const ::gpk::SPSIn									& inPS
	, ::gpk::SColorBGRA										& outputPixel
	) { 
	const ::gpk::SCoord3<float>							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::SColorFloat							diffuse						= ::gpk::lightCalcDiffuse(::gpk::DARKGREEN, inPS.WeightedNormal, lightVecW);
	outputPixel										= ::gpk::SColorFloat(diffuse).Clamp();
	return 0; 
}

::gpk::error_t										the1::psPocket
	( const ::gpk::SEngineSceneConstants					& constants
	, const ::gpk::SPSIn									& inPS
	, ::gpk::SColorBGRA										& outputPixel
	) { 
	const ::gpk::SCoord3<float>							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::SColorFloat							diffuse						= ::gpk::lightCalcDiffuse(::gpk::DARKGRAY, inPS.WeightedNormal, lightVecW);
	outputPixel										= ::gpk::SColorFloat(diffuse).Clamp();
	return 0; 
}

::gpk::error_t										the1::psStick
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


::gpk::error_t										the1::psBallCue				
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

::gpk::error_t										the1::psBallSolid				
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

::gpk::error_t										the1::psBallStripped			
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

static	::gpk::error_t								drawBuffers
	( ::gpk::view_grid<::gpk::SColorBGRA>					& backBufferColors
	, ::gpk::view2d_uint32									backBufferDepth
	, ::gpk::SVSOutput										& outVS
	, ::gpk::SVSCache										& cacheVS
	, const ::gpk::SRenderMaterial							& material
	, ::gpk::view_grid<const ::gpk::SColorBGRA>				surface
	, const ::gpk::SEngineSceneConstants					& constants
	, ::gpk::TFuncPixelShader								& ps
	) {	// 
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
	, ::gpk::view2d_uint32					backBufferDepth
	, ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
	, int32_t								iRenderNode
	) {
	const ::gpk::SRenderNode								& renderNode				= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
	const ::gpk::SSkin										& skin						= *scene.Graphics->Skins.Elements[renderNode.Skin];
	const ::gpk::SRenderMaterial							& material					= skin.Material;
	const uint32_t											tex							= skin.Textures[0];
	const ::gpk::SSurface									& surface					= *scene.Graphics->Surfaces[tex];
	::gpk::TFuncPixelShader									& ps						
		= (0 == iRenderNode) ? ::the1::psBallCue
		: (8 >= iRenderNode) ? ::the1::psBallSolid
		: ::the1::psBallStripped
		;

	drawBuffers(backBufferColors
		, backBufferDepth
		, renderCache.OutputVertexShader
		, renderCache.CacheVertexShader
		, material
		, {(const ::gpk::SColorBGRA*)surface.Data.begin(), surface.Desc.Dimensions.Cast<uint32_t>()}
		, constants
		, ps
	);
	return 0;
}


int32_t												the1::shaderHole
	( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
	, ::gpk::view2d_uint32					backBufferDepth
	, ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
	, int32_t								iRenderNode
	) {
	const ::gpk::SRenderNode								& renderNode				= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
	const ::gpk::SSkin										& skin						= *scene.Graphics->Skins.Elements[renderNode.Skin];
	const ::gpk::SRenderMaterial							& material					= skin.Material;
	const uint32_t											tex							= skin.Textures[0];
	const ::gpk::SSurface									& surface					= *scene.Graphics->Surfaces[tex];

	drawBuffers(backBufferColors
		, backBufferDepth
		, renderCache.OutputVertexShader
		, renderCache.CacheVertexShader
		, material
		, {(const ::gpk::SColorBGRA*)surface.Data.begin(), surface.Desc.Dimensions.Cast<uint32_t>()}
		, constants
		, ::the1::psPocket
	);
	return 0;
}


int32_t												the1::shaderStick
	( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
	, ::gpk::view2d_uint32					backBufferDepth
	, ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
	, int32_t								iRenderNode
	) {
	const ::gpk::SRenderNode								& renderNode				= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
	const ::gpk::SSkin										& skin						= *scene.Graphics->Skins.Elements[renderNode.Skin];
	const ::gpk::SRenderMaterial							& material					= skin.Material;
	const uint32_t											tex							= skin.Textures[0];
	const ::gpk::SSurface									& surface					= *scene.Graphics->Surfaces[tex];

	drawBuffers(backBufferColors
		, backBufferDepth
		, renderCache.OutputVertexShader
		, renderCache.CacheVertexShader
		, material
		, {(const ::gpk::SColorBGRA*)surface.Data.begin(), surface.Desc.Dimensions.Cast<uint32_t>()}
		, constants
		, ::the1::psStick
	);
	return 0;
}

int32_t												the1::shaderCloth
	( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
	, ::gpk::view2d_uint32					backBufferDepth
	, ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
	, int32_t								iRenderNode
	) {
	const ::gpk::SRenderNode								& renderNode				= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
	const ::gpk::SSkin										& skin						= *scene.Graphics->Skins.Elements[renderNode.Skin];
	const ::gpk::SRenderMaterial							& material					= skin.Material;
	const uint32_t											tex							= skin.Textures[0];
	const ::gpk::SSurface									& surface					= *scene.Graphics->Surfaces[tex];

	drawBuffers(backBufferColors
		, backBufferDepth
		, renderCache.OutputVertexShader
		, renderCache.CacheVertexShader
		, material
		, {(const ::gpk::SColorBGRA*)surface.Data.begin(), surface.Desc.Dimensions.Cast<uint32_t>()}
		, constants
		, ::the1::psTableCloth
	);
	return 0;
}
