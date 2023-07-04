#include "ssiege.h"
#include "ssiege_event.h"

#ifndef SSIEGE_SERVER_H_23701
#define SSIEGE_SERVER_H_23701

namespace ssiege
{
	struct SSSiegeServerMessage { ::ssiege::campid_t Id; ::ssiege::EventSSiege Event; };

	struct SSSiegeServer : ::ssiege::SSSiegeApp {
		::gpk::apobj<::ssiege::EventSSiege>	EventsToSend;
		::gpk::apobj<::ssiege::EventSSiege>	EventsReceived;
	};

	::gpk::error_t		handleCHAR_ACTION		(SSSiegeServer & app, const ::ssiege::EViewMinime		& gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_ADMIN		(SSSiegeServer & app, const ::ssiege::EViewAdmin		& gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_EVENT		(SSSiegeServer & app, const ::ssiege::EViewWorld		& gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleCLIENT_ASKS		(SSSiegeServer & app, const ::ssiege::EViewClient		& gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_SETUP		(SSSiegeServer & app, const ::ssiege::EViewWorldSetup	& gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_VALUE		(SSSiegeServer & app, const ::ssiege::EViewWorldValue	& gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);

	::gpk::error_t		ssiegeServerUpdate		(SSSiegeServer & ssiege, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SSystemEvent> systemEvents);
} // namespace 

#endif // SSIEGE_SERVER_H_23701
