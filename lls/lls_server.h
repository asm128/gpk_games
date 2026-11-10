#include "lls.h"
#include "lls_event.h"

#ifndef SSIEGE_SERVER_H_23701
#define SSIEGE_SERVER_H_23701

namespace lls
{
	stct LLSServerMessage { ::lls::llsid_t Id; ::lls::EventLLS Event; };

	stct LLSServer : ::lls::LLSApp {
		::gpk::apobj<::lls::EventLLS>	EventsToSend;
		::gpk::apobj<::lls::EventLLS>	EventsReceived;
	};

	gpk::err_t		handleWORLD_EVENT		(::lls::LLSServer & world, const ::gpk::SEventView<lls::WORLD_EVENT> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	gpk::err_t		handleWORLD_ADMIN		(::lls::LLSServer & world, const ::gpk::SEventView<lls::WORLD_ADMIN> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	gpk::err_t		handleACTION_CHAR		(::lls::LLSServer & world, const ::gpk::SEventView<lls::ACTION_CHAR> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stin gpk::err_t	handleACT_SAILING		(::lls::LLSServer & world, const ::gpk::SEventView<lls::ACT_SAILING> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stin gpk::err_t	handleACT_ENGINES		(::lls::LLSServer & world, const ::gpk::SEventView<lls::ACT_ENGINES> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stin gpk::err_t	handleACT_AIRSHIP		(::lls::LLSServer & world, const ::gpk::SEventView<lls::ACT_AIRSHIP> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stin gpk::err_t	handleACT_WHEELED		(::lls::LLSServer & world, const ::gpk::SEventView<lls::ACT_WHEELED> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	gpk::err_t		handleCLIENT_ASKS		(::lls::LLSServer & world, const ::gpk::SEventView<lls::CLIENT_ASKS> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	gpk::err_t		handleWORLD_SETUP		(::lls::LLSServer & world, const ::gpk::SEventView<lls::WORLD_SETUP> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	gpk::err_t		handleWORLD_VALUE		(::lls::LLSServer & world, const ::gpk::SEventView<lls::WORLD_VALUE> & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }

	gpk::err_t		llsServerUpdate		(LLSServer & lls, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> systemEvents);
} // namespace 

#endif // SSIEGE_SERVER_H_23701
