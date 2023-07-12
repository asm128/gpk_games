#include "ssiege_world.h"

#ifndef SSIEGE_WORLD_HANDLE_H_23701
#define SSIEGE_WORLD_HANDLE_H_23701

namespace ssiege
{
	::gpk::error_t		handleADMIN_WORLD		(::ssiege::SWorldView & world, const ::ssiege::EViewAdminWorld  & gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleCHAR_ACTION		(::ssiege::SWorldView & world, const ::ssiege::EViewMinime & gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_EVENT		(::ssiege::SWorldView & world, const ::ssiege::EViewWorld  & gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleCLIENT_ASKS		(::ssiege::SWorldView & world, const ::ssiege::EViewClient & gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_SETUP		(::ssiege::SWorldView & world, const ::ssiege::EViewWorldSetup & gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_VALUE		(::ssiege::SWorldView & world, const ::ssiege::EViewWorldValue & gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
} // namespace

#endif // SSIEGE_WORLD_HANDLE_H_23701
