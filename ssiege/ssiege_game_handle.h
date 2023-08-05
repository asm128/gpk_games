#include "ssiege_game.h"

#ifndef SSIEGE_GAME_HANDLE_H
#define SSIEGE_GAME_HANDLE_H

namespace ssg
{
	::gpk::error_t		handleADMIN_WORLD		(::ssg::SSiegeGame & world, const ::ssg::EViewAdminWorld & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleCHAR_ACTION		(::ssg::SSiegeGame & world, const ::ssg::EViewMinime		& gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_EVENT		(::ssg::SSiegeGame & world, const ::ssg::EViewWorld		& gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleCLIENT_ASKS		(::ssg::SSiegeGame & world, const ::ssg::EViewClient		& gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_SETUP		(::ssg::SSiegeGame & world, const ::ssg::EViewWorldSetup & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_VALUE		(::ssg::SSiegeGame & world, const ::ssg::EViewWorldValue & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
} // namespace

#endif // SSIEGE_GAME_HANDLE_H
