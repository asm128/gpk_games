#include "gpk_pool_game.h"

#ifndef GPK_POOL_GAME_UPDATE_H_2348723524
#define GPK_POOL_GAME_UPDATE_H_2348723524

namespace d1 
{
	::gpk::error_t	processInputEvents		(::d1::SPoolGame & pool, ::gpk::view<const ::d1::SEventPlayer> inputEvents, ::gpk::apobj<::d1::SEventPool> & outputEvents);
	::gpk::error_t	poolGamePhysicsUpdate	(::d1::SPoolGame & pool, ::gpk::apobj<::d1::SEventPool> & outputEvents, double secondsElapsed);
	::gpk::error_t	evaluateTurnProgress	(::d1::SPoolGame & pool, ::gpk::apobj<::d1::SEventPool> & outputEvents, uint32_t eventOffset);

	::gpk::error_t	debugPrintMatchState	(const ::d1::SPoolMatchState & matchState);
	::gpk::error_t	debugPrintTurnState		(const ::d1::SPoolTurnState & turnState);
	::gpk::error_t	debugPrintStickControl	(const ::d1::SStickControl & stickControl);
	::gpk::error_t	debugPrintTurnInfo		(const ::d1::SPoolTurnInfo & turnInfo);
} // namespace

#endif // GPK_POOL_GAME_UPDATE_H_2348723524