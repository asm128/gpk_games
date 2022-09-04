#include "galaxy_hell_win32.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_raster_lh.h"

#include <time.h>

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "PNG Test");

::gpk::error_t							cleanup							(::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.MainDisplay); }
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
	return 0;
}

int										update				(SApplication & app, bool exitSignal)	{
	::gpk::SFramework							& framework			= app.Framework;
	//::gpk::STimer								timer;
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "%s", "Exit requested by runtime.");
	{
		::gpk::mutex_guard						lock				(app.LockRender);
		app.Framework.MainDisplayOffscreen	= app.Offscreen;
	}
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "%s", "Exit requested by framework update.");
	if(1 == ::ghg::galaxyHellUpdate(app.GalaxyHellApp, framework.FrameInfo.Seconds.LastFrame, *framework.Input, framework.MainDisplay.EventQueue, framework.MainDisplay.Size.Cast<uint16_t>()))
		return ::gpk::APPLICATION_STATE_EXIT;

	::gpk::sleep(1);
	return 0;
}

int														draw					(SApplication & app) {
	::ghg::galaxyHellDraw(app.GalaxyHellApp, app.Framework.MainDisplay.Size.Cast<uint16_t>());

	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	target			= app.GalaxyHellApp.RenderTarget[0];
	{
		::gpk::mutex_guard										lock					(app.LockRender);
		app.GalaxyHellApp.RenderTarget[0]					= app.Offscreen;
		app.Offscreen										= target;
	}

	return 0;
}
