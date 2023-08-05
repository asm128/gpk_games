#include "ssiege.h"
#include "ssiege_event.h"

#ifndef SSIEGE_CLIENT_H_23701
#define SSIEGE_CLIENT_H_23701

namespace ssg
{
	struct SSSiegeClient : ::ssg::SSSiegeApp {
		::gpk::apobj<::ssg::EventSSiege>	EventsToSend;
		::gpk::apobj<::ssg::EventSSiege>	EventsReceived;
	};

	::gpk::error_t		handleCHAR_ACTION		(SSSiegeClient & app, const ::ssg::EViewMinime & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleADMIN_WORLD		(SSSiegeClient & app, const ::ssg::EViewAdminWorld  & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_EVENT		(SSSiegeClient & app, const ::ssg::EViewWorld  & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleCLIENT_ASKS		(SSSiegeClient & app, const ::ssg::EViewClient & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_SETUP		(SSSiegeClient & app, const ::ssg::EViewWorldSetup & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_VALUE		(SSSiegeClient & app, const ::ssg::EViewWorldValue & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);

	::gpk::error_t		ssiegeClientUpdate		(SSSiegeClient & ssiege, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SSystemEvent> systemEvents);
} // namespace 

#endif // SSIEGE_CLIENT_H_23701
