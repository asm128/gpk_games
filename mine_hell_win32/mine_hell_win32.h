#include "gpk_mine_hell.h"

#include "gpk_rigidbody.h"
#include "gpk_model.h"
#include "gpk_image.h"
#include "gpk_framework.h"

#ifndef CED_DEMO_08_H_298837492837
#define CED_DEMO_08_H_298837492837


struct SApplication {
				::gpk::SFramework													Framework;
				::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Offscreen					= {};

				int32_t																IdExit						= -1;
				::gpk::array_pod<int32_t>											IdMineMapDialog				= {};
				::gpk::SImage<int32_t>												IdMineMapCells				= {};

				::std::mutex														LockGUI;
				::std::mutex														LockRender;

				::gpkg::SMineHell													MineHell					= {};

																				SApplication				(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
};

#endif // CED_DEMO_08_H_298837492837
