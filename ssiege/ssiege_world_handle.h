#include "ssiege_world.h"

#ifndef CAMPP_WORLD_HANDLE_H_23701
#define CAMPP_WORLD_HANDLE_H_23701

namespace ssiege
{
	::gpk::error_t		handleCHAR_ACTION		(::ssiege::SWorldView & world, const ::ssiege::EViewMinime & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleWORLD_ADMIN		(::ssiege::SWorldView & world, const ::ssiege::EViewAdmin  & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleWORLD_EVENT		(::ssiege::SWorldView & world, const ::ssiege::EViewWorld  & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleCLIENT_ASKS		(::ssiege::SWorldView & world, const ::ssiege::EViewClient & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleWORLD_SETUP		(::ssiege::SWorldView & world, const ::ssiege::EViewWorldSetup & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleWORLD_VALUE		(::ssiege::SWorldView & world, const ::ssiege::EViewWorldValue & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
} // namespace

#endif // CAMPP_WORLD_HANDLE_H_23701
