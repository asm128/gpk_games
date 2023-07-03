#include "ssiege_client.h"
#include "gpk_udp_client_app.h"
#include "gpk_framework.h"

//#define DISABLE_D3D11
//#define DISABLE_XAUDIO2

#ifndef DISABLE_D3D11
#	include "gpk_engine_d3d_app.h"
#endif
#ifndef DISABLE_XAUDIO2
#	include "gpk_xaudio2.h"
#endif

#ifndef CAMPP_CLIENT_WIN32_H_23701
#define CAMPP_CLIENT_WIN32_H_23701


struct SApplication {
	::gpk::SFramework		Framework	;
#ifndef DISABLE_D3D11
	::gpk::SD3DApplication	D3DApp			= {};
#endif
#ifndef DISABLE_XAUDIO2
	::gpk::AUDIO_STATE		AudioState		= {};
#endif
	::gpk::SClient			Client			= {};

	::ssiege::SCamppClient	CampApp			= {};

							SApplication	(::gpk::SRuntimeValues & runtimeValues)	noexcept
	: Framework(runtimeValues) { 
		if(!Framework.GUI) 
			Framework.GUI.create(); 

		CampApp.GUI				= Framework.GUI; 
		
		CampApp.World.Engine.Scene->Graphics.create(); 
	}
};

#endif // CAMPP_CLIENT_WIN32_H_23701
