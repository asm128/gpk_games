#include "ssiege_client.h"
#include "ssiege_event_args.h"

::gpk::error_t		ssg::ssiegeClientUpdate		(::ssg::SSSiegeClient & app, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SSystemEvent> systemEvents) { 
	gpk_necs(::ssg::ssiegeUpdate(app, lastTimeSeconds, inputState, systemEvents, [&app](::gpk::pobj<::ssg::EventSSiege> & _eventToProcess, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) {
		if(!_eventToProcess)
			return false;

		const ::ssg::EventSSiege	& eventToProcess		= *_eventToProcess;
		info_printf("%s", ::gpk::get_value_namep(eventToProcess.Type));

		::gpk::error_t			result			= 0; 
		switch(eventToProcess.Type) {
		case ::ssg::SSG_EVENT_CHAR_ACTION: { es_if_failed(result = ::ssg::eventExtractAndHandle<::ssg::CHAR_ACTION>(eventToProcess, [&app, &outputEvents, &eventToProcess](auto ev){ return ::ssg::handleCHAR_ACTION(app, ev, outputEvents); })); break; }
		case ::ssg::SSG_EVENT_ADMIN_WORLD: { es_if_failed(result = ::ssg::eventExtractAndHandle<::ssg::ADMIN_WORLD>(eventToProcess, [&app, &outputEvents, &eventToProcess](auto ev){ return ::ssg::handleADMIN_WORLD(app, ev, outputEvents); })); break; }
		case ::ssg::SSG_EVENT_WORLD_EVENT: { es_if_failed(result = ::ssg::eventExtractAndHandle<::ssg::WORLD_EVENT>(eventToProcess, [&app, &outputEvents, &eventToProcess](auto ev){ return ::ssg::handleWORLD_EVENT(app, ev, outputEvents); })); break; }
		case ::ssg::SSG_EVENT_CLIENT_ASKS: { es_if_failed(result = ::ssg::eventExtractAndHandle<::ssg::CLIENT_ASKS>(eventToProcess, [&app, &outputEvents, &eventToProcess](auto ev){ return ::ssg::handleCLIENT_ASKS(app, ev, outputEvents); })); break; }
		case ::ssg::SSG_EVENT_WORLD_SETUP: { es_if_failed(result = ::ssg::eventExtractAndHandle<::ssg::WORLD_SETUP>(eventToProcess, [&app, &outputEvents, &eventToProcess](auto ev){ return ::ssg::handleWORLD_SETUP(app, ev, outputEvents); })); break; }
		case ::ssg::SSG_EVENT_WORLD_VALUE: { es_if_failed(result = ::ssg::eventExtractAndHandle<::ssg::WORLD_VALUE>(eventToProcess, [&app, &outputEvents, &eventToProcess](auto ev){ return ::ssg::handleWORLD_VALUE(app, ev, outputEvents); })); break; }
		default: 
			gpk_warning_unhandled_event(eventToProcess); 
			break;
		}
		return bool(result == 1);
	})); 
	return 0; 
}

::gpk::error_t	ssg::handleCHAR_ACTION		(::ssg::SSSiegeClient & app, const ::ssg::EViewMinime & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
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
	case ::ssg::CHAR_ACTION_Spawn: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Shoot: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Throw: gpk_warning_not_implemented(gameEvent); break;
	case ::ssg::CHAR_ACTION_Sleep: gpk_warning_not_implemented(gameEvent); break;
	} 
	return 0; 
}

::gpk::error_t	ssg::handleADMIN_WORLD		(::ssg::SSSiegeClient & app, const ::ssg::EViewAdminWorld & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)app; 
	const ::ssg::SArgsEvent	& eventArgs				= *(const ::ssg::SArgsEvent*)gameEvent.Data.begin();
	::ssg::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Create	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Rename	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Delete	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Locate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Rotate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Resize	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Reskin	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Relocate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::ADMIN_WORLD_Generate	: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}

::gpk::error_t	ssg::handleWORLD_EVENT		(::ssg::SSSiegeClient & app, const ::ssg::EViewWorld & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
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

::gpk::error_t	ssg::handleCLIENT_ASKS		(::ssg::SSSiegeClient & app, const ::ssg::EViewClient & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
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

::gpk::error_t	ssg::handleWORLD_SETUP		(::ssg::SSSiegeClient & world, const ::ssg::EViewWorldSetup & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	(void)outputEvents; (void)world; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssg::WORLD_SETUP_Seed		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_SETUP_MaxPlayers: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssg::WORLD_SETUP_BlockSize	: gpk_warning_not_implemented(gameEvent); break; 
	}
	return 0;
}

::gpk::error_t	ssg::handleWORLD_VALUE		(::ssg::SSSiegeClient & world, const ::ssg::EViewWorldValue & gameEvent, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
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
