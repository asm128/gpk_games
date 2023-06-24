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

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "D1");

// --- Cleanup application resources.
::gpk::error_t			cleanup					(::SApplication & app)											{
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::SWindow				& mainWindow			= framework.RootWindow;
	gpk_necs(::d1::d1Update(app.D1, 0, mainWindow.Input, mainWindow.EventQueue));
#if !defined(DISABLE_D3D11)
	app.D3DApp.Shutdown();
#endif

	gpk_necs(::gpk::mainWindowDestroy(mainWindow));
	gpk_necs(::d1::d1Update(app.D1, 0, mainWindow.Input, mainWindow.EventQueue));
	return 0;
}

static	::gpk::error_t	updateSizeDependentResources(::SApplication & app)											{
	::gpk::SWindow				& mainWindow			= app.Framework.RootWindow;
	const ::gpk::n2u16			newSize					= mainWindow.Size;
#if !defined(DISABLE_D3D11)
	mainWindow.BackBuffer	= {};
	gpk_necs(app.D3DApp.SetWindowSize(newSize));
#else
	gpk_necs(mainWindow.BackBuffer->resize(newSize, ::gpk::bgra{0, 0, 0, 0}, 0xFFFFFFFF));
#endif
	mainWindow.Resized		= false;
	return 0;
}

static	::gpk::error_t	processScreenEvent		(::SApplication & app, const ::gpk::SEventView<::gpk::EVENT_SCREEN> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_SCREEN_Create:
#if !defined(DISABLE_D3D11)
		gpk_necs(app.D3DApp.Initialize(app.Framework.RootWindow.PlatformDetail.WindowHandle, app.D1.MainGame.Pool.Engine.Scene->Graphics));
#endif
	case ::gpk::EVENT_SCREEN_Resize: 
		gpk_necs(::updateSizeDependentResources(app));
		break;
	}
	return 0;
}

static	::gpk::error_t	processKeyboardEvent	(::SApplication & app, const ::gpk::SEventView<::gpk::EVENT_KEYBOARD> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_KEYBOARD_Down:
	case ::gpk::EVENT_KEYBOARD_SysDown:
		switch(screenEvent.Data[0]) {
		case VK_RETURN:
			if(GetAsyncKeyState(VK_MENU) & 0xFFF0)
				gpk_necs(::gpk::fullScreenToggle(app.Framework.RootWindow));
			break;
		}
		break;
	}
	return 0;
}

static	::gpk::error_t	processSystemEvent	(::SApplication & app, const ::gpk::SSystemEvent & sysEvent) { 
	switch(sysEvent.Type) {
	default: break;
	case ::gpk::SYSTEM_EVENT_Screen		: gpk_necs(::gpk::eventExtractAndHandle<::gpk::EVENT_SCREEN  >(sysEvent, [&app](const ::gpk::SScreenEventView & screenEvent) { return ::processScreenEvent  (app, screenEvent); })); break;
	case ::gpk::SYSTEM_EVENT_Keyboard	: gpk_necs(::gpk::eventExtractAndHandle<::gpk::EVENT_KEYBOARD>(sysEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_KEYBOARD> & screenEvent) { return ::processKeyboardEvent(app, screenEvent); })); break;
	}
	return 0;
}
::gpk::error_t			setup					(::SApplication& app)											{
#if !defined(DISABLE_D3D11)
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::SWindow				& mainWindow			= framework.RootWindow;
	mainWindow.Size			= {1280, 720};
	gpk_necs(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input));

	gpk_necs(mainWindow.EventQueue.for_each([&app](const ::gpk::pobj<::gpk::SSystemEvent> & sysEvent) { return ::processSystemEvent(app, *sysEvent); }));

	rvis_if(::gpk::APPLICATION_STATE_EXIT, ::d1::APP_STATE_Quit == ::d1::d1Update(app.D1, 0, mainWindow.Input, mainWindow.EventQueue));

	return 0;
}

::gpk::error_t			update					(::SApplication& app, bool systemRequestedExit)					{
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::error_t				frameworkResult			= ::gpk::updateFramework(framework);
	gpk_necs(frameworkResult);
	rvis_if(::gpk::APPLICATION_STATE_EXIT, frameworkResult == ::gpk::APPLICATION_STATE_EXIT);

	::gpk::SWindow				& mainWindow			= app.Framework.RootWindow;

	gpk_necs(mainWindow.EventQueue.for_each([&app](const ::gpk::pobj<::gpk::SSystemEvent> & sysEvent) { return ::processSystemEvent(app, *sysEvent); }));

	::gpk::SFrameInfo			& frameInfo				= framework.FrameInfo;
	{
		::gpk::STimer				timer					= {};
		rvis_if(::gpk::APPLICATION_STATE_EXIT, ::d1::APP_STATE_Quit == ::d1::d1Update(app.D1, frameInfo.Seconds.LastFrame, mainWindow.Input, mainWindow.EventQueue))

		timer.Frame();
		//info_printf("Update engine in %f seconds", timer.LastTimeSeconds);
	}

	rvis_if(::gpk::APPLICATION_STATE_EXIT, systemRequestedExit);

#if !defined(DISABLE_D3D11)
	if(app.D1.ActiveState >= ::d1::APP_STATE_Welcome && app.D3DApp.Scene.IndexBuffer.size() < app.D1.MainGame.Pool.Engine.Scene->Graphics->Meshes.size()) {
		//gpk_necs(app.D3DApp.CreateDeviceDependentEngineResources(app.D3DApp.DeviceResources->GetD3DDevice(), *app.D1.MainGame.Pool.Engine.Scene->Graphics));
		gpk_necs(app.D3DApp.CreateDeviceResources(*app.D1.MainGame.Pool.Engine.Scene->Graphics));
	}
	app.D3DApp.Text.Update(frameInfo.Seconds.LastFrame, frameInfo.Seconds.Total, (uint32_t)frameInfo.FramesPerSecond);
#endif

	//-----------------------------
	::gpk::STimer				& timer					= app.Framework.Timer;
	char						buffer	[256]			= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND						windowHandle			= mainWindow.PlatformDetail.WindowHandle;
	SetWindowTextA(windowHandle, buffer);
	
	return 0;
}

::gpk::error_t			draw					(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
#if !defined(DISABLE_D3D11) 
	memset(app.D3DApp.GUIStuff.RenderTarget.begin(), 0, app.D3DApp.GUIStuff.RenderTarget.byte_count());
	
	gpk_necs(::gpk::guiDraw(*app.D1.AppUI.Dialog.GUI, app.D3DApp.GUIStuff.RenderTarget));

	const ::d1::SCamera			& cameraSelected		= app.D1.MainGame.CameraSelected();
	const ::gpk::SEngineScene	& engineScene			= *app.D1.MainGame.Pool.Engine.Scene;
	const ::gpk::rgbaf			clearColor				= app.D1.AppUI.ClearColor;
	const ::gpk::n3f32			& lightPos				= app.D1.MainGame.LightPos;

	gpk_necs(::gpk::d3dAppDraw(app.D3DApp, engineScene, clearColor, lightPos, cameraSelected.Position, cameraSelected.Target));
#else 
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::pobj<::gpk::rtgbra8d32>	backBuffer	= framework.RootWindow.BackBuffer;
	backBuffer->resize(framework.RootWindow.BackBuffer->Color.metrics(), 0xFF000030, (uint32_t)-1);
	gpk_necs(::d1::d1Draw(app.D1.AppUI, app.D1.MainGame, *backBuffer, framework.FrameInfo.Seconds.Total));
	memcpy(framework.RootWindow.BackBuffer->Color.View.begin(), backBuffer->Color.View.begin(), backBuffer->Color.View.byte_count());
	//::gpk::grid_mirror_y(framework.RootWindow.BackBuffer->Color.View, backBuffer->Color.View);
	//framework.RootWindow.BackBuffer		= backBuffer;
#endif
	return 0;
}
