#include "gpk_galaxy_hell.h"

#include "gpk_gui.h"

#include "gpk_array_static.h"

#ifndef GALAXY_HELL_DISPLAY_H
#define GALAXY_HELL_DISPLAY_H

namespace ghg 
{
	struct STextOverlay {
		::gpk::SCoord3<float>								LightVector0			= {-15, 50, -15};

		::gpk::SCoord3<float>								CameraTarget			= {0, 0, 0};
		::gpk::SCoord3<float>								CameraPosition			= {.0001f, 650.1f, -0.00001f};
		::gpk::SCoord3<float>								CameraUp				= {0, 0, 1};
		::gpk::array_static<::gpk::SGeometryQuads, 256>		GeometryLetters			= {};

		::gpk::array_static<uint8_t, 32>					MapToDraw[5]			=
			{ {0xDA, 0xC4,  0xC4,  0xC4	,  0xC4	,  0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xBF}
			, {0xB3, '\0', '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	,  '\0',  '\0',  '\0'	, '\0'	, '\0'	, '\0'	,  '\0'	,  '\0'	,  '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, 0xB3}
			, {0xC3, 0xC4,  0xC4,  0xC4	,  0xC4	,  0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xB4}
			, {0xB3,  'T',  'i'	,  'm'	,  'e'	,  ':'	,  '0'	,  '0'	,  ':'	,  '0'	,  '0'	, ':'	, '0'	, '0'	, '.'	, '0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0',  '\0'	, '\0'	, '\0'	, '\0'	, 0xB3}
			, {0xC0, 0xC4,  0xC4,  0xC4	,  0xC4	,  0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xD9}
			};

		static constexpr	const ::gpk::SCoord3<float>		ControlTranslation		= {0, 0, 20.0f};
		static constexpr	const ::gpk::SCoord2<uint32_t>	MetricsLetter			= {12, 12};
		static constexpr	const ::gpk::SCoord2<uint32_t>	MetricsMap				= {16, 16};
	};

	::gpk::error_t		overlaySetup			(::ghg::STextOverlay & textOverlay);
	::gpk::error_t		overlayUpdate			(::ghg::STextOverlay & textOverlay, uint32_t stage, uint32_t score, double playSeconds);
	::gpk::error_t		overlayDraw				(const ::ghg::STextOverlay & textOverlay, ::ghg::SGalaxyHellDrawCache & drawCache, double playSeconds, ::gpk::view2d_uint32 & depthBuffer, ::gpk::view_grid<::gpk::SColorBGRA> targetPixels);

	struct SUIControlGauge {
		::gpk::array_pod<::gpk::SCoord3<float>>				Vertices				= {};
		::gpk::array_pod<::gpk::STriangleWeights<int16_t>>	Indices					= {};
		int16_t												CurrentValue			= 0;
		int16_t												MaxValue				= 64;
		::gpk::SImage<::gpk::SColorBGRA>					Image					= {};

		inline	int16_t										SetValue				(float weight)							noexcept	{ return CurrentValue = int16_t(weight * MaxValue); }
				int16_t										SetValue				(float currentValue, uint32_t maxValue)	noexcept	{ return SetValue(currentValue / maxValue); }
	};

			::gpk::error_t	gaugeBuildRadial		(::ghg::SUIControlGauge & gauge, const ::gpk::SCircle<float> & gaugeMetrics, int16_t resolution, int16_t width);
			::gpk::error_t	gaugeImageUpdate		(::ghg::SUIControlGauge & gauge, ::gpk::view_grid<::gpk::SColorBGRA> target, ::gpk::SColorBGRA colorMin, ::gpk::SColorBGRA colorMid, ::gpk::SColorBGRA colorMax);
	inline	::gpk::error_t	gaugeImageUpdate		(::ghg::SUIControlGauge & gauge, ::gpk::SColorBGRA colorMin = ::gpk::GREEN, ::gpk::SColorBGRA colorMid = ::gpk::ORANGE, ::gpk::SColorBGRA colorMax = ::gpk::LIGHTRED) {
		gauge.Image.Texels.fill({0, 0, 0, 0});
		return ::ghg::gaugeImageUpdate(gauge, gauge.Image, colorMin, colorMid, colorMax);
	}

} // namespace

#endif // GALAXY_HELL_DISPLAY_H