#include "ssiege.h"

#ifndef CAMPP_HANDLE_H_23701
#define CAMPP_HANDLE_H_23701

namespace ssiege
{
	::gpk::error_t		handleCHAR_ACTION		(::ssiege::SCampApp & app, const ::ssiege::EViewMinime		& gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleWORLD_ADMIN		(::ssiege::SCampApp & app, const ::ssiege::EViewAdmin			& gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleWORLD_EVENT		(::ssiege::SCampApp & app, const ::ssiege::EViewWorld			& gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleCLIENT_ASKS		(::ssiege::SCampApp & app, const ::ssiege::EViewClient		& gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleWORLD_SETUP		(::ssiege::SCampApp & app, const ::ssiege::EViewWorldSetup	& gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
	::gpk::error_t		handleWORLD_VALUE		(::ssiege::SCampApp & app, const ::ssiege::EViewWorldValue	& gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents);
} // namespace

#endif // CAMPP_HANDLE_H_23701
