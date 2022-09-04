#include "gpk_galaxy_hell_app.h"
#include "gpk_grid_copy.h"
#include <gpk_raster_lh.h>

::gpk::error_t					ghg::galaxyHellUpdate			(::ghg::SGalaxyHellApp & app, double lastTimeSeconds, const ::gpk::SInput & inputState, const ::gpk::view_array<::gpk::SSysEvent> & systemEvents, const ::gpk::SCoord2<uint16_t> & windowSize) {
	if(app.ActiveState == ::ghg::APP_STATE_Quit)
		return 1;

	for(uint32_t iEvent = 0; iEvent < systemEvents.size(); ++iEvent) {
		switch(systemEvents[iEvent].Type) {
		case gpk::SYSEVENT_RESIZE: {
			app.World.DrawCache.RenderTargetMetrics = *(const ::gpk::SCoord2<uint16_t>*)systemEvents[iEvent].Data.begin();

			if(app.World.DrawCache.RenderTargetMetrics.x > 1280) app.World.DrawCache.RenderTargetMetrics.x = 1600;
			if(app.World.DrawCache.RenderTargetMetrics.y >  720) app.World.DrawCache.RenderTargetMetrics.y = 900;

			double								currentRatio					= app.World.DrawCache.RenderTargetMetrics.y / (double)app.World.DrawCache.RenderTargetMetrics.x;
			double								targetRatioY					=  9 / 16.0;
			if(currentRatio >= targetRatioY)
				app.World.DrawCache.RenderTargetMetrics.y = (uint16_t)(app.World.DrawCache.RenderTargetMetrics.x * targetRatioY + .1f);
			else 
				app.World.DrawCache.RenderTargetMetrics.x = (uint16_t)(app.World.DrawCache.RenderTargetMetrics.y / targetRatioY + .1f);
			}
		}
	}
	switch(app.ActiveState) {
	case APP_STATE_Load		: 
		::ghg::solarSystemSetup(app.World, app.World.DrawCache.RenderTargetMetrics);
		::ghg::overlaySetup(app.Overlay);
		::ghg::guiSetup(app.UI);
		app.ActiveState = APP_STATE_Home;
		break;
	case  APP_STATE_Play	: 
		break;
	}	 
	::ghg::solarSystemUpdate(app.World	, (app.ActiveState != ::ghg::APP_STATE_Play) ? 0.000001 : lastTimeSeconds, systemEvents);
	::ghg::overlayUpdate	(app.Overlay, app.World.PlayState.Stage, app.World.Score, app.World.PlayState.TimePlayed);
	app.ActiveState = (::ghg::APP_STATE)::ghg::guiUpdate(app.UI, app.World, app.ActiveState, inputState, systemEvents, windowSize.Cast<uint16_t>());
	return 0;
}

::gpk::error_t					ghg::galaxyHellDraw				(::ghg::SGalaxyHellApp & app, ::gpk::SCoord2<uint16_t> renderTargetSize) {
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	target			= app.RenderTarget[0];
	target->resize(renderTargetSize.Cast<uint32_t>(), {}, 0xFFFFFFFFU);
	::gpk::view_grid<::gpk::SColorBGRA>							targetPixels			= target->Color.View;
	::gpk::view_grid<uint32_t>									depthBuffer				= target->DepthStencil.View;
	switch(app.ActiveState) {
	default					: 
	case APP_STATE_Play		: 
		::ghg::solarSystemDraw(app.World, app.World.DrawCache, app.World.LockUpdate);
		::gpk::ptr_obj<::ghg::TRenderTarget>		sourceRT	= {};
		{
			::std::lock_guard<::std::mutex>				lockRTQueue	(app.World.DrawCache.RenderTargetQueueMutex);
			sourceRT								= app.World.DrawCache.RenderTargetQueue;
			app.World.DrawCache.RenderTargetQueue	= {};
		}

		::gpk::view_grid<::gpk::SColorBGRA>			cameraView			= sourceRT->Color.View;
		::gpk::SCoord2<int16_t>						cameraViewMetrics	= cameraView.metrics().Cast<int16_t>();
		::gpk::grid_copy(targetPixels, cameraView, ::gpk::SCoord2<uint32_t>
				{ targetPixels.metrics().x / 2 - cameraView.metrics().x / 2
				, targetPixels.metrics().y / 2 - cameraView.metrics().y / 2
				}
			);

		const ::gpk::SCoord2<int16_t>		targetCenter		= targetPixels.metrics().Cast<int16_t>() / 2;
		const ::gpk::SCoord2<int16_t>		cameraCenter		= cameraView.metrics().Cast<int16_t>() / 2;
		::gpk::SCoord2<int16_t>				cornerTopLeft		= targetCenter + ::gpk::SCoord2<int16_t>{int16_t(-cameraCenter.x), int16_t(-cameraCenter.y)};
		::gpk::SCoord2<int16_t>				cornerTopRight		= targetCenter + ::gpk::SCoord2<int16_t>{int16_t( cameraCenter.x), int16_t(-cameraCenter.y)};
		::gpk::SCoord2<int16_t>				cornerBottomLeft	= targetCenter + ::gpk::SCoord2<int16_t>{int16_t(-cameraCenter.x), int16_t( cameraCenter.y)};
		::gpk::SCoord2<int16_t>				cornerBottomRight	= targetCenter + ::gpk::SCoord2<int16_t>{int16_t( cameraCenter.x), int16_t( cameraCenter.y)};

		cornerTopLeft		.InPlaceClamp({}, {(int16_t)(renderTargetSize.x - 1), (int16_t)(renderTargetSize.y - 1)});
		cornerTopRight		.InPlaceClamp({}, {(int16_t)(renderTargetSize.x - 1), (int16_t)(renderTargetSize.y - 1)});
		cornerBottomLeft	.InPlaceClamp({}, {(int16_t)(renderTargetSize.x - 1), (int16_t)(renderTargetSize.y - 1)});
		cornerBottomRight	.InPlaceClamp({}, {(int16_t)(renderTargetSize.x - 1), (int16_t)(renderTargetSize.y - 1)});

		::gpk::drawLine(targetPixels, ::gpk::SLine2<int16_t>{cornerTopLeft		, cornerTopRight	}, ::gpk::DARKGRAY);
		::gpk::drawLine(targetPixels, ::gpk::SLine2<int16_t>{cornerTopLeft		, cornerBottomLeft	}, ::gpk::DARKGRAY);
		::gpk::drawLine(targetPixels, ::gpk::SLine2<int16_t>{cornerTopRight		, cornerBottomRight	}, ::gpk::GRAY * 1.1);
		::gpk::drawLine(targetPixels, ::gpk::SLine2<int16_t>{cornerBottomLeft	, cornerBottomRight	}, ::gpk::GRAY * 1.1);

		{
			::std::lock_guard<::std::mutex>				lockRTQueue	(app.World.DrawCache.RenderTargetQueueMutex);
			if(app.World.DrawCache.RenderTargetQueue)
				app.World.DrawCache.RenderTargetPool.push_back(sourceRT);
			else
				app.World.DrawCache.RenderTargetQueue = sourceRT;
		}


		::ghg::overlayDraw		(app.Overlay, app.World.DrawCache, app.World.PlayState.TimePlayed, depthBuffer, targetPixels);
		break;
	}

	::gpk::guiDraw(app.UI.GUIPerState[app.ActiveState], targetPixels);
	app.RenderTarget[0] = target;
	return 0;
}
