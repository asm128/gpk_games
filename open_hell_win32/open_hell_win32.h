#include "audio.h"

#include "gpk_open_hell_app.h"

#include "gpk_framework.h"

#ifndef CED_DEMO_08_H_298837492837
#define CED_DEMO_08_H_298837492837


struct SApplication {
	::gpk::SFramework		Framework;

	::std::mutex			LockGUI;
	::std::mutex			LockRender;

	bool					DebugUIEnabled		= true;
	::gpk::SGUI				DebugUI				= {};
	AUDIO_STATE				AudioState;

	::ohg::SOpenHellApp		OpenHellApp;

	HDC						DrawingContext		= {};
	HGLRC					GLRenderContext		= {}; 
 
	//::gpk::SUDPClient		Client;

							SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
};

#endif // CED_DEMO_08_H_298837492837
