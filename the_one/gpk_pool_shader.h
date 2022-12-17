#include "gpk_engine_scene.h"

#ifndef GPK_POOL_DRAW_H_098273498237423
#define GPK_POOL_DRAW_H_098273498237423

namespace the1
{
	int32_t												shaderBall				
		( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
		, ::gpk::view_grid<uint32_t>			backBufferDepth
		, ::gpk::SEngineRenderCache				& renderCache
		, const ::gpk::SEngineScene				& scene
		, const ::gpk::SEngineSceneConstants	& constants
		, int32_t								iRenderNode
		);

	int32_t												shaderHole
		( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
		, ::gpk::view_grid<uint32_t>			backBufferDepth
		, ::gpk::SEngineRenderCache				& renderCache
		, const ::gpk::SEngineScene				& scene
		, const ::gpk::SEngineSceneConstants	& constants
		, int32_t								iRenderNode
		);

	int32_t												shaderStick
		( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
		, ::gpk::view_grid<uint32_t>			backBufferDepth
		, ::gpk::SEngineRenderCache				& renderCache
		, const ::gpk::SEngineScene				& scene
		, const ::gpk::SEngineSceneConstants	& constants
		, int32_t								iRenderNode
		);

	int32_t												shaderCloth
		( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
		, ::gpk::view_grid<uint32_t>			backBufferDepth
		, ::gpk::SEngineRenderCache				& renderCache
		, const ::gpk::SEngineScene				& scene
		, const ::gpk::SEngineSceneConstants	& constants
		, int32_t								iRenderNode
		);

} // namespace 

#endif // GPK_POOL_DRAW_H_098273498237423