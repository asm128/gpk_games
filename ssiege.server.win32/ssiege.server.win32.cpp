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

#if !defined(DISABLE_D3D11)
#include <DirectXColors.h>
#endif

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "SSiege Server v0.1");
GPK_USING_TYPEINT();

static	gpk::err_t	loadNetworkConfig		(const ::gpk::SJSONReader & jsonConfig, ::gpk::vcst_t & port, ::gpk::vcst_t & adapter) {
	gpk::err_t				appNodeIndex;
	if_fail_fe(appNodeIndex = ::gpk::jsonExpressionResolve("application.test_udp_server", jsonConfig, 0));
	return ::gpk::loadServerConfig(jsonConfig, appNodeIndex, port, adapter);
}

// --- Cleanup application resources.
gpk::err_t			cleanup					(::SApplication & app)											{
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::SWindow				& mainWindow			= framework.RootWindow;
	if_fail_w(::ssg::ssiegeUpdate(app.SSiegeApp, 0, mainWindow.Input, mainWindow.EventQueue, {}));

#if !defined(DISABLE_D3D11)
	app.D3DApp.Shutdown();
#endif

	if_fail_w(::gpk::serverStop(app.Server->SocketServer));
	if_fail_w(::gpk::mainWindowDestroy(mainWindow));
	if_fail_w(::ssg::ssiegeUpdate(app.SSiegeApp, 0, mainWindow.Input, mainWindow.EventQueue, {}));
	if_fail_w(::gpk::tcpipShutdown());
	return 0;
}

static	gpk::err_t	updateSizeDependentResources(::SApplication & app)											{
	::gpk::SWindow				& mainWindow			= app.Framework.RootWindow;
	const ::gpk::n2u1_t			newSize					= mainWindow.Size;
#if !defined(DISABLE_D3D11)
	mainWindow.BackBuffer	= {};
	if_fail_fe(app.D3DApp.SetWindowSize(newSize));
#else
	if_fail_fe(mainWindow.BackBuffer->resize(newSize, ::gpk::bgra{0, 0, 0, 0}, 0xFFFFFFFF));
#endif
	mainWindow.Resized		= false;

	return 0;
}

static	gpk::err_t	processScreenEvent		(::SApplication & app, const ::gpk::SEventView<::gpk::EVENT_SCREEN> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_SCREEN_Create:
#if !defined(DISABLE_D3D11)
		if_fail_fe(app.D3DApp.Initialize(app.Framework.RootWindow.PlatformDetail.WindowHandle, app.SSiegeApp.Game.Engine.Scene->Graphics));
#endif
	case ::gpk::EVENT_SCREEN_Resize: 
		if_fail_fe(::updateSizeDependentResources(app));
		break;
	}
	return 0;
}

static	gpk::err_t	processSystemEvent		(::SApplication & app, const ::gpk::SEventSystem & sysEvent) { 
	switch(sysEvent.Type) {
	default: break;
	case ::gpk::SYSTEM_EVENT_Screen	: return ::gpk::eventExtractAndHandle<::gpk::EVENT_SCREEN		>(sysEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_SCREEN		> & screenEvent) { return processScreenEvent(app, screenEvent); }); 
	case ::gpk::SYSTEM_EVENT_GUI	: return ::gpk::eventExtractAndHandle<::gpk::EVENT_GUI_CONTROL	>(sysEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL	> & screenEvent) { return ::gpk::processGUIEvent(*app.Server, *app.Framework.GUI, screenEvent); }); 
	}
	return 0;
}

gpk::err_t			setup					(::SApplication& app)											{
#if !defined(DISABLE_D3D11)
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::SWindow				& mainWindow			= framework.RootWindow;
	mainWindow.Size			= {1280, 720};
	if_fail_fe(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input));

	if_fail_fe(::gpk::setupGUI(app.Server->UI, *framework.GUI));

	if_fail_fe(::gpk::tcpipInitialize());

	if_fail_w(::loadNetworkConfig(framework.JSONConfig.Reader, app.Server->Port, app.Server->Adapter));
	if_fail_e(::gpk::serverStart(*app.Server, *framework.GUI));

	return 0;
}

gpk::err_t			update					(::SApplication & app, bool systemRequestedExit) {
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::SWindow				& mainWindow			= app.Framework.RootWindow;
	::gpk::SGUI					& gui					= *framework.GUI;

	bool						systemExit				= false;

	::gpk::TQueueSystemEvent	eventsToProcess			= mainWindow.EventQueue;
	if_fail_fe(eventsToProcess.append(gui.Controls.EventQueue));
	if_fail_fe(eventsToProcess.for_each([&app, &systemExit](const ::gpk::pobj<::gpk::SEventSystem> & sysEvent) { 
		gpk::err_t				result; 
		if_fail_fe(result = ::processSystemEvent(app, *sysEvent)); 
		if(result == 1) 
			systemExit				= true; 
		return result;
	}));

	if_true_vif(::gpk::APPLICATION_STATE_EXIT, systemExit || systemRequestedExit, "%u || %u", systemExit, systemRequestedExit);

	::gpk::SFrameInfo			& frameInfo				= framework.FrameInfo;
	{
		::gpk::STimer				timer					= {};
		if_true_vi(::gpk::APPLICATION_STATE_EXIT, ::ssg::APP_STATE_Quit == ::ssg::ssiegeServerUpdate(app.SSiegeApp, frameInfo.Seconds.LastFrame, mainWindow.Input, mainWindow.EventQueue));
		timer.Frame();
		//info_printf("Update engine in %f seconds", timer.LastTimeSeconds);
	}

	s2_t						serverResult;
	if_fail_e(serverResult = ::gpk::serverUpdate(*app.Server, gui));
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, serverResult == 1);

	if_fail_e(app.Server->QueueReceived.for_each([&app](::gpk::TPacketQueue & messages) {
		messages.enumerate([&app](u2_t & iClient, ::gpk::pobj<::gpk::SMessage> & message) {
			if(!message)
				return 0;

			::gpk::vcu0_t						input					= message->Payload;
			gpk::pobj<ssg::EventSSiege>	newEvent;
			if_fail_fe(newEvent->Load(input));
			info_printf("iClient=%u newEvent->Type=%s newEvent->Data(%u):%.*s.", iClient, ::gpk::get_enum_namep(newEvent->Type), newEvent->Data.size(), GPK_VIEW_ARGS(newEvent->Data));
			if_fail_fe(app.SSiegeApp.EventsReceived.push_back(newEvent));
			return 0;
		});
		return messages.clear();
	}));

#if !defined(DISABLE_D3D11)
	if(app.SSiegeApp.ActiveState >= ::ssg::APP_STATE_Welcome && app.D3DApp.Scene.IndexBuffer.size() < app.SSiegeApp.Game.Engine.Scene->Graphics->Meshes.size() || !app.D3DApp.GUIStuff.IndexBuffer) {
		//if_fail_fe(app.D3DApp.CreateDeviceDependentEngineResources(app.D3DApp.DeviceResources->GetD3DDevice(), *app.D1.Game.Engine.Scene->Graphics));
		if_fail_fe(app.D3DApp.CreateDeviceResources(*app.SSiegeApp.Game.Engine.Scene->Graphics));
	}
	if_fail_fe(app.D3DApp.Text.Update(frameInfo.Seconds.LastFrame, frameInfo.Seconds.Total, (u2_t)frameInfo.FrameMeter.FramesPerSecond));
#endif

	return ::gpk::updateFramework(framework);
}

gpk::err_t			draw					(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::gpk::n3f2_t			sunlightPos				= {}; //::gpk::calcSunPosition();
	//const double				sunlightFactor			= .1;//:::gpk::calcSunlightFactor(app.SSiegeApp.World.WorldState.DaylightRatioExtra, app.SSiegeApp.World.WorldState.DaylightOffsetMinutes);
	const ::gpk::rgbaf			clearColor				= ::gpk::DARKBLUE * .0625;//::gpk::interpolate_linear(::gpk::DARKBLUE * .25, ::gpk::LIGHTBLUE * 1.1, sunlightFactor);

#if !defined(DISABLE_D3D11) 
	memset(app.D3DApp.GUIStuff.RenderTarget.begin(), 0, app.D3DApp.GUIStuff.RenderTarget.byte_count());
	
	if_fail_fe(::gpk::guiDraw(*app.SSiegeApp.GUI, app.D3DApp.GUIStuff.RenderTarget));

	app.D3DApp.Scene.ConstantBufferScene.Time	= (float)app.Framework.FrameInfo.Seconds.Total;

	const ::gpk::SCamera		& cameraSelected		= app.SSiegeApp.Game.Camera;
	const ::gpk::SEngineScene	& engineScene			= *app.SSiegeApp.Game.Engine.Scene;
	if_fail_fe(::gpk::d3dAppDraw(app.D3DApp, engineScene, clearColor, sunlightPos, cameraSelected.Position, cameraSelected.Target, cameraSelected.NearFar));
#else 
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::prtbgra8d32			backBuffer				= framework.RootWindow.BackBuffer;
	backBuffer->resize(framework.RootWindow.BackBuffer->Color.metrics(), clearColor, (u2_t)-1);
	if_fail_fe(::ssg::ssiegeDraw(app.SSiegeApp, *backBuffer, false));
	memcpy(framework.RootWindow.BackBuffer->Color.View.begin(), backBuffer->Color.View.begin(), backBuffer->Color.View.byte_count());
	//::gpk::grid_mirror_y(framework.RootWindow.BackBuffer->Color.View, backBuffer->Color.View);
	//framework.RootWindow.BackBuffer		= backBuffer;
#endif
	return 0;
}
