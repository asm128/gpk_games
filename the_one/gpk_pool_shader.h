#include "gpk_engine_scene.h"

#ifndef GPK_POOL_DRAW_H_098273498237423
#define GPK_POOL_DRAW_H_098273498237423

namespace the1
{
	::gpk::error_t	psTableCloth	(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::SColorBGRA & outputPixel);
	::gpk::error_t	psPocket		(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::SColorBGRA & outputPixel);
	::gpk::error_t	psStick			(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::SColorBGRA & outputPixel);
	::gpk::error_t	psBallCue		(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::SColorBGRA & outputPixel);
	::gpk::error_t	psBallSolid		(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::SColorBGRA & outputPixel);
	::gpk::error_t	psBallStripped	(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::SColorBGRA & outputPixel);
} // namespace 

#endif // GPK_POOL_DRAW_H_098273498237423