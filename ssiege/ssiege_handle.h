#include "ssiege.h"

#ifndef SSIEGE_HANDLE_H_23701
#define SSIEGE_HANDLE_H_23701

namespace ssg
{
	::gpk::error_t		handleCHAR_ACTION		(::ssg::SSSiegeApp & app, const ::ssg::EViewMinime		& gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleADMIN_WORLD		(::ssg::SSSiegeApp & app, const ::ssg::EViewAdminWorld	& gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_EVENT		(::ssg::SSSiegeApp & app, const ::ssg::EViewWorld			& gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleCLIENT_ASKS		(::ssg::SSSiegeApp & app, const ::ssg::EViewClient		& gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_SETUP		(::ssg::SSSiegeApp & app, const ::ssg::EViewWorldSetup	& gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
	::gpk::error_t		handleWORLD_VALUE		(::ssg::SSSiegeApp & app, const ::ssg::EViewWorldValue	& gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents);
} // namespace

#endif // SSIEGE_HANDLE_H_23701
