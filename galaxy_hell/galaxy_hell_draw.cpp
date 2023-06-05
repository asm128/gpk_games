#include "gpk_galaxy_hell.h"

#include "gpk_raster_lh.h"
#include "gpk_gui_text.h"
#include "gpk_font.h"

#include "gpk_label.h"

#include <algorithm>

static constexpr	const uint32_t					MAX_LIGHT_RANGE		= 10;

static	int											drawStars			(const ::ghg::SStars & stars, ::gpk::view2d<::gpk::bgra> targetPixels)	{
	::gpk::bgra										colors[]			=
		{ {0xfF, 0xfF, 0xfF, }
		, {0xC0, 0xC0, 0xfF, }
		, {0xfF, 0xC0, 0xC0, }
		, {0xC0, 0xD0, 0xC0, }
		};
	for(uint32_t iStar = 0; iStar < stars.Brightness.size(); ++iStar) {
		::gpk::n2f32									starPos				= stars.Position[iStar];
		::gpk::bgra										starFinalColor		= colors[iStar % ::gpk::size(colors)] * stars.Brightness[iStar];
		::gpk::setPixel(targetPixels, starPos.Cast<int16_t>(), starFinalColor);
		const	int32_t											brightRadius		= 1 + (iStar % 3) + (rand() % 2);
		const	double											brightRadiusSquared	= brightRadius * (double)brightRadius;
		double													brightUnit			= 1.0 / brightRadiusSquared;
		for(int32_t y = -brightRadius; y < brightRadius; ++y)
		for(int32_t x = -brightRadius; x < brightRadius; ++x) {
			::gpk::n2f32									brightPos			= {(float)x, (float)y};
			const double											brightDistance		= brightPos.LengthSquared();
			if(brightDistance <= brightRadiusSquared) {
				::gpk::n2<int16_t>									pixelPos			= (starPos + brightPos).Cast<int16_t>();
				if( pixelPos.y >= 0 && pixelPos.y < (int32_t)targetPixels.metrics().y
				 && pixelPos.x >= 0 && pixelPos.x < (int32_t)targetPixels.metrics().x
 				)
					::gpk::setPixel(targetPixels, pixelPos, targetPixels[pixelPos.y][pixelPos.x] + starFinalColor * (1.0-(brightDistance * brightUnit * (1 + (rand() % 3)))));
			}
		}
	}
	return 0;
}

static	int													drawDebris
	( ::gpk::view2d<::gpk::bgra>					targetPixels
	, const ::ghg::SDebris							& debris
	, const ::gpk::m4f32					& matrixVPV
	, ::gpk::view2d<uint32_t>						depthBuffer
	, const ::gpk::view<const ::gpk::bgra>	& debrisColors
	)	{

	float prebrightness = 2.5f * (1.0f / debris.Brightness.size());
	for(uint32_t iParticle = 0; iParticle < debris.Brightness.size(); ++iParticle) {
		const ::gpk::rgbaf								& colorShot			= debrisColors[iParticle % debrisColors.size()];
		::gpk::n3f32									starPos				= debris.Particles.Position[iParticle];
		starPos												= matrixVPV.Transform(starPos);
		if(starPos.z > 1 || starPos.z < 0)
			continue;
		const ::gpk::n2<int32_t>							pixelCoord			= {(int32_t)starPos.x, (int32_t)starPos.y};
		if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
		 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
		)
			continue;
		uint32_t												depth				= uint32_t(starPos.z * 0xFFFFFFFFU);
		if(depth > depthBuffer[pixelCoord.y][pixelCoord.x])
			continue;
		float													brightness		= debris.Brightness[iParticle];
		::gpk::rgbaf										starFinalColor	= colorShot * brightness;
		starFinalColor.g									= ::gpk::max(0.0f, starFinalColor.g - (1.0f - ::gpk::min(1.0f, brightness * prebrightness * (iParticle * 2))));
		starFinalColor.b									= ::gpk::max(0.0f, starFinalColor.b - (1.0f - ::gpk::min(1.0f, brightness * prebrightness * (iParticle * 1))));
		//::gpk::setPixel(targetPixels, pixelCoord, starFinalColor);
		static constexpr	const double						brightRadius		= 1.5;
		static constexpr	const double						brightRadiusSquared	= brightRadius * brightRadius;
		static constexpr	const double						brightUnit			= 1.0 / brightRadiusSquared;

		for(int32_t y = (int32_t)-brightRadius - 1, yStop = (int32_t)brightRadius + 1; y < yStop; ++y)
		for(int32_t x = (int32_t)-brightRadius - 1; x < yStop; ++x) {
			::gpk::n2f32											brightPos			= {(float)x, (float)y};
			const double											brightDistance		= brightPos.LengthSquared();
			if(brightDistance <= brightRadiusSquared) {
				::gpk::n2<int32_t>									blendPos			= pixelCoord + (brightPos).Cast<int32_t>();
				if( blendPos.y < 0 || blendPos.y >= (int32_t)targetPixels.metrics().y
				 || blendPos.x < 0 || blendPos.x >= (int32_t)targetPixels.metrics().x
				)
					continue;
				uint32_t												& blendVal			= depthBuffer[blendPos.y][blendPos.x];
				if(depth > blendVal)
					continue;
				blendVal											= depth;
				double													finalBrightness					= 1.0-(brightDistance * brightUnit);
				::gpk::bgra										& pixelVal						= targetPixels[blendPos.y][blendPos.x];
				pixelVal											= (starFinalColor * finalBrightness + pixelVal).Clamp();
			}
		}
	}
	return 0;
}

int													drawScoreParticles
	( ::gpk::view2d<::gpk::bgra>				targetPixels
	, const ::ghg::SScoreParticles						& debris
	, const ::gpk::m4f32						& matrixVPV
	, ::gpk::view2d<uint32_t>						depthBuffer
	, const ::gpk::SRasterFont							& font
	)	{

	static constexpr	const double						brightRadius		= 1.5;
	static constexpr	const double						brightRadiusSquared	= brightRadius * brightRadius;
	static constexpr	const double						brightUnit			= 1.0 / brightRadiusSquared;
	for(uint32_t iParticle = 0; iParticle < debris.Scores.size(); ++iParticle) {
		::gpk::n3f32									starPos				= debris.Particles.Position[iParticle];
		starPos												= matrixVPV.Transform(starPos);
		if(starPos.z > 1 || starPos.z < 0)
			continue;
		const ::gpk::n2<int16_t>							pixelCoord			= {(int16_t)starPos.x, (int16_t)starPos.y};
		if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
		 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
		)
			continue;
		uint32_t												depth				= uint32_t(starPos.z * 0xFFFFFFFFU);
		if(depth > depthBuffer[pixelCoord.y][pixelCoord.x])
			continue;

		::ghg::SScoreParticle									particle			= debris.Scores[iParticle];
		::gpk::rgbaf										starFinalColor		= ::gpk::GREEN;
		starFinalColor.r									+= 1.0f - ::gpk::min(1.0f, particle.Brightness);
		starFinalColor.g									-= 1.0f - ::gpk::min(1.0f, particle.Brightness);
		starFinalColor.b									-= 1.0f - ::gpk::min(1.0f, particle.Brightness);
		starFinalColor.Clamp();
		starFinalColor.a = starFinalColor.g;
		::gpk::apod<::gpk::n2<uint16_t>>				dstCoords;
		char													textToShow[64]		= {};
		sprintf_s(textToShow, "%i", particle.Score);
		const ::gpk::vcs							finalText			= textToShow;
		::gpk::rect2i16								rectText			= {{}, {int16_t(font.CharSize.x * finalText.size()), font.CharSize.y}};
		rectText.Offset = (pixelCoord - ::gpk::n2i16{int16_t(rectText.Size.x >> 1), int16_t(rectText.Size.y >> 1)});

		gpk_necs(::gpk::textLineRaster(targetPixels.metrics(), font.CharSize, rectText, font.Texture, finalText, dstCoords));
		for(uint32_t iCoord = 0; iCoord < dstCoords.size(); ++iCoord) {
			::gpk::pixelBlend(targetPixels, dstCoords[iCoord].Cast<int16_t>(), starFinalColor);
		}
	}
	return 0;
}

::gpk::error_t										drawPixels
	( ::gpk::view2d<::gpk::bgra>				targetPixels
	, const ::gpk::n3		<float>					& cameraPos
	, const ::gpk::tri3	<float>					& triangleWorld
	, const ::gpk::n3		<float>					& normal
	, const ::gpk::bgra							texelColor
	, ::gpk::apod<::gpk::n2<int16_t>>			& pixelCoords
	, ::gpk::apod<::gpk::tri<float>>			& pixelVertexWeights
	, const ::std::function<::gpk::error_t(::gpk::view2d<::gpk::bgra> targetPixels, const ::gpk::n2<int16_t> & pixelCoord, const ::gpk::bgra & color)> & funcSetPixel
	) {
	int32_t								countPixels				= 0;
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::gpk::n2<int16_t>									pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::tri<float>							& vertexWeights			= pixelVertexWeights[iPixelCoord];
		const ::gpk::n3f32								position				= ::gpk::triangleWeight(vertexWeights, triangleWorld);
		static constexpr	const double						rangeLight				= 100.0;
		static constexpr	const double						rangeLightSquared		= rangeLight * rangeLight;
		static constexpr	const double						rangeUnit				= 1.0 / rangeLightSquared;
		const ::gpk::n3f32								lightToPoint			= cameraPos - position;
		::gpk::n3f32									vectorToLight			= lightToPoint;
		vectorToLight.Normalize();
		const double											lightFactor				= vectorToLight.Dot(normal);
		const double											distanceToLight			= lightToPoint.LengthSquared();
		if(lightFactor <= 0)
			continue;
		const double											invAttenuation			= ::std::max(0.0, 1.0 - (distanceToLight * rangeUnit));
		::gpk::rgbaf										fragmentColor			= (::gpk::rgbaf(texelColor) * invAttenuation).Clamp();
		countPixels											+= funcSetPixel(targetPixels, pixelCoord, fragmentColor);
	}
	return countPixels;
}

int													drawQuadTriangle
	( ::gpk::view2d<::gpk::bgra>				targetPixels
	, const ::gpk::n3		<float>					& cameraPos
	, const ::gpk::bgra							texelColor
	, const ::gpk::SGeometryQuads						& geometry
	, const int											iTriangle
	, const ::gpk::m4f32						& matrixTransform
	, const ::gpk::m4f32						& matrixTransformVP
	, ::gpk::apod<::gpk::n2<int16_t>>			& pixelCoords
	, ::gpk::apod<::gpk::tri<float>>			& pixelVertexWeights
	, ::gpk::view2d<uint32_t>						depthBuffer
	, const ::std::function<::gpk::error_t(::gpk::view2d<::gpk::bgra> targetPixels, const ::gpk::n2<int16_t> & pixelCoord, const ::gpk::bgra & color)> & funcSetPixel
	) {
	const ::gpk::tri3	<float>							& triangle			= geometry.Triangles	[iTriangle];;
	const ::gpk::n3	<float>							& normal			= geometry.Normals		[iTriangle / 2];
	::gpk::drawQuadTriangle(targetPixels.metrics(), triangle, matrixTransformVP, pixelCoords, pixelVertexWeights, depthBuffer);
	const ::gpk::n3	<float>							xnormal				= matrixTransform.TransformDirection(normal).Normalize();
	::gpk::tri3		<float>							triangleWorld		= triangle;
	::gpk::transform(triangleWorld, matrixTransform);
	return ::drawPixels(targetPixels, cameraPos, triangleWorld, xnormal, texelColor, pixelCoords, pixelVertexWeights, funcSetPixel);
}


static int										drawCannonball
	( const ::ghg::SShipManager							& shipState
	, const ::gpk::n3f32						& position
	, const ::gpk::n3f32						& prevPosition
	, const ::gpk::n3f32						& _direction
	, const ::gpk::rgbaf							& bulletColor
	, float												animationTime
	, const ::gpk::m4f32						& matrixVP
	, ::gpk::view2d<::gpk::bgra>				& targetPixels
	, ::gpk::view2d<uint32_t>						depthBuffer
	, ::ghg::SGalaxyHellDrawCache						& drawCache
	) {
	uint32_t												pixelsDrawn				= 0;
	double													absanim					= fabsf(sinf(animationTime * 2));
	::gpk::m4f32									matrixTransform				= {};
	matrixTransform.SetIdentity();
	matrixTransform.SetTranslation(position, false);
	::gpk::img<::gpk::bgra>						image						= {};
	image.resize(::gpk::n2u32{1, 1286}, bulletColor);
	{
		::gpk::m4f32									matrixTransformVP			= matrixTransform * matrixVP;
		::ghg::getLightArrays(matrixTransform.GetTranslation(), drawCache.LightPointsWorld, drawCache.LightColorsWorld, drawCache.LightPointsModel, drawCache.LightColorsModel);
		const ::gpk::SGeometryQuads								& mesh						= shipState.Scene.Geometry[::ghg::SHIP_GEOMETRY_SPHERE];

		for(uint32_t iTriangle = 0; iTriangle < mesh.Triangles.size(); ++iTriangle) {
			::gpk::clear(drawCache.PixelCoords, drawCache.PixelVertexWeights);
			pixelsDrawn += ::gpk::drawQuadTriangle(targetPixels, mesh, iTriangle, matrixTransform, matrixTransformVP, shipState.Scene.Global.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel, depthBuffer
				, [bulletColor, absanim](::gpk::view2d<::gpk::bgra> targetPixels, const ::gpk::n2<int16_t> & pixelCoord, const ::gpk::bgra & color) {
					targetPixels[pixelCoord.y][pixelCoord.x] = ::gpk::interpolate_linear(::gpk::rgbaf(color), bulletColor, absanim).Clamp(); 
					return 0;
			});
		}
	}
	{
		//image.resize({1, 64}, bulletColor);
		for(uint32_t y = 0; y < image.metrics().y; ++y)
			image[y][0].a	= uint8_t(y * 2);
		::gpk::m4f32									mS							= {};

		(void)position; (void)prevPosition;
		::gpk::n3f32									direction					= _direction;//position - prevPosition;
		double													blurLength					= direction.Length();
		mS.Scale(1, (float)blurLength, 1, true);

		::gpk::m4f32									mRY							= {};
		::gpk::m4f32									mRP							= {};
		//direction.AngleWith();
		::gpk::quat<float>								qry							= {}; 
		::gpk::quat<float>								qrz							= {}; 
		direction.Normalize();
		::gpk::n2f32									vxz							= {_direction.x, _direction.z}; vxz.InPlaceNormalize();
		double													angleY						= vxz.AngleWith({1, 0});
		double													angleZ						= ::gpk::math_pi_2;
		if(_direction.z >= 0) 
			angleY *= -1;
		qrz.CreateFromAxisAngle({0, 0, 1}, angleZ); qrz.Normalize();
		qry.CreateFromAxisAngle({0, 1, 0}, angleY); qry.Normalize();
		mRY.SetOrientation(qrz);
		mRP.SetOrientation(qry);
		matrixTransform										= mS * mRY * mRP;
		matrixTransform.SetTranslation(position, false);
		const ::gpk::m4f32							matrixTransformVP			= matrixTransform * matrixVP;
		::gpk::clear(drawCache.LightPointsModel, drawCache.LightColorsModel);
		const ::gpk::SGeometryQuads								& mesh						= shipState.Scene.Geometry[::ghg::SHIP_GEOMETRY_CYLINDER];
		for(uint32_t iTriangle = 0; iTriangle < mesh.Triangles.size(); ++iTriangle) {
			::gpk::clear(drawCache.PixelCoords, drawCache.PixelVertexWeights);
			pixelsDrawn += ::gpk::drawQuadTriangle(targetPixels, mesh, iTriangle, matrixTransform, matrixTransformVP, shipState.Scene.Global.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel, depthBuffer
				, [](::gpk::view2d<::gpk::bgra> targetPixels, const ::gpk::n2<int16_t> & pixelCoord, const ::gpk::bgra & color) {
					::gpk::bgra & tp = targetPixels[pixelCoord.y][pixelCoord.x];
					tp = ::gpk::interpolate_linear(color, tp, color.a / 255.0f); 
					return 0;
			});
		}
	}
	return pixelsDrawn;
}

static	int											drawShots			(::gpk::view2d<::gpk::bgra> targetPixels
	, const ::ghg::SShipManager					& shipState
	, const ::ghg::SShots						& shots
	, const ::gpk::m4f32				& matrixVPV
	, float										animationTime
	, const ::ghg::SShipCore					& shipCore
	, const ::ghg::SWeapon						& weapon
	, ::gpk::view2d<uint32_t>				depthBuffer
	, ::gpk::apod<::gpk::n3f32>	pixelCoordsCache
	, ::ghg::SGalaxyHellDrawCache				& drawCache
	) {
	::gpk::rgbaf									colorShot				= ::gpk::WHITE;
	double												brightRadius			= 1;
	double												intensity				= 1;
	bool												line					= true;
	if(::ghg::WEAPON_LOAD_Cannonball == weapon.Load) {
		colorShot			= shipCore.Team ? ::gpk::rgbaf{1.0f, 0.25f, 0.75f} : ::gpk::TURQUOISE;
		line				= false;
	}
	else {
		if(::ghg::WEAPON_LOAD_Ray == weapon.Load) { 
			colorShot			= ::gpk::rgbaf{1.0f, 0.1f, 0.0f}; 
			intensity			=  2; 
		}
		else if(::ghg::WEAPON_LOAD_Bullet == weapon.Load) { 
			colorShot			= ::gpk::DARKGRAY; 
			intensity			= .25; 
		}
		else if(::ghg::WEAPON_LOAD_Shell == weapon.Load) { 
			colorShot			= ::gpk::GRAY; 
			intensity			= .25; 
		}
		else if(::ghg::WEAPON_LOAD_Rocket == weapon.Load) {
			colorShot			= shipCore.Team ? ::gpk::rgbaf{1.0f, 0.125f, 0.25f} : ::gpk::LIGHTORANGE;
			//brightRadius		= 2.6;
			line				= false;
		}
		else if(::ghg::WEAPON_LOAD_Missile == weapon.Load) {
			colorShot			= shipCore.Team ? ::gpk::rgbaf{1.0f, 0.025f, 0.05f} : ::gpk::CYAN;
			//brightRadius		= 2.6;
			line				= false;
		}
	}
	const ::gpk::n2<int32_t>						targetMetrics			= targetPixels.metrics().Cast<int32_t>();
	for(uint32_t iShot = 0; iShot < shots.Brightness.size(); ++iShot) {
		float												brightness				= shots.Brightness[iShot];
		pixelCoordsCache.clear();
		const ::gpk::n3f32							& starPosPrev			= shots.PositionDraw[iShot];
		const ::gpk::n3f32							& starPos				= shots.Particles.Position[iShot];
		//::gpk::line3<float>								raySegment				= {starPosPrev, starPos};

		::gpk::line3<float>								raySegment				= {starPos, starPosPrev}; //raySegmentWorld;
		raySegment.A									= matrixVPV.Transform(raySegment.A);
		raySegment.B									= matrixVPV.Transform(raySegment.B);
		if(raySegment.A.z < 0 || raySegment.A.z > 1) continue;
		if(raySegment.B.z < 0 || raySegment.B.z > 1) continue;
		::gpk::drawLine(targetMetrics.Cast<uint16_t>(), raySegment, pixelCoordsCache, depthBuffer);
		if(line) {
		}
		else {
			//pixelCoordsCache.push_back(raySegment.B);
			::drawCannonball(shipState, starPos, starPosPrev, shots.Particles.Direction[iShot], colorShot, animationTime, matrixVPV, targetPixels, depthBuffer, drawCache);
		}
		const double											pixelCoordUnit		= 1.0 / (pixelCoordsCache.size());
		for(uint32_t iPixelCoord = 0, countPixelCoords = pixelCoordsCache.size(); iPixelCoord < countPixelCoords; ++iPixelCoord) {
			const ::gpk::n3f32							& pixelCoord		= pixelCoordsCache[iPixelCoord];
			if(pixelCoord.z < 0 || pixelCoord.z > 1)
				continue;
			if( pixelCoord.y < 0 || pixelCoord.y >= targetMetrics.y
			 || pixelCoord.x < 0 || pixelCoord.x >= targetMetrics.x
			)
				continue;
			targetPixels[(uint32_t)pixelCoord.y][(uint32_t)pixelCoord.x]	= colorShot;
			const uint32_t											depth				= uint32_t(pixelCoord.z * 0xFFFFFFFFU);
			const	double											brightRadiusSquared	= brightRadius * brightRadius;
			double													brightUnit			= 1.0 / brightRadiusSquared * brightness;
			for(int32_t y = (int32_t)-brightRadius, brightCount = (int32_t)brightRadius; y < brightCount; ++y)
			for(int32_t x = (int32_t)-brightRadius; x < brightCount; ++x) {
				::gpk::n2f32									brightPos			= {(float)x, (float)y};
				const double											brightDistance		= brightPos.LengthSquared();
				if(brightDistance <= brightRadiusSquared) {
					::gpk::n2<int32_t>									blendPos			= ::gpk::n2<int32_t>{(int32_t)pixelCoord.x, (int32_t)pixelCoord.y} + (brightPos).Cast<int32_t>();
					if( blendPos.y < 0 || blendPos.y >= targetMetrics.y
					 || blendPos.x < 0 || blendPos.x >= targetMetrics.x
					)
						continue;
					uint32_t												& blendVal			= depthBuffer[blendPos.y][blendPos.x];
					if(depth > blendVal)
						continue;
					blendVal											= depth;
					::gpk::bgra										& pixelVal						= targetPixels[blendPos.y][blendPos.x];
					double													finalBrightness
						= line
						? brightDistance * brightUnit * pixelCoordUnit * (countPixelCoords - 1 - iPixelCoord)
						: 1.0 - (brightDistance * brightUnit)
						;
					::gpk::rgbaf										pixelColor						= ::gpk::interpolate_linear(::gpk::rgbaf{pixelVal}, colorShot, finalBrightness * intensity);
					pixelVal											= pixelColor.Clamp();
				}
			}
		}

	}
	return 0;
}

int													ghg::getLightArraysFromDebris
	( const ::ghg::SDecoState								& decoState
	, ::gpk::apod<::gpk::n3f32>				& lightPoints
	, ::gpk::apod<::gpk::bgra>					& lightColors
	, const ::gpk::view<const ::gpk::bgra>		& debrisColors
	)						{
	for(uint32_t iParticle = 0; iParticle < decoState.Debris.Particles.Position.size(); ++iParticle) {
		lightPoints.push_back(decoState.Debris.Particles.Position[iParticle]);
		::gpk::rgbaf										colorShot			= debrisColors[iParticle % debrisColors.size()];
		lightColors.push_back(colorShot * decoState.Debris.Brightness[iParticle]);
	}
	return 0;
}


int													ghg::getLightArraysFromShips
	( const ::ghg::SShipManager							& shipState
	, ::gpk::apod<::gpk::n3f32>			& lightPoints
	, ::gpk::apod<::gpk::bgra>				& lightColors
	) {
	constexpr ::gpk::bgra								colorLightPlayer		= ::gpk::bgra{0xFF, 0x88, 0xFF};
	constexpr ::gpk::bgra								colorLightEnemy			= ::gpk::bgra{0xFF, 0x88, 0x88};
	for(uint32_t iShip = 0; iShip < shipState.ShipCores.size(); ++iShip) {
		const ::ghg::SShipCore									& ship					= shipState.ShipCores[iShip];
		lightPoints.push_back(shipState.ShipPhysics.Centers[shipState.EntitySystem.Entities[ship.Entity].Body].Position);
		lightColors.push_back((0 == shipState.ShipCores[iShip].Team) ? colorLightPlayer : colorLightEnemy);
		for(uint32_t iPart = 0; iPart < shipState.ShipParts[iShip].size(); ++iPart) {
			const ::ghg::SOrbiter									& shipPart				= shipState.Orbiters[shipState.ShipParts[iShip][iPart]];
			const ::gpk::rgbaf								colorShot
				= (::ghg::WEAPON_LOAD_Ray			== shipState.Weapons[shipPart.Weapon].Load) ? ::gpk::rgbaf{1.0f, 0.1f, 0.0f}
				: (::ghg::WEAPON_LOAD_Cannonball	== shipState.Weapons[shipPart.Weapon].Load) ? ship.Team ? ::gpk::rgbaf{1.0f, 0.125f, 0.25f} : ::gpk::TURQUOISE
				: (::ghg::WEAPON_LOAD_Bullet		== shipState.Weapons[shipPart.Weapon].Load) ? ::gpk::GRAY
				: ::gpk::rgbaf{::gpk::bgra{0xFF, 0xFF, 0xFF}}
				;
			for(uint32_t iShot = 0; iShot < shipState.Shots[shipPart.Weapon].Particles.Position.size(); ++iShot) {
				lightPoints.push_back(shipState.Shots[shipPart.Weapon].Particles.Position[iShot]);
				lightColors.push_back(colorShot);
			}
		}
	}
	return 0;
}

int												ghg::getLightArrays
	( const ::ghg::SShipManager							& shipState
	, const ::ghg::SDecoState							& decoState
	, ::gpk::apod<::gpk::n3f32>			& lightPoints
	, ::gpk::apod<::gpk::bgra>				& lightColors
	, const ::gpk::view<const ::gpk::bgra>	& debrisColors
	) {
	::ghg::getLightArraysFromShips(shipState, lightPoints, lightColors);
	::ghg::getLightArraysFromDebris(decoState, lightPoints, lightColors, debrisColors);
	return 0;
}

int												ghg::getLightArrays
	( const ::gpk::n3f32							& modelPosition
	, const ::gpk::apod<::gpk::n3f32>			& lightPointsWorld
	, const ::gpk::apod<::gpk::bgra>				& lightColorsWorld
	, ::gpk::apod<::gpk::n3f32>				& lightPointsModel
	, ::gpk::apod<::gpk::bgra>					& lightColorsModel
	) {
	::gpk::clear(lightPointsModel, lightColorsModel);
	for(uint32_t iLightPoint = 0; iLightPoint < lightPointsWorld.size(); ++iLightPoint) {
		const ::gpk::n3f32								& lightPoint		=	lightPointsWorld[iLightPoint];
		if((lightPoint - modelPosition).LengthSquared() < (MAX_LIGHT_RANGE * MAX_LIGHT_RANGE)) {
			lightPointsModel.push_back(lightPoint);
			lightColorsModel.push_back(lightColorsWorld[iLightPoint]);
		}
	}
	return 0;
}
//
//static	int											getLightArrays
//	( const ::gpk::n3f32							& modelPosition
//	, const ::gpk::apod<::gpk::n3f32>			& lightPointsWorld
//	, ::gpk::apod<uint16_t>							& indicesPointLights
//	) {
//	indicesPointLights.clear();
//	for(uint32_t iLightPoint = 0; iLightPoint < lightPointsWorld.size(); ++iLightPoint) {
//		const ::gpk::n3f32								& lightPoint		=	lightPointsWorld[iLightPoint];
//		if((lightPoint - modelPosition).LengthSquared() < (MAX_LIGHT_RANGE * MAX_LIGHT_RANGE))
//			indicesPointLights.push_back((uint16_t)iLightPoint);
//	}
//	return 0;
//}

int												ghg::drawOrbiter
	( const ::ghg::SShipManager							& shipState
	, const ::ghg::SOrbiter								& shipPart
	, const ::gpk::rgbaf							& shipColor
	, float												animationTime
	, const ::gpk::m4f32						& matrixVP
	, ::gpk::view2d<::gpk::bgra>				& targetPixels
	, ::gpk::view2d<uint32_t>						depthBuffer
	, ::ghg::SGalaxyHellDrawCache						& drawCache
	) {
	uint32_t												pixelsDrawn				= 0;
	const ::gpk::au32						& entityChildren		= shipState.EntitySystem.EntityChildren[shipPart.Entity];
	double													absanim					= fabsf(sinf(animationTime * 3));
	const ::gpk::rgbaf								shadedColor				= (absanim < .5) ? ::gpk::rgbaf{} : shipColor * (absanim * .5);
	for(uint32_t iEntity = 0; iEntity < entityChildren.size(); ++iEntity) {
		const ::ghg::SEntity									& entityChild				= shipState.EntitySystem.Entities[entityChildren[iEntity]];
		if(-1 == entityChild.Parent)
			continue;
		if(-1 == entityChild.Geometry)
			continue;
		const ::gpk::m4f32							& matrixTransform				= shipState.Scene.Transforms[entityChild.Transform];
		::gpk::m4f32									matrixTransformVP			= matrixTransform * matrixVP;
		::ghg::getLightArrays(matrixTransform.GetTranslation(), drawCache.LightPointsWorld, drawCache.LightColorsWorld, drawCache.LightPointsModel, drawCache.LightColorsModel);
		const ::gpk::SGeometryQuads								& mesh						= shipState.Scene.Geometry[entityChild.Geometry];
		const ::gpk::view2d<const ::gpk::bgra>					image						= shipState.Scene.Image	[entityChild.Image].View;
		for(uint32_t iTriangle = 0; iTriangle < mesh.Triangles.size(); ++iTriangle) {
			::gpk::clear(drawCache.PixelCoords, drawCache.PixelVertexWeights);
			pixelsDrawn += ::gpk::drawQuadTriangle(targetPixels, mesh, iTriangle, matrixTransform, matrixTransformVP, shipState.Scene.Global.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel, depthBuffer
				, [shadedColor](::gpk::view2d<::gpk::bgra> targetPixels, const ::gpk::n2<int16_t> & pixelCoord, const ::gpk::bgra & color) {
					//if( color.r > 64
					// || color.g > 128
					//)
						targetPixels[pixelCoord.y][pixelCoord.x] = (::gpk::rgbaf(color) + shadedColor).Clamp();
					//else
					//	targetPixels[pixelCoord.y][pixelCoord.x] = color; 
					return 1;
			});
		}
	}

	return pixelsDrawn;
}

static	int											drawShip
	( const ::ghg::SGalaxyHell							& solarSystem
	, int32_t											iShip
	, const ::gpk::m4f32						& matrixVP
	, ::gpk::view2d<::gpk::bgra>				& targetPixels
	, ::gpk::view2d<uint32_t>						depthBuffer
	, ::ghg::SGalaxyHellDrawCache						& drawCache
	, const ::gpk::SRasterFont							& font
	) {
	const ::ghg::SShipCore									& shipCore			= solarSystem.ShipState.ShipCores[iShip];

	uint32_t												pixelsDrawn			= 0;
	const ::gpk::bgra									playerColor			= ((uint32_t)iShip < solarSystem.PlayState.CountPlayers) ? solarSystem.Pilots[iShip].Color : ::gpk::bgra(::gpk::RED);
	const ::gpk::view<const uint32_t>					shipParts			= solarSystem.ShipState.ShipParts[iShip];
	for(uint32_t iPart = 0; iPart < shipParts.size(); ++iPart) {
		const ::ghg::SOrbiter									& shipPart				= solarSystem.ShipState.Orbiters[shipParts[iPart]];
		if(shipPart.Health <= 0)
			continue;
		pixelsDrawn += ::ghg::drawOrbiter(solarSystem.ShipState, shipPart, playerColor, (float)solarSystem.DecoState.AnimationTime, matrixVP, targetPixels, depthBuffer, drawCache);
	}

	if(iShip >= (int32_t)solarSystem.PlayState.CountPlayers || shipCore.Team)
		return 0;

	const ::ghg::SEntity									& entity			= solarSystem.ShipState.EntitySystem.Entities[shipCore.Entity];
	const ::gpk::m4f32							& matrixTransform	= solarSystem.ShipState.Scene.Transforms[entity.Transform];
	::gpk::n3f32									starPos				= matrixTransform.GetTranslation() + ::gpk::n3f32{0.0f, 8.0f, 0.0f};
	starPos												= matrixVP.Transform(starPos);
	starPos.x = ::gpk::clamp(starPos.x, 20.0f, targetPixels.metrics().x - 20.0f);
	starPos.y = ::gpk::clamp(starPos.y, 20.0f, targetPixels.metrics().y - 20.0f);
	const ::gpk::n2<int32_t>							pixelCoord			= {(int32_t)starPos.x, (int32_t)starPos.y};

	const ::gpk::vcs										finalText			= solarSystem.Pilots[iShip].Name;
	::gpk::rect2<int16_t>								rectText			= {{}, {int16_t(font.CharSize.x * finalText.size()), font.CharSize.y}};
	rectText.Offset = (pixelCoord - ::gpk::n2<int32_t>{(rectText.Size.x >> 1), (rectText.Size.y >> 1)}).Cast<int16_t>();

	::gpk::apod<::gpk::n2<uint16_t>>				dstCoords;
	gpk_necs(::gpk::textLineRaster(targetPixels.metrics(), font.CharSize, rectText, font.Texture, finalText, dstCoords));
	for(uint32_t iCoord = 0; iCoord < dstCoords.size(); ++iCoord) {
		const ::gpk::n2<uint16_t>										dstCoord												= dstCoords[iCoord];
		if(::gpk::in_range(dstCoord, {{}, targetPixels.metrics().Cast<uint16_t>()})) {
			::gpk::setPixel(targetPixels, dstCoord.Cast<int16_t>(), (shipCore.Health > 0) ? playerColor : ::gpk::bgra(::gpk::interpolate_linear(::gpk::rgbaf(playerColor), ::gpk::rgbaf(targetPixels[dstCoord.y][dstCoord.x]), .8f)));
		}
	}
	return pixelsDrawn;
}

static	int											drawExplosion
	( const ::ghg::SGalaxyHell							& solarSystem
	, const ::ghg::SExplosion							& explosion
	, const ::gpk::m4f32						& matrixView
	, ::gpk::view2d<::gpk::bgra>				& targetPixels
	, ::gpk::view2d<uint32_t>						depthBuffer
	, ::ghg::SGalaxyHellDrawCache						& drawCache
	) {
	::gpk::view2d<const ::gpk::bgra>				image					= solarSystem.ShipState.Scene.Image		[explosion.IndexImage].View;
	const ::gpk::SGeometryQuads								& mesh					= solarSystem.ShipState.Scene.Geometry	[explosion.IndexMesh];
	for(uint32_t iExplosionPart = 0; iExplosionPart < explosion.Particles.Position.size(); ++iExplosionPart) {
		const ::gpk::SRange<uint16_t>							& sliceMesh				= explosion.Slices[iExplosionPart];
		::gpk::m4f32									matrixPart				= {};
		matrixPart.Identity();
		if(iExplosionPart % 5) matrixPart.RotationY(solarSystem.DecoState.AnimationTime * 2);
		if(iExplosionPart % 3) matrixPart.RotationX(solarSystem.DecoState.AnimationTime * 2);
		if(iExplosionPart % 2) matrixPart.RotationZ(solarSystem.DecoState.AnimationTime * 2);
		matrixPart.SetTranslation(explosion.Particles.Position[iExplosionPart], false);
		::gpk::m4f32									matrixTransformView		= matrixPart * matrixView;
		::ghg::getLightArrays(matrixPart.GetTranslation(), drawCache.LightPointsWorld, drawCache.LightColorsWorld, drawCache.LightPointsModel, drawCache.LightColorsModel);
		for(uint32_t iTriangle = 0, countTriangles = sliceMesh.Count; iTriangle < countTriangles; ++iTriangle) {
			drawCache.PixelCoords			.clear();
			drawCache.PixelVertexWeights	.clear();
			const uint32_t											iActualTriangle		= sliceMesh.Offset + iTriangle;
			::gpk::tri3	<float>								triangle			= mesh.Triangles	[iActualTriangle];
			::gpk::tri3	<float>								triangleWorld		= mesh.Triangles	[iActualTriangle];
			::gpk::n3		<float>								normal				= mesh.Normals		[iActualTriangle / 2];
			::gpk::tri2	<float>								triangleTexCoords	= mesh.TextureCoords[iActualTriangle];
			::gpk::tri3	<float>								triangleScreen		= triangleWorld;
			::gpk::transform(triangleScreen, matrixTransformView);
			if(triangleScreen.ClipZ())
				continue;

			::gpk::transform(triangleWorld, matrixPart);
			normal												= matrixPart.TransformDirection(normal).Normalize();
 			::gpk::drawQuadTriangle(targetPixels.metrics(), triangle, matrixTransformView, drawCache.PixelCoords, drawCache.PixelVertexWeights, depthBuffer);
 			::gpk::drawPixels(targetPixels, triangleWorld, normal, triangleTexCoords, solarSystem.ShipState.Scene.Global.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel);
			drawCache.PixelCoords			.clear();
			drawCache.PixelVertexWeights	.clear();
			triangle											= {triangle.A, triangle.C, triangle.B};
			triangleWorld										= {triangleWorld.A, triangleWorld.C, triangleWorld.B};
			triangleTexCoords									= {triangleTexCoords.A, triangleTexCoords.C, triangleTexCoords.B};
			normal												*= -1;
			::gpk::drawQuadTriangle(targetPixels.metrics(), triangle, matrixTransformView, drawCache.PixelCoords, drawCache.PixelVertexWeights, depthBuffer);
  			::gpk::drawPixels(targetPixels, triangleWorld, normal, triangleTexCoords, solarSystem.ShipState.Scene.Global.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel);
		}
	}
	return 0;
}

int													ghg::solarSystemDraw		(const ::ghg::SGalaxyHell & solarSystem, ::ghg::SGalaxyHellDrawCache & drawCache, ::std::mutex & mutexUpdate)	{
	drawCache.RenderTarget->resize(drawCache.RenderTargetMetrics);
	::gpk::pobj<::ghg::TRenderTarget>					renderTarget				= drawCache.RenderTarget;
	::gpk::view2d<::gpk::bgra>						targetPixels				= renderTarget->Color			; 
	::gpk::view2d<uint32_t>								depthBuffer					= renderTarget->DepthStencil	;

	// ------------------------------------------- Transform and Draw
	if(0 == targetPixels.size())
		return 1;

	memset(targetPixels.begin(), 0, targetPixels.byte_count());
	memset(depthBuffer.begin(), -1, depthBuffer.byte_count());
	{
		::std::lock_guard							lockUpdate					(mutexUpdate);
		::drawStars(solarSystem.DecoState.Stars, targetPixels);
	}

	::gpk::m4f32									matrixView					= {};
	const ::gpk::SCameraPoints						& camera					= solarSystem.ShipState.Scene.Global.Camera[solarSystem.ShipState.Scene.Global.CameraMode];
	::gpk::n3f32									cameraUp					= {0, 1, 0};
	matrixView.LookAt(camera.Position, camera.Target, cameraUp);
	matrixView											*= solarSystem.ShipState.Scene.Global.MatrixProjection;
	drawCache.LightPointsWorld.clear();
	drawCache.LightColorsWorld.clear();
	{
		::std::lock_guard							lockUpdate					(mutexUpdate);
		::ghg::getLightArrays(solarSystem.ShipState, solarSystem.DecoState, drawCache.LightPointsWorld, drawCache.LightColorsWorld, ::ghg::DEBRIS_COLORS);
	}
	drawCache.LightPointsModel.reserve(drawCache.LightPointsWorld.size());
	drawCache.LightColorsModel.reserve(drawCache.LightColorsWorld.size());
	{
		::std::lock_guard							lockUpdate					(mutexUpdate);
		for(uint32_t iShip = 0; iShip < solarSystem.ShipState.ShipCores.size(); ++iShip) {
			const ::ghg::SShipCore									& ship						= solarSystem.ShipState.ShipCores[iShip];
			if(ship.Health <= 0 && ship.Team)
				continue;

			::drawShip(solarSystem, iShip, matrixView, targetPixels, depthBuffer, drawCache, *solarSystem.DecoState.FontManager.Fonts[10]);
		}
	}

	{
		::std::lock_guard							lockUpdate					(mutexUpdate);
		for(uint32_t iExplosion = 0; iExplosion < solarSystem.DecoState.Explosions.size(); ++iExplosion) {
			const ::ghg::SExplosion									& explosion					= solarSystem.DecoState.Explosions[iExplosion];
			if(0 == explosion.Slices.size())
				continue;
			::drawExplosion(solarSystem, explosion, matrixView, targetPixels, depthBuffer, drawCache);
		}
	}
#pragma pack(push, 1)
	struct SRenderNode	{
		uint32_t					Mesh				;
		int32_t						Image				;
		::gpk::SSlice<uint32_t>		Slice				;
		::gpk::au32					IndicesPointLight	;
	};
#pragma pack(pop)
	{
		::std::lock_guard							lockUpdate					(mutexUpdate);
		for(uint32_t iShip = 0; iShip < solarSystem.ShipState.ShipCores.size(); ++iShip) {
			const ::ghg::SShipCore									& shipCore			= solarSystem.ShipState.ShipCores[iShip];
			const ::gpk::au32						& shipParts			= solarSystem.ShipState.ShipParts[iShip];
			for(uint32_t iPart = 0; iPart < shipParts.size(); ++iPart) {
				const ::ghg::SOrbiter									& orbiter				= solarSystem.ShipState.Orbiters[shipParts[iPart]];
				const ::ghg::SWeapon									& weapon				= solarSystem.ShipState.Weapons[orbiter.Weapon];
				::drawShots(targetPixels, solarSystem.ShipState, solarSystem.ShipState.Shots[orbiter.Weapon], matrixView, (float)solarSystem.DecoState.AnimationTime, shipCore, weapon, depthBuffer, drawCache.LightPointsModel, drawCache);

			}
		}
	}
	{
		::std::lock_guard							lockUpdate					(mutexUpdate);
		::drawDebris(targetPixels, solarSystem.DecoState.Debris, matrixView, depthBuffer, ::ghg::DEBRIS_COLORS);
	}
	{
		::std::lock_guard							lockUpdate					(mutexUpdate);
		::drawScoreParticles(targetPixels, solarSystem.DecoState.ScoreParticles, matrixView, depthBuffer, *solarSystem.DecoState.FontManager.Fonts[8]);
	}
	return 0;
}
