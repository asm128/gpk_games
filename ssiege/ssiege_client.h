#include "ssiege.h"
#include "ssiege_event.h"

#ifndef SSIEGE_CLIENT_H_23701
#define SSIEGE_CLIENT_H_23701

namespace ssg
{
	struct SSiegeClient : ::ssg::SSiegeApp {
		::gpk::apobj<::ssg::EventSSiege>	EventsToSend;
		::gpk::apobj<::ssg::EventSSiege>	EventsReceived;
	};

	::gpk::error_t			handleWORLD_EVENT		(::ssg::SSiegeClient & world, const ::gpk::SEventView<ssg::WORLD_EVENT> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents); // { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	::gpk::error_t			handleWORLD_ADMIN		(::ssg::SSiegeClient & world, const ::gpk::SEventView<ssg::WORLD_ADMIN> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents); // { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	::gpk::error_t			handleACTION_CHAR		(::ssg::SSiegeClient & world, const ::gpk::SEventView<ssg::ACTION_CHAR> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents); // { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stainli	::gpk::error_t	handleACT_SAILING		(::ssg::SSiegeClient & world, const ::gpk::SEventView<ssg::ACT_SAILING> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stainli	::gpk::error_t	handleACT_ENGINES		(::ssg::SSiegeClient & world, const ::gpk::SEventView<ssg::ACT_ENGINES> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stainli	::gpk::error_t	handleACT_AIRSHIP		(::ssg::SSiegeClient & world, const ::gpk::SEventView<ssg::ACT_AIRSHIP> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stainli	::gpk::error_t	handleACT_WHEELED		(::ssg::SSiegeClient & world, const ::gpk::SEventView<ssg::ACT_WHEELED> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	::gpk::error_t			handleCLIENT_ASKS		(::ssg::SSiegeClient & world, const ::gpk::SEventView<ssg::CLIENT_ASKS> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	::gpk::error_t			handleWORLD_SETUP		(::ssg::SSiegeClient & world, const ::gpk::SEventView<ssg::WORLD_SETUP> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	::gpk::error_t			handleWORLD_VALUE		(::ssg::SSiegeClient & world, const ::gpk::SEventView<ssg::WORLD_VALUE> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);// { (void)world, (void)gameEvent, (void)outputEvents; return 0; }

	::gpk::error_t		ssiegeClientUpdate		(SSiegeClient & ssiege, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> systemEvents);
} // namespace 

#endif // SSIEGE_CLIENT_H_23701
