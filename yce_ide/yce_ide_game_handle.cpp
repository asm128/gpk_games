#include "yce_ide_game_handle.h"
#include "yce_ide_event_args.h"

::gpk::error_t	ssg::handleACTION_CHAR		(::ssg::SSiegeGame & world, const ::ssg::EViewMe & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)world; 
	const ::ssg::SArgsEvent	& eventArgs		= *(const ::ssg::SArgsEvent*)gameEvent.Data.begin();
	::ssg::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::ACTION_CHAR_Walk	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::ACTION_CHAR_Turn	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::ACTION_CHAR_Rush	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::ACTION_CHAR_Hide	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::ACTION_CHAR_Dash	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::ACTION_CHAR_Jump	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::ACTION_CHAR_Duck	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::ACTION_CHAR_Warp	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::ACTION_CHAR_Pick	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::ACTION_CHAR_Grab	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::ACTION_CHAR_Wear	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::ACTION_CHAR_Use		: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::ACTION_CHAR_Spawn	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::ACTION_CHAR_Shoot	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::ACTION_CHAR_Throw	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::ACTION_CHAR_Sleep	: gpk_warning_not_implemented(gameEvent); break;
	}
	return 0; 
}
::gpk::error_t	ssg::handleWORLD_ADMIN		(::ssg::SSiegeGame & world, const ::ssg::EViewAdminWorld & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)world; 
	const ::ssg::SArgsEvent	& eventArgs			= *(const ::ssg::SArgsEvent*)gameEvent.Data.begin();
	::ssg::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Create		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Rename		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Delete		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Locate		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Rotate		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Resize		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Reskin		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Relocate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Generate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Reset		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Initialize	: 
		gpk_necs(::ssg::ssiegeGameSetup(world));
		break; 
	} 
	return 0; 
}
::gpk::error_t	ssg::handleWORLD_EVENT		(::ssg::SSiegeGame & world, const ::ssg::EViewWorld & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)world; 
	const ::ssg::SArgsEvent	& eventArgs			= *(const ::ssg::SArgsEvent*)gameEvent.Data.begin();
	::ssg::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::WORLD_EVENT_Tick		: 
		//gpk_warning_not_implemented(gameEvent); 
		break; 
	case ::ssg::WORLD_EVENT_Rain		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_EVENT_Thunderstorm: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_EVENT_Earthquake	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_EVENT_Tornado		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_EVENT_Wave		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_EVENT_Lightning	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_EVENT_Wildfire	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_EVENT_Flood		: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}
::gpk::error_t	ssg::handleCLIENT_ASKS		(::ssg::SSiegeGame & world, const ::ssg::EViewClient & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)world;
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::CLIENT_ASKS_Join	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::CLIENT_ASKS_Quit	: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}

::gpk::error_t	ssg::handleWORLD_SETUP		(::ssg::SSiegeGame & world, const ::ssg::EViewWorldSetup & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)world; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::WORLD_SETUP_Seed		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_SETUP_MaxPlayers	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_SETUP_BlockSize	: gpk_warning_not_implemented(gameEvent); break; 
	}
	return 0;
}

::gpk::error_t	ssg::handleWORLD_VALUE		(::ssg::SSiegeGame & world, const ::ssg::EViewWorldValue & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)world; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::WORLD_VALUE_TimeCreated				: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_VALUE_TimeOffset				: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_VALUE_DaylightOffsetMinutes	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_VALUE_DaylightRatioExtra		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_VALUE_TimeLastSaved			: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_VALUE_WorldSize				: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_VALUE_TimeScale				: gpk_warning_not_implemented(gameEvent); break; 
	}
	return 0;
}
