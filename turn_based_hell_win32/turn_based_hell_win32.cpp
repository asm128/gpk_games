#include "turn_based_hell_win32.h"

#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_raster_lh.h"

#include <time.h>

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "PNG Test");

static ::gpk::error_t					setupExitButton				(::gpk::SGUI & gui) { // Build the exit button
	gui.ColorModeDefault					= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault						= ::gpk::ASCII_COLOR_DARKGREEN * 16 + 8;
	int32_t										idExit						= ::gpk::controlCreate(gui);
	::gpk::SControl								& controlExit				= gui.Controls.Controls[idExit];
	controlExit.Area						= {{}, {64, 20}};
	controlExit.Border						= {10, 10, 10, 10};
	controlExit.Margin						= {1, 1, 1, 1};
	controlExit.Align						= ::gpk::ALIGN_TOP_LEFT;
	::gpk::SControlText							& controlText				= gui.Controls.Text[idExit];
	controlText.Text						= "Exit";
	controlText.Align						= ::gpk::ALIGN_CENTER;
	::gpk::controlSetParent(gui, idExit, -1);
	return idExit;
}

::gpk::error_t							cleanup							(::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.MainDisplay); }
::gpk::error_t							setup							(::SApplication & app)						{
	::gpk::SFramework							& framework						= app.Framework;
	::gpk::SWindow								& mainWindow					= framework.MainDisplay;

	mainWindow.Size							= {1280, 720};

	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window. %s.", "why?!");
	//framework.UseDoubleBuffer				= true;
	srand((uint32_t)time(0));
	app.IdExit								= ::setupExitButton(*framework.GUI);

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

	::gpk::SGUI									& gui				= *framework.GUI;
	::gpk::array_pod<uint32_t>					controlsToProcess	= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);
	for(uint32_t iControl = 0, countControls = controlsToProcess.size(); iControl < countControls; ++iControl) {
		uint32_t									idControl			= controlsToProcess		[iControl];
		const ::gpk::SControlState					& controlState		= gui.Controls.States	[idControl];
		if(controlState.Execute) {
			info_printf("Executed %u.", idControl);
			if(idControl == (uint32_t)app.IdExit)
				return 1;
		}
	}

	Sleep(1);
	return 0;
}

int														draw					(SApplication & app) {
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	target			= app.SwapOffscreen;
	target->resize(app.Framework.MainDisplay.Size, {}, 0xFFFFFFFFU);
	::gpk::view_grid<::gpk::SColorBGRA>							targetPixels			= target->Color.View;
	::gpk::view_grid<uint32_t>									depthBuffer				= target->DepthStencil.View;
	{
		::gpk::mutex_guard										lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(*app.Framework.GUI, 0, target->Color.View);
	}
	{
		::gpk::mutex_guard										lock					(app.LockRender);
		app.SwapOffscreen									= app.Offscreen;
		app.Offscreen										= target;
	}

	return 0;
}

