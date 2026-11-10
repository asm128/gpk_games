#include "lls.h"

#ifndef SSIEGE_HANDLE_H_23701
#define SSIEGE_HANDLE_H_23701

namespace lls
{
	gpk::err_t			handleWORLD_EVENT(::lls::LLSApp & world, const ::gpk::SEventView<lls::WORLD_EVENT> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);
	gpk::err_t			handleWORLD_ADMIN(::lls::LLSApp & world, const ::gpk::SEventView<lls::WORLD_ADMIN> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);
	gpk::err_t			handleACTION_CHAR(::lls::LLSApp & world, const ::gpk::SEventView<lls::ACTION_CHAR> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);
	stin	gpk::err_t	handleACT_SAILING(::lls::LLSApp & world, const ::gpk::SEventView<lls::ACT_SAILING> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; };
	stin	gpk::err_t	handleACT_ENGINES(::lls::LLSApp & world, const ::gpk::SEventView<lls::ACT_ENGINES> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stin	gpk::err_t	handleACT_AIRSHIP(::lls::LLSApp & world, const ::gpk::SEventView<lls::ACT_AIRSHIP> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stin	gpk::err_t	handleACT_WHEELED(::lls::LLSApp & world, const ::gpk::SEventView<lls::ACT_WHEELED> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	gpk::err_t			handleCLIENT_ASKS(::lls::LLSApp & world, const ::gpk::SEventView<lls::CLIENT_ASKS> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);
	gpk::err_t			handleWORLD_SETUP(::lls::LLSApp & world, const ::gpk::SEventView<lls::WORLD_SETUP> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);
	gpk::err_t			handleWORLD_VALUE(::lls::LLSApp & world, const ::gpk::SEventView<lls::WORLD_VALUE> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);
} // namespace

#endif // SSIEGE_HANDLE_H_23701
