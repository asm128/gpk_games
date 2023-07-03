#include "ssiege.h"
#include "ssiege_event.h"

#ifndef CAMPP_SERVER_H_23701
#define CAMPP_SERVER_H_23701

namespace ssiege
{
	struct SCamppServerMessage { ::ssiege::campid_t Id; ::ssiege::EventCampp Event; };

	struct SCamppServer : ::ssiege::SCampApp {
		::gpk::apobj<::ssiege::EventCampp>	EventsToSend;
		::gpk::apobj<::ssiege::EventCampp>	EventsReceived;
	};

	::gpk::error_t		handleCHAR_ACTION		(SCamppServer & app, const ::ssiege::EViewMinime		& gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleWORLD_ADMIN		(SCamppServer & app, const ::ssiege::EViewAdmin		& gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleWORLD_EVENT		(SCamppServer & app, const ::ssiege::EViewWorld		& gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleCLIENT_ASKS		(SCamppServer & app, const ::ssiege::EViewClient		& gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleWORLD_SETUP		(SCamppServer & app, const ::ssiege::EViewWorldSetup	& gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleWORLD_VALUE		(SCamppServer & app, const ::ssiege::EViewWorldValue	& gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);

	::gpk::error_t		ssiegeServerUpdate		(SCamppServer & ssiege, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SSystemEvent> systemEvents);
} // namespace 

#endif // CAMPP_SERVER_H_23701
