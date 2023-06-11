//#define DISABLE_D3D11
#ifndef DISABLE_D3D11
#	include "gpk_engine_d3d_app.h"
#endif
#ifndef DISABLE_XAUDIO2
#	include "gpk_xaudio2.h"
#endif

#include "gpk_d1.h"


#include "gpk_framework.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

struct SApplication {
	::gpk::SFramework		Framework		;
	::d1::SD1				D1			= {};
#ifndef DISABLE_D3D11
	::gpk::SD3DApplication	D3DApp			;
#endif
#ifndef DISABLE_XAUDIO2
	::gpk::AUDIO_STATE		AudioState;
#endif
							SApplication	(::gpk::SRuntimeValues & runtimeValues)	noexcept
		: Framework(runtimeValues) { 
		D1.MainGame.Pool.Engine.Scene->Graphics.create(); 
	}
};

#endif // APPLICATION_H_098273498237423
