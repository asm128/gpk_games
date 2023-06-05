#include "gpk_image.h"
#include "gpk_color.h"
#include "gpk_ptr.h"

#ifndef GPK_GALAXY_HELL_DRAW_H_293874239874
#define GPK_GALAXY_HELL_DRAW_H_293874239874

namespace ghg
{

	typedef ::gpk::rt<::gpk::bgra, uint32_t>	TRenderTarget;

	static constexpr ::gpk::bgra				DEBRIS_COLORS[]			=
		{ {0x80, 0xAF, 0xFF, }
		, {0x40, 0x80, 0xFF, }
		, {0x20, 0x80, 0xFF, }
		, {0x00, 0x00, 0xFF, }
		};

	struct SGalaxyHellDrawCache {
		// Used when rastering triangles
		::gpk::apod<::gpk::n2<int16_t>>			PixelCoords				;
		::gpk::apod<::gpk::tri<float>>			PixelVertexWeights		;
		
		// Used for pixel shading
		::gpk::apod<::gpk::n3f32>				LightPointsWorld		;
		::gpk::apod<::gpk::bgra>					LightColorsWorld		;
		::gpk::apod<::gpk::n3f32>				LightPointsModel		;
		::gpk::apod<::gpk::bgra>					LightColorsModel		;

		// Gameplay render target. I decided the world will have its own rendertarget to facilitate reuse of the code.
		::gpk::n2<uint16_t>						RenderTargetMetrics		= {1280, 720};
		::gpk::pobj<::ghg::TRenderTarget>				RenderTarget			= {};

		::gpk::error_t										Clear					() {
			::gpk::clear
				( PixelCoords
				, PixelVertexWeights		
				, LightPointsWorld		
				, LightColorsWorld		
				, LightPointsModel		
				, LightColorsModel				
				);
			return 0;
		}
	};
} // namespace 

#endif // GPK_GALAXY_HELL_DRAW_H_293874239874