#include "gpk_image_color.h"
#include "gpk_array_ptr.h"

#ifndef GPK_GALAXY_HELL_DRAW_H_293874239874
#define GPK_GALAXY_HELL_DRAW_H_293874239874

namespace ghg
{

	typedef ::gpk::rtbgra8d32			TRenderTarget;

	static constexpr ::gpk::bgra		DEBRIS_COLORS[]			=
		{ {0x80, 0xAF, 0xFF, }
		, {0x40, 0x80, 0xFF, }
		, {0x20, 0x80, 0xFF, }
		, {0x00, 0x00, 0xFF, }
		};

	struct SGalaxyHellDrawCache {
		// Used when rastering triangles
		::gpk::apod<::gpk::n2i16>			PixelCoords				;
		::gpk::apod<::gpk::trif32>			PixelVertexWeights		;
		
		// Used for pixel shading
		::gpk::apod<::gpk::n3f32>			LightPointsWorld		;
		::gpk::a8bgra						LightColorsWorld		;
		::gpk::apod<::gpk::n3f32>			LightPointsModel		;
		::gpk::a8bgra						LightColorsModel		;

		// Gameplay render target. I decided the world will have its own rendertarget to facilitate reuse of the code.
		::gpk::n2u16						RenderTargetMetrics		= {1280, 720};
		::gpk::pobj<::ghg::TRenderTarget>	RenderTarget			= {};

		::gpk::error_t						Clear					() {
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