#include "ssiege_client.h"
#include "ssiege_event_args.h"

::gpk::error_t		ssiege::ssiegeClientUpdate		(::ssiege::SCamppClient & app, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SSystemEvent> systemEvents) { 
	gpk_necs(::ssiege::ssiegeUpdate(app, lastTimeSeconds, inputState, systemEvents, [&app](::gpk::pobj<::ssiege::EventCampp> & _eventToProcess, ::gpk::apobj<::ssiege::EventCampp> & outputEvents) {
		if(!_eventToProcess)
			return false;

		const ::ssiege::EventCampp	& eventToProcess		= *_eventToProcess;
		info_printf("%s", ::gpk::get_value_namep(eventToProcess.Type));

		::gpk::error_t			result			= 0; 
		switch(eventToProcess.Type) {
		case ::ssiege::CAMPP_EVENT_CHAR_ACTION: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::CHAR_ACTION>(eventToProcess, [&app, &outputEvents, &eventToProcess](auto ev){ return ::ssiege::handleCHAR_ACTION(app, ev, outputEvents); })); break; }
		case ::ssiege::CAMPP_EVENT_WORLD_ADMIN: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::WORLD_ADMIN>(eventToProcess, [&app, &outputEvents, &eventToProcess](auto ev){ return ::ssiege::handleWORLD_ADMIN(app, ev, outputEvents); })); break; }
		case ::ssiege::CAMPP_EVENT_WORLD_EVENT: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::WORLD_EVENT>(eventToProcess, [&app, &outputEvents, &eventToProcess](auto ev){ return ::ssiege::handleWORLD_EVENT(app, ev, outputEvents); })); break; }
		case ::ssiege::CAMPP_EVENT_CLIENT_ASKS: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::CLIENT_ASKS>(eventToProcess, [&app, &outputEvents, &eventToProcess](auto ev){ return ::ssiege::handleCLIENT_ASKS(app, ev, outputEvents); })); break; }
		case ::ssiege::CAMPP_EVENT_WORLD_SETUP: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::WORLD_SETUP>(eventToProcess, [&app, &outputEvents, &eventToProcess](auto ev){ return ::ssiege::handleWORLD_SETUP(app, ev, outputEvents); })); break; }
		case ::ssiege::CAMPP_EVENT_WORLD_VALUE: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::WORLD_VALUE>(eventToProcess, [&app, &outputEvents, &eventToProcess](auto ev){ return ::ssiege::handleWORLD_VALUE(app, ev, outputEvents); })); break; }
		default: 
			gpk_warning_unhandled_event(eventToProcess); 
			break;
		}
		return bool(result == 1);
	})); 
	return 0; 
}

::gpk::error_t	ssiege::handleCHAR_ACTION		(::ssiege::SCamppClient & app, const ::ssiege::EViewMinime & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents) { 
	(void)outputEvents; (void)app; 
	const ::ssiege::SArgsEvent	& eventArgs				= *(const ::ssiege::SArgsEvent*)gameEvent.Data.begin();
	::ssiege::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssiege::CHAR_ACTION_Walk	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssiege::CHAR_ACTION_Turn	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssiege::CHAR_ACTION_Rush	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssiege::CHAR_ACTION_Hide	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssiege::CHAR_ACTION_Dash	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssiege::CHAR_ACTION_Jump	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssiege::CHAR_ACTION_Duck	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssiege::CHAR_ACTION_Warp	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssiege::CHAR_ACTION_Pick	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssiege::CHAR_ACTION_Grab	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssiege::CHAR_ACTION_Wear	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssiege::CHAR_ACTION_Use	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssiege::CHAR_ACTION_Spawn	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssiege::CHAR_ACTION_Shoot	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssiege::CHAR_ACTION_Throw	: gpk_warning_not_implemented(gameEvent); break;
	case ::ssiege::CHAR_ACTION_Sleep	: gpk_warning_not_implemented(gameEvent); break;
	} 
	return 0; 
}

::gpk::error_t	ssiege::handleWORLD_ADMIN		(::ssiege::SCamppClient & app, const ::ssiege::EViewAdmin & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents) { 
	(void)outputEvents; (void)app; 
	const ::ssiege::SArgsEvent	& eventArgs				= *(const ::ssiege::SArgsEvent*)gameEvent.Data.begin();
	::ssiege::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssiege::WORLD_ADMIN_Create	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_ADMIN_Rename	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_ADMIN_Delete	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_ADMIN_Locate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_ADMIN_Rotate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_ADMIN_Resize	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_ADMIN_Reskin	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_ADMIN_Relocate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_ADMIN_Generate	: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}

::gpk::error_t	ssiege::handleWORLD_EVENT		(::ssiege::SCamppClient & app, const ::ssiege::EViewWorld & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents) { 
	(void)outputEvents; (void)app; 
	const ::ssiege::SArgsEvent	& eventArgs				= *(const ::ssiege::SArgsEvent*)gameEvent.Data.begin();
	::ssiege::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssiege::WORLD_EVENT_Rain			: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_EVENT_Thunderstorm	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_EVENT_Earthquake	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_EVENT_Tornado		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_EVENT_Wave			: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_EVENT_Lightning		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_EVENT_Wildfire		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_EVENT_Flood			: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}

::gpk::error_t	ssiege::handleCLIENT_ASKS		(::ssiege::SCamppClient & app, const ::ssiege::EViewClient & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents) { 
	(void)outputEvents; (void)app; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssiege::CLIENT_ASKS_Join: 
	case ::ssiege::CLIENT_ASKS_Quit: 
		gpk_necs(::gpk::eventEnqueueChild(app.EventsToSend, ::ssiege::CAMPP_EVENT_CLIENT_ASKS, gameEvent.Type, gameEvent.Data));
		return 1; 
	} 
	return 0; 
}

::gpk::error_t	ssiege::handleWORLD_SETUP		(::ssiege::SCamppClient & world, const ::ssiege::EViewWorldSetup & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents) { 
	(void)outputEvents; (void)world; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssiege::WORLD_SETUP_Seed		: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_SETUP_MaxPlayers: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_SETUP_BlockSize	: gpk_warning_not_implemented(gameEvent); break; 
	}
	return 0;
}

::gpk::error_t	ssiege::handleWORLD_VALUE		(::ssiege::SCamppClient & world, const ::ssiege::EViewWorldValue & gameEvent, ::gpk::apobj<::ssiege::EventCampp> & outputEvents) { 
	(void)outputEvents; (void)world; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::ssiege::WORLD_VALUE_TimeCreated			: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_VALUE_TimeOffset			: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_VALUE_DaylightOffsetMinutes	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_VALUE_DaylightRatioExtra	: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_VALUE_TimeLastSaved			: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_VALUE_WorldSize				: gpk_warning_not_implemented(gameEvent); break; 
	case ::ssiege::WORLD_VALUE_TimeScale				: gpk_warning_not_implemented(gameEvent); break; 
	}
	return 0;
}
