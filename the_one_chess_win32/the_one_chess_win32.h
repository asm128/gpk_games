#include "resource.h"

#include "gpk_chess.h"

#include "gpk_framework.h"

//#define DISABLE_D3D11
//#define DISABLE_XAUDIO2

#ifndef DISABLE_D3D11
#	include "gpk_engine_d3d_app.h"
#endif

#ifndef DISABLE_XAUDIO2
#	include "gpk_xaudio2.h"
#endif

#ifndef THE_ONE_CHESS_WIN32_H
#define THE_ONE_CHESS_WIN32_H

struct SApplication {
	::gpk::SFramework		Framework	;
#ifndef DISABLE_D3D11
	::gpk::SD3DApplication	D3DApp			= {};
#endif
#ifndef DISABLE_XAUDIO2
	::gpk::AUDIO_STATE		AudioState		= {};
#endif

	::gpk::SChessApp		ChessApp		= {};

							SApplication	(::gpk::SRuntimeValues & runtimeValues)	noexcept	
	: Framework(runtimeValues) { 
		if(!Framework.GUI) 
			Framework.GUI.create(); 
		SSiegeApp.GUI				= Framework.GUI; 
		es_if_failed(::gpk::rasterFontDefaults(SSiegeApp.Game.Engine.Scene->Graphics->Fonts));
	}
};


#endif // 
