#include "gpk_xaudio2.h"

#include "gpk_galaxy_hell_app.h"

#include "gpk_framework.h"

//#include "gpk_udp_client_app.h"

#ifndef CED_DEMO_08_H_298837492837
#define CED_DEMO_08_H_298837492837

struct SApplication {
	::gpk::SFramework		Framework;

	::std::mutex			LockGUI;
	::std::mutex			LockRender;

	::gpk::AUDIO_STATE		AudioState;

	::ghg::SGalaxyHellApp	GalaxyHellApp;

	HDC						DrawingContext		= {};
	HGLRC					GLRenderContext	= {}; 
 
	//::gpk::SClient		Client;

							SApplication		(::gpk::SRuntimeValues & runtimeValues)	: Framework(runtimeValues)		{ GalaxyHellApp.Game.ShipState.Engine.Scene->Graphics.create(); }
};

#endif // CED_DEMO_08_H_298837492837
