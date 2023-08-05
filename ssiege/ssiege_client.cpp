#include "ssiege_client.h"
#include "ssiege_event_args.h"

::gpk::error_t		ssg::ssiegeClientUpdate		(::ssg::SSiegeClient & app, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SSystemEvent> systemEvents) { 
	gpk_necs(::ssg::ssiegeUpdate(app, lastTimeSeconds, inputState, systemEvents, [&app](::gpk::pobj<::ssg::EventSSiege> & _eventToProcess, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) {
		if(!_eventToProcess)
			return false;

		const ::ssg::EventSSiege	& eventToProcess		= *_eventToProcess;
		info_printf("%s", ::gpk::get_value_namep(eventToProcess.Type));

		::gpk::error_t			result			= 0; 
		switch(eventToProcess.Type) {
		CASE_SSG_EVENT(result, app, WORLD_EVENT, outputEvents); 
		CASE_SSG_EVENT(result, app, WORLD_ADMIN, outputEvents); 
		CASE_SSG_EVENT(result, app, ACT_SAILING, outputEvents); 
		CASE_SSG_EVENT(result, app, ACTION_CHAR, outputEvents); 
		CASE_SSG_EVENT(result, app, ACT_ENGINES, outputEvents); 
		CASE_SSG_EVENT(result, app, ACT_AIRSHIP, outputEvents); 
		CASE_SSG_EVENT(result, app, ACT_WHEELED, outputEvents); 
		CASE_SSG_EVENT(result, app, CLIENT_ASKS, outputEvents); 
		CASE_SSG_EVENT(result, app, WORLD_SETUP, outputEvents); 
		CASE_SSG_EVENT(result, app, WORLD_VALUE, outputEvents); 
		default: 
			gpk_warning_unhandled_event(eventToProcess); 
			break;
		}
		return bool(result == 1);
	})); 
	return 0; 
}

::gpk::error_t	ssg::handleACTION_CHAR		(::ssg::SSiegeClient & app, const ::ssg::EViewMe & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)app; 
	const ::ssg::SArgsEvent	& eventArgs				= *(const ::ssg::SArgsEvent*)gameEvent.Data.begin();
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

::gpk::error_t	ssg::handleWORLD_ADMIN		(::ssg::SSiegeClient & app, const ::ssg::EViewAdminWorld & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)app; 
	const ::ssg::SArgsEvent	& eventArgs				= *(const ::ssg::SArgsEvent*)gameEvent.Data.begin();
	::ssg::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Create	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Rename	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Delete	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Locate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Rotate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Resize	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Reskin	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Relocate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_ADMIN_Generate	: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}

::gpk::error_t	ssg::handleWORLD_EVENT		(::ssg::SSiegeClient & app, const ::ssg::EViewWorld & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
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

::gpk::error_t	ssg::handleCLIENT_ASKS		(::ssg::SSiegeClient & app, const ::ssg::EViewClient & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)app; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::CLIENT_ASKS_Join: 
	case ::ssg::CLIENT_ASKS_Quit: 
		gpk_necs(::gpk::eventEnqueueChild(app.EventsToSend, ::ssg::SSG_EVENT_CLIENT_ASKS, gameEvent.Type, gameEvent.Data));
		return 1; 
	} 
	return 0; 
}

::gpk::error_t	ssg::handleWORLD_SETUP		(::ssg::SSiegeClient & world, const ::ssg::EViewWorldSetup & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)world; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::WORLD_SETUP_Seed		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_SETUP_MaxPlayers: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_SETUP_BlockSize	: gpk_warning_not_implemented(gameEvent); break; 
	}
	return 0;
}

::gpk::error_t	ssg::handleWORLD_VALUE		(::ssg::SSiegeClient & world, const ::ssg::EViewWorldValue & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
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
