// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "ssiege.server.win32.h"
#include "gpk_sun.h"

#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_png.h"

#include "gpk_app_impl.h"
#include "gpk_bitmap_target.h"

#include <DirectXColors.h>

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "SSiege Server v0.1");

static	::gpk::error_t	loadNetworkConfig		(const ::gpk::SJSONReader & jsonConfig, ::gpk::vcc & port, ::gpk::vcc & adapter) {
	::gpk::error_t				appNodeIndex;
	gpk_necs(appNodeIndex = ::gpk::jsonExpressionResolve(::gpk::vcs{"ssiege.server"}, jsonConfig, 0));
	return ::gpk::loadServerConfig(jsonConfig, appNodeIndex, port, adapter);
}

// --- Cleanup application resources.
::gpk::error_t			cleanup					(::SApplication & app)											{
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::SWindow				& mainWindow			= framework.RootWindow;
	ws_if_failed(::ssg::ssiegeUpdate(app.SSiegeApp, 0, mainWindow.Input, mainWindow.EventQueue, {}));

#if !defined(DISABLE_D3D11)
	app.D3DApp.Shutdown();
#endif

	ws_if_failed(::gpk::serverStop(app.Server->UDP));
	ws_if_failed(::gpk::mainWindowDestroy(mainWindow));
	ws_if_failed(::ssg::ssiegeUpdate(app.SSiegeApp, 0, mainWindow.Input, mainWindow.EventQueue, {}));
	ws_if_failed(::gpk::tcpipShutdown());
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
		gpk_necs(app.D3DApp.Initialize(app.Framework.RootWindow.PlatformDetail.WindowHandle, app.SSiegeApp.Game.Engine.Scene->Graphics));
#endif
	case ::gpk::EVENT_SCREEN_Resize: 
		gpk_necs(::updateSizeDependentResources(app));
		break;
	}
	return 0;
}

static	::gpk::error_t	processSystemEvent		(::SApplication & app, const ::gpk::SSystemEvent & sysEvent) { 
	switch(sysEvent.Type) {
	default: break;
	case ::gpk::SYSTEM_EVENT_Screen	: return ::gpk::eventExtractAndHandle<::gpk::EVENT_SCREEN		>(sysEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_SCREEN		> & screenEvent) { return processScreenEvent(app, screenEvent); }); 
	case ::gpk::SYSTEM_EVENT_GUI	: return ::gpk::eventExtractAndHandle<::gpk::EVENT_GUI_CONTROL	>(sysEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL	> & screenEvent) { return ::gpk::processGUIEvent(*app.Server, *app.Framework.GUI, screenEvent); }); 
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

	gpk_necs(::gpk::setupGUI(app.Server->UI, *framework.GUI));

	gpk_necs(::gpk::tcpipInitialize());

	ws_if_failed(::loadNetworkConfig(framework.JSONConfig.Reader, app.Server->Port, app.Server->Adapter))
	es_if_failed(::gpk::serverStart(*app.Server, *framework.GUI));

	return 0;
}

::gpk::error_t			update					(::SApplication & app, bool systemRequestedExit) {
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::SWindow				& mainWindow			= app.Framework.RootWindow;
	::gpk::SGUI					& gui					= *framework.GUI;

	bool						systemExit				= false;

	::gpk::TQueueSystemEvent	eventsToProcess			= mainWindow.EventQueue;
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
		rvis_if(::gpk::APPLICATION_STATE_EXIT, ::ssg::APP_STATE_Quit == ::ssg::ssiegeServerUpdate(app.SSiegeApp, frameInfo.Seconds.LastFrame, mainWindow.Input, mainWindow.EventQueue));
		timer.Frame();
		//info_printf("Update engine in %f seconds", timer.LastTimeSeconds);
	}

	int32_t						serverResult;
	es_if_failed(serverResult = ::gpk::serverUpdate(*app.Server, gui));
	rvi_if(::gpk::APPLICATION_STATE_EXIT, serverResult == 1, "User requested close. Terminating execution.");

	app.Server->QueueReceived.for_each([&app](::gpk::TUDPQueue & messages) {
		messages.enumerate([&app](uint32_t & index, ::gpk::pobj<::gpk::SUDPMessage> & message) {
			if(!message)
				return 0;

			gpk::vcu8						input					= message->Payload;
			gpk::pobj<ssg::EventSSiege>	newEvent;
			gpk_necs(newEvent->Load(input));
			info_printf("Received '%s' from client %i: %s.", ::gpk::get_enum_namep(newEvent->Type), index, ::gpk::get_value_namep(newEvent->Type));
			gpk_necs(app.SSiegeApp.EventsReceived.push_back(newEvent));
			return 0;
		});
		messages.clear();
	});

#if !defined(DISABLE_D3D11)
	if(app.SSiegeApp.ActiveState >= ::ssg::APP_STATE_Welcome && app.D3DApp.Scene.IndexBuffer.size() < app.SSiegeApp.Game.Engine.Scene->Graphics->Meshes.size() || !app.D3DApp.GUIStuff.IndexBuffer) {
		//gpk_necs(app.D3DApp.CreateDeviceDependentEngineResources(app.D3DApp.DeviceResources->GetD3DDevice(), *app.D1.Game.Engine.Scene->Graphics));
		gpk_necs(app.D3DApp.CreateDeviceResources(*app.SSiegeApp.Game.Engine.Scene->Graphics));
	}
	gpk_necs(app.D3DApp.Text.Update(frameInfo.Seconds.LastFrame, frameInfo.Seconds.Total, (uint32_t)frameInfo.FramesPerSecond));
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
	const ::gpk::n3f32			sunlightPos				= {}; //::gpk::calcSunPosition();
	//const double				sunlightFactor			= .1;//:::gpk::calcSunlightFactor(app.SSiegeApp.World.WorldState.DaylightRatioExtra, app.SSiegeApp.World.WorldState.DaylightOffsetMinutes);
	const ::gpk::rgbaf			clearColor				= ::gpk::DARKBLUE * .0625;//::gpk::interpolate_linear(::gpk::DARKBLUE * .25, ::gpk::LIGHTBLUE * 1.1, sunlightFactor);

#if !defined(DISABLE_D3D11) 
	memset(app.D3DApp.GUIStuff.RenderTarget.begin(), 0, app.D3DApp.GUIStuff.RenderTarget.byte_count());
	
	gpk_necs(::gpk::guiDraw(*app.SSiegeApp.GUI, app.D3DApp.GUIStuff.RenderTarget));

	const ::gpk::SCamera		& cameraSelected		= app.SSiegeApp.Game.Camera;
	const ::gpk::SEngineScene	& engineScene			= *app.SSiegeApp.Game.Engine.Scene;
	gpk_necs(::gpk::d3dAppDraw(app.D3DApp, engineScene, clearColor, sunlightPos, cameraSelected.Position, cameraSelected.Target, cameraSelected.NearFar));
#else 
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::pobj<::gpk::rtgbra8d32>	backBuffer	= framework.RootWindow.BackBuffer;
	backBuffer->resize(framework.RootWindow.BackBuffer->Color.metrics(), clearColor, (uint32_t)-1);
	gpk_necs(::ssg::ssiegeDraw(app.D1.AppUI, app.D1.MainGame, *backBuffer, false));
	memcpy(framework.RootWindow.BackBuffer->Color.View.begin(), backBuffer->Color.View.begin(), backBuffer->Color.View.byte_count());
	//::gpk::grid_mirror_y(framework.RootWindow.BackBuffer->Color.View, backBuffer->Color.View);
	//framework.RootWindow.BackBuffer		= backBuffer;
#endif
	return 0;
}
