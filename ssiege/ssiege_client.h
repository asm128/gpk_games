#include "ssiege.h"
#include "ssiege_event.h"

#ifndef SSIEGE_CLIENT_H_23701
#define SSIEGE_CLIENT_H_23701

namespace ssiege
{
	struct SSSiegeClient : ::ssiege::SSSiegeApp {
		::gpk::apobj<::ssiege::EventSSiege>	EventsToSend;
		::gpk::apobj<::ssiege::EventSSiege>	EventsReceived;
	};

	::gpk::error_t		handleCHAR_ACTION		(SSSiegeClient & app, const ::ssiege::EViewMinime & gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_ADMIN		(SSSiegeClient & app, const ::ssiege::EViewAdmin  & gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_EVENT		(SSSiegeClient & app, const ::ssiege::EViewWorld  & gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleCLIENT_ASKS		(SSSiegeClient & app, const ::ssiege::EViewClient & gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_SETUP		(SSSiegeClient & app, const ::ssiege::EViewWorldSetup & gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_VALUE		(SSSiegeClient & app, const ::ssiege::EViewWorldValue & gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);

	::gpk::error_t		ssiegeClientUpdate		(SSSiegeClient & ssiege, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SSystemEvent> systemEvents);
} // namespace 

#endif // SSIEGE_CLIENT_H_23701
