#include "galaxy_hell_win32.h"

#include "audio.h"

#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_raster_lh.h"

#include <time.h>

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "PNG Test");

::gpk::error_t							cleanup							(::SApplication & app)						{ app.AudioState.CleanupAudio(); return ::gpk::mainWindowDestroy(app.Framework.MainDisplay); }
::gpk::error_t							setup							(::SApplication & app)						{
	::gpk::SFramework							& framework						= app.Framework;
	::gpk::SWindow								& mainWindow					= framework.MainDisplay;

	mainWindow.Size							= {1280, 720};

	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window. %s.", "why?!");
	//framework.UseDoubleBuffer				= true;
	srand((uint32_t)time(0));

	::ghg::SGalaxyHellDrawCache	& drawCache = app.GalaxyHellApp.World.DrawCache;
	::gpk::resize(4096 * 1024, drawCache.PixelCoords, drawCache.PixelVertexWeights, drawCache.LightColorsModel, drawCache.LightColorsWorld, drawCache.LightPointsModel, drawCache.LightPointsWorld);
	app.SwapOffscreen->resize(mainWindow.Size);
	app.AudioState.InitAudio();
	app.AudioState.PrepareAudio("heli.wav");
	return 0;
}

int										update				(SApplication & app, bool exitSignal)	{
	::gpk::SFramework							& framework			= app.Framework;
	//::gpk::STimer								timer;
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "%s", "Exit requested by runtime.");
	if(1 == ::ghg::galaxyHellUpdate(app.GalaxyHellApp, framework.FrameInfo.Seconds.LastFrame, framework.Input, framework.MainDisplay.EventQueue, framework.MainDisplay.Size.Cast<uint16_t>()))
		return ::gpk::APPLICATION_STATE_EXIT;

	app.AudioState.vListenerPos = app.GalaxyHellApp.World.ShipState.Scene.Global.Camera[app.GalaxyHellApp.World.ShipState.Scene.Global.CameraMode].Position * .05; app.AudioState.vListenerPos.y = 0; app.AudioState.listener.OrientTop = {0, 1, 0}; app.AudioState.listener.OrientFront = {1, 0, 0};  
	
	if(app.GalaxyHellApp.World.ShipState.ShipCores.size())
		app.AudioState.vEmitterPos					= app.GalaxyHellApp.World.ShipState.ShipPhysics.Transforms[app.GalaxyHellApp.World.ShipState.EntitySystem.Entities[app.GalaxyHellApp.World.ShipState.ShipCores[0].Entity].Body].Position;
	
	app.AudioState.vEmitterPos.y				=  0;//{app.AudioState.vEmitterPos.x, 0, app.AudioState.vEmitterPos.z};
	app.AudioState.vEmitterPos					*= .1;

	if(::ghg::CAMERA_MODE_SKY == app.GalaxyHellApp.World.ShipState.Scene.Global.CameraMode) {
		app.AudioState.vEmitterPos .x				=  0;
		app.AudioState.vListenerPos.x				=  0;
		app.AudioState.vEmitterPos .z				*= .75f;
		app.AudioState.vListenerPos.z				*= .75f;
	}

	app.AudioState.emitter.DopplerScaler		= 4.0f; //float(app.GalaxyHellApp.World.PlayState.TimeScale);
	app.AudioState.FrequencyRatio				= (float)app.GalaxyHellApp.World.PlayState.TimeScale;
	app.AudioState.UpdateAudio(framework.FrameInfo.Seconds.LastFrame);// / (app.GalaxyHellApp.World.ShipState.Ships.size() - 1));
	{
		::gpk::mutex_guard						lock				(app.LockRender);
		app.GalaxyHellApp.RenderTarget[0]	= app.Framework.MainDisplayOffscreen;
		app.Framework.MainDisplayOffscreen	= app.Offscreen;
	}
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "%s", "Exit requested by framework update.");
	return 0;
}

int														draw					(SApplication & app) {
	::ghg::galaxyHellDraw(app.GalaxyHellApp, app.Framework.MainDisplay.Size.Cast<uint16_t>());

	{
		::gpk::mutex_guard										lock					(app.LockRender);
		::gpk::ptr_obj<::ghg::TRenderTarget>					target					= app.GalaxyHellApp.RenderTarget[0];
		app.GalaxyHellApp.RenderTarget[0]					= app.Offscreen;
		app.Offscreen										= target;
	}

	return 0;
}
