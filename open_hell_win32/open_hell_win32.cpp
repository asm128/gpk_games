#include "open_hell_win32.h"

#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_raster_lh.h"

#include <time.h>
#include <direct.h>

//#include <joystickapi.h>

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

//#define USE_STEAM_CLIENT
//#define STEAM_CEG
#if defined(USE_STEAM_CLIENT)
#include "steam/steam_api.h"
#include "steam/isteamuserstats.h"
#include "steam/isteamremotestorage.h"
#include "steam/isteammatchmaking.h"
#include "steam/steam_gameserver.h"

#if defined(STEAM_CEG)
// Steam DRM header file
#include "cegclient.h"
#else
#define Steamworks_InitCEGLibrary() (true)
#define Steamworks_TermCEGLibrary()
#define Steamworks_TestSecret()
#define Steamworks_SelfCheck()
#endif
#endif

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "Open Hell v0.1");

::gpk::error_t							cleanup							(::SApplication & app)						{ 
	//::gpk::SFramework							& framework						= app.Framework;
	//::ohg::openHellUpdate(app.OpenHellApp, framework.FrameInfo.Seconds.LastFrame, framework.Input, framework.MainDisplay.EventQueue);
	app.AudioState.CleanupAudio(); 


	// --- when the rendering context is no longer needed ...   
 	// make the rendering context not current  
	if(app.GLRenderContext) {
		wglMakeCurrent		(NULL, NULL) ; 
		wglDeleteContext	(app.GLRenderContext);	// delete the rendering context  
	}
#ifdef USE_STEAM_CLIENT
	SteamAPI_Shutdown();
	Steamworks_TermCEGLibrary();
#endif
	//::gpk::clientDisconnect(app.Client);
	//::gpk::tcpipShutdown();
	return ::gpk::mainWindowDestroy(app.Framework.MainDisplay); 
}

#ifdef USE_STEAM_CLIENT
//-----------------------------------------------------------------------------
// Purpose: callback hook for debug text emitted from the Steam API
//-----------------------------------------------------------------------------
extern "C" void __cdecl SteamAPIDebugTextHook( int nSeverity, const char *pchDebugText ) {
	// if you're running in the debugger, only warnings (nSeverity >= 1) will be sent
	// if you add -debug_steamapi to the command-line, a lot of extra informational messages will also be sent
	error_printf("%s", pchDebugText );
	if ( nSeverity >= 1 ) {
		int x = 3;	// place to set a breakpoint for catching API errors
		(void)x;
	}
}

// Extracts some feature from the command line
bool ParseCommandLine( const char *pchCmdLine, const char **ppchServerAddress, const char **ppchLobbyID ) {
	// Look for the +connect ipaddress:port parameter in the command line, Steam will pass this when a user has used the Steam Server browser to find a server for our game and is trying to join it. 
	const char *pchConnectParam = "+connect ";
	const char *pchConnect = strstr( pchCmdLine, pchConnectParam );
	*ppchServerAddress = NULL;
	if ( pchConnect && strlen( pchCmdLine ) > (pchConnect - pchCmdLine) + strlen( pchConnectParam ) )
		*ppchServerAddress = pchCmdLine + ( pchConnect - pchCmdLine ) + strlen( pchConnectParam );	// Address should be right after the +connect

	// look for +connect_lobby lobbyid paramter on the command line. Steam will pass this in if a user taken up an invite to a lobby
	const char *pchConnectLobbyParam = "+connect_lobby ";
	const char *pchConnectLobby = strstr( pchCmdLine, pchConnectLobbyParam );
	*ppchLobbyID = NULL;
	if ( pchConnectLobby && strlen( pchCmdLine ) > (pchConnectLobby - pchCmdLine) + strlen( pchConnectLobbyParam ) )
		*ppchLobbyID = pchCmdLine + ( pchConnectLobby - pchCmdLine ) + strlen( pchConnectLobbyParam );	// lobby ID should be right after the +connect_lobby

	return *ppchServerAddress || *ppchLobbyID;
}
#endif

::gpk::error_t							setup							(::SApplication & app)						{
#ifdef USE_STEAM_CLIENT
	// if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the local Steam client and also launches this game again.
	// Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and removed steam_appid.txt from the game depot.
	ree_if(SteamAPI_RestartAppIfNecessary( k_uAppIdInvalid ), "%s", "");
	ree_if(!Steamworks_InitCEGLibrary(), "%s", "Steamworks_InitCEGLibrary() failed\n" );	// Init Steam CEG

	// Initialize SteamAPI, if this fails we bail out since we depend on Steam for lots of stuff.
	// You don't necessarily have to if you write your code to check whether all the Steam interfaces are NULL before using them and provide alternate paths when they are unavailable.
	// This will also load the in-game steam overlay dll into your process. That dll is normally injected by steam when it launches games, but by calling this you cause it to always load, even when not launched via steam.
	ree_if(!SteamAPI_Init(), "%s", "SteamAPI_Init() failed\n");
	SteamClient()->SetWarningMessageHook( &SteamAPIDebugTextHook );	// set our debug handler
	ree_if(!SteamUser()->BLoggedOn(), "%s", "Steam user is not logged in\n");	// Ensure that the user has logged into Steam. This will always return true if the game is launched from Steam, but if Steam is at the login prompt when you run your game from the debugger, it will return false.

	const char *pchServerAddress = 0, *pchLobbyID = 0;
	if ( !ParseCommandLine( app.Framework.RuntimeValues.PlatformDetail.EntryPointArgsWin.lpCmdLine, &pchServerAddress, &pchLobbyID ) ) {
		// no connect string on process command line. If app was launched via a Steam URL, the extra command line parameters in that URL get be retrieved with GetLaunchCommandLine. 
		// This way an attacker can't put malicious parameters in the process command line which might allow much more functionality then indented.
		char szCommandLine[1024] = {};
		if ( SteamApps()->GetLaunchCommandLine( szCommandLine, sizeof( szCommandLine ) ) > 0 )
			ParseCommandLine( szCommandLine, &pchServerAddress, &pchLobbyID );
	}

	Steamworks_SelfCheck();	// do a DRM self check
	ree_if(!SteamInput()->Init( false ), "%s", "SteamInput()->Init failed.\n" );

	char rgchCWD[1024] = {};
	if (!_getcwd(rgchCWD, sizeof(rgchCWD)))
		strcpy_s( rgchCWD, "." );

	char rgchFullPath[1024] = {};
#if defined(GPK_WINDOWS)
	sprintf_s(rgchFullPath, "%s/%s", rgchCWD, "steam_input_manifest.vdf" );
#elif defined(GPK_OSX)
	_snprintf( rgchFullPath, sizeof( rgchFullPath ), "%s/steamworksexample.app/Contents/Resources/%s", rgchCWD, "steam_input_manifest.vdf" );	// hack for now, because we do not have utility functions available for finding the resource path alternatively we could disable the SteamController init on OS X
#else
	_snprintf( rgchFullPath, sizeof( rgchFullPath ), "%s/%s", rgchCWD, "steam_input_manifest.vdf" );
#endif
	SteamInput()->SetInputActionManifestFilePath( rgchFullPath );

#endif // USE_STEAM_CLIENT

	//gpk_necall(::gpk::tcpipInitialize(), "Failed to initialize network subsystem: '%s'.", "Unknown error");

	::gpk::SFramework							& framework						= app.Framework;
	::gpk::SWindow								& mainWindow					= framework.MainDisplay;

	mainWindow.Size							= {1280, 720};

	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window. %s.", "why?!");

	srand((uint32_t)time(0));

	//::ohg::SOpenHellDrawCache					& drawCache						= app.OpenHellApp.Game.DrawCache;
	//::gpk::resize(4096 * 1024, drawCache.PixelCoords, drawCache.PixelVertexWeights, drawCache.LightColorsModel, drawCache.LightColorsWorld, drawCache.LightPointsModel, drawCache.LightPointsWorld);

	app.AudioState.InitAudio();
	app.AudioState.PrepareAudio("thrust.wav");

	//app.Client.AddressConnect											= {};
	//::gpk::tcpipAddress(9898, 0, ::gpk::TRANSPORT_PROTOCOL_UDP, app.Client.AddressConnect);	// If loading the remote IP from the json fails, we fall back to the local address.
	//app.Client.AddressConnect.IP[0] = 192;
	//app.Client.AddressConnect.IP[1] = 168;
	//app.Client.AddressConnect.IP[2] = 0;
	//app.Client.AddressConnect.IP[3] = 188;
	//::gpk::clientConnect(app.Client);

	//joySetThreshold(JOYSTICKID1, 1);
	//joySetThreshold(JOYSTICKID2, 1);
	//joySetCapture(framework.MainDisplay.PlatformDetail.WindowHandle, JOYSTICKID1, 10, 0);
	//joySetCapture(framework.MainDisplay.PlatformDetail.WindowHandle, JOYSTICKID2, 10, 0);

	framework.MainDisplay.Input->JoystickCurrent.resize(4, {});
	//for(uint32_t i = 0; i < 4; ++i) 
	//	app.GamePad.Resume();


	// create a rendering context  
	app.DrawingContext				= GetDC(framework.MainDisplay.PlatformDetail.WindowHandle);
	app.GLRenderContext				= wglCreateContext(app.DrawingContext); 
 
	// make it the calling thread's current rendering context 
	wglMakeCurrent (app.DrawingContext, app.GLRenderContext);
 
// call OpenGL APIs as desired ... 
 

	return 0;
}

int										update				(SApplication & app, bool exitSignal)	{
	::gpk::SFramework							& framework			= app.Framework;
	//::gpk::STimer								timer;
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "%s", "Exit requested by runtime.");

	for(uint32_t iEvent = 0; iEvent < framework.MainDisplay.EventQueue.size(); ++iEvent) {
		switch(framework.MainDisplay.EventQueue[iEvent].Type) {
		case ::gpk::SYSEVENT_ACTIVATE:
			break;
		case ::gpk::SYSEVENT_DEACTIVATE:
			break;
		case ::gpk::SYSEVENT_SYSKEY_DOWN:
		case ::gpk::SYSEVENT_KEY_DOWN:
			switch(framework.MainDisplay.EventQueue[iEvent].Data[0]) {
			case VK_RETURN:
				if(GetAsyncKeyState(VK_MENU) & 0xFFF0)
					gpk_necs(::gpk::fullScreenToggle(app.Framework.MainDisplay));
				break;
			}
		}
	}

	//if(1 == ::ghg::galaxyHellUpdate(app.OpenHellApp, framework.FrameInfo.Seconds.LastFrame, framework.Input, framework.MainDisplay.EventQueue))
	//	return ::gpk::APPLICATION_STATE_EXIT;

	//{
	//	::std::lock_guard<::std::mutex>			lockRTQueue	(app.OpenHellApp.RenderTargetLockQueue);
	//	if(app.OpenHellApp.RenderTargetQueue.size()) {
	//		::std::lock_guard<::std::mutex>			lockRTPool	(app.OpenHellApp.RenderTargetLockPool);
	//
	//		app.OpenHellApp.RenderTargetPool.push_back(app.Framework.BackBuffer);
	//		app.Framework.BackBuffer	= app.OpenHellApp.RenderTargetQueue[app.OpenHellApp.RenderTargetQueue.size() - 1];
	//		app.OpenHellApp.RenderTargetQueue.pop_back(0);
	//		for(uint32_t iRT = 0; iRT < app.OpenHellApp.RenderTargetQueue.size(); ++iRT) {
	//			app.OpenHellApp.RenderTargetPool.push_back(app.OpenHellApp.RenderTargetQueue[iRT]);
	//		}
	//		app.OpenHellApp.RenderTargetQueue.clear();
	//	}
	//}
	//app.AudioState.vListenerPos = app.OpenHellApp.Game.ShipState.Scene.Global.Camera[app.OpenHellApp.Game.ShipState.Scene.Global.CameraMode].Position; 
	//app.AudioState.listener.OrientTop	= {0, 1, 0}; 
	//app.AudioState.listener.OrientFront	= {1, 0, 0};  
	//
	//if(app.OpenHellApp.Game.ShipState.ShipCores.size())
	//	app.AudioState.vEmitterPos					= app.OpenHellApp.Game.ShipState.ShipPhysics.Transforms[app.OpenHellApp.Game.ShipState.EntitySystem.Entities[app.OpenHellApp.Game.ShipState.ShipCores[0].Entity].Body].Position;
	//
	//app.AudioState.vEmitterPos 			*= .03f;
	//app.AudioState.vListenerPos			*= .03f;
	//if(::ohg::CAMERA_MODE_SKY == app.OpenHellApp.Game.ShipState.Scene.Global.CameraMode) {
	//	app.AudioState.vEmitterPos .x				=  0;
	//	app.AudioState.vListenerPos.x				=  0;
	//}
	//
	//app.AudioState.emitter.DopplerScaler	= 6.0f; //float(app.OpenHellApp.World.PlayState.TimeScale);
	//app.AudioState.FrequencyRatio			= (float)app.OpenHellApp.Game.PlayState.TimeScale;
	//
	//if(app.OpenHellApp.ActiveState == ::ohg::APP_STATE_Play && false == app.OpenHellApp.Game.PlayState.Paused) {
	//	app.AudioState.PauseAudio(true);
	//	//app.GamePad.Resume();
	//}
	//else {
	//	app.AudioState.PauseAudio(false);
	//	//app.GamePad.Suspend();
	//}

	app.AudioState.UpdateAudio(framework.FrameInfo.Seconds.LastFrame);// / (app.OpenHellApp.World.ShipState.Ships.size() - 1));
#if defined(USE_STEAM_CLIENT)
	SteamAPI_RunCallbacks();
#endif

	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "%s", "Exit requested by framework update.");

	//for(uint32_t i = 0; i < 4; ++i) {
	//	DirectX::GamePad::State padState = app.GamePad.GetState(i);
	//	if(false == padState.IsConnected())
	//		warning_printf("Pad %i not connected", i);
	//	if(padState.IsLeftThumbStickDown	()) { 
	//		framework.MainDisplay.Input->JoystickCurrent[i].Deltas.y = -1; }
	//	if(padState.IsLeftThumbStickLeft	()) { 
	//		framework.MainDisplay.Input->JoystickCurrent[i].Deltas.x = -1; }
	//	if(padState.IsLeftThumbStickUp		()) { 
	//		framework.MainDisplay.Input->JoystickCurrent[i].Deltas.y = +1; }
	//	if(padState.IsLeftThumbStickRight	()) { 
	//		framework.MainDisplay.Input->JoystickCurrent[i].Deltas.x = +1; }
	//}
	
	//const GLubyte *	glGetString(GL_EXTENSIONS);
	return 0;
}

int														draw					(SApplication & /*app*/) {
    ///* rotate a triangle around */
    //glClear(GL_COLOR_BUFFER_BIT);
    //glBegin(GL_TRIANGLES);
    //glColor3f(1.0f, 0.0f, 0.0f);
    //glVertex2i(0,  1);
    //glColor3f(0.0f, 1.0f, 0.0f);
    //glVertex2i(-1, -1);
    //glColor3f(0.0f, 0.0f, 1.0f);
    //glVertex2i(1, -1);
    //glEnd();
    //glFlush();
	return 0; //::ohg::openHellDraw(app.OpenHellApp, app.Framework.MainDisplay.Size.Cast<uint16_t>());
}
