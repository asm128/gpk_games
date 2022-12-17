// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "the_one_win32.h"

#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_matrix.h"
#include "gpk_png.h"

#include "gpk_app_impl.h"
#include "gpk_bitmap_target.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "The One");

static				::gpk::error_t										updateSizeDependentResources				(::SApplication& app)											{
	const ::gpk::SCoord2<uint32_t>												newSize										= app.Framework.MainDisplay.Size;
	::gpk::updateSizeDependentTarget(app.Framework.MainDisplayOffscreen->Color, newSize);
	return 0;
}

// --- Cleanup application resources.
					::gpk::error_t										cleanup										(::SApplication& app)											{
	::gpk::SWindowPlatformDetail												& displayDetail								= app.Framework.MainDisplay.PlatformDetail;
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	::UnregisterClass(displayDetail.WindowClassName, displayDetail.WindowClass.hInstance);
	return 0;
}

					::gpk::error_t										setup										(::SApplication& app)											{
	::gpk::SFramework															& framework									= app.Framework;
	::gpk::SWindow																& mainWindow								= framework.MainDisplay;
	mainWindow.Size															= {1280, 720};
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?!");

	//::the1::theOneSetup(app.TheOne);

	//app.EntityCamera					= app.Engine.CreateCamera	();
	//app.EntityLightDirectional		= app.Engine.CreateLight	(::gpk::LIGHT_TYPE_Directional	);
	//app.EntityLightPoint				= app.Engine.CreateLight	(::gpk::LIGHT_TYPE_Point		);
	//app.EntityLightSpot				= app.Engine.CreateLight	(::gpk::LIGHT_TYPE_Spot			);
	//app.EntityBox					= app.Engine.CreateBox		();
	//app.EntitySphere				= app.Engine.CreateSphere	();

	//if(-1 != app.EntityCamera				)	app.Engine.SetPosition(app.EntityCamera				, {0, 0, 0});
	//if(-1 != app.EntityLightDirectional		)	app.Engine.SetPosition(app.EntityLightDirectional	, {0, 0, 0});
	//if(-1 != app.EntityLightPoint			)	app.Engine.SetPosition(app.EntityLightPoint			, {0, 0, 0});
	//if(-1 != app.EntityLightSpot			)	app.Engine.SetPosition(app.EntityLightSpot			, {0, 0, 0});
	//if(-1 != app.EntityBox					)	app.Engine.SetPosition(app.EntityBox				, {0, 0.5f, 0});
	//if(-1 != app.EntitySphere				)	app.Engine.SetPosition(app.EntitySphere				, {0, 0.5f, 2});

	ree_if	(errored(::updateSizeDependentResources	(app)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	return 0;
}

::gpk::error_t										update										(::SApplication& app, bool systemRequestedExit)					{
	::gpk::SFramework									& framework									= app.Framework;
	::gpk::SFrameInfo									& frameInfo									= framework.FrameInfo;
	{
		::gpk::STimer										timer;
		::the1::theOneUpdate(app.TheOne, frameInfo.Seconds.LastFrame, framework.Input, framework.MainDisplay.EventQueue);

		timer.Frame();
		info_printf("Update engine in %f seconds", timer.LastTimeSeconds);
	}


	retval_ginfo_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");

	::gpk::error_t											frameworkResult								= ::gpk::updateFramework(app.Framework);
	ree_if(errored(frameworkResult), "Unknown error.");
	rvi_if(1, frameworkResult == 1, "Framework requested close. Terminating execution.");
	ree_if(errored(::updateSizeDependentResources(app)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	//-----------------------------
	::gpk::STimer																& timer										= app.Framework.Timer;
	::gpk::SWindow																& mainWindow								= app.Framework.MainDisplay;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowTextA(windowHandle, buffer);


	
	return 0;
}

					::gpk::error_t										draw										(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework															& framework									= app.Framework;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>			backBuffer	= framework.MainDisplayOffscreen;
	backBuffer->resize(framework.MainDisplayOffscreen->Color.metrics(), 0xFF000030, (uint32_t)-1);

	::the1::theOneDraw(app.TheOne, *backBuffer, framework.FrameInfo.Seconds.Total);

	//memcpy(framework.MainDisplayOffscreen->Color.View.begin(), backBuffer->Color.View.begin(), backBuffer->Color.View.byte_count());
	//::gpk::grid_mirror_y(framework.MainDisplayOffscreen->Color.View, backBuffer->Color.View);
	//framework.MainDisplayOffscreen = backBuffer;
	//------------------------------------------------
	return 0;
}

