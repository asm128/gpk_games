#include "gpk_galaxy_hell_app.h"
#include "gpk_grid_copy.h"
#include "gpk_raster_lh.h"
#include "gpk_storage.h"

::gpk::error_t					ghg::galaxyHellUpdate			(::ghg::SGalaxyHellApp & app, double lastTimeSeconds, const ::gpk::ptr_obj<::gpk::SInput> & inputState, const ::gpk::view_array<::gpk::SSysEvent> & systemEvents, const ::gpk::SCoord2<uint16_t> & windowSize) {
	if(app.ActiveState == ::ghg::APP_STATE_Quit)
		return 1;

	for(uint32_t iEvent = 0; iEvent < systemEvents.size(); ++iEvent) {
		switch(systemEvents[iEvent].Type) {
		case gpk::SYSEVENT_RESIZE: {
			app.World.DrawCache.RenderTargetMetrics = *(const ::gpk::SCoord2<uint16_t>*)systemEvents[iEvent].Data.begin();

			if(app.World.DrawCache.RenderTargetMetrics.x > 1600) app.World.DrawCache.RenderTargetMetrics.x = 1600;
			if(app.World.DrawCache.RenderTargetMetrics.y >  900) app.World.DrawCache.RenderTargetMetrics.y = 900;

			double								currentRatio					= app.World.DrawCache.RenderTargetMetrics.y / (double)app.World.DrawCache.RenderTargetMetrics.x;
			double								targetRatioY					=  9 / 16.0;
			if(currentRatio >= targetRatioY)
				app.World.DrawCache.RenderTargetMetrics.y = (uint16_t)(app.World.DrawCache.RenderTargetMetrics.x * targetRatioY + .1f);
			else 
				app.World.DrawCache.RenderTargetMetrics.x = (uint16_t)(app.World.DrawCache.RenderTargetMetrics.y / targetRatioY + .1f);
			break;
		}
		case ::gpk::SYSEVENT_DEACTIVATE: {
			::gpk::array_pod<byte_t>			serialized;
			app.World.Save(serialized);
			char								fileName[4096] = {};
			sprintf_s(fileName, "./%llu.autosave.ghs", 0ULL); //::gpk::timeCurrentInMs());
			::gpk::fileFromMemory(fileName, serialized);

			app.World.PlayState.Paused			= true;
			app.ActiveState						= ::ghg::APP_STATE_Home;
		}
		}
	}
	switch(app.ActiveState) {
	case APP_STATE_Load		: {
		::ghg::solarSystemSetup(app.World, app.World.DrawCache.RenderTargetMetrics);
		::ghg::overlaySetup(app.Overlay);
		::ghg::guiSetup(app, inputState);

		::gpk::array_pod<byte_t>			serialized;
		//char								fileName[4096]		= {};
		::gpk::array_obj<::gpk::array_pod<char>>	pathFiles;
		::gpk::pathList(::gpk::vcs{"./"}, pathFiles);
		for(uint32_t iFile = 0; iFile < pathFiles.size(); ++iFile)  {
			if(pathFiles[iFile].size() < strlen(".autosave.ghs"))
				continue;
			if(0 == strcmp(&pathFiles[iFile][pathFiles[iFile].size() - (uint32_t)strlen(".autosave.ghs")], ".autosave.ghs")) {
				::gpk::fileToMemory(pathFiles[iFile], serialized);
				::gpk::view_array<const byte_t>		viewSerialized		= {(const byte_t*)serialized.begin(), serialized.size()};
				app.World.Load(viewSerialized);
				break;
			}
		}

		app.ActiveState						= APP_STATE_Home;
		break;
	}
	case  APP_STATE_Play	: 
		break;
	}	 
	::ghg::solarSystemUpdate(app.World, (app.ActiveState != ::ghg::APP_STATE_Play) ? 0.000001 : lastTimeSeconds, *inputState, systemEvents);

	::ghg::overlayUpdate	(app.Overlay, app.World.PlayState.Stage, app.World.ShipState.ShipCores.size() ? app.World.ShipState.ShipCores[0].Score : 0, app.World.PlayState.TimeWorld);

	app.ActiveState						= (::ghg::APP_STATE)::ghg::guiUpdate(app, systemEvents, windowSize.Cast<uint16_t>());
	return 0;
}

::gpk::error_t					ghg::galaxyHellDraw				(::ghg::SGalaxyHellApp & app, ::gpk::SCoord2<uint16_t> renderTargetSize) {
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	target			= app.RenderTarget[0];
	target->resize(renderTargetSize.Cast<uint32_t>(), {0, 0, 0, 1}, 0xFFFFFFFFU);
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
				{ (targetPixels.metrics().x >> 1) - (cameraView.metrics().x >> 1)
				, (targetPixels.metrics().y >> 1) - (cameraView.metrics().y >> 1)
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


		::ghg::overlayDraw		(app.Overlay, app.World.DrawCache, app.World.PlayState.TimeWorld, depthBuffer, targetPixels);
		break;
	}

	::gpk::guiDraw(*app.DialogPerState[app.ActiveState].GUI, targetPixels);
	app.RenderTarget[0] = target;
	return 0;
}
