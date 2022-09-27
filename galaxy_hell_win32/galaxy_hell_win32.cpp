#include "galaxy_hell_win32.h"

#include "audio.h"

#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_raster_lh.h"

#include <time.h>

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT_MT(::SApplication, "PNG Test");

::gpk::error_t							cleanup							(::SApplication & app)						{ 
	::gpk::SFramework							& framework						= app.Framework;
	::ghg::galaxyHellUpdate(app.GalaxyHellApp, framework.FrameInfo.Seconds.LastFrame, framework.Input, framework.MainDisplay.EventQueue);
	app.AudioState.CleanupAudio(); 
	return ::gpk::mainWindowDestroy(app.Framework.MainDisplay); 
}
::gpk::error_t							setup							(::SApplication & app)						{
	::gpk::SFramework							& framework						= app.Framework;
	::gpk::SWindow								& mainWindow					= framework.MainDisplay;

	mainWindow.Size							= {1280, 720};

	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window. %s.", "why?!");
	//framework.UseDoubleBuffer				= true;
	srand((uint32_t)time(0));

	::ghg::SGalaxyHellDrawCache	& drawCache = app.GalaxyHellApp.Game.DrawCache;
	::gpk::resize(4096 * 1024, drawCache.PixelCoords, drawCache.PixelVertexWeights, drawCache.LightColorsModel, drawCache.LightColorsWorld, drawCache.LightPointsModel, drawCache.LightPointsWorld);
	app.AudioState.InitAudio();
	app.AudioState.PrepareAudio("thrust.wav");
	return 0;
}

int										update				(SApplication & app, bool exitSignal)	{
	::gpk::SFramework							& framework			= app.Framework;
	//::gpk::STimer								timer;
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "%s", "Exit requested by runtime.");
	if(1 == ::ghg::galaxyHellUpdate(app.GalaxyHellApp, framework.FrameInfo.Seconds.LastFrame, framework.Input, framework.MainDisplay.EventQueue))
		return ::gpk::APPLICATION_STATE_EXIT;

	app.AudioState.vListenerPos = app.GalaxyHellApp.Game.ShipState.Scene.Global.Camera[app.GalaxyHellApp.Game.ShipState.Scene.Global.CameraMode].Position; 
	app.AudioState.listener.OrientTop	= {0, 1, 0}; 
	app.AudioState.listener.OrientFront	= {1, 0, 0};  
	
	if(app.GalaxyHellApp.Game.ShipState.ShipCores.size())
		app.AudioState.vEmitterPos					= app.GalaxyHellApp.Game.ShipState.ShipPhysics.Transforms[app.GalaxyHellApp.Game.ShipState.EntitySystem.Entities[app.GalaxyHellApp.Game.ShipState.ShipCores[0].Entity].Body].Position;
	
	app.AudioState.vEmitterPos 			*= .03f;
	app.AudioState.vListenerPos			*= .03f;
	if(::ghg::CAMERA_MODE_SKY == app.GalaxyHellApp.Game.ShipState.Scene.Global.CameraMode) {
		app.AudioState.vEmitterPos .x				=  0;
		app.AudioState.vListenerPos.x				=  0;
	}

	app.AudioState.emitter.DopplerScaler	= 6.0f; //float(app.GalaxyHellApp.World.PlayState.TimeScale);
	app.AudioState.FrequencyRatio			= (float)app.GalaxyHellApp.Game.PlayState.TimeScale;

	if(app.GalaxyHellApp.ActiveState == ::ghg::APP_STATE_Play && false == app.GalaxyHellApp.Game.PlayState.Paused)
		app.AudioState.PauseAudio(true);
	else
		app.AudioState.PauseAudio(false);

	for(uint32_t iEvent = 0; iEvent < framework.MainDisplay.EventQueue.size(); ++iEvent) {
		switch(framework.MainDisplay.EventQueue[iEvent].Type) {
		case ::gpk::SYSEVENT_ACTIVATE:
			break;
		case ::gpk::SYSEVENT_DEACTIVATE:
			break;
		}
	}
	app.AudioState.UpdateAudio(framework.FrameInfo.Seconds.LastFrame);// / (app.GalaxyHellApp.World.ShipState.Ships.size() - 1));

	{
		::std::lock_guard<::std::mutex>			lockRTQueue	(app.GalaxyHellApp.RenderTargetLockQueue);
		if(app.GalaxyHellApp.RenderTargetQueue.size()) {
			::std::lock_guard<::std::mutex>			lockRTPool	(app.GalaxyHellApp.RenderTargetLockPool);
			app.GalaxyHellApp.RenderTargetPool.push_back(app.Framework.MainDisplayOffscreen);
			app.Framework.MainDisplayOffscreen = app.GalaxyHellApp.RenderTargetQueue[app.GalaxyHellApp.RenderTargetQueue.size() - 1];
			app.GalaxyHellApp.RenderTargetQueue.pop_back(0);
			for(uint32_t iRT = 0; iRT < app.GalaxyHellApp.RenderTargetQueue.size(); ++iRT) {
				app.GalaxyHellApp.RenderTargetPool.push_back(app.GalaxyHellApp.RenderTargetQueue[iRT]);
			}
			app.GalaxyHellApp.RenderTargetQueue.clear();
		}
	}
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "%s", "Exit requested by framework update.");
	return 0;
}

int														draw					(SApplication & app) {
	return ::ghg::galaxyHellDraw(app.GalaxyHellApp, app.Framework.MainDisplay.Size.Cast<uint16_t>());
}
