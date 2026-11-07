#include "gpk_engine_scene.h"

#ifndef GPK_POOL_SHADER_H_098273498237423
#define GPK_POOL_SHADER_H_098273498237423

namespace d1p
{
	::gpk::err_t	psTableCloth	(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	::gpk::err_t	psTableCushion	(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	::gpk::err_t	psPocket		(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	::gpk::err_t	psStick			(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	::gpk::err_t	psBallCue		(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	::gpk::err_t	psBallSolid		(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	::gpk::err_t	psBallStripped	(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
} // namespace 

#endif // GPK_POOL_SHADER_H_098273498237423