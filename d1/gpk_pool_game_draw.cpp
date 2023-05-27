#include "gpk_pool_game.h"
#include "gpk_engine_shader.h"

#include "gpk_timer.h"

::gpk::error_t								transformFragments				()	{ return 0; }

::gpk::error_t								d1p::poolGameDraw				
	( ::d1p::SPoolGame							& pool
	, ::gpk::rt<::gpk::bgra, uint32_t>			& backBuffer
	, const ::gpk::n3f							& cameraPosition
	, const ::gpk::n3f							& cameraTarget
	, const ::gpk::n3f							& cameraUp
	, double									totalSeconds
	) {

	const ::gpk::n2<uint16_t>						offscreenMetrics				= backBuffer.Color.View.metrics().Cast<uint16_t>();

	::gpk::n3f										cameraFront						= (cameraTarget - cameraPosition).Normalized();

	::gpk::SEngineSceneConstants					constants						= {};
	constants.CameraPosition					= cameraPosition;
	constants.CameraFront						= cameraFront;
	constants.LightPosition						= {0, 10, 0};
	constants.LightDirection					= {0, -1, 0};
	::gpk::SMinMax									nearFar 						= {.0001f, 10.0f}; 

	constants.View.LookAt(cameraPosition, cameraTarget, cameraUp);
	constants.Perspective.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar.Min, nearFar.Max);
	constants.Screen.ViewportLH(offscreenMetrics);
	constants.VP								= constants.View * constants.Perspective;
	constants.VPS								= constants.VP * constants.Screen;

	::gpk::STimer									timer;
	::gpk::SEngine									& engine						= pool.Engine;

	::gpk::apod<::gpk::n3f>							& wireframePixelCoords			= engine.Scene->RenderCache.VertexShaderCache.WireframePixelCoords;
	for(uint32_t iBall = 0; iBall < pool.MatchState.CountBalls; ++iBall) {
		for(uint32_t iDelta = ::gpk::max(0, (int32_t)pool.PositionDeltas[iBall].size() - 20); iDelta < pool.PositionDeltas[iBall].size(); ++iDelta) {
			::gpk::line3<float>							screenDelta				= pool.PositionDeltas[iBall][iDelta];
			wireframePixelCoords.clear();
			screenDelta.A.y = screenDelta.B.y = 0;
			::gpk::drawLine(offscreenMetrics, screenDelta, constants.VPS, wireframePixelCoords, backBuffer.DepthStencil);
			for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
				::gpk::n3<int16_t>								coord					= wireframePixelCoords[iCoord].Cast<int16_t>();
				backBuffer.Color.View[coord.y][coord.x]		= pool.BallColors[iBall];
			}
		}
	}

	const gpk::n2<float>							halfDimensions					= pool.MatchState.Table.Dimensions.Slate * .5;

	wireframePixelCoords.clear();
	const	::gpk::n3f								limitsBottom	[4]				=
		{ { halfDimensions.x, .0f,  halfDimensions.y}
		, {-halfDimensions.x, .0f,  halfDimensions.y}
		, { halfDimensions.x, .0f, -halfDimensions.y}
		, {-halfDimensions.x, .0f, -halfDimensions.y}
		};
	const	::gpk::n3f								limitsTop		[4]				= 
		{ limitsBottom[0] + ::gpk::n3f{0, pool.MatchState.Table.Dimensions.Height, 0}
		, limitsBottom[1] + ::gpk::n3f{0, pool.MatchState.Table.Dimensions.Height, 0}
		, limitsBottom[2] + ::gpk::n3f{0, pool.MatchState.Table.Dimensions.Height, 0}
		, limitsBottom[3] + ::gpk::n3f{0, pool.MatchState.Table.Dimensions.Height, 0}
		};

	::gpk::drawLine(offscreenMetrics, ::gpk::line3<float>{limitsBottom[3], limitsBottom[2]}, constants.VPS, wireframePixelCoords, backBuffer.DepthStencil);
	::gpk::drawLine(offscreenMetrics, ::gpk::line3<float>{limitsBottom[3], limitsBottom[1]}, constants.VPS, wireframePixelCoords, backBuffer.DepthStencil);
	::gpk::drawLine(offscreenMetrics, ::gpk::line3<float>{limitsBottom[0], limitsBottom[1]}, constants.VPS, wireframePixelCoords, backBuffer.DepthStencil);
	::gpk::drawLine(offscreenMetrics, ::gpk::line3<float>{limitsBottom[0], limitsBottom[2]}, constants.VPS, wireframePixelCoords, backBuffer.DepthStencil);
	::gpk::drawLine(offscreenMetrics, ::gpk::line3<float>{limitsTop[3], limitsTop[2]}, constants.VPS, wireframePixelCoords, backBuffer.DepthStencil);
	::gpk::drawLine(offscreenMetrics, ::gpk::line3<float>{limitsTop[3], limitsTop[1]}, constants.VPS, wireframePixelCoords, backBuffer.DepthStencil);
	::gpk::drawLine(offscreenMetrics, ::gpk::line3<float>{limitsTop[0], limitsTop[1]}, constants.VPS, wireframePixelCoords, backBuffer.DepthStencil);
	::gpk::drawLine(offscreenMetrics, ::gpk::line3<float>{limitsTop[0], limitsTop[2]}, constants.VPS, wireframePixelCoords, backBuffer.DepthStencil);
	::gpk::drawLine(offscreenMetrics, ::gpk::line3<float>{limitsBottom[0], limitsTop[0]}, constants.VPS, wireframePixelCoords, backBuffer.DepthStencil);
	::gpk::drawLine(offscreenMetrics, ::gpk::line3<float>{limitsBottom[1], limitsTop[1]}, constants.VPS, wireframePixelCoords, backBuffer.DepthStencil);
	::gpk::drawLine(offscreenMetrics, ::gpk::line3<float>{limitsBottom[2], limitsTop[2]}, constants.VPS, wireframePixelCoords, backBuffer.DepthStencil);
	::gpk::drawLine(offscreenMetrics, ::gpk::line3<float>{limitsBottom[3], limitsTop[3]}, constants.VPS, wireframePixelCoords, backBuffer.DepthStencil);

	for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
		::gpk::n3i16										coord		= wireframePixelCoords[iCoord].Cast<int16_t>();
		::gpk::rgbaf										color		= 
			{ (float)(totalSeconds - iCoord / 1.0f / totalSeconds)
			, (float)(totalSeconds - iCoord / 2.0f / totalSeconds)
			, (float)(totalSeconds - iCoord / 3.0f / totalSeconds)
			};
		backBuffer.Color.View[coord.y][coord.x]		= color;
	}

	::gpk::drawScene(backBuffer.Color.View, backBuffer.DepthStencil.View, engine.Scene->RenderCache, *engine.Scene, constants);
	timer.Frame();
	info_printf("Render scene in %f seconds", timer.LastTimeSeconds);
	return 0;
}