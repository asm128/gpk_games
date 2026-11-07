#include "lls_game_handle.h"
#include "lls_event_args.h"

using ::gpk::get_value_namep, ::gpk::get_enum_namep, ::gpk::failed;
GPK_USING_TYPEINT();

::gpk::error_t	lls::handleACTION_CHAR		(::lls::LLSGame & world, const ::lls::EViewMe & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)world; 
	const ::lls::SArgsEvent	& eventArgs		= *(const ::lls::SArgsEvent*)gameEvent.Data.begin();
	::lls::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::ACTION_CHAR_Walk	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Turn	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Rush	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Hide	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Dash	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Jump	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Duck	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Warp	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Pick	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Grab	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Wear	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Use		: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Spawn	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Shoot	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Throw	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Sleep	: gpk_warning_not_implemented(gameEvent); break;
	}
	return 0; 
}
::gpk::error_t	lls::handleWORLD_ADMIN		(::lls::LLSGame & world, const ::lls::EViewAdminWorld & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)world; 
	const ::lls::SArgsEvent	& eventArgs			= *(const ::lls::SArgsEvent*)gameEvent.Data.begin();
	::lls::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Create		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Rename		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Delete		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Locate		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Rotate		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Resize		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Reskin		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Relocate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Generate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Reset		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Initialize	: 
		gpk_necs(::lls::llsGameSetup(world));
		break; 
	} 
	return 0; 
}
::gpk::error_t	lls::handleWORLD_EVENT		(::lls::LLSGame & world, const ::lls::EViewWorld & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)world; 
	const ::lls::SArgsEvent	& eventArgs			= *(const ::lls::SArgsEvent*)gameEvent.Data.begin();
	::lls::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::WORLD_EVENT_Tick		: 
		//gpk_warning_not_implemented(gameEvent); 
		break; 
	case ::lls::WORLD_EVENT_Rain		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Thunderstorm: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Earthquake	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Tornado		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Wave		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Lightning	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Wildfire	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Flood		: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}
::gpk::error_t	lls::handleCLIENT_ASKS		(::lls::LLSGame & world, const ::lls::EViewClient & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)world;
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::CLIENT_ASKS_Join	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::CLIENT_ASKS_Quit	: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}

::gpk::error_t	lls::handleWORLD_SETUP		(::lls::LLSGame & world, const ::lls::EViewWorldSetup & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)world; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::WORLD_SETUP_Seed		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_SETUP_MaxPlayers	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_SETUP_BlockSize	: gpk_warning_not_implemented(gameEvent); break; 
	}
	return 0;
}

::gpk::error_t	lls::handleWORLD_VALUE		(::lls::LLSGame & world, const ::lls::EViewWorldValue & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)world; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::WORLD_VALUE_TimeCreated				: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_TimeOffset				: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_DaylightOffsetMinutes	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_DaylightRatioExtra		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_TimeLastSaved			: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_WorldSize				: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_TimeScale				: gpk_warning_not_implemented(gameEvent); break; 
	}
	return 0;
}
