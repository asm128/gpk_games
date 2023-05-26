#include "gpk_pool_game.h"

#ifndef GPK_POOL_GAME_UPDATE_H_2348723524
#define GPK_POOL_GAME_UPDATE_H_2348723524

namespace d1p 
{
	::gpk::error_t	processInputEvents		(::d1p::SPoolGame & pool, ::gpk::view<const ::d1p::SEventPlayer> inputEvents, ::gpk::apobj<::d1p::SEventPool> & outputEvents);
	::gpk::error_t	poolGamePhysicsUpdate	(::d1p::SPoolGame & pool, ::gpk::apobj<::d1p::SEventPool> & outputEvents, double secondsElapsed);
	::gpk::error_t	evaluateTurnProgress	(::d1p::SPoolGame & pool, ::gpk::apobj<::d1p::SEventPool> & outputEvents, uint32_t eventOffset);

	::gpk::error_t	debugPrintMatchState	(const ::d1p::SMatchState & matchState);
	::gpk::error_t	debugPrintStickControl	(const ::d1p::SStickControl & stickControl);
	::gpk::error_t	debugPrintTurnInfo		(const ::d1p::STurnInfo & turnInfo);
} // namespace

#endif // GPK_POOL_GAME_UPDATE_H_2348723524