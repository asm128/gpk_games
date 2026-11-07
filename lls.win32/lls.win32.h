#include "lls_client.h"
#include "lls_server.h"

#include "gpk_udp_client_app.h"
#include "gpk_udp_server_app.h"
#include "gpk_framework.h"

//#define DISABLE_D3D11
#define DISABLE_XAUDIO2

#ifndef DISABLE_D3D11
#	include "gpk_engine_d3d_app.h"
#endif
#ifndef DISABLE_XAUDIO2
#	include "gpk_xaudio2.h"
#endif

#ifndef LLS_WIN32_H_23701
#define LLS_WIN32_H_23701

struct SClientSystem {
	::gpk::SClient			Network			= {};
	::lls::LLSClient		LLSApp			= {};
};

struct SServerSystem {
	::gpk::po<::gpk::SServer>	Network;
	::lls::LLSServer		LLSApp			= {};
};

struct SApplication {
	::gpk::SFramework		Framework;
#ifndef DISABLE_D3D11
	::gpk::SD3DApplication	D3DApp			= {};
#endif
#ifndef DISABLE_XAUDIO2
	::gpk::AUDIO_STATE		AudioState		= {};
#endif
	::SClientSystem			Client			= {};
	::SServerSystem			Server			= {};

							SApplication	(::gpk::SRuntimeValues & runtimeValues) noexcept
	: Framework(runtimeValues) {
		if(!Framework.GUI)
			Framework.GUI.create();
		Client.LLSApp.GUI		= Framework.GUI;
		Server.LLSApp.GUI		= Framework.GUI;
		es_if_failed(::gpk::rasterFontDefaults(Client.LLSApp.Game.Engine.Scene->Graphics->Fonts));
		es_if_failed(::gpk::rasterFontDefaults(Server.LLSApp.Game.Engine.Scene->Graphics->Fonts));
	}
};

#endif // LLS_WIN32_H_23701
