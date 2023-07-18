#include "gpk_galaxy_hell.h"
#include "gpk_geometry.h"

#include "gpk_raster_lh.h"
#include "gpk_gui_text.h"
#include "gpk_font.h"

#include "gpk_label.h"
#include "gpk_slice.h"

#include <algorithm>

stacxpr	uint32_t		MAX_LIGHT_RANGE		= 10;

static	::gpk::error_t	drawStars			(const ::ghg::SStars & stars, ::gpk::g8bgra targetPixels)	{
	::gpk::bgra					colors[]			=
		{ {0xfF, 0xfF, 0xfF, }
		, {0xC0, 0xC0, 0xfF, }
		, {0xfF, 0xC0, 0xC0, }
		, {0xC0, 0xD0, 0xC0, }
		};
	for(uint32_t iStar = 0; iStar < stars.Brightness.size(); ++iStar) {
		::gpk::n2f32				starPos				= stars.Position[iStar];
		::gpk::bgra					starFinalColor		= colors[iStar % ::gpk::size(colors)] * stars.Brightness[iStar];
		::gpk::setPixel(targetPixels, starPos.i16(), starFinalColor);
		const	int32_t					brightRadius		= 1 + (iStar % 3) + (rand() % 2);
		const	double					brightRadiusSquared	= brightRadius * (double)brightRadius;
		double							brightUnit			= 1.0 / brightRadiusSquared;
		for(int32_t y = -brightRadius; y < brightRadius; ++y)
		for(int32_t x = -brightRadius; x < brightRadius; ++x) {
			::gpk::n2f32					brightPos			= {(float)x, (float)y};
			const double					brightDistance		= brightPos.LengthSquared();
			if(brightDistance <= brightRadiusSquared) {
				::gpk::n2i16				pixelPos			= (starPos + brightPos).i16();
				if( pixelPos.y >= 0 && pixelPos.y < (int32_t)targetPixels.metrics().y
				 && pixelPos.x >= 0 && pixelPos.x < (int32_t)targetPixels.metrics().x
 				)
					::gpk::setPixel(targetPixels, pixelPos, targetPixels[pixelPos.y][pixelPos.x] + starFinalColor * (1.0-(brightDistance * brightUnit * (1 + (rand() % 3)))));
			}
		}
	}
	return 0;
}

static	::gpk::error_t	drawDebris
	( ::gpk::g8bgra			targetPixels
	, const ::ghg::SDebris					& debris
	, const ::gpk::m4f32					& matrixVPV
	, ::gpk::grid<uint32_t>				depthBuffer
	, const ::gpk::view<const ::gpk::bgra>	& debrisColors
	)	{

	float prebrightness = 2.5f * (1.0f / debris.Brightness.size());
	for(uint32_t iParticle = 0; iParticle < debris.Brightness.size(); ++iParticle) {
		const ::gpk::rgbaf			& colorShot			= debrisColors[iParticle % debrisColors.size()];
		::gpk::n3f32				starPos				= debris.Particles.Position[iParticle];
		starPos					= matrixVPV.Transform(starPos);
		if(starPos.z > 1 || starPos.z < 0)
			continue;
		const ::gpk::n2i32			pixelCoord			= {(int32_t)starPos.x, (int32_t)starPos.y};
		if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
		 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
		)
			continue;
		uint32_t					depth				= uint32_t(starPos.z * 0xFFFFFFFFU);
		if(depth > depthBuffer[pixelCoord.y][pixelCoord.x])
			continue;
		float						brightness		= debris.Brightness[iParticle];
		::gpk::rgbaf				starFinalColor	= colorShot * brightness;
		starFinalColor.g		= ::gpk::max(0.0f, starFinalColor.g - (1.0f - ::gpk::min(1.0f, brightness * prebrightness * (iParticle * 2))));
		starFinalColor.b		= ::gpk::max(0.0f, starFinalColor.b - (1.0f - ::gpk::min(1.0f, brightness * prebrightness * (iParticle * 1))));
		//::gpk::setPixel(targetPixels, pixelCoord, starFinalColor);
		stacxpr	 double				brightRadius		= 1.5;
		stacxpr	 double				brightRadiusSquared	= brightRadius * brightRadius;
		stacxpr	 double				brightUnit			= 1.0 / brightRadiusSquared;

		for(int32_t y = (int32_t)-brightRadius - 1, yStop = (int32_t)brightRadius + 1; y < yStop; ++y)
		for(int32_t x = (int32_t)-brightRadius - 1; x < yStop; ++x) {
			::gpk::n2f32				brightPos			= {(float)x, (float)y};
			const double				brightDistance		= brightPos.LengthSquared();
			if(brightDistance <= brightRadiusSquared) {
				::gpk::n2i32				blendPos			= pixelCoord + (brightPos).i32();
				if( blendPos.y < 0 || blendPos.y >= (int32_t)targetPixels.metrics().y
				 || blendPos.x < 0 || blendPos.x >= (int32_t)targetPixels.metrics().x
				)
					continue;
				uint32_t					& blendVal			= depthBuffer[blendPos.y][blendPos.x];
				if(depth > blendVal)
					continue;
				blendVal				= depth;
				double						finalBrightness		= 1.0-(brightDistance * brightUnit);
				::gpk::bgra					& pixelVal			= targetPixels[blendPos.y][blendPos.x];
				pixelVal				= (starFinalColor * finalBrightness + pixelVal).Clamp();
			}
		}
	}
	return 0;
}

static	::gpk::error_t	drawScoreParticles
	( ::gpk::g8bgra	targetPixels
	, const ::ghg::SScoreParticles	& debris
	, const ::gpk::m4f32			& matrixVPV
	, ::gpk::grid<uint32_t>		depthBuffer
	, const ::gpk::SRasterFont		& font
	)	{

	stacxpr	double				brightRadius		= 1.5;
	stacxpr	double				brightRadiusSquared	= brightRadius * brightRadius;
	stacxpr	double				brightUnit			= 1.0 / brightRadiusSquared;
	for(uint32_t iParticle = 0; iParticle < debris.Scores.size(); ++iParticle) {
		::gpk::n3f32				starPos				= debris.Particles.Position[iParticle];
		starPos					= matrixVPV.Transform(starPos);
		if(starPos.z > 1 || starPos.z < 0)
			continue;
		const ::gpk::n2i16			pixelCoord			= {(int16_t)starPos.x, (int16_t)starPos.y};
		if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
		 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
		)
			continue;
		uint32_t					depth				= uint32_t(starPos.z * 0xFFFFFFFFU);
		if(depth > depthBuffer[pixelCoord.y][pixelCoord.x])
			continue;

		::ghg::SScoreParticle		particle			= debris.Scores[iParticle];
		::gpk::rgbaf				starFinalColor		= ::gpk::GREEN;
		starFinalColor.r		+= 1.0f - ::gpk::min(1.0f, particle.Brightness);
		starFinalColor.g		-= 1.0f - ::gpk::min(1.0f, particle.Brightness);
		starFinalColor.b		-= 1.0f - ::gpk::min(1.0f, particle.Brightness);
		starFinalColor.Clamp();
		starFinalColor.a		= starFinalColor.g;
		::gpk::apod<::gpk::n2u16>	dstCoords;

		char						textToShow[64]		= {};
		sprintf_s(textToShow, "%i", particle.Score);
		const ::gpk::vcs			finalText			= textToShow;
		::gpk::rect2i16				rectText			= {{}, {int16_t(font.CharSize.x * finalText.size()), font.CharSize.y}};
		rectText.Offset = (pixelCoord - ::gpk::n2i16{int16_t(rectText.Size.x >> 1), int16_t(rectText.Size.y >> 1)});

		gpk_necs(::gpk::textLineRaster(targetPixels.metrics(), font.CharSize, rectText, font.Texture, finalText, dstCoords));
		for(uint32_t iCoord = 0; iCoord < dstCoords.size(); ++iCoord) {
			::gpk::pixelBlend(targetPixels, dstCoords[iCoord].i16(), starFinalColor);
		}
	}
	return 0;
}

typedef ::gpk::error_t (FSetPixel)	(::gpk::g8bgra targetPixels, const ::gpk::n2i16 & pixelCoord, const ::gpk::bgra & color);

static	::gpk::error_t	drawCannonball
	( const ::ghg::SShipManager		& shipState
	, const ::gpk::n3f32			& position
	, const ::gpk::n3f32			& prevPosition
	, const ::gpk::n3f32			& _direction
	, const ::gpk::rgbaf			& bulletColor
	, const float					animationTime
	, const ::gpk::m4f32			& matrixVP
	, ::gpk::g8bgra					& targetPixels
	, ::gpk::gu32					depthBuffer
	, ::ghg::SGalaxyHellDrawCache	& drawCache
	) {
	uint32_t					pixelsDrawn				= 0;
	double						absanim					= fabsf(sinf(animationTime * 2));
	::gpk::m4f32				matrixTransform				= {};
	matrixTransform.SetIdentity();
	matrixTransform.SetTranslation(position, false);
	::gpk::img<::gpk::bgra>		image						= {};
	image.resize(::gpk::n2u32{1, 1286}, bulletColor);
	{
		::gpk::m4f32				matrixTransformVP			= matrixTransform * matrixVP;
		::ghg::getLightArrays(matrixTransform.GetTranslation(), drawCache.LightPointsWorld, drawCache.LightColorsWorld, drawCache.LightPointsModel, drawCache.LightColorsModel);
		const ::gpk::SGeometryQuads	& mesh						= shipState.Scene.Geometry[::gpk::SHIP_GEOMETRY_Sphere];

		for(uint32_t iTriangle = 0; iTriangle < mesh.Triangles.size(); ++iTriangle) {
			::gpk::clear(drawCache.PixelCoords, drawCache.PixelVertexWeights);
			pixelsDrawn += ::gpk::drawQuadTriangle(targetPixels, mesh, iTriangle, matrixTransform, matrixTransformVP, shipState.Scene.Global.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel, depthBuffer
				, [bulletColor, absanim](::gpk::g8bgra targetPixels, const ::gpk::n2i16 & pixelCoord, const ::gpk::bgra & color) {
					targetPixels[pixelCoord.y][pixelCoord.x] = ::gpk::interpolate_linear(::gpk::rgbaf(color), bulletColor, absanim).Clamp(); 
					return 0;
			});
		}
	}
	{
		//image.resize({1, 64}, bulletColor);
		for(uint32_t y = 0; y < image.metrics().y; ++y)
			image[y][0].a	= uint8_t(y * 2);
		::gpk::m4f32				mS							= {};

		(void)position; (void)prevPosition;
		::gpk::n3f32				direction					= _direction;//position - prevPosition;
		double						blurLength					= direction.Length();
		mS.Scale(1, (float)blurLength, 1, true);

		::gpk::m4f32				mRY							= {};
		::gpk::m4f32				mRP							= {};
		//direction.AngleWith();
		::gpk::quatf32				qry							= {}; 
		::gpk::quatf32				qrz							= {}; 
		direction.Normalize();
		::gpk::n2f32				vxz							= {_direction.x, _direction.z}; vxz.InPlaceNormalize();
		double						angleY						= vxz.AngleWith({1, 0});
		double						angleZ						= ::gpk::math_pi_2;
		if(_direction.z >= 0) 
			angleY *= -1;
		qrz.CreateFromAxisAngle({0, 0, 1}, angleZ); qrz.Normalize();
		qry.CreateFromAxisAngle({0, 1, 0}, angleY); qry.Normalize();
		mRY.SetOrientation(qrz);
		mRP.SetOrientation(qry);
		matrixTransform			= mS * mRY * mRP;
		matrixTransform.SetTranslation(position, false);
		const ::gpk::m4f32			matrixTransformVP			= matrixTransform * matrixVP;
		::gpk::clear(drawCache.LightPointsModel, drawCache.LightColorsModel);
		const ::gpk::SGeometryQuads	& mesh						= shipState.Scene.Geometry[::gpk::SHIP_GEOMETRY_Cylinder];
		for(uint32_t iTriangle = 0; iTriangle < mesh.Triangles.size(); ++iTriangle) {
			::gpk::clear(drawCache.PixelCoords, drawCache.PixelVertexWeights);
			pixelsDrawn += ::gpk::drawQuadTriangle(targetPixels, mesh, iTriangle, matrixTransform, matrixTransformVP, shipState.Scene.Global.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel, depthBuffer
				, [](::gpk::g8bgra targetPixels, const ::gpk::n2i16 & pixelCoord, const ::gpk::bgra & color) {
					::gpk::bgra			& tp						= targetPixels[pixelCoord.y][pixelCoord.x];
					tp				= ::gpk::interpolate_linear(color, tp, color.a / 255.0f); 
					return 0;
			});
		}
	}
	return pixelsDrawn;
}

static	::gpk::error_t	drawShots			(::gpk::g8bgra targetPixels
	, const ::ghg::SShipManager		& shipState
	, const ::gpk::SShots			& shots
	, const ::gpk::m4f32			& matrixVPV
	, float							animationTime
	, const ::gpk::SSpaceshipCore		& shipCore
	, const ::gpk::SWeapon			& weapon
	, ::gpk::grid<uint32_t>		depthBuffer
	, ::gpk::apod<::gpk::n3f32>		pixelCoordsCache
	, ::ghg::SGalaxyHellDrawCache	& drawCache
	) {
	::gpk::rgbaf				colorShot				= ::gpk::WHITE;
	double						brightRadius			= 1;
	double						intensity				= 1;
	bool						line					= true;
	if(::gpk::WEAPON_LOAD_Cannonball == weapon.Shot.Type) {
		colorShot				= shipCore.Team ? ::gpk::rgbaf{1.0f, 0.25f, 0.75f} : ::gpk::TURQUOISE;
		line					= false;
	}
	else {
		if(::gpk::WEAPON_LOAD_Ray == weapon.Shot.Type) { 
			colorShot				= ::gpk::rgbaf{1.0f, 0.1f, 0.0f}; 
			intensity				=  2; 
		}
		else if(::gpk::WEAPON_LOAD_Bullet == weapon.Shot.Type) { 
			colorShot				= ::gpk::DARKGRAY; 
			intensity				= .25; 
		}
		else if(::gpk::WEAPON_LOAD_Shot == weapon.Shot.Type) { 
			colorShot				= ::gpk::GRAY; 
			intensity				= .25; 
		}
		else if(::gpk::WEAPON_LOAD_Rocket == weapon.Shot.Type) {
			colorShot				= shipCore.Team ? ::gpk::rgbaf{1.0f, 0.125f, 0.25f} : ::gpk::LIGHTORANGE;
			//brightRadius			= 2.6;
			line					= false;
		}
		else if(::gpk::WEAPON_LOAD_Missile == weapon.Shot.Type) {
			colorShot				= shipCore.Team ? ::gpk::rgbaf{1.0f, 0.025f, 0.05f} : ::gpk::CYAN;
			//brightRadius			= 2.6;
			line					= false;
		}
	}
	const ::gpk::n2i32			targetMetrics			= targetPixels.metrics().i32();
	for(uint32_t iShot = 0; iShot < shots.Brightness.size(); ++iShot) {
		float						brightness				= shots.Brightness[iShot];
		pixelCoordsCache.clear();
		const ::gpk::n3f32			& starPosPrev			= shots.PositionDraw[iShot];
		const ::gpk::n3f32			& starPos				= shots.Particles.Position[iShot];
		//::gpk::line3<float>		raySegment				= {starPosPrev, starPos};

		::gpk::line3f32				raySegment				= {starPos, starPosPrev}; //raySegmentWorld;
		raySegment.A			= matrixVPV.Transform(raySegment.A);
		raySegment.B			= matrixVPV.Transform(raySegment.B);
		if(raySegment.A.z < 0 || raySegment.A.z > 1) continue;
		if(raySegment.B.z < 0 || raySegment.B.z > 1) continue;
		::gpk::drawLine(targetMetrics.u16(), raySegment, pixelCoordsCache, depthBuffer);
		if(line) {
		}
		else {
			//pixelCoordsCache.push_back(raySegment.B);
			::drawCannonball(shipState, starPos, starPosPrev, shots.Particles.Direction[iShot], colorShot, animationTime, matrixVPV, targetPixels, depthBuffer, drawCache);
		}
		const double			pixelCoordUnit		= 1.0 / (pixelCoordsCache.size());
		for(uint32_t iPixelCoord = 0, countPixelCoords = pixelCoordsCache.size(); iPixelCoord < countPixelCoords; ++iPixelCoord) {
			const ::gpk::n3f32		& pixelCoord		= pixelCoordsCache[iPixelCoord];
			if(pixelCoord.z < 0 || pixelCoord.z > 1)
				continue;
			if( pixelCoord.y < 0 || pixelCoord.y >= targetMetrics.y
			 || pixelCoord.x < 0 || pixelCoord.x >= targetMetrics.x
			)
				continue;
			targetPixels[(uint32_t)pixelCoord.y][(uint32_t)pixelCoord.x]	= colorShot;
			const uint32_t			depth				= uint32_t(pixelCoord.z * 0xFFFFFFFFU);
			const double			brightRadiusSquared	= brightRadius * brightRadius;
			double					brightUnit			= 1.0 / brightRadiusSquared * brightness;
			for(int32_t y = (int32_t)-brightRadius, brightCount = (int32_t)brightRadius; y < brightCount; ++y)
			for(int32_t x = (int32_t)-brightRadius; x < brightCount; ++x) {
				::gpk::n2f32			brightPos			= {(float)x, (float)y};
				const double			brightDistance		= brightPos.LengthSquared();
				if(brightDistance <= brightRadiusSquared) {
					::gpk::n2i32			blendPos			= ::gpk::n2<int32_t>{(int32_t)pixelCoord.x, (int32_t)pixelCoord.y} + (brightPos).i32();
					if( blendPos.y < 0 || blendPos.y >= targetMetrics.y
					 || blendPos.x < 0 || blendPos.x >= targetMetrics.x
					)
						continue;
					uint32_t				& blendVal			= depthBuffer[blendPos.y][blendPos.x];
					if(depth > blendVal)
						continue;
					blendVal			= depth;
					::gpk::bgra				& pixelVal			= targetPixels[blendPos.y][blendPos.x];
					double					finalBrightness
						= line ? brightDistance * brightUnit * pixelCoordUnit * (countPixelCoords - 1 - iPixelCoord)
						: 1.0 - (brightDistance * brightUnit)
						;
					::gpk::rgbaf			pixelColor			= ::gpk::interpolate_linear(::gpk::rgbaf{pixelVal}, colorShot, finalBrightness * intensity);
					pixelVal			= pixelColor.Clamp();
				}
			}
		}

	}
	return 0;
}

::gpk::error_t			ghg::getLightArraysFromDebris
	( const ::ghg::SDecoState								& decoState
	, ::gpk::an3f32			& lightPoints
	, ::gpk::a8bgra			& lightColors
	, const ::gpk::vc8bgra	& debrisColors
	)						{
	for(uint32_t iParticle = 0; iParticle < decoState.Debris.Particles.Position.size(); ++iParticle) {
		lightPoints.push_back(decoState.Debris.Particles.Position[iParticle]);
		::gpk::rgbaf										colorShot			= debrisColors[iParticle % debrisColors.size()];
		lightColors.push_back(colorShot * decoState.Debris.Brightness[iParticle]);
	}
	return 0;
}


::gpk::error_t			ghg::getLightArraysFromShips
	( const ::ghg::SShipManager	& shipState
	, ::gpk::an3f32				& lightPoints
	, ::gpk::a8bgra				& lightColors
	) {
	constexpr ::gpk::bgra		colorLightPlayer		= ::gpk::bgra{0xFF, 0x88, 0xFF};
	constexpr ::gpk::bgra		colorLightEnemy			= ::gpk::bgra{0xFF, 0x88, 0x88};
	for(uint32_t iShip = 0; iShip < shipState.SpaceshipManager.ShipCores.size(); ++iShip) {
		lightPoints.push_back(shipState.GetShipPosition(iShip));
		const ::gpk::SSpaceshipCore	& ship					= shipState.SpaceshipManager.ShipCores[iShip];
		lightColors.push_back((0 == ship.Team) ? colorLightPlayer : colorLightEnemy);
		for(uint32_t iPart = 0; iPart < shipState.SpaceshipManager.ShipParts[iShip].size(); ++iPart) {
			const ::gpk::SSpaceshipOrbiter									& shipPart				= shipState.SpaceshipManager.Orbiters[shipState.SpaceshipManager.ShipParts[iShip][iPart]];
			const ::gpk::rgbaf								colorShot
				= (::gpk::WEAPON_LOAD_Ray			== shipState.WeaponManager.Weapons[shipPart.Weapon].Shot.Type) ? ::gpk::rgbaf{1.0f, 0.1f, 0.0f}
				: (::gpk::WEAPON_LOAD_Cannonball	== shipState.WeaponManager.Weapons[shipPart.Weapon].Shot.Type) ? ship.Team ? ::gpk::rgbaf{1.0f, 0.125f, 0.25f} : ::gpk::TURQUOISE
				: (::gpk::WEAPON_LOAD_Bullet		== shipState.WeaponManager.Weapons[shipPart.Weapon].Shot.Type) ? ::gpk::GRAY
				: ::gpk::rgbaf{::gpk::bgra{0xFF, 0xFF, 0xFF}}
				;
			for(uint32_t iShot = 0; iShot < shipState.WeaponManager.Shots[shipPart.Weapon].Particles.Position.size(); ++iShot) {
				lightPoints.push_back(shipState.WeaponManager.Shots[shipPart.Weapon].Particles.Position[iShot]);
				lightColors.push_back(colorShot);
			}
		}
	}
	return 0;
}

::gpk::error_t			ghg::getLightArrays
	( const ::ghg::SShipManager	& shipState
	, const ::ghg::SDecoState	& decoState
	, ::gpk::an3f32				& lightPoints
	, ::gpk::a8bgra				& lightColors
	, const ::gpk::vc8bgra		& debrisColors
	) {
	::ghg::getLightArraysFromShips(shipState, lightPoints, lightColors);
	::ghg::getLightArraysFromDebris(decoState, lightPoints, lightColors, debrisColors);
	return 0;
}

::gpk::error_t			ghg::getLightArrays
	( const ::gpk::n3f32		& modelPosition
	, const ::gpk::vn3f32		& lightPointsWorld
	, const ::gpk::a8bgra		& lightColorsWorld
	, ::gpk::an3f32				& lightPointsModel
	, ::gpk::a8bgra				& lightColorsModel
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
//static	::gpk::error_t						getLightArrays
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

::gpk::error_t			ghg::drawOrbiter
	( const ::ghg::SShipManager			& shipState
	, const uint32_t					iPartEntity
	, const ::gpk::rgbaf				& shipColor
	, float								animationTime
	, const ::gpk::m4f32				& matrixVP
	, ::gpk::g8bgra						& targetPixels
	, ::gpk::gu32						depthBuffer
	, ::ghg::SGalaxyHellDrawCache		& drawCache
	) {
	uint32_t												pixelsDrawn				= 0;
	const ::gpk::au32						& entityChildren		= shipState.EntitySystem.EntityChildren[iPartEntity];
	double													absanim					= fabsf(sinf(animationTime * 3));
	const ::gpk::rgbaf								shadedColor				= (absanim < .5) ? ::gpk::rgbaf{} : shipColor * (absanim * .5);
	for(uint32_t iEntity = 0; iEntity < entityChildren.size(); ++iEntity) {
		const ::ghg::SGHEntity									& entityChild				= shipState.EntitySystem.Entities[entityChildren[iEntity]];
		if(-1 == entityChild.Parent)
			continue;
		if(-1 == entityChild.Geometry)
			continue;
		const ::gpk::m4f32							& matrixTransform				= shipState.Scene.Transforms[entityChild.Transform];
		::gpk::m4f32									matrixTransformVP			= matrixTransform * matrixVP;
		::ghg::getLightArrays(matrixTransform.GetTranslation(), drawCache.LightPointsWorld, drawCache.LightColorsWorld, drawCache.LightPointsModel, drawCache.LightColorsModel);
		const ::gpk::SGeometryQuads								& mesh						= shipState.Scene.Geometry[entityChild.Geometry];
		const ::gpk::gc8bgra					image						= shipState.Scene.Image	[entityChild.Image].View;
		for(uint32_t iTriangle = 0; iTriangle < mesh.Triangles.size(); ++iTriangle) {
			::gpk::clear(drawCache.PixelCoords, drawCache.PixelVertexWeights);
			pixelsDrawn += ::gpk::drawQuadTriangle(targetPixels, mesh, iTriangle, matrixTransform, matrixTransformVP, shipState.Scene.Global.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel, depthBuffer
				, [shadedColor](::gpk::g8bgra targetPixels, const ::gpk::n2i16 & pixelCoord, const ::gpk::bgra & color) {
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

static	::gpk::error_t	drawShip
	( const ::ghg::SGalaxyHell		& solarSystem
	, int32_t						iShip
	, const ::gpk::m4f32			& matrixVP
	, ::gpk::g8bgra					& targetPixels
	, ::gpk::gu32					depthBuffer
	, ::ghg::SGalaxyHellDrawCache	& drawCache
	, const ::gpk::SRasterFont		& font
	) {
	const ::gpk::SSpaceshipCore				& shipCore			= solarSystem.ShipState.SpaceshipManager.ShipCores[iShip];

	uint32_t							pixelsDrawn			= 0;
	const ::gpk::bgra					playerColor			= ((uint32_t)iShip < solarSystem.PlayState.Constants.Players) ? ::gpk::bgra(solarSystem.Pilots[iShip].Color) : ::gpk::bgra(::gpk::RED);
	const ::gpk::vcu16					shipParts			= solarSystem.ShipState.SpaceshipManager.ShipParts[iShip];
	for(uint32_t iPart = 0; iPart < shipParts.size(); ++iPart) {
		const ::gpk::SSpaceshipOrbiter				& shipPart			= solarSystem.ShipState.SpaceshipManager.Orbiters[shipParts[iPart]];
		if(shipPart.Health.Value <= 0)
			continue;
		pixelsDrawn += ::ghg::drawOrbiter(solarSystem.ShipState, solarSystem.ShipState.ShipPartEntity[shipParts[iPart]], playerColor, (float)solarSystem.DecoState.AnimationTime, matrixVP, targetPixels, depthBuffer, drawCache);
	}

	if(iShip >= (int32_t)solarSystem.PlayState.Constants.Players || shipCore.Team)
		return 0;

	const ::ghg::SGHEntity				& entity			= solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipCoreEntity[iShip]];
	const ::gpk::m4f32					& matrixTransform	= solarSystem.ShipState.Scene.Transforms[entity.Transform];
	::gpk::n3f32						starPos				= matrixTransform.GetTranslation() + ::gpk::n3f32{0.0f, 8.0f, 0.0f};
	starPos							= matrixVP.Transform(starPos);
	starPos.x = ::gpk::clamp(starPos.x, 20.0f, targetPixels.metrics().x - 20.0f);
	starPos.y = ::gpk::clamp(starPos.y, 20.0f, targetPixels.metrics().y - 20.0f);
	const ::gpk::n2<int32_t>			pixelCoord			= {(int32_t)starPos.x, (int32_t)starPos.y};

	const ::gpk::vcs					finalText			= solarSystem.Pilots[iShip].Name;
	::gpk::rect2i16						rectText			= {{}, {int16_t(font.CharSize.x * finalText.size()), font.CharSize.y}};
	rectText.Offset = (pixelCoord - ::gpk::n2i32{(rectText.Size.x >> 1), (rectText.Size.y >> 1)}).i16();

	::gpk::an2u16						dstCoords;
	gpk_necs(::gpk::textLineRaster(targetPixels.metrics(), font.CharSize, rectText, font.Texture, finalText, dstCoords));
	for(uint32_t iCoord = 0; iCoord < dstCoords.size(); ++iCoord) {
		const ::gpk::n2u16					dstCoord			= dstCoords[iCoord];
		if(::gpk::in_range(dstCoord, {{}, targetPixels.metrics().u16()})) {
			::gpk::setPixel(targetPixels, dstCoord.i16(), (shipCore.Health > 0) ? playerColor : ::gpk::bgra(::gpk::interpolate_linear(::gpk::rgbaf(playerColor), ::gpk::rgbaf(targetPixels[dstCoord.y][dstCoord.x]), .8f)));
		}
	}
	return pixelsDrawn;
}

static	::gpk::error_t	drawExplosion
	( const ::ghg::SGalaxyHell		& solarSystem
	, const ::ghg::SExplosion		& explosion
	, const ::gpk::m4f32			& matrixView
	, ::gpk::g8bgra					& targetPixels
	, ::gpk::gu32					depthBuffer
	, ::ghg::SGalaxyHellDrawCache	& drawCache
	) {
	::gpk::gc8bgra						image				= solarSystem.ShipState.Scene.Image		[explosion.IndexImage].View;
	const ::gpk::SGeometryQuads			& mesh				= solarSystem.ShipState.Scene.Geometry	[explosion.IndexMesh];
	for(uint32_t iExplosionPart = 0; iExplosionPart < explosion.Particles.Position.size(); ++iExplosionPart) {
		const ::gpk::rangeu16				& sliceMesh			= explosion.Slices[iExplosionPart];
		::gpk::m4f32						matrixPart			= {};
		matrixPart.Identity();
		if(iExplosionPart % 5) matrixPart.RotationY(solarSystem.DecoState.AnimationTime * 2);
		if(iExplosionPart % 3) matrixPart.RotationX(solarSystem.DecoState.AnimationTime * 2);
		if(iExplosionPart % 2) matrixPart.RotationZ(solarSystem.DecoState.AnimationTime * 2);
		matrixPart.SetTranslation(explosion.Particles.Position[iExplosionPart], false);
		::gpk::m4f32						matrixTransformView	= matrixPart * matrixView;
		::ghg::getLightArrays(matrixPart.GetTranslation(), drawCache.LightPointsWorld, drawCache.LightColorsWorld, drawCache.LightPointsModel, drawCache.LightColorsModel);
		for(uint32_t iTriangle = 0, countTriangles = sliceMesh.Count; iTriangle < countTriangles; ++iTriangle) {
			drawCache.PixelCoords			.clear();
			drawCache.PixelVertexWeights	.clear();
			const uint32_t						iActualTriangle		= sliceMesh.Offset + iTriangle;
			::gpk::tri3f32						triangle			= mesh.Triangles	[iActualTriangle];
			::gpk::tri3f32						triangleWorld		= mesh.Triangles	[iActualTriangle];
			::gpk::n3f32						normal				= mesh.Normals		[iActualTriangle / 2];
			::gpk::tri2f32						triangleTexCoords	= mesh.TextureCoords[iActualTriangle];
			::gpk::tri3f32						triangleScreen		= triangleWorld;
			::gpk::transform(triangleScreen, matrixTransformView);
			if(triangleScreen.ClipZ())
				continue;

			::gpk::transform(triangleWorld, matrixPart);
			normal							= matrixPart.TransformDirection(normal).Normalize();
 			::gpk::drawQuadTriangle(targetPixels.metrics(), triangle, matrixTransformView, drawCache.PixelCoords, drawCache.PixelVertexWeights, depthBuffer);
 			::gpk::drawPixels(targetPixels, triangleWorld, normal, triangleTexCoords, solarSystem.ShipState.Scene.Global.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel);
			drawCache.PixelCoords			.clear();
			drawCache.PixelVertexWeights	.clear();
			triangle						= {triangle.A, triangle.C, triangle.B};
			triangleWorld					= {triangleWorld.A, triangleWorld.C, triangleWorld.B};
			triangleTexCoords				= {triangleTexCoords.A, triangleTexCoords.C, triangleTexCoords.B};
			normal							*= -1;
			::gpk::drawQuadTriangle(targetPixels.metrics(), triangle, matrixTransformView, drawCache.PixelCoords, drawCache.PixelVertexWeights, depthBuffer);
  			::gpk::drawPixels(targetPixels, triangleWorld, normal, triangleTexCoords, solarSystem.ShipState.Scene.Global.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel);
		}
	}
	return 0;
}

::gpk::error_t			ghg::solarSystemDraw		(const ::ghg::SGalaxyHell & solarSystem, ::ghg::SGalaxyHellDrawCache & drawCache, ::std::mutex & mutexUpdate)	{
	drawCache.RenderTarget->resize(drawCache.RenderTargetMetrics);
	::gpk::pobj<::ghg::TRenderTarget>	renderTarget				= drawCache.RenderTarget;
	::gpk::g8bgra			targetPixels				= renderTarget->Color			; 
	::gpk::grid<uint32_t>				depthBuffer					= renderTarget->DepthStencil	;

	// ------------------------------------------- Transform and Draw
	if(0 == targetPixels.size())
		return 1;

	memset(targetPixels.begin(), 0, targetPixels.byte_count());
	memset(depthBuffer.begin(), -1, depthBuffer.byte_count());
	{
		::std::lock_guard			lockUpdate					(mutexUpdate);
		::drawStars(solarSystem.DecoState.Stars, targetPixels);
	}

	::gpk::m4f32				matrixView					= {};
	const ::gpk::SCameraPoints	& camera					= solarSystem.ShipState.Scene.Global.Camera[solarSystem.ShipState.Scene.Global.CameraMode];
	::gpk::n3f32				cameraUp					= {0, 1, 0};
	matrixView.LookAt(camera.Position, camera.Target, cameraUp);
	matrixView				*= solarSystem.ShipState.Scene.Global.MatrixProjection;
	drawCache.LightPointsWorld.clear();
	drawCache.LightColorsWorld.clear();
	{
		::std::lock_guard		lockUpdate					(mutexUpdate);
		::ghg::getLightArrays(solarSystem.ShipState, solarSystem.DecoState, drawCache.LightPointsWorld, drawCache.LightColorsWorld, ::ghg::DEBRIS_COLORS);
	}
	drawCache.LightPointsModel.reserve(drawCache.LightPointsWorld.size());
	drawCache.LightColorsModel.reserve(drawCache.LightColorsWorld.size());
	{
		::std::lock_guard		lockUpdate					(mutexUpdate);
		for(uint32_t iShip = 0; iShip < solarSystem.ShipState.SpaceshipManager.ShipCores.size(); ++iShip) {
			const ::gpk::SSpaceshipCore	& ship						= solarSystem.ShipState.SpaceshipManager.ShipCores[iShip];
			if(ship.Health <= 0 && ship.Team)
				continue;

			::drawShip(solarSystem, iShip, matrixView, targetPixels, depthBuffer, drawCache, *solarSystem.DecoState.FontManager.Fonts[10]);
		}
	}

	{
		::std::lock_guard		lockUpdate					(mutexUpdate);
		for(uint32_t iExplosion = 0; iExplosion < solarSystem.DecoState.Explosions.size(); ++iExplosion) {
			const ::ghg::SExplosion	& explosion					= solarSystem.DecoState.Explosions[iExplosion];
			if(0 == explosion.Slices.size())
				continue;
			::drawExplosion(solarSystem, explosion, matrixView, targetPixels, depthBuffer, drawCache);
		}
	}
#pragma pack(push, 1)
	struct SRenderNode	{
		uint32_t			Mesh				;
		int32_t				Image				;
		::gpk::sliceu32		Slice				;
		::gpk::au32			IndicesPointLight	;
	};
#pragma pack(pop)
	{
		::std::lock_guard		lockUpdate			(mutexUpdate);
		for(uint32_t iShip = 0; iShip < solarSystem.ShipState.SpaceshipManager.ShipCores.size(); ++iShip) {
			const ::gpk::SSpaceshipCore	& shipCore			= solarSystem.ShipState.SpaceshipManager.ShipCores[iShip];
			const ::gpk::au16		& shipParts			= solarSystem.ShipState.SpaceshipManager.ShipParts[iShip];
			for(uint32_t iPart = 0; iPart < shipParts.size(); ++iPart) {
				const ::gpk::SSpaceshipOrbiter	& orbiter			= solarSystem.ShipState.SpaceshipManager.Orbiters[shipParts[iPart]];
				const ::gpk::SWeapon	& weapon			= solarSystem.ShipState.WeaponManager.Weapons[orbiter.Weapon];
				::drawShots(targetPixels, solarSystem.ShipState, solarSystem.ShipState.WeaponManager.Shots[orbiter.Weapon], matrixView, (float)solarSystem.DecoState.AnimationTime, shipCore, weapon, depthBuffer, drawCache.LightPointsModel, drawCache);

			}
		}
	}
	{
		::std::lock_guard		lockUpdate			(mutexUpdate);
		::drawDebris(targetPixels, solarSystem.DecoState.Debris, matrixView, depthBuffer, ::ghg::DEBRIS_COLORS);
	}
	{
		::std::lock_guard		lockUpdate			(mutexUpdate);
		::drawScoreParticles(targetPixels, solarSystem.DecoState.ScoreParticles, matrixView, depthBuffer, *solarSystem.DecoState.FontManager.Fonts[8]);
	}
	return 0;
}
