// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "d1_win32.h"

#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_matrix.h"
#include "gpk_png.h"

#include "gpk_app_impl.h"
#include "gpk_bitmap_target.h"

#include <DirectXColors.h>

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "The One");

// --- Cleanup application resources.
::gpk::error_t						cleanup								(::SApplication& app)											{
	::gpk::SFramework						& framework							= app.Framework;
	::d1::theOneUpdate(app.TheOne, 0, framework.RootWindow.Input, framework.RootWindow.EventQueue);
#if !defined(DISABLE_D3D11)
	app.D3DApp.Shutdown();
#endif

	::gpk::mainWindowDestroy(app.Framework.RootWindow);
	::d1::theOneUpdate(app.TheOne, 0, framework.RootWindow.Input, framework.RootWindow.EventQueue);
	return 0;
}

static	::gpk::error_t				updateSizeDependentResources		(::SApplication& app)											{
	::gpk::SWindow							& mainWindow						= app.Framework.RootWindow;
	const ::gpk::n2<uint16_t>				newSize								= mainWindow.Size;
#if !defined(DISABLE_D3D11)
	mainWindow.BackBuffer				= {};
	app.D3DApp.SetWindowSize(newSize);
#else
	gpk_necs(mainWindow.BackBuffer->resize(newSize, ::gpk::bgra{0, 0, 0, 0}, 0xFFFFFFFF));
#endif
	mainWindow.Resized					= false;
	return 0;
}

static	::gpk::error_t				processSystemEvent					(::SApplication & app, const ::gpk::SSysEvent & sysEvent) { 
	switch(sysEvent.Type) {
	default								: break;
	case ::gpk::SYSEVENT_WINDOW_CREATE	: 
#if !defined(DISABLE_D3D11)
		gpk_necs(app.D3DApp.Initialize(app.Framework.RootWindow.PlatformDetail.WindowHandle, app.TheOne.MainGame.Pool.Engine.Scene->Graphics));
#endif
	case ::gpk::SYSEVENT_WINDOW_RESIZE	: 
		gpk_necs(::updateSizeDependentResources(app));
		break;
	}
	return 0;
};

::gpk::error_t						setup								(::SApplication& app)											{
#if !defined(DISABLE_D3D11)
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif
	::gpk::SFramework						& framework							= app.Framework;
	::gpk::SWindow							& mainWindow						= framework.RootWindow;
	mainWindow.Size						= {1280, 720};
	gpk_necs(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input));

	static const ::gpk::FSysEvent			funcEvent							= [&app](const ::gpk::SSysEvent & sysEvent) { return ::processSystemEvent(app, sysEvent); };
	gpk_necs(mainWindow.EventQueue.for_each(funcEvent));

	if(::d1::APP_STATE_Quit == ::d1::theOneUpdate(app.TheOne, 0, mainWindow.Input, mainWindow.EventQueue))
		return 1;

	return 0;
}

::gpk::error_t						update								(::SApplication& app, bool systemRequestedExit)					{
	::gpk::SFramework						& framework							= app.Framework;
	::gpk::error_t							frameworkResult						= ::gpk::updateFramework(framework);
	gpk_necs(frameworkResult);
	rvi_if(1, frameworkResult == 1, "Framework requested close. Terminating execution.");

	::gpk::SWindow							& mainWindow						= app.Framework.RootWindow;

	const ::gpk::FSysEvent					funcEvent							= [&app](const ::gpk::SSysEvent & sysEvent) { return ::processSystemEvent(app, sysEvent); };
	gpk_necs(mainWindow.EventQueue.for_each(funcEvent));

	::gpk::SFrameInfo						& frameInfo							= framework.FrameInfo;
	{
		::gpk::STimer							timer;
		if(::d1::APP_STATE_Quit == ::d1::theOneUpdate(app.TheOne, frameInfo.Seconds.LastFrame, mainWindow.Input, mainWindow.EventQueue))
			return 1;

		timer.Frame();
		//info_printf("Update engine in %f seconds", timer.LastTimeSeconds);
	}

#if !defined(DISABLE_D3D11)
	if(app.TheOne.ActiveState != ::d1::APP_STATE_Init && app.D3DApp.Scene.IndexBuffer.size() < app.TheOne.MainGame.Pool.Engine.Scene->Graphics->Meshes.size()) {
		//gpk_necs(app.D3DApp.CreateDeviceDependentEngineResources(app.D3DApp.DeviceResources->GetD3DDevice(), *app.TheOne.MainGame.Pool.Engine.Scene->Graphics));
		gpk_necs(app.D3DApp.CreateDeviceResources(*app.TheOne.MainGame.Pool.Engine.Scene->Graphics));
	}
	app.D3DApp.Text.Update(frameInfo.Seconds.LastFrame, frameInfo.Seconds.Total, (uint32_t)frameInfo.FramesPerSecond);
#endif

	retval_ginfo_if(1, systemRequestedExit, "%s", "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");

	//-----------------------------
	::gpk::STimer							& timer								= app.Framework.Timer;
	char									buffer		[256]					= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND									windowHandle						= mainWindow.PlatformDetail.WindowHandle;
	SetWindowTextA(windowHandle, buffer);
	
	return 0;
}

::gpk::error_t						draw								(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
#if !defined(DISABLE_D3D11) 
	memset(app.D3DApp.GUIStuff.RenderTarget.begin(), 0, app.D3DApp.GUIStuff.RenderTarget.byte_count());
	
	gpk_necs(::gpk::guiDraw(*app.TheOne.AppUI.Dialog.GUI, app.D3DApp.GUIStuff.RenderTarget));

	const ::d1::SCamera						& cameraSelected	= app.TheOne.MainGame.CameraSelected();
	const ::gpk::SEngineScene				& engineScene		= *app.TheOne.MainGame.Pool.Engine.Scene;
	const ::gpk::rgbaf						clearColor			= app.TheOne.AppUI.ClearColor;
	const ::gpk::n3f						& lightPos			= app.TheOne.MainGame.LightPos;

	gpk_necs(::d1_win32::d3dAppDraw(app.D3DApp, engineScene, clearColor, lightPos, cameraSelected.Position, cameraSelected.Target));
#else 
	::gpk::SFramework						& framework							= app.Framework;
	::gpk::pobj<::gpk::SWindow::TOffscreen>	backBuffer							= framework.RootWindow.BackBuffer;
	backBuffer->resize(framework.RootWindow.BackBuffer->Color.metrics(), 0xFF000030, (uint32_t)-1);
	gpk_necs(::d1::theOneDraw(app.TheOne.AppUI, app.TheOne.MainGame, *backBuffer, framework.FrameInfo.Seconds.Total));
	memcpy(framework.RootWindow.BackBuffer->Color.View.begin(), backBuffer->Color.View.begin(), backBuffer->Color.View.byte_count());
#endif

	//::gpk::grid_mirror_y(framework.RootWindow.BackBuffer->Color.View, backBuffer->Color.View);
	//framework.RootWindow.BackBuffer		= backBuffer;
	return 0;
}
