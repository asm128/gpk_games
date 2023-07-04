#include "ssiege.h"

#ifndef SSIEGE_HANDLE_H_23701
#define SSIEGE_HANDLE_H_23701

namespace ssiege
{
	::gpk::error_t		handleCHAR_ACTION		(::ssiege::SSSiegeApp & app, const ::ssiege::EViewMinime		& gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_ADMIN		(::ssiege::SSSiegeApp & app, const ::ssiege::EViewAdmin			& gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_EVENT		(::ssiege::SSSiegeApp & app, const ::ssiege::EViewWorld			& gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleCLIENT_ASKS		(::ssiege::SSSiegeApp & app, const ::ssiege::EViewClient		& gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_SETUP		(::ssiege::SSSiegeApp & app, const ::ssiege::EViewWorldSetup	& gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_VALUE		(::ssiege::SSSiegeApp & app, const ::ssiege::EViewWorldValue	& gameEvent, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents);
} // namespace

#endif // SSIEGE_HANDLE_H_23701
