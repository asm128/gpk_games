#include "audio.h"

#include "gpk_galaxy_hell_app.h"

#include "gpk_framework.h"


#ifndef CED_DEMO_08_H_298837492837
#define CED_DEMO_08_H_298837492837


struct SApplication {
	::gpk::SFramework													Framework;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Offscreen					= {};
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	SwapOffscreen				= {};

	::std::mutex														LockGUI;
	::std::mutex														LockRender;

	bool																DebugUIEnabled				= true;
	::gpk::SGUI															DebugUI						= {};
	AUDIO_STATE															AudioState;

	::ghg::SGalaxyHellApp												GalaxyHellApp;

																	SApplication				(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
};

#endif // CED_DEMO_08_H_298837492837
