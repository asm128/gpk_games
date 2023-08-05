#include "ssiege.h"

#ifndef SSIEGE_HANDLE_H_23701
#define SSIEGE_HANDLE_H_23701

namespace ssg
{
	::gpk::error_t			handleWORLD_EVENT(::ssg::SSiegeApp & world, const ::gpk::SEventView<ssg::WORLD_EVENT> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t			handleWORLD_ADMIN(::ssg::SSiegeApp & world, const ::gpk::SEventView<ssg::WORLD_ADMIN> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t			handleACTION_CHAR(::ssg::SSiegeApp & world, const ::gpk::SEventView<ssg::ACTION_CHAR> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	stainli	::gpk::error_t	handleACT_SAILING(::ssg::SSiegeApp & world, const ::gpk::SEventView<ssg::ACT_SAILING> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; };
	stainli	::gpk::error_t	handleACT_ENGINES(::ssg::SSiegeApp & world, const ::gpk::SEventView<ssg::ACT_ENGINES> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stainli	::gpk::error_t	handleACT_AIRSHIP(::ssg::SSiegeApp & world, const ::gpk::SEventView<ssg::ACT_AIRSHIP> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	stainli	::gpk::error_t	handleACT_WHEELED(::ssg::SSiegeApp & world, const ::gpk::SEventView<ssg::ACT_WHEELED> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; }
	::gpk::error_t			handleCLIENT_ASKS(::ssg::SSiegeApp & world, const ::gpk::SEventView<ssg::CLIENT_ASKS> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t			handleWORLD_SETUP(::ssg::SSiegeApp & world, const ::gpk::SEventView<ssg::WORLD_SETUP> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t			handleWORLD_VALUE(::ssg::SSiegeApp & world, const ::gpk::SEventView<ssg::WORLD_VALUE> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
} // namespace

#endif // SSIEGE_HANDLE_H_23701
