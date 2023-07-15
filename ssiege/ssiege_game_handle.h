#include "ssiege_game.h"

#ifndef SSIEGE_GAME_HANDLE_H
#define SSIEGE_GAME_HANDLE_H

namespace ssg
{
	::gpk::error_t		handleADMIN_WORLD		(::ssg::SSiegeGame & world, const ::ssiege::EViewAdminWorld & gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleCHAR_ACTION		(::ssg::SSiegeGame & world, const ::ssiege::EViewMinime		& gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_EVENT		(::ssg::SSiegeGame & world, const ::ssiege::EViewWorld		& gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleCLIENT_ASKS		(::ssg::SSiegeGame & world, const ::ssiege::EViewClient		& gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_SETUP		(::ssg::SSiegeGame & world, const ::ssiege::EViewWorldSetup & gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_VALUE		(::ssg::SSiegeGame & world, const ::ssiege::EViewWorldValue & gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
} // namespace

#endif // SSIEGE_GAME_HANDLE_H
