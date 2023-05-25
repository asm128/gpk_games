//#define DISABLE_D3D11
#ifndef DISABLE_D3D11
#	include "d1_d3d.h"
#endif
#ifndef DISABLE_XAUDIO2
#	include "audio.h"
#endif

#include "gpk_d1.h"


#include "gpk_framework.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

struct SApplication {
	::gpk::SFramework			Framework		;
	::d1::SD1				TheOne			= {};
#ifndef DISABLE_D3D11
	::d1_win32::SD3DApplication	D3DApp			;
#endif
#ifndef DISABLE_XAUDIO2
	AUDIO_STATE					AudioState;
#endif
								SApplication	(::gpk::SRuntimeValues & runtimeValues)	noexcept
		: Framework(runtimeValues) { 
		TheOne.MainGame.Pool.Engine.Scene->Graphics.create(); 
	}
};

#endif // APPLICATION_H_098273498237423
