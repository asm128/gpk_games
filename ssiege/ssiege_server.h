#include "ssiege.h"
#include "ssiege_event.h"

#ifndef SSIEGE_SERVER_H_23701
#define SSIEGE_SERVER_H_23701

namespace ssg
{
	struct SSiegeServerMessage { ::ssg::ssiegeid_t Id; ::ssg::EventSSiege Event; };

	struct SSiegeServer : ::ssg::SSiegeApp {
		::gpk::apobj<::ssg::EventSSiege>	EventsToSend;
		::gpk::apobj<::ssg::EventSSiege>	EventsReceived;
	};

	::gpk::error_t			handleWORLD_EVENT		(::ssg::SSiegeServer & world, const ::gpk::SEventView<ssg::WORLD_EVENT> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	::gpk::error_t			handleWORLD_ADMIN		(::ssg::SSiegeServer & world, const ::gpk::SEventView<ssg::WORLD_ADMIN> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	::gpk::error_t			handleACTION_CHAR		(::ssg::SSiegeServer & world, const ::gpk::SEventView<ssg::ACTION_CHAR> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stainli	::gpk::error_t	handleACT_SAILING		(::ssg::SSiegeServer & world, const ::gpk::SEventView<ssg::ACT_SAILING> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stainli	::gpk::error_t	handleACT_ENGINES		(::ssg::SSiegeServer & world, const ::gpk::SEventView<ssg::ACT_ENGINES> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stainli	::gpk::error_t	handleACT_AIRSHIP		(::ssg::SSiegeServer & world, const ::gpk::SEventView<ssg::ACT_AIRSHIP> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stainli	::gpk::error_t	handleACT_WHEELED		(::ssg::SSiegeServer & world, const ::gpk::SEventView<ssg::ACT_WHEELED> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	::gpk::error_t			handleCLIENT_ASKS		(::ssg::SSiegeServer & world, const ::gpk::SEventView<ssg::CLIENT_ASKS> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	::gpk::error_t			handleWORLD_SETUP		(::ssg::SSiegeServer & world, const ::gpk::SEventView<ssg::WORLD_SETUP> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	::gpk::error_t			handleWORLD_VALUE		(::ssg::SSiegeServer & world, const ::gpk::SEventView<ssg::WORLD_VALUE> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }

	::gpk::error_t		ssiegeServerUpdate		(SSiegeServer & ssiege, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> systemEvents);
} // namespace 

#endif // SSIEGE_SERVER_H_23701
