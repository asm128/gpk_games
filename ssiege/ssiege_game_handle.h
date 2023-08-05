#include "ssiege_game.h"

#ifndef SSIEGE_GAME_HANDLE_H
#define SSIEGE_GAME_HANDLE_H

namespace ssg
{
	::gpk::error_t			handleWORLD_EVENT(::ssg::SSiegeGame & world, const ::gpk::SEventView<ssg::WORLD_EVENT> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t			handleWORLD_ADMIN(::ssg::SSiegeGame & world, const ::gpk::SEventView<ssg::WORLD_ADMIN> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t			handleACTION_CHAR(::ssg::SSiegeGame & world, const ::gpk::SEventView<ssg::ACTION_CHAR> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	stainli	::gpk::error_t	handleACT_SAILING(::ssg::SSiegeGame & world, const ::gpk::SEventView<ssg::ACT_SAILING> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; };
	stainli	::gpk::error_t	handleACT_ENGINES(::ssg::SSiegeGame & world, const ::gpk::SEventView<ssg::ACT_ENGINES> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; };
	stainli	::gpk::error_t	handleACT_AIRSHIP(::ssg::SSiegeGame & world, const ::gpk::SEventView<ssg::ACT_AIRSHIP> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; };
	stainli	::gpk::error_t	handleACT_WHEELED(::ssg::SSiegeGame & world, const ::gpk::SEventView<ssg::ACT_WHEELED> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { (void)world, (void)gameEvent, (void)outputEvents; return 0; };
	::gpk::error_t			handleCLIENT_ASKS(::ssg::SSiegeGame & world, const ::gpk::SEventView<ssg::CLIENT_ASKS> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t			handleWORLD_SETUP(::ssg::SSiegeGame & world, const ::gpk::SEventView<ssg::WORLD_SETUP> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t			handleWORLD_VALUE(::ssg::SSiegeGame & world, const ::gpk::SEventView<ssg::WORLD_VALUE> & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
} // namespace

#endif // SSIEGE_GAME_HANDLE_H
