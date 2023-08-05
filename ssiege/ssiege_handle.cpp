#include "ssiege_handle.h"
#include "ssiege_event_args.h"

::gpk::error_t	ssg::handleCHAR_ACTION		(::ssg::SSSiegeApp & app, const ::ssg::EViewMinime & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)app; 
	const ::ssg::SArgsEvent	& eventArgs				= *(const ::ssg::SArgsEvent*)gameEvent.Data.begin();
	::ssg::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::CHAR_ACTION_Walk	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Turn	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Rush	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Hide	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Dash	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Jump	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Duck	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Warp	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Pick	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Grab	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Wear	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Use	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Spawn	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Shoot	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Throw	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Sleep	: gpk_warning_not_implemented(gameEvent); break;
	} 
	return 0; 
}

::gpk::error_t	ssg::handleADMIN_WORLD		(::ssg::SSSiegeApp & app, const ::ssg::EViewAdminWorld & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)app; 
	const ::ssg::SArgsEvent	& eventArgs				= *(const ::ssg::SArgsEvent*)gameEvent.Data.begin();
	::ssg::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Create		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Rename		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Delete		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Locate		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Rotate		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Resize		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Reskin		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Relocate		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Generate		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Initialize	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Reset		: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}

::gpk::error_t	ssg::handleWORLD_EVENT		(::ssg::SSSiegeApp & app, const ::ssg::EViewWorld & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)app; 
	const ::ssg::SArgsEvent	& eventArgs				= *(const ::ssg::SArgsEvent*)gameEvent.Data.begin();
	::ssg::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::WORLD_EVENT_Rain			: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_EVENT_Thunderstorm	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_EVENT_Earthquake	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_EVENT_Tornado		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_EVENT_Wave			: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_EVENT_Lightning		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_EVENT_Wildfire		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_EVENT_Flood			: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}

::gpk::error_t	ssg::handleCLIENT_ASKS		(::ssg::SSSiegeApp & app, const ::ssg::EViewClient & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)app; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::CLIENT_ASKS_Join: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::CLIENT_ASKS_Quit: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}

::gpk::error_t	ssg::handleWORLD_SETUP		(::ssg::SSSiegeApp & world, const ::ssg::EViewWorldSetup & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)world; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::WORLD_SETUP_Seed		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_SETUP_MaxPlayers: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_SETUP_BlockSize	: gpk_warning_not_implemented(gameEvent); break; 
	}
	return 0;
}

::gpk::error_t	ssg::handleWORLD_VALUE		(::ssg::SSSiegeApp & world, const ::ssg::EViewWorldValue & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)world; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::WORLD_VALUE_TimeCreated			: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_VALUE_TimeOffset			: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_VALUE_DaylightOffsetMinutes	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_VALUE_DaylightRatioExtra	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_VALUE_TimeLastSaved			: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_VALUE_WorldSize				: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_VALUE_TimeScale				: gpk_warning_not_implemented(gameEvent); break; 
	}
	return 0;
}
