#include "ssiege.h"
#include "ssiege_event.h"

#ifndef SSIEGE_SERVER_H_23701
#define SSIEGE_SERVER_H_23701

namespace ssg
{
	struct SSSiegeServerMessage { ::ssg::ssiegeid_t Id; ::ssg::EventSSiege Event; };

	struct SSSiegeServer : ::ssg::SSSiegeApp {
		::gpk::apobj<::ssg::EventSSiege>	EventsToSend;
		::gpk::apobj<::ssg::EventSSiege>	EventsReceived;
	};

	::gpk::error_t		handleCHAR_ACTION		(SSSiegeServer & app, const ::ssg::EViewMinime		& gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleADMIN_WORLD		(SSSiegeServer & app, const ::ssg::EViewAdminWorld		& gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_EVENT		(SSSiegeServer & app, const ::ssg::EViewWorld		& gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleCLIENT_ASKS		(SSSiegeServer & app, const ::ssg::EViewClient		& gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_SETUP		(SSSiegeServer & app, const ::ssg::EViewWorldSetup	& gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_VALUE		(SSSiegeServer & app, const ::ssg::EViewWorldValue	& gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);

	::gpk::error_t		ssiegeServerUpdate		(SSSiegeServer & ssiege, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SSystemEvent> systemEvents);
} // namespace 

#endif // SSIEGE_SERVER_H_23701
