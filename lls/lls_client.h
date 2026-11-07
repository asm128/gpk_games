#include "lls.h"
#include "lls_event.h"

#ifndef SSIEGE_CLIENT_H_23701
#define SSIEGE_CLIENT_H_23701

namespace lls
{
	struct LLSClient : ::lls::LLSApp {
		::gpk::apobj<::lls::EventLLS>	EventsToSend;
		::gpk::apobj<::lls::EventLLS>	EventsReceived;
	};

	::gpk::error_t			handleWORLD_EVENT		(::lls::LLSClient & world, const ::gpk::SEventView<lls::WORLD_EVENT> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents); // { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	::gpk::error_t			handleWORLD_ADMIN		(::lls::LLSClient & world, const ::gpk::SEventView<lls::WORLD_ADMIN> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents); // { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	::gpk::error_t			handleACTION_CHAR		(::lls::LLSClient & world, const ::gpk::SEventView<lls::ACTION_CHAR> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents); // { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stainli	::gpk::error_t	handleACT_SAILING		(::lls::LLSClient & world, const ::gpk::SEventView<lls::ACT_SAILING> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stainli	::gpk::error_t	handleACT_ENGINES		(::lls::LLSClient & world, const ::gpk::SEventView<lls::ACT_ENGINES> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stainli	::gpk::error_t	handleACT_AIRSHIP		(::lls::LLSClient & world, const ::gpk::SEventView<lls::ACT_AIRSHIP> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stainli	::gpk::error_t	handleACT_WHEELED		(::lls::LLSClient & world, const ::gpk::SEventView<lls::ACT_WHEELED> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	::gpk::error_t			handleCLIENT_ASKS		(::lls::LLSClient & world, const ::gpk::SEventView<lls::CLIENT_ASKS> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	::gpk::error_t			handleWORLD_SETUP		(::lls::LLSClient & world, const ::gpk::SEventView<lls::WORLD_SETUP> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	::gpk::error_t			handleWORLD_VALUE		(::lls::LLSClient & world, const ::gpk::SEventView<lls::WORLD_VALUE> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }

	::gpk::error_t		llsClientUpdate		(LLSClient & lls, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> systemEvents);
} // namespace 

#endif // SSIEGE_CLIENT_H_23701
