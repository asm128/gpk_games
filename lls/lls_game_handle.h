#include "lls_game.h"

#ifndef SSIEGE_GAME_HANDLE_H
#define SSIEGE_GAME_HANDLE_H

namespace lls
{
	gpk::err_t			handleWORLD_EVENT(::lls::LLSGame & world, const ::gpk::SEventView<lls::WORLD_EVENT> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);
	gpk::err_t			handleWORLD_ADMIN(::lls::LLSGame & world, const ::gpk::SEventView<lls::WORLD_ADMIN> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);
	gpk::err_t			handleACTION_CHAR(::lls::LLSGame & world, const ::gpk::SEventView<lls::ACTION_CHAR> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);
	stin	gpk::err_t	handleACT_SAILING(::lls::LLSGame & world, const ::gpk::SEventView<lls::ACT_SAILING> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; };
	stin	gpk::err_t	handleACT_ENGINES(::lls::LLSGame & world, const ::gpk::SEventView<lls::ACT_ENGINES> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; };
	stin	gpk::err_t	handleACT_AIRSHIP(::lls::LLSGame & world, const ::gpk::SEventView<lls::ACT_AIRSHIP> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; };
	stin	gpk::err_t	handleACT_WHEELED(::lls::LLSGame & world, const ::gpk::SEventView<lls::ACT_WHEELED> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; };
	gpk::err_t			handleCLIENT_ASKS(::lls::LLSGame & world, const ::gpk::SEventView<lls::CLIENT_ASKS> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);
	gpk::err_t			handleWORLD_SETUP(::lls::LLSGame & world, const ::gpk::SEventView<lls::WORLD_SETUP> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);
	gpk::err_t			handleWORLD_VALUE(::lls::LLSGame & world, const ::gpk::SEventView<lls::WORLD_VALUE> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);
} // namespace

#endif // SSIEGE_GAME_HANDLE_H
