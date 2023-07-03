#include "ssiege.h"
#include "ssiege_event.h"

#ifndef CAMPP_CLIENT_H_23701
#define CAMPP_CLIENT_H_23701

namespace ssiege
{
	struct SCamppClient : ::ssiege::SCampApp {
		::gpk::apobj<::ssiege::EventCampp>	EventsToSend;
		::gpk::apobj<::ssiege::EventCampp>	EventsReceived;
	};

	::gpk::error_t		handleCHAR_ACTION		(SCamppClient & app, const ::ssiege::EViewMinime & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleWORLD_ADMIN		(SCamppClient & app, const ::ssiege::EViewAdmin  & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleWORLD_EVENT		(SCamppClient & app, const ::ssiege::EViewWorld  & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleCLIENT_ASKS		(SCamppClient & app, const ::ssiege::EViewClient & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleWORLD_SETUP		(SCamppClient & app, const ::ssiege::EViewWorldSetup & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleWORLD_VALUE		(SCamppClient & app, const ::ssiege::EViewWorldValue & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);

	::gpk::error_t		ssiegeClientUpdate		(SCamppClient & ssiege, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SSystemEvent> systemEvents);
} // namespace 

#endif // CAMPP_CLIENT_H_23701
