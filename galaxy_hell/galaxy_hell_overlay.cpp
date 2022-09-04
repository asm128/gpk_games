#include "gpk_galaxy_hell_overlay.h"

#include "gpk_galaxy_hell.h"

#include "gpk_raster_lh.h"
#include "gpk_png.h"


static	int											drawPixels
	( ::gpk::view_grid<::gpk::SColorBGRA>				targetPixels
	, const ::gpk::STriangle3	<float>					& triangleWorld
	, const ::gpk::SCoord3		<float>					& normal
	, const ::gpk::SCoord3		<float>					& lightVector
	, const ::gpk::SColorFloat							& texelColor
	, ::gpk::array_pod<::gpk::SCoord2<int16_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<float>>	& pixelVertexWeights
	, double											timeAnimation
	) {
	double													lightFactorDirectional		= normal.Dot(lightVector);
	(void)lightFactorDirectional;
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::gpk::SCoord2<int16_t>									pixelCoord					= pixelCoords		[iPixelCoord];
		const ::gpk::STriangleWeights<float>					& vertexWeights				= pixelVertexWeights[iPixelCoord];
		const ::gpk::SCoord3<float>								position					= ::gpk::triangleWeight(vertexWeights, triangleWorld);
		double													factorWave					= (::gpk::max(0.0, sin(- timeAnimation * 4 + position.y * .75))) * .6;
		double													factorWave2					= (::gpk::max(0.0, sin(- timeAnimation + position.x * .0125 + position.z * .125))) * .5;
		::gpk::setPixel(targetPixels, pixelCoord, targetPixels[pixelCoord.y][pixelCoord.x] * .25 + ((texelColor * (lightFactorDirectional * 2) + texelColor * factorWave + texelColor * factorWave2)).Clamp());
	}
	return 0;
}

int													draw3DCharacter
	( const ::gpk::view_grid<::gpk::SColorBGRA>				& targetPixels
	, const ::gpk::SCoord2<uint32_t>						& metricsCharacter
	, const ::gpk::SCoord2<uint32_t>						& metricsMap
	, const uint8_t											asciiCode
	, const ::gpk::SCoord3<float>							& position
	, const ::gpk::SCoord3<float>							& lightVector
	, const ::gpk::SMatrix4<float>							& matrixView
	, const ::gpk::view_grid<const ::gpk::SGeometryQuads>	& viewGeometries
	, ::ghg::SGalaxyHellDrawCache							& drawCache
	, ::gpk::view_grid<uint32_t>							& depthBuffer
	, double												timeAnimation
	)	{
	::gpk::SMatrix4<float>									matrixScale				;
	::gpk::SMatrix4<float>									matrixPosition			;
	::gpk::SCoord3<float>									translation				= {};
	translation.x										= float(position.x * metricsCharacter.x);
	translation.z										= float(position.z * metricsCharacter.y);
	matrixPosition	.SetTranslation	({translation.x, 0, translation.z}, true);
	matrixScale		.Scale			({1, 1, 1}, true);
	::gpk::SMatrix4<float>									matrixTransform										= matrixScale * matrixPosition;
	::gpk::SMatrix4<float>									matrixTransformView									= matrixTransform * matrixView;
	const ::gpk::SCoord2<uint32_t>							asciiCoords				= {asciiCode % metricsMap.x, asciiCode / metricsMap.x};
	const ::gpk::SGeometryQuads								& geometry				= viewGeometries[asciiCoords.y][asciiCoords.x];
	::gpk::SColorFloat										color					= ::gpk::PANOCHE;//::gpk::COLOR_TABLE[((int)timeAnimation) % ::gpk::size(::gpk::COLOR_TABLE)];
	for(uint32_t iTriangle = 0; iTriangle < geometry.Triangles.size(); ++iTriangle) {
		drawCache.PixelCoords			.clear();
		drawCache.PixelVertexWeights	.clear();
		::gpk::STriangle3		<float>							triangle			= geometry.Triangles	[iTriangle];;
		const ::gpk::SCoord3	<float>							& normal			= geometry.Normals		[iTriangle / 2];
		::gpk::drawQuadTriangle(targetPixels.metrics(), triangle, matrixTransformView, drawCache.PixelCoords, drawCache.PixelVertexWeights, depthBuffer);
		::gpk::SCoord3	<float>									xnormal				= matrixTransform.TransformDirection(normal).Normalize();
		::gpk::STriangle3		<float>							triangleWorld		= triangle;
		::gpk::transform(triangleWorld, matrixTransform);
		::drawPixels(targetPixels, triangleWorld, xnormal, lightVector, color * .75, drawCache.PixelCoords, drawCache.PixelVertexWeights, timeAnimation);
	}
	return 0;
}

int													ghg::overlaySetup	(::ghg::STextOverlay & textOverlay) {
	const ::gpk::SCoord2<uint32_t>							metricsMap			= textOverlay.MetricsMap;
	const ::gpk::SCoord2<uint32_t>							metricsLetter		= textOverlay.MetricsLetter;
	//::gpk::SImage<::gpk::SColorBGRA>						fontImagePre;
	//::gpk::pngFileLoad(::gpk::view_const_string{"../gpk_data/images/dos_8x8_font_white.png"}, fontImagePre);
	::gpk::SImage<::gpk::SColorBGRA>						fontImage;
	gpk_necall(::gpk::pngFileLoad(::gpk::view_const_string{"./codepage_437_24_12x12.png"}, fontImage), "%s", "");
	//fontImage.resize(::gpk::SCoord2<uint32_t>{textOverlay.MetricsMap}.InPlaceScale(textOverlay.MetricsLetter));
	//for(uint32_t y = 0; y < metricsMap.y; ++y)
	//for(uint32_t x = 0; x < metricsMap.x; ++x) {
	//	::gpk::SRectangle2<uint32_t>											dstRect				= {{x * metricsLetter.x, y * metricsLetter.y}, metricsLetter};
	//	::gpk::SRectangle2<uint32_t>											srcRect				= 
	//		{ {x * metricsLetter.x + 1 + x, y * metricsLetter.y + 1 + y}
	//		, metricsLetter
	//		};
	//	//::gpk::grid_copy_ex(fontImage.View, fontImagePre.View, metricsLetter.Cast<int32_t>(), dstRect.Offset.Cast<int32_t>(), srcRect.Offset.Cast<int32_t>());
	//	::gpk::grid_copy(fontImage.View, fontImagePre.View, dstRect.Offset, srcRect);
	//}
	::gpk::view_grid<::gpk::SGeometryQuads>					viewGeometries		= {textOverlay.GeometryLetters.begin(), {16, 16}};
	const uint32_t											imagePitch			= metricsLetter.x * metricsMap.x;

	::gpk::array_pod<::gpk::STile>							tiles;
	for(uint32_t y = 0; y < metricsMap.y; ++y)
	for(uint32_t x = 0; x < metricsMap.x; ++x) {
		tiles.clear();
		const uint32_t											asciiCode			= y * textOverlay.MetricsMap.x + x;
		const ::gpk::SCoord2<uint32_t>							asciiCoords			= {asciiCode %		metricsMap.x, asciiCode / textOverlay.MetricsMap.x};
		const uint32_t											offsetPixelCoord	= (asciiCoords.y *	metricsLetter.y) * imagePitch + (asciiCoords.x * textOverlay.MetricsLetter.x);
		::gpk::geometryBuildTileListFromImage({&fontImage.Texels[offsetPixelCoord], textOverlay.MetricsLetter}, tiles, textOverlay.MetricsLetter.x * textOverlay.MetricsMap.x);
		::gpk::geometryBuildGridFromTileList(textOverlay.GeometryLetters[asciiCode], ::gpk::view_grid<::gpk::STile>{tiles.begin(), textOverlay.MetricsLetter}, {}, {1, 12.0f, 1});
	}
	return 0;
}

int													ghg::overlayUpdate	(::ghg::STextOverlay & textOverlay, uint32_t stage, uint32_t score, double playSeconds) {
	::gpk::view_grid<uint8_t>								mapToDraw			= {&textOverlay.MapToDraw[0][0], {32, 5}};
	uint32_t												timeHours			= (int)playSeconds / 3600;
	uint32_t												timeMinutes			= (int)playSeconds / 60 % 60;
	uint32_t												timeSeconds			= (int)playSeconds % 60;
	uint32_t												timeCents			= int (playSeconds * 10) % 10;

	char													strStage		[17]	= {};
	char													strScore		[17]	= {};
	char													strTimeHours	[3]	= {};
	char													strTimeMinutes	[3]	= {};
	char													strTimeSeconds	[3]	= {};
	char													strTimeCents	[2]	= {};
	memset(&mapToDraw[1][1], 0, mapToDraw.metrics().x - 2);
	sprintf_s(strStage		, "Stage:%u", (uint32_t)(stage)); memcpy(&mapToDraw[1][1], strStage, strlen(strStage));
	sprintf_s(strScore		, "Score:%u", (uint32_t)(score)); memcpy(&mapToDraw[1][mapToDraw.metrics().x - (uint32_t)strlen(strScore) - 1], strScore, strlen(strScore));
	sprintf_s(strTimeHours	, "%.2u" , timeHours	); memcpy(&mapToDraw[3][ 6], strTimeHours	, 2);
	sprintf_s(strTimeMinutes, "%.2u" , timeMinutes	); memcpy(&mapToDraw[3][ 9], strTimeMinutes	, 2);
	sprintf_s(strTimeSeconds, "%.2u" , timeSeconds	); memcpy(&mapToDraw[3][12], strTimeSeconds	, 2);
	sprintf_s(strTimeCents	, "%.1u" , timeCents	); memcpy(&mapToDraw[3][15], strTimeCents	, 1);

	textOverlay.LightVector0.Normalize();
	return 0;
}

int													ghg::overlayDraw	(const ::ghg::STextOverlay & textOverlay, ::ghg::SGalaxyHellDrawCache & drawCache, double playSeconds, ::gpk::view2d_uint32 & depthBuffer, ::gpk::view_grid<::gpk::SColorBGRA> targetPixels) {
	drawCache.Clear();

	const ::gpk::view_grid<const uint8_t>					mapToDraw			= {&textOverlay.MapToDraw[0][0], {textOverlay.MapToDraw[0].size(), 5}};
	::gpk::view_grid<const ::gpk::SGeometryQuads>			viewGeometries		= {textOverlay.GeometryLetters.begin(), {16, 16}};
	uint32_t												colorIndex			= 0;
	::gpk::SCoord3<float>									offset				= textOverlay.ControlTranslation;
	offset												-= ::gpk::SCoord3<float>{mapToDraw.metrics().x * .5f, 0, mapToDraw.metrics().y * .5f * -1.f};

	memset(depthBuffer.begin(), -1, sizeof(uint32_t) * depthBuffer.size());
	::gpk::SMatrix4<float>									matrixView					= {};
	::gpk::SMatrix4<float>									matrixProjection			= {};
	::gpk::SMatrix4<float>									matrixViewport				= {};
	matrixView.Identity();
	matrixProjection.Identity();
	matrixViewport.Identity();
	matrixView.LookAt(textOverlay.CameraPosition, textOverlay.CameraTarget, textOverlay.CameraUp);
	matrixProjection.FieldOfView(::gpk::math_pi * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.01, 1000);
	matrixViewport.ViewportLH(targetPixels.metrics());
	matrixView											*= matrixProjection;
	matrixView											*= matrixViewport;

	{
		for(uint32_t y = 0; y < mapToDraw.metrics().y; ++y)
		for(uint32_t x = 0; x < mapToDraw.metrics().x; ++x) {
			const uint8_t											asciiCode			= mapToDraw[y][x];
			if(0 == asciiCode)
				continue;
			::gpk::SCoord3<float>									position			= offset;
			position.x											+= x;
			position.z											-= y;
			draw3DCharacter(targetPixels, textOverlay.MetricsLetter, textOverlay.MetricsMap, asciiCode, position, textOverlay.LightVector0, matrixView, viewGeometries, drawCache, depthBuffer, playSeconds);
			++colorIndex;
		}
	}
	return 0;
}


::gpk::error_t					ghg::gaugeBuildRadial			(::ghg::SUIControlGauge & gauge, const ::gpk::SCircle<float> & gaugeMetrics, int16_t resolution, int16_t width) {
	const double						stepUnit						= (1.0 / resolution) * ::gpk::math_2pi;
	const ::gpk::SSphere<float>			sphereMetrics					= {gaugeMetrics.Radius, {gaugeMetrics.Center.x, gaugeMetrics.Center.y, .5f}};
	for(int16_t iStep = 0, stepCount = resolution; iStep < stepCount; ++iStep) {
		::gpk::SPairSinCos					sinCos							= {sin(iStep * stepUnit), -cos(iStep * stepUnit)};
		const double						finalRadius						= gaugeMetrics.Radius; //::gpk::interpolate_linear(gaugeMetrics.Radius, gaugeMetrics.Radius * .5, ::gpk::clamp(abs(sinCos.Cos), 0.0, 1.0)); //
 		const ::gpk::SCoord3<double>		relativePosSmall				=
			{ sinCos.Sin * (finalRadius - width)
			, sinCos.Cos * (finalRadius - width)
			};
		const ::gpk::SCoord3<double>		relativePos				=
			{ sinCos.Sin * finalRadius
	  		, sinCos.Cos * finalRadius
			};
		gauge.Vertices.push_back(sphereMetrics.Center + relativePosSmall.Cast<float>());
		gauge.Vertices.push_back(sphereMetrics.Center + relativePos.Cast<float>());
	}
	for(int16_t iStep = 0, stepCount = (int16_t)(resolution * 2) - 3; iStep < stepCount; ++iStep) {
		gauge.Indices.push_back({int16_t(0 + iStep), int16_t(1 + iStep), int16_t(2 + iStep)});
		gauge.Indices.push_back({int16_t(1 + iStep), int16_t(3 + iStep), int16_t(2 + iStep)});
	}
	const int16_t						iStep							= int16_t(gauge.Vertices.size() - 2);
	gauge.Indices.push_back({int16_t(0 + iStep), int16_t(1 + iStep), 0});
	gauge.Indices.push_back({int16_t(1 + iStep), 1, 0});
	gauge.MaxValue					= resolution;
	return 0;
}


::gpk::error_t					ghg::gaugeImageUpdate			(::ghg::SUIControlGauge & gauge, ::gpk::view_grid<::gpk::SColorBGRA> target, ::gpk::SColorBGRA colorMin, ::gpk::SColorBGRA colorMid, ::gpk::SColorBGRA colorMax)  {
	static ::gpk::SImage<uint32_t>		dummyDepth;
	const ::gpk::SCoord3<float>			center3							= (gauge.Vertices[1] - gauge.Vertices[gauge.Vertices.size() / 2 + 1]) / 2 + gauge.Vertices[gauge.Vertices.size() / 2 + 1];
	const ::gpk::SCoord2<float>			center2							= {center3.x, center3.y};
	const double						radiusLarge						= (center3 - gauge.Vertices[1]).Length();
	const double						radiusSmall						= (center3 - gauge.Vertices[0]).Length();
	const double						radiusCenter					= (radiusLarge - radiusSmall) / 2 + radiusSmall;
	dummyDepth.resize(target.metrics(), 0xFFFFFFFFU);
	::gpk::array_pod<::gpk::SCoord2<int16_t>>			pixelCoords;
	::gpk::array_pod<::gpk::STriangleWeights<float>>	triangleWeights;
	for(uint32_t iTriangle = 0, triangleCount = gauge.Indices.size(); iTriangle < triangleCount; ++iTriangle) {
		const ::gpk::STriangle3<float>		triangleCoords					=
			{ gauge.Vertices[gauge.Indices[iTriangle].A]
			, gauge.Vertices[gauge.Indices[iTriangle].B]
			, gauge.Vertices[gauge.Indices[iTriangle].C]
			};
		const double						colorFactor						= ::gpk::min(1.0, iTriangle / (double)triangleCount);
		::gpk::SColorBGRA					finalColor;
		const bool							isEmptyGauge					= ((iTriangle + 2) >> 1) >= (uint32_t)gauge.CurrentValue * 2;
		finalColor						= isEmptyGauge
			? ::gpk::SColorBGRA{::gpk::BROWN} + ::gpk::SColorBGRA{::gpk::DARKBLUE * .5}
			: (colorFactor < .5)
				? ::gpk::interpolate_linear(colorMin, colorMid, (::gpk::min(1.0, iTriangle * 2.25 / (double)triangleCount)))
				: ::gpk::interpolate_linear(colorMid, colorMax, (colorFactor - .5) * 2)
				;
		pixelCoords.clear();
		::gpk::drawTriangle(target.metrics(), triangleCoords, pixelCoords, triangleWeights, dummyDepth);
		for(uint32_t iPixelCoords = 0; iPixelCoords < pixelCoords.size(); ++iPixelCoords) {
			const ::gpk::SCoord2<int16_t>		pixelCoord						= pixelCoords[iPixelCoords];
//#define GAUGE_NO_SHADING
#ifndef GAUGE_NO_SHADING
			const ::gpk::SCoord2<float>			floatCoord						= pixelCoord.Cast<float>();
			const double						distanceFromCenter				= (floatCoord - center2).Length();
			const double						distanceFromRadiusCenter		= fabs(distanceFromCenter - radiusCenter) / ((radiusLarge - radiusSmall) / 2);
			finalColor.a					= (uint8_t)::gpk::clamp((1 - distanceFromRadiusCenter) * 255, 0.0, 255.0);
#endif
			target[pixelCoord.y][pixelCoord.x] = finalColor;
		}
	}
	return 0;
}
