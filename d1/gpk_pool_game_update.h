#include "gpk_pool_game.h"

#ifndef GPK_POOL_GAME_UPDATE_H_2348723524
#define GPK_POOL_GAME_UPDATE_H_2348723524

namespace d1p 
{
	::gpk::err_t	processInputEvents		(::d1p::SPoolGame & pool, ::gpk::view<const ::d1p::SEventPlayer> inputEvents, ::gpk::apobj<::d1p::SEventPool> & outputEvents);
	::gpk::err_t	poolGamePhysicsUpdate	(::d1p::SPoolGame & pool, ::gpk::apobj<::d1p::SEventPool> & outputEvents, double secondsElapsed);
	::gpk::err_t	evaluateTurnProgress	(::d1p::SPoolGame & pool, ::gpk::apobj<::d1p::SEventPool> & outputEvents, u2_t  eventOffset);

	::gpk::err_t	debugPrintMatchState	(const ::d1p::SMatchState & matchState);
	::gpk::err_t	debugPrintStickControl	(const ::d1p::SStickControl & stickControl);
	::gpk::err_t	debugPrintTurnInfo		(const ::d1p::STurnInfo & turnInfo);
} // namespace

#endif // GPK_POOL_GAME_UPDATE_H_2348723524