#include "gpk_galaxy_hell.h"
#include "gpk_geometry.h"

#include "gpk_raster_lh.h"
#include "gpk_gui_text.h"
#include "gpk_font.h"

#include "gpk_label.h"
#include "gpk_slice.h"

using ::gpk::get_value_namep, ::gpk::get_enum_namep, ::gpk::failed;
GPK_USING_TYPEINT();

stxp	u2_t			MAX_LIGHT_RANGE		= 10;

static	gpk::err_t	drawStars			(const ::ghg::SStars & stars, ::gpk::g8bgra targetPixels)	{
	::gpk::bgra					colors[]			=
		{ {0xfF, 0xfF, 0xfF, }
		, {0xC0, 0xC0, 0xfF, }
		, {0xfF, 0xC0, 0xC0, }
		, {0xC0, 0xD0, 0xC0, }
		};
	for(u2_t iStar = 0; iStar < stars.Brightness.size(); ++iStar) {
		::gpk::n2f2_t				starPos				= stars.Position[iStar];
		::gpk::bgra					starFinalColor		= colors[iStar % ::gpk::size(colors)] * stars.Brightness[iStar];
		::gpk::setPixel(targetPixels, starPos.s1(), starFinalColor);
		const	s2_t					brightRadius		= 1 + (iStar % 3) + (rand() % 2);
		const	double					brightRadiusSquared	= brightRadius * (double)brightRadius;
		double							brightUnit			= 1.0 / brightRadiusSquared;
		for(s2_t y = -brightRadius; y < brightRadius; ++y)
		for(s2_t x = -brightRadius; x < brightRadius; ++x) {
			::gpk::n2f2_t					brightPos			= {(float)x, (float)y};
			const double					brightDistance		= brightPos.LengthSquared();
			if(brightDistance <= brightRadiusSquared) {
				::gpk::n2s1_t				pixelPos			= (starPos + brightPos).s1();
				if( pixelPos.y >= 0 && pixelPos.y < (s2_t)targetPixels.metrics().y
				 && pixelPos.x >= 0 && pixelPos.x < (s2_t)targetPixels.metrics().x
 				)
					::gpk::setPixel(targetPixels, pixelPos, targetPixels[pixelPos.y][pixelPos.x] + starFinalColor * (1.0-(brightDistance * brightUnit * (1 + (rand() % 3)))));
			}
		}
	}
	return 0;
}

static	gpk::err_t	drawDebris
	( ::gpk::g8bgra			targetPixels
	, const ::ghg::SDebris					& debris
	, const ::gpk::m4f2_t					& matrixVPV
	, ::gpk::grid<u2_t>				depthBuffer
	, const ::gpk::view<const ::gpk::bgra>	& debrisColors
	)	{

	float prebrightness = 2.5f * (1.0f / debris.Brightness.size());
	for(u2_t iParticle = 0; iParticle < debris.Brightness.size(); ++iParticle) {
		const ::gpk::rgbaf			& colorShot			= debrisColors[iParticle % debrisColors.size()];
		::gpk::n3f2_t				starPos				= debris.Particles.Position[iParticle];
		starPos					= matrixVPV.Transform(starPos);
		if(starPos.z > 1 || starPos.z < 0)
			continue;
		const ::gpk::n2s2_t			pixelCoord			= {(s2_t)starPos.x, (s2_t)starPos.y};
		if( pixelCoord.y < 0 || pixelCoord.y >= (s2_t)targetPixels.metrics().y
		 || pixelCoord.x < 0 || pixelCoord.x >= (s2_t)targetPixels.metrics().x
		)
			continue;
		u2_t					depth				= u2_t(starPos.z * 0xFFFFFFFFU);
		if(depth > depthBuffer[pixelCoord.y][pixelCoord.x])
			continue;
		float						brightness		= debris.Brightness[iParticle];
		::gpk::rgbaf				starFinalColor	= colorShot * brightness;
		starFinalColor.g		= ::gpk::max(0.0f, starFinalColor.g - (1.0f - ::gpk::min(1.0f, brightness * prebrightness * (iParticle * 2))));
		starFinalColor.b		= ::gpk::max(0.0f, starFinalColor.b - (1.0f - ::gpk::min(1.0f, brightness * prebrightness * (iParticle * 1))));
		//::gpk::setPixel(targetPixels, pixelCoord, starFinalColor);
		stxp	 double				brightRadius		= 1.5;
		stxp	 double				brightRadiusSquared	= brightRadius * brightRadius;
		stxp	 double				brightUnit			= 1.0 / brightRadiusSquared;

		for(s2_t y = (s2_t)-brightRadius - 1, yStop = (s2_t)brightRadius + 1; y < yStop; ++y)
		for(s2_t x = (s2_t)-brightRadius - 1; x < yStop; ++x) {
			::gpk::n2f2_t				brightPos			= {(float)x, (float)y};
			const double				brightDistance		= brightPos.LengthSquared();
			if(brightDistance <= brightRadiusSquared) {
				::gpk::n2s2_t				blendPos			= pixelCoord + (brightPos).s2();
				if( blendPos.y < 0 || blendPos.y >= (s2_t)targetPixels.metrics().y
				 || blendPos.x < 0 || blendPos.x >= (s2_t)targetPixels.metrics().x
				)
					continue;
				u2_t					& blendVal			= depthBuffer[blendPos.y][blendPos.x];
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

static	gpk::err_t	drawScoreParticles
	( ::gpk::g8bgra	targetPixels
	, const ::ghg::SScoreParticles	& debris
	, const ::gpk::m4f2_t			& matrixVPV
	, ::gpk::grid<u2_t>		depthBuffer
	, const ::gpk::SRasterFont		& font
	)	{

	stxp	double				brightRadius		= 1.5;
	stxp	double				brightRadiusSquared	= brightRadius * brightRadius;
	stxp	double				brightUnit			= 1.0 / brightRadiusSquared;
	for(u2_t iParticle = 0; iParticle < debris.Scores.size(); ++iParticle) {
		::gpk::n3f2_t				starPos				= debris.Particles.Position[iParticle];
		starPos					= matrixVPV.Transform(starPos);
		if(starPos.z > 1 || starPos.z < 0)
			continue;
		const ::gpk::n2s1_t			pixelCoord			= {(s1_t)starPos.x, (s1_t)starPos.y};
		if( pixelCoord.y < 0 || pixelCoord.y >= (s2_t)targetPixels.metrics().y
		 || pixelCoord.x < 0 || pixelCoord.x >= (s2_t)targetPixels.metrics().x
		)
			continue;
		u2_t					depth				= u2_t(starPos.z * 0xFFFFFFFFU);
		if(depth > depthBuffer[pixelCoord.y][pixelCoord.x])
			continue;

		::ghg::SScoreParticle		particle			= debris.Scores[iParticle];
		::gpk::rgbaf				starFinalColor		= ::gpk::GREEN;
		starFinalColor.r		+= 1.0f - ::gpk::min(1.0f, particle.Brightness);
		starFinalColor.g		-= 1.0f - ::gpk::min(1.0f, particle.Brightness);
		starFinalColor.b		-= 1.0f - ::gpk::min(1.0f, particle.Brightness);
		starFinalColor.Clamp();
		starFinalColor.a		= starFinalColor.g;
		::gpk::apod<::gpk::n2u1_t>	dstCoords;

		sc_t						textToShow[64]		= {};
		::gpk::sprintf_s(textToShow, "%i", particle.Score);
		const ::gpk::vcst_t			finalText			= textToShow;
	::gpk::rect2s1_t			rectText			= {{}, {s1_t(font.CharSize.x * finalText.size()), font.CharSize.y}};
		rectText.Offset = (pixelCoord - ::gpk::n2s1_t{s1_t(rectText.Size.x >> 1), s1_t(rectText.Size.y >> 1)});

		if_fail_fe(::gpk::textLineRaster(targetPixels.metrics(), font.CharSize, rectText, font.Texture, finalText, dstCoords));
		for(u2_t iCoord = 0; iCoord < dstCoords.size(); ++iCoord) {
			::gpk::pixelBlend(targetPixels, dstCoords[iCoord].s1(), starFinalColor);
		}
	}
	return 0;
}

typedef gpk::err_t (FSetPixel)	(::gpk::g8bgra targetPixels, const ::gpk::n2s1_t & pixelCoord, const ::gpk::bgra & color);

static	gpk::err_t	drawCannonball
	( const ::ghg::SShipManager		& shipState
	, const ::gpk::n3f2_t			& position
	, const ::gpk::n3f2_t			& prevPosition
	, const ::gpk::n3f2_t			& _direction
	, const ::gpk::rgbaf			& bulletColor
	, const float					animationTime
	, const ::gpk::m4f2_t			& matrixVP
	, ::gpk::g8bgra					& targetPixels
	, ::gpk::gu2_t					depthBuffer
	, ::ghg::SGalaxyHellDrawCache	& drawCache
	) {
	u2_t					pixelsDrawn				= 0;
	double						absanim					= fabsf(sinf(animationTime * 2));
	::gpk::m4f2_t				matrixTransform				= {};
	matrixTransform.Identity();
	matrixTransform.SetTranslation(position, false);
	::gpk::img<::gpk::bgra>		image						= {};
	image.resize(::gpk::n2u2_t{1, 1286}, bulletColor);
	{
	::gpk::m4f2_t				matrixTransformVP			= matrixTransform * matrixVP;
		::ghg::getLightArrays(matrixTransform.GetTranslation(), drawCache.LightPointsWorld, drawCache.LightColorsWorld, drawCache.LightPointsModel, drawCache.LightColorsModel);
		const ::gpk::SGeometryQuads	& mesh						= shipState.Scene.Geometry[::gpk::SHIP_GEOMETRY_Sphere];

		for(u2_t iTriangle = 0; iTriangle < mesh.Triangles.size(); ++iTriangle) {
			::gpk::clear(drawCache.PixelCoords, drawCache.PixelVertexWeights);
			pixelsDrawn += ::gpk::drawQuadTriangle(targetPixels, mesh, iTriangle, matrixTransform, matrixTransformVP, shipState.Scene.Global.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel, depthBuffer
				, [bulletColor, absanim](::gpk::g8bgra targetPixels, const ::gpk::n2s1_t & pixelCoord, const ::gpk::bgra & color) {
					targetPixels[pixelCoord.y][pixelCoord.x] = ::gpk::interpolate_linear(::gpk::rgbaf(color), bulletColor, absanim).Clamp(); 
					return 0;
			});
		}
	}
	{
		//image.resize({1, 64}, bulletColor);
		for(u2_t y = 0; y < image.metrics().y; ++y)
			image[y][0].a	= u0_t(y * 2);
	::gpk::m4f2_t				mS							= {};

		(void)position; (void)prevPosition;
		::gpk::n3f2_t				direction					= _direction;//position - prevPosition;
		double						blurLength					= direction.Length();
		mS.Scale({1, (float)blurLength, 1}, true);

	::gpk::m4f2_t				mRY							= {};
	::gpk::m4f2_t				mRP							= {};
		//direction.AngleWith();
		::gpk::quatf2_t				qry							= {}; 
		::gpk::quatf2_t				qrz							= {}; 
		direction.Normalize();
		::gpk::n2f2_t				vxz							= {_direction.x, _direction.z}; vxz.InPlaceNormalize();
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
	const ::gpk::m4f2_t			matrixTransformVP			= matrixTransform * matrixVP;
		::gpk::clear(drawCache.LightPointsModel, drawCache.LightColorsModel);
		const ::gpk::SGeometryQuads	& mesh						= shipState.Scene.Geometry[::gpk::SHIP_GEOMETRY_Cylinder];
		for(u2_t iTriangle = 0; iTriangle < mesh.Triangles.size(); ++iTriangle) {
			::gpk::clear(drawCache.PixelCoords, drawCache.PixelVertexWeights);
			pixelsDrawn += ::gpk::drawQuadTriangle(targetPixels, mesh, iTriangle, matrixTransform, matrixTransformVP, shipState.Scene.Global.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel, depthBuffer
				, [](::gpk::g8bgra targetPixels, const ::gpk::n2s1_t & pixelCoord, const ::gpk::bgra & color) {
					::gpk::bgra			& tp						= targetPixels[pixelCoord.y][pixelCoord.x];
					tp				= ::gpk::interpolate_linear(color, tp, color.a / 255.0f); 
					return 0;
			});
		}
	}
	return pixelsDrawn;
}

static	gpk::err_t	drawShots			(::gpk::g8bgra targetPixels
	, const ::ghg::SShipManager		& shipState
	, const ::gpk::SShots			& shots
	, const ::gpk::m4f2_t			& matrixVPV
	, float							animationTime
	, const ::gpk::SSpaceshipCore		& shipCore
	, const ::gpk::SWeapon			& weapon
	, ::gpk::grid<u2_t>		depthBuffer
	, ::gpk::apod<::gpk::n3f2_t>		pixelCoordsCache
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
	const ::gpk::n2s2_t			targetMetrics			= targetPixels.metrics().s2();
	for(u2_t iShot = 0; iShot < shots.Brightness.size(); ++iShot) {
		float						brightness				= shots.Brightness[iShot];
		pixelCoordsCache.clear();
		const ::gpk::n3f2_t			& starPosPrev			= shots.PositionDraw[iShot];
		const ::gpk::n3f2_t			& starPos				= shots.Particles.Position[iShot];
		//::gpk::line3<float>		raySegment				= {starPosPrev, starPos};

		::gpk::line3f2_t				raySegment				= {starPos, starPosPrev}; //raySegmentWorld;
		raySegment.A			= matrixVPV.Transform(raySegment.A);
		raySegment.B			= matrixVPV.Transform(raySegment.B);
		if(raySegment.A.z < 0 || raySegment.A.z > 1) continue;
		if(raySegment.B.z < 0 || raySegment.B.z > 1) continue;
		::gpk::drawLine(targetMetrics.u1(), raySegment, pixelCoordsCache, depthBuffer);
		if(line) {
		}
		else {
			//pixelCoordsCache.push_back(raySegment.B);
			::drawCannonball(shipState, starPos, starPosPrev, shots.Particles.Direction[iShot], colorShot, animationTime, matrixVPV, targetPixels, depthBuffer, drawCache);
		}
		const double			pixelCoordUnit		= 1.0 / (pixelCoordsCache.size());
		for(u2_t iPixelCoord = 0, countPixelCoords = pixelCoordsCache.size(); iPixelCoord < countPixelCoords; ++iPixelCoord) {
			const ::gpk::n3f2_t		& pixelCoord		= pixelCoordsCache[iPixelCoord];
			if(pixelCoord.z < 0 || pixelCoord.z > 1)
				continue;
			if( pixelCoord.y < 0 || pixelCoord.y >= targetMetrics.y
			 || pixelCoord.x < 0 || pixelCoord.x >= targetMetrics.x
			)
				continue;
			targetPixels[(u2_t)pixelCoord.y][(u2_t)pixelCoord.x]	= colorShot;
			const u2_t			depth				= u2_t(pixelCoord.z * 0xFFFFFFFFU);
			const double			brightRadiusSquared	= brightRadius * brightRadius;
			double					brightUnit			= 1.0 / brightRadiusSquared * brightness;
			for(s2_t y = (s2_t)-brightRadius, brightCount = (s2_t)brightRadius; y < brightCount; ++y)
			for(s2_t x = (s2_t)-brightRadius; x < brightCount; ++x) {
				::gpk::n2f2_t			brightPos			= {(float)x, (float)y};
				const double			brightDistance		= brightPos.LengthSquared();
				if(brightDistance <= brightRadiusSquared) {
					::gpk::n2s2_t			blendPos			= ::gpk::n2<s2_t>{(s2_t)pixelCoord.x, (s2_t)pixelCoord.y} + (brightPos).s2();
					if( blendPos.y < 0 || blendPos.y >= targetMetrics.y
					 || blendPos.x < 0 || blendPos.x >= targetMetrics.x
					)
						continue;
					u2_t				& blendVal			= depthBuffer[blendPos.y][blendPos.x];
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

gpk::err_t			ghg::getLightArraysFromDebris
	( const ::ghg::SDecoState								& decoState
	, ::gpk::a3f2_t			& lightPoints
	, ::gpk::a8bgra			& lightColors
	, const ::gpk::vc8bgra	& debrisColors
	)						{
	for(u2_t iParticle = 0; iParticle < decoState.Debris.Particles.Position.size(); ++iParticle) {
		lightPoints.push_back(decoState.Debris.Particles.Position[iParticle]);
		::gpk::rgbaf										colorShot			= debrisColors[iParticle % debrisColors.size()];
		lightColors.push_back(colorShot * decoState.Debris.Brightness[iParticle]);
	}
	return 0;
}


gpk::err_t			ghg::getLightArraysFromShips
	( const ::ghg::SShipManager	& shipState
	, ::gpk::a3f2_t				& lightPoints
	, ::gpk::a8bgra				& lightColors
	) {
	constexpr ::gpk::bgra		colorLightPlayer		= ::gpk::bgra{0xFF, 0x88, 0xFF};
	constexpr ::gpk::bgra		colorLightEnemy			= ::gpk::bgra{0xFF, 0x88, 0x88};
	for(u2_t iShip = 0; iShip < shipState.SpaceshipManager.ShipCores.size(); ++iShip) {
		lightPoints.push_back(shipState.GetShipPosition(iShip));
		const ::gpk::SSpaceshipCore	& ship					= shipState.SpaceshipManager.ShipCores[iShip];
		lightColors.push_back((0 == ship.Team) ? colorLightPlayer : colorLightEnemy);
		for(u2_t iPart = 0; iPart < shipState.SpaceshipManager.ShipParts[iShip].size(); ++iPart) {
			const ::gpk::SSpaceshipOrbiter									& shipPart				= shipState.SpaceshipManager.Orbiters[shipState.SpaceshipManager.ShipParts[iShip][iPart]];
			const ::gpk::rgbaf								colorShot
				= (::gpk::WEAPON_LOAD_Ray			== shipState.WeaponManager.Weapons[shipPart.Weapon].Shot.Type) ? ::gpk::rgbaf{1.0f, 0.1f, 0.0f}
				: (::gpk::WEAPON_LOAD_Cannonball	== shipState.WeaponManager.Weapons[shipPart.Weapon].Shot.Type) ? ship.Team ? ::gpk::rgbaf{1.0f, 0.125f, 0.25f} : ::gpk::TURQUOISE
				: (::gpk::WEAPON_LOAD_Bullet		== shipState.WeaponManager.Weapons[shipPart.Weapon].Shot.Type) ? ::gpk::GRAY
				: ::gpk::rgbaf{::gpk::bgra{0xFF, 0xFF, 0xFF}}
				;
			for(u2_t iShot = 0; iShot < shipState.WeaponManager.Shots[shipPart.Weapon].Particles.Position.size(); ++iShot) {
				lightPoints.push_back(shipState.WeaponManager.Shots[shipPart.Weapon].Particles.Position[iShot]);
				lightColors.push_back(colorShot);
			}
		}
	}
	return 0;
}

gpk::err_t			ghg::getLightArrays
	( const ::ghg::SShipManager	& shipState
	, const ::ghg::SDecoState	& decoState
	, ::gpk::a3f2_t				& lightPoints
	, ::gpk::a8bgra				& lightColors
	, const ::gpk::vc8bgra		& debrisColors
	) {
	::ghg::getLightArraysFromShips(shipState, lightPoints, lightColors);
	::ghg::getLightArraysFromDebris(decoState, lightPoints, lightColors, debrisColors);
	return 0;
}

gpk::err_t			ghg::getLightArrays
	( const ::gpk::n3f2_t		& modelPosition
	, const ::gpk::v3f2_t		& lightPointsWorld
	, const ::gpk::a8bgra		& lightColorsWorld
	, ::gpk::a3f2_t				& lightPointsModel
	, ::gpk::a8bgra				& lightColorsModel
	) {
	::gpk::clear(lightPointsModel, lightColorsModel);
	for(u2_t iLightPoint = 0; iLightPoint < lightPointsWorld.size(); ++iLightPoint) {
		const ::gpk::n3f2_t								& lightPoint		=	lightPointsWorld[iLightPoint];
		if((lightPoint - modelPosition).LengthSquared() < (MAX_LIGHT_RANGE * MAX_LIGHT_RANGE)) {
			lightPointsModel.push_back(lightPoint);
			lightColorsModel.push_back(lightColorsWorld[iLightPoint]);
		}
	}
	return 0;
}
//
//static	gpk::err_t						getLightArrays
//	( const ::gpk::n3f2_t							& modelPosition
//	, const ::gpk::apod<::gpk::n3f2_t>			& lightPointsWorld
//	, ::gpk::apod<u1_t>							& indicesPointLights
//	) {
//	indicesPointLights.clear();
//	for(u2_t iLightPoint = 0; iLightPoint < lightPointsWorld.size(); ++iLightPoint) {
//		const ::gpk::n3f2_t								& lightPoint		=	lightPointsWorld[iLightPoint];
//		if((lightPoint - modelPosition).LengthSquared() < (MAX_LIGHT_RANGE * MAX_LIGHT_RANGE))
//			indicesPointLights.push_back((u1_t)iLightPoint);
//	}
//	return 0;
//}

gpk::err_t			ghg::drawOrbiter
	( const ::ghg::SShipManager			& shipState
	, const u2_t					iPartEntity
	, const ::gpk::rgbaf				& shipColor
	, float								animationTime
	, const ::gpk::m4f2_t				& matrixVP
	, ::gpk::g8bgra						& targetPixels
	, ::gpk::gu2_t						depthBuffer
	, ::ghg::SGalaxyHellDrawCache		& drawCache
	) {
	u2_t												pixelsDrawn				= 0;
	const ::gpk::au2_t						& entityChildren		= shipState.EntitySystem.EntityChildren[iPartEntity];
	double													absanim					= fabsf(sinf(animationTime * 3));
	const ::gpk::rgbaf								shadedColor				= (absanim < .5) ? ::gpk::rgbaf{} : shipColor * (absanim * .5);
	for(u2_t iEntity = 0; iEntity < entityChildren.size(); ++iEntity) {
		const ::ghg::SGHEntity									& entityChild				= shipState.EntitySystem.Entities[entityChildren[iEntity]];
		if(-1 == entityChild.Parent)
			continue;
		if(-1 == entityChild.Geometry)
			continue;
		const ::gpk::m4f2_t							& matrixTransform				= shipState.Scene.Transforms[entityChild.Transform];
		::gpk::m4f2_t									matrixTransformVP			= matrixTransform * matrixVP;
		::ghg::getLightArrays(matrixTransform.GetTranslation(), drawCache.LightPointsWorld, drawCache.LightColorsWorld, drawCache.LightPointsModel, drawCache.LightColorsModel);
		const ::gpk::SGeometryQuads								& mesh						= shipState.Scene.Geometry[entityChild.Geometry];
		const ::gpk::gc8bgra					image						= shipState.Scene.Image	[entityChild.Image].View;
		for(u2_t iTriangle = 0; iTriangle < mesh.Triangles.size(); ++iTriangle) {
			::gpk::clear(drawCache.PixelCoords, drawCache.PixelVertexWeights);
			pixelsDrawn += ::gpk::drawQuadTriangle(targetPixels, mesh, iTriangle, matrixTransform, matrixTransformVP, shipState.Scene.Global.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel, depthBuffer
				, [shadedColor](::gpk::g8bgra targetPixels, const ::gpk::n2s1_t & pixelCoord, const ::gpk::bgra & color) {
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

static	gpk::err_t	drawShip
	( const ::ghg::SGalaxyHell		& solarSystem
	, s2_t						iShip
	, const ::gpk::m4f2_t			& matrixVP
	, ::gpk::g8bgra					& targetPixels
	, ::gpk::gu2_t					depthBuffer
	, ::ghg::SGalaxyHellDrawCache	& drawCache
	, const ::gpk::SRasterFont		& font
	) {
	const ::gpk::SSpaceshipCore				& shipCore			= solarSystem.ShipState.SpaceshipManager.ShipCores[iShip];

	u2_t							pixelsDrawn			= 0;
	const ::gpk::bgra					playerColor			= ((u2_t)iShip < solarSystem.PlayState.Constants.Players) ? ::gpk::bgra(solarSystem.Pilots[iShip].Color) : ::gpk::bgra(::gpk::RED);
	const ::gpk::vcu1_t					shipParts			= solarSystem.ShipState.SpaceshipManager.ShipParts[iShip];
	for(u2_t iPart = 0; iPart < shipParts.size(); ++iPart) {
		const ::gpk::SSpaceshipOrbiter				& shipPart			= solarSystem.ShipState.SpaceshipManager.Orbiters[shipParts[iPart]];
		if(shipPart.Health.Value <= 0)
			continue;
		pixelsDrawn += ::ghg::drawOrbiter(solarSystem.ShipState, solarSystem.ShipState.ShipPartEntity[shipParts[iPart]], playerColor, (float)solarSystem.DecoState.AnimationTime, matrixVP, targetPixels, depthBuffer, drawCache);
	}

	if(iShip >= (s2_t)solarSystem.PlayState.Constants.Players || shipCore.Team)
		return 0;

	const ::ghg::SGHEntity				& entity			= solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipCoreEntity[iShip]];
	const ::gpk::m4f2_t					& matrixTransform	= solarSystem.ShipState.Scene.Transforms[entity.Transform];
	::gpk::n3f2_t						starPos				= matrixTransform.GetTranslation() + ::gpk::n3f2_t{0.0f, 8.0f, 0.0f};
	starPos							= matrixVP.Transform(starPos);
	starPos.x = ::gpk::clamped(starPos.x, 20.0f, targetPixels.metrics().x - 20.0f);
	starPos.y = ::gpk::clamped(starPos.y, 20.0f, targetPixels.metrics().y - 20.0f);
	const ::gpk::n2<s2_t>			pixelCoord			= {(s2_t)starPos.x, (s2_t)starPos.y};

	const ::gpk::vcst_t					finalText			= solarSystem.Pilots[iShip].Name;
	::gpk::rect2s1_t						rectText			= {{}, {s1_t(font.CharSize.x * finalText.size()), font.CharSize.y}};
	rectText.Offset = (pixelCoord - ::gpk::n2s2_t{(rectText.Size.x >> 1), (rectText.Size.y >> 1)}).s1();

	::gpk::a2u1_t						dstCoords;
	if_fail_fe(::gpk::textLineRaster(targetPixels.metrics(), font.CharSize, rectText, font.Texture, finalText, dstCoords));
	for(u2_t iCoord = 0; iCoord < dstCoords.size(); ++iCoord) {
		const ::gpk::n2u1_t					dstCoord			= dstCoords[iCoord];
		if(::gpk::in_range(dstCoord, {{}, targetPixels.metrics().u1()})) {
			::gpk::setPixel(targetPixels, dstCoord.s1(), (shipCore.Health > 0) ? playerColor : ::gpk::bgra(::gpk::interpolate_linear(::gpk::rgbaf(playerColor), ::gpk::rgbaf(targetPixels[dstCoord.y][dstCoord.x]), .8f)));
		}
	}
	return pixelsDrawn;
}

static	gpk::err_t	drawExplosion
	( const ::ghg::SGalaxyHell		& solarSystem
	, const ::ghg::SExplosion		& explosion
	, const ::gpk::m4f2_t			& matrixView
	, ::gpk::g8bgra					& targetPixels
	, ::gpk::gu2_t					depthBuffer
	, ::ghg::SGalaxyHellDrawCache	& drawCache
	) {
	::gpk::gc8bgra						image				= solarSystem.ShipState.Scene.Image		[explosion.IndexImage].View;
	const ::gpk::SGeometryQuads			& mesh				= solarSystem.ShipState.Scene.Geometry	[explosion.IndexMesh];
	for(u2_t iExplosionPart = 0; iExplosionPart < explosion.Particles.Position.size(); ++iExplosionPart) {
		const ::gpk::rangeu1_t				& sliceMesh			= explosion.Slices[iExplosionPart];
	::gpk::m4f2_t						matrixPart			= {};
		matrixPart.Identity();
		if(iExplosionPart % 5) matrixPart.RotationY(solarSystem.DecoState.AnimationTime * 2);
		if(iExplosionPart % 3) matrixPart.RotationX(solarSystem.DecoState.AnimationTime * 2);
		if(iExplosionPart % 2) matrixPart.RotationZ(solarSystem.DecoState.AnimationTime * 2);
		matrixPart.SetTranslation(explosion.Particles.Position[iExplosionPart], false);
	::gpk::m4f2_t						matrixTransformView	= matrixPart * matrixView;
		::ghg::getLightArrays(matrixPart.GetTranslation(), drawCache.LightPointsWorld, drawCache.LightColorsWorld, drawCache.LightPointsModel, drawCache.LightColorsModel);
		for(u2_t iTriangle = 0, countTriangles = sliceMesh.Count; iTriangle < countTriangles; ++iTriangle) {
			drawCache.PixelCoords			.clear();
			drawCache.PixelVertexWeights	.clear();
			const u2_t						iActualTriangle		= sliceMesh.Offset + iTriangle;
			::gpk::tri3f2_t						triangleWorld		= mesh.Triangles	[iActualTriangle];
			::gpk::n3f2_t						normal				= mesh.Normals		[iActualTriangle / 2];
			::gpk::tri2f2_t						triangleTexCoords	= mesh.TextureCoords[iActualTriangle];
			::gpk::tri3f2_t						triangleScreen		= triangleWorld;
			::gpk::transform(triangleScreen, matrixTransformView);
			if(triangleScreen.ClipZ())
				continue;
			if(triangleScreen.A.z <= 0 || triangleScreen.B.z <= 0 || triangleScreen.C.z <= 0)
				continue;

			::gpk::transform(triangleWorld, matrixPart);
			normal							= matrixPart.TransformDirection(normal).Normalize();
			::gpk::drawQuadTriangle(targetPixels.metrics(), triangleScreen, drawCache.PixelCoords, drawCache.PixelVertexWeights, depthBuffer);
 			::gpk::drawPixels(targetPixels, triangleWorld, normal, triangleTexCoords, solarSystem.ShipState.Scene.Global.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel);
			drawCache.PixelCoords			.clear();
			drawCache.PixelVertexWeights	.clear();
			triangleScreen				= {triangleScreen.A, triangleScreen.C, triangleScreen.B};
			triangleWorld					= {triangleWorld.A, triangleWorld.C, triangleWorld.B};
			triangleTexCoords				= {triangleTexCoords.A, triangleTexCoords.C, triangleTexCoords.B};
			normal							*= -1;
			::gpk::drawQuadTriangle(targetPixels.metrics(), triangleScreen, drawCache.PixelCoords, drawCache.PixelVertexWeights, depthBuffer);
  			::gpk::drawPixels(targetPixels, triangleWorld, normal, triangleTexCoords, solarSystem.ShipState.Scene.Global.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel);
		}
	}
	return 0;
}

gpk::err_t			ghg::solarSystemDraw		(const ::ghg::SGalaxyHell & solarSystem, ::ghg::SGalaxyHellDrawCache & drawCache, ::std::mutex & mutexUpdate)	{
	drawCache.RenderTarget->resize(drawCache.RenderTargetMetrics);
	::gpk::pobj<::ghg::TRenderTarget>	renderTarget				= drawCache.RenderTarget;
	::gpk::g8bgra			targetPixels				= renderTarget->Color			; 
	::gpk::grid<u2_t>				depthBuffer					= renderTarget->DepthStencil	;

	// ------------------------------------------- Transform and Draw
	if(0 == targetPixels.size())
		return 1;

	memset(targetPixels.begin(), 0, targetPixels.byte_count());
	memset(depthBuffer.begin(), -1, depthBuffer.byte_count());
	{
		::std::lock_guard			lockUpdate					(mutexUpdate);
		::drawStars(solarSystem.DecoState.Stars, targetPixels);
	}

	::gpk::m4f2_t				matrixView					= {};
	const ::gpk::SCameraPoints	& camera					= solarSystem.ShipState.Scene.Global.Camera[solarSystem.ShipState.Scene.Global.CameraMode];
	::gpk::n3f2_t				cameraUp					= {0, 1, 0};
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
		for(u2_t iShip = 0; iShip < solarSystem.ShipState.SpaceshipManager.ShipCores.size(); ++iShip) {
			const ::gpk::SSpaceshipCore	& ship						= solarSystem.ShipState.SpaceshipManager.ShipCores[iShip];
			if(ship.Health <= 0 && ship.Team)
				continue;

			::drawShip(solarSystem, iShip, matrixView, targetPixels, depthBuffer, drawCache, *solarSystem.DecoState.FontManager.Fonts[10]);
		}
	}

	{
		::std::lock_guard		lockUpdate					(mutexUpdate);
		for(u2_t iExplosion = 0; iExplosion < solarSystem.DecoState.Explosions.size(); ++iExplosion) {
			const ::ghg::SExplosion	& explosion					= solarSystem.DecoState.Explosions[iExplosion];
			if(0 == explosion.Slices.size())
				continue;
			::drawExplosion(solarSystem, explosion, matrixView, targetPixels, depthBuffer, drawCache);
		}
	}
#pragma pack(push, 1)
	struct SRenderNode	{
		u2_t			Mesh				;
		s2_t				Image				;
		::gpk::sliceu2_t	Slice				;
		::gpk::au2_t		IndicesPointLight	;
	};
#pragma pack(pop)
	{
		::std::lock_guard		lockUpdate			(mutexUpdate);
		for(u2_t iShip = 0; iShip < solarSystem.ShipState.SpaceshipManager.ShipCores.size(); ++iShip) {
			const ::gpk::SSpaceshipCore	& shipCore			= solarSystem.ShipState.SpaceshipManager.ShipCores[iShip];
			const ::gpk::au1_t		& shipParts			= solarSystem.ShipState.SpaceshipManager.ShipParts[iShip];
			for(u2_t iPart = 0; iPart < shipParts.size(); ++iPart) {
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
