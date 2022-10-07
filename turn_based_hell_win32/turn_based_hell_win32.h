#include "gpk_turn_based_hell.h"
#include "gpk_framework.h"

#ifndef CED_DEMO_08_H_298837492837
#define CED_DEMO_08_H_298837492837


struct SApplication {
	::gpk::SFramework													Framework;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Offscreen					= {};
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	SwapOffscreen				= {};

	int32_t																IdExit						= -1;

	::std::mutex														LockGUI;
	::std::mutex														LockRender;

	::ghg::STurnBasedHellApp											TurnBasedHellApp;

																	SApplication				(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
};

#endif // CED_DEMO_08_H_298837492837
