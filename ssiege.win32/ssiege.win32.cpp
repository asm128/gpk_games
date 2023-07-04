// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "ssiege.win32.h"
#include "gpk_sun.h"

#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_matrix.h"
#include "gpk_png.h"

#include "gpk_app_impl.h"
#include "gpk_bitmap_target.h"

#include <DirectXColors.h>

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "SSiege v0.1");

// --- Cleanup application resources.
::gpk::error_t			cleanup					(::SApplication & app)											{
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::SWindow				& mainWindow			= framework.RootWindow;
#if !defined(DISABLE_D3D11)
	app.D3DApp.Shutdown();
#endif

	gpk_necs(::gpk::mainWindowDestroy(mainWindow));
	gpk_necs(::ssiege::ssiegeUpdate(app.SSiegeApp, 0, mainWindow.Input, mainWindow.EventQueue, {}));
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
		gpk_necs(app.D3DApp.Initialize(app.Framework.RootWindow.PlatformDetail.WindowHandle, app.SSiegeApp.World.Engine.Scene->Graphics));
#endif
	case ::gpk::EVENT_SCREEN_Resize: 
		gpk_necs(::updateSizeDependentResources(app));
		break;
	}
	return 0;
}

static	::gpk::error_t	processGUIEvent			(::SApplication & /*app*/, const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_GUI_CONTROL_StateChange: {
		const ::gpk::SChangeControlState	stateChange	= *(const ::gpk::SChangeControlState*)screenEvent.Data.begin();
		if(0 == (stateChange.Set & ::gpk::GUI_CONTROL_FLAG_Action))
			return 0;
	}
	}
	return 0;
}

static	::gpk::error_t	processSystemEvent		(::SApplication & app, const ::gpk::SSystemEvent & sysEvent) { 
	switch(sysEvent.Type) {
	default: break;
	case ::gpk::SYSTEM_EVENT_Screen	: return ::gpk::eventExtractAndHandle<::gpk::EVENT_SCREEN		>(sysEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_SCREEN		> & screenEvent) { return processScreenEvent(app, screenEvent); }); 
	case ::gpk::SYSTEM_EVENT_GUI	: return ::gpk::eventExtractAndHandle<::gpk::EVENT_GUI_CONTROL	>(sysEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL	> & screenEvent) { return processGUIEvent	(app, screenEvent); }); 
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

	return 0;
}

::gpk::error_t			update					(::SApplication& app, bool systemRequestedExit)					{
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::SWindow				& mainWindow			= app.Framework.RootWindow;
	::gpk::SGUI					& gui					= *framework.GUI;

	bool						systemExit				= false;

	::gpk::apobj<::gpk::SSystemEvent>	eventsToProcess	= mainWindow.EventQueue;
	gpk_necs(eventsToProcess.append(gui.Controls.EventQueue));
	gpk_necs(eventsToProcess.for_each([&app, &systemExit](const ::gpk::pobj<::gpk::SSystemEvent> & sysEvent) { 
		::gpk::error_t				result; 
		gpk_necs(result = ::processSystemEvent(app, *sysEvent)); 
		if(result == 1) 
			systemExit				= true; 
		return result;
	}));

	rvi_if(::gpk::APPLICATION_STATE_EXIT, systemExit || systemRequestedExit, "%s || %s", ::gpk::bool2char(systemExit), ::gpk::bool2char(systemRequestedExit));

	::gpk::SFrameInfo			& frameInfo				= framework.FrameInfo;
	{
		::gpk::STimer				timer					= {};
		rvis_if(::gpk::APPLICATION_STATE_EXIT, ::ssiege::APP_STATE_Quit == ::ssiege::ssiegeUpdate(app.SSiegeApp, frameInfo.Seconds.LastFrame, mainWindow.Input, mainWindow.EventQueue, {}))
		timer.Frame();
		//info_printf("Update engine in %f seconds", timer.LastTimeSeconds);
	}

#if !defined(DISABLE_D3D11)
	if(app.SSiegeApp.ActiveState >= ::ssiege::APP_STATE_Welcome && app.D3DApp.Scene.IndexBuffer.size() < app.SSiegeApp.World.Engine.Scene->Graphics->Meshes.size() || !app.D3DApp.GUIStuff.IndexBuffer) {
		gpk_necs(app.D3DApp.CreateDeviceResources(*app.SSiegeApp.World.Engine.Scene->Graphics));
	}
	app.D3DApp.Text.Update(frameInfo.Seconds.LastFrame, frameInfo.Seconds.Total, (uint32_t)frameInfo.FramesPerSecond);
#endif

	//-----------------------------
	::gpk::STimer				& timer					= app.Framework.Timer;
	char						buffer	[256]			= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND						windowHandle			= mainWindow.PlatformDetail.WindowHandle;
	SetWindowTextA(windowHandle, buffer);
	
	return ::gpk::updateFramework(framework);
}

::gpk::error_t			draw					(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::gpk::n3f32			sunlightPos				= ::gpk::calcSunPosition();
	const double				sunlightFactor			= ::gpk::calcSunlightFactor(app.SSiegeApp.World.WorldState.DaylightRatioExtra, app.SSiegeApp.World.WorldState.DaylightOffsetMinutes);
	const ::gpk::rgbaf			clearColor				= ::gpk::interpolate_linear(::gpk::DARKBLUE * .25, ::gpk::LIGHTBLUE * 1.1, sunlightFactor);

#if !defined(DISABLE_D3D11) 
	memset(app.D3DApp.GUIStuff.RenderTarget.begin(), 0, app.D3DApp.GUIStuff.RenderTarget.byte_count());
	
	gpk_necs(::gpk::guiDraw(*app.SSiegeApp.GUI, app.D3DApp.GUIStuff.RenderTarget));

	const ::ssiege::SCamera		& cameraSelected		= app.SSiegeApp.Camera;
	const ::gpk::SEngineScene	& engineScene			= *app.SSiegeApp.World.Engine.Scene;
	gpk_necs(::gpk::d3dAppDraw(app.D3DApp, engineScene, clearColor, sunlightPos, cameraSelected.Offset, cameraSelected.Target, {.01f, 10000.f}));
#else 
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::pobj<::gpk::rtgbra8d32>	backBuffer	= framework.RootWindow.BackBuffer;
	backBuffer->resize(framework.RootWindow.BackBuffer->Color.metrics(), clearColor, (uint32_t)-1);
	gpk_necs(::ssiege::d1Draw(app.D1.AppUI, app.D1.MainGame, *backBuffer, framework.FrameInfo.Seconds.Total));
	memcpy(framework.RootWindow.BackBuffer->Color.View.begin(), backBuffer->Color.View.begin(), backBuffer->Color.View.byte_count());
	//::gpk::grid_mirror_y(framework.RootWindow.BackBuffer->Color.View, backBuffer->Color.View);
	//framework.RootWindow.BackBuffer		= backBuffer;
#endif
	return 0;
}
