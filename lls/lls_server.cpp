#include "lls_server.h"
#include "lls_event_args.h"

using ::gpk::get_value_namep, ::gpk::get_enum_namep, ::gpk::failed;
GPK_USING_TYPEINT();

gpk::err_t	lls::llsServerUpdate		(::lls::LLSServer & app, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> systemEvents) { 
	gpk_necs(::lls::llsUpdate(app, lastTimeSeconds, inputState, systemEvents, [&app](::gpk::pobj<::lls::EventLLS> & _eventToProcess, ::gpk::apobj<::lls::EventLLS> & outputEvents) {
		if(!_eventToProcess)
			return false;

		const ::lls::EventLLS	& eventToProcess		= *_eventToProcess;
		info_printf("%s", ::gpk::get_value_namep(eventToProcess.Type));

		gpk::err_t			result			= 0; 
		switch(eventToProcess.Type) {
		CASE_LLS_EVENT(result, app, WORLD_EVENT, outputEvents); 
		CASE_LLS_EVENT(result, app, WORLD_ADMIN, outputEvents); 
		CASE_LLS_EVENT(result, app, ACT_SAILING, outputEvents); 
		CASE_LLS_EVENT(result, app, ACTION_CHAR, outputEvents); 
		CASE_LLS_EVENT(result, app, ACT_ENGINES, outputEvents); 
		CASE_LLS_EVENT(result, app, ACT_AIRSHIP, outputEvents); 
		CASE_LLS_EVENT(result, app, ACT_WHEELED, outputEvents); 
		CASE_LLS_EVENT(result, app, CLIENT_ASKS, outputEvents); 
		CASE_LLS_EVENT(result, app, WORLD_SETUP, outputEvents); 
		CASE_LLS_EVENT(result, app, WORLD_VALUE, outputEvents); 		default: 
			gpk_warning_unhandled_event(eventToProcess); 
			break;
		}
		return bool(result == 1);
	})); 
	return 0; 
}

gpk::err_t	lls::handleACTION_CHAR		(::lls::LLSServer & app, const ::lls::EViewMe & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)app; 
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

gpk::err_t	lls::handleWORLD_ADMIN		(::lls::LLSServer & app, const ::lls::EViewAdminWorld & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)app; 
	const ::lls::SArgsEvent	& eventArgs		= *(const ::lls::SArgsEvent*)gameEvent.Data.begin();
	::lls::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Create	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Rename	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Delete	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Locate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Rotate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Resize	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Reskin	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Relocate: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Generate: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}

gpk::err_t	lls::handleWORLD_EVENT		(::lls::LLSServer & app, const ::lls::EViewWorld & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)app; 
	const ::lls::SArgsEvent	& eventArgs		= *(const ::lls::SArgsEvent*)gameEvent.Data.begin();
	::lls::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::WORLD_EVENT_Rain			: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Thunderstorm	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Earthquake		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Tornado			: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Wave			: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Lightning		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Wildfire		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Flood			: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}

gpk::err_t	lls::handleCLIENT_ASKS		(::lls::LLSServer & app, const ::lls::EViewClient & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)app; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::CLIENT_ASKS_Join: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::CLIENT_ASKS_Quit: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}

gpk::err_t	lls::handleWORLD_SETUP		(::lls::LLSServer & world, const ::lls::EViewWorldSetup & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)world; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::WORLD_SETUP_Seed		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_SETUP_MaxPlayers	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_SETUP_BlockSize	: gpk_warning_not_implemented(gameEvent); break; 
	}
	return 0;
}

gpk::err_t	lls::handleWORLD_VALUE		(::lls::LLSServer & world, const ::lls::EViewWorldValue & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
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
