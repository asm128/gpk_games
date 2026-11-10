#include "lls_parse.h"
#include "lls_event_args.h"

using ::gpk::get_value_namep, ::gpk::get_enum_namep, ::gpk::failed;
GPK_USING_TYPEINT();

#define warning_unhandled_command(command, commandArgs)	warning_printf("Unhandled command for '%.*s': '%s' (args:'%s').", ::gpk::get_enum_namep(command), ::gpk::get_value_namep(command), GPK_VIEW_ARGS(commandArgs))
#define warning_not_implemented(command, commandArgs)	warning_printf("Implement for '%.*s'! '%s' (args:'%s').", ::gpk::get_enum_namep(command), ::gpk::get_value_namep(command), GPK_VIEW_ARGS(commandArgs))

static	gpk::err_t	interpretArgsAndEnqueue	(::gpk::apobj<::lls::EventLLS> & /*queue*/, ::lls::WORLD_ADMIN command, ::gpk::vcsc_t commandArgs) { 
	switch(command) {
	case ::lls::WORLD_ADMIN_Create		: warning_not_implemented(command, commandArgs); break;	
	case ::lls::WORLD_ADMIN_Delete		: warning_not_implemented(command, commandArgs); break;	
	case ::lls::WORLD_ADMIN_Rename		: warning_not_implemented(command, commandArgs); break;	
	case ::lls::WORLD_ADMIN_Rotate		: warning_not_implemented(command, commandArgs); break;	
	case ::lls::WORLD_ADMIN_Resize		: warning_not_implemented(command, commandArgs); break;	
	case ::lls::WORLD_ADMIN_Reskin		: warning_not_implemented(command, commandArgs); break;	
	case ::lls::WORLD_ADMIN_Deform		: warning_not_implemented(command, commandArgs); break;	
	case ::lls::WORLD_ADMIN_Locate		: warning_not_implemented(command, commandArgs); break;	
	case ::lls::WORLD_ADMIN_Relocate	: warning_not_implemented(command, commandArgs); break;
	case ::lls::WORLD_ADMIN_Generate	: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	gpk::err_t	interpretArgsAndEnqueue	(::gpk::apobj<::lls::EventLLS> & /*queue*/, ::lls::WORLD_EVENT command, ::gpk::vcsc_t commandArgs) { 
	::lls::SArgsEvent interpetedArgs	= {};
	switch(command) {
	case ::lls::WORLD_EVENT_Rain			: warning_not_implemented(command, commandArgs); break;
	case ::lls::WORLD_EVENT_Thunderstorm	: warning_not_implemented(command, commandArgs); break;
	case ::lls::WORLD_EVENT_Earthquake	: warning_not_implemented(command, commandArgs); break;	
	case ::lls::WORLD_EVENT_Tornado		: warning_not_implemented(command, commandArgs); break;	
	case ::lls::WORLD_EVENT_Wave			: warning_not_implemented(command, commandArgs); break;
	case ::lls::WORLD_EVENT_Lightning		: warning_not_implemented(command, commandArgs); break;
	case ::lls::WORLD_EVENT_Wildfire		: warning_not_implemented(command, commandArgs); break;
	case ::lls::WORLD_EVENT_Flood			: warning_not_implemented(command, commandArgs); break;	
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	gpk::err_t	interpretArgsAndEnqueue	(::gpk::apobj<::lls::EventLLS> & /*queue*/, ::lls::WORLD_VALUE command, ::gpk::vcsc_t commandArgs) { 
	::lls::SArgsEvent interpetedArgs	= {};
	switch(command) {
	case ::lls::WORLD_VALUE_TimeCreated			: warning_not_implemented(command, commandArgs); break;
	case ::lls::WORLD_VALUE_TimeOffset			: warning_not_implemented(command, commandArgs); break;
	case ::lls::WORLD_VALUE_DaylightOffsetMinutes	: warning_not_implemented(command, commandArgs); break;	
	case ::lls::WORLD_VALUE_DaylightRatioExtra	: warning_not_implemented(command, commandArgs); break;	
	case ::lls::WORLD_VALUE_TimeLastSaved			: warning_not_implemented(command, commandArgs); break;
	case ::lls::WORLD_VALUE_WorldSize				: warning_not_implemented(command, commandArgs); break;
	case ::lls::WORLD_VALUE_TimeScale				: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	gpk::err_t	interpretArgsAndEnqueue	(::gpk::apobj<::lls::EventLLS> & /*queue*/, ::lls::WORLD_SETUP command, ::gpk::vcsc_t commandArgs) { 
	::lls::SArgsEvent interpetedArgs	= {};
	switch(command) {
	case ::lls::WORLD_SETUP_BlockSize		: warning_not_implemented(command, commandArgs); break;
	case ::lls::WORLD_SETUP_MaxPlayers	: warning_not_implemented(command, commandArgs); break;
	case ::lls::WORLD_SETUP_Seed			: warning_not_implemented(command, commandArgs); break;	
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	gpk::err_t	interpretArgsAndEnqueue	(::gpk::apobj<::lls::EventLLS> & /*queue*/, ::lls::ACTION_CHAR command, ::gpk::vcsc_t commandArgs) { 
	switch(command) {
	case ::lls::ACTION_CHAR_Walk	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Turn	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Hide	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Plow	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Dash	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Jump	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Duck	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Warp	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Pick	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Grab	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Take	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Wear	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Rush	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Read	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Use	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Climb	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Throw	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Sleep	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Shoot	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Steal	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Study	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Spawn	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Dance	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Taunt	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACTION_CHAR_Hello	: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	gpk::err_t	interpretArgsAndEnqueue	(::gpk::apobj<::lls::EventLLS> & /*queue*/, ::lls::ACT_SAILING command, ::gpk::vcsc_t commandArgs) { 
	switch(command) {
	case ::lls::ACT_SAILING_Brake		: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACT_SAILING_Handbrake	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACT_SAILING_Reverse	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACT_SAILING_Turn		: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	gpk::err_t	interpretArgsAndEnqueue	(::gpk::apobj<::lls::EventLLS> & /*queue*/, ::lls::ACT_WHEELED command, ::gpk::vcsc_t commandArgs) { 
	switch(command) {
	case ::lls::ACT_WHEELED_Brake		: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACT_WHEELED_Handbrake	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACT_WHEELED_Reverse	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACT_WHEELED_Turn		: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	gpk::err_t	interpretArgsAndEnqueue	(::gpk::apobj<::lls::EventLLS> & /*queue*/, ::lls::ACT_AIRSHIP command, ::gpk::vcsc_t commandArgs) { 
	switch(command) {
	case ::lls::ACT_AIRSHIP_Brake		: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACT_AIRSHIP_Handbrake	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACT_AIRSHIP_Reverse		: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACT_AIRSHIP_Turn		: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	gpk::err_t	interpretArgsAndEnqueue	(::gpk::apobj<::lls::EventLLS> & /*queue*/, ::lls::ACT_ENGINES command, ::gpk::vcsc_t commandArgs) { 
	switch(command) {
	case ::lls::ACT_ENGINES_Off			: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACT_ENGINES_On			: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACT_ENGINES_Accelerate	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACT_ENGINES_Decelerate	: warning_not_implemented(command, commandArgs); break;
	case ::lls::ACT_ENGINES_Nitro		: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	gpk::err_t	interpretArgsAndEnqueue	(::gpk::apobj<::lls::EventLLS> & queue, ::lls::CLIENT_ASKS command, ::gpk::vcsc_t commandArgs) { 
	switch(command) {
	case ::lls::CLIENT_ASKS_Join: 
	case ::lls::CLIENT_ASKS_Quit: 
		gpk_necs(::gpk::eventEnqueueChild<::lls::LLS_EVENT>(queue, ::lls::LLS_EVENT_CLIENT_ASKS, command, commandArgs)); 
		break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

tplt<tpnm _tEnum>
static	gpk::err_t	parseCommand		(::gpk::apobj<::lls::EventLLS> & queue, ::gpk::vcsc_t commandName, ::gpk::vcsc_t commandArgs) { 
	const _tEnum				command				= ::gpk::get_value<_tEnum>(commandName);
	if(::gpk::get_value_count<_tEnum>() <= command)
		return -1;  /// having an error message here makes it too verbose
	info_printf("Command matched for %.*s: %s(args:%s).", ::gpk::get_enum_namep(command), ::gpk::get_value_namep(command), GPK_VIEW_ARGS(commandArgs));
	gpk_necs(::interpretArgsAndEnqueue(queue, command, commandArgs));
	return command;
}

gpk::err_t			lls::parseCommandLine	(::gpk::apobj<::lls::EventLLS> & queue, ::gpk::vcsc_t inputLine) { 
	::gpk::vcsc_t					commandName, commandArgs;
	gpk_necs(inputLine.slice(inputLine, 1));
	if_if(failed(::gpk::split(::gpk::vcs{" "}, inputLine, commandName, commandArgs)), "No arguments provided for command %.*s.", GPK_VIEW_ARGS(commandArgs));
	bool						failed
		=  -1 == ::parseCommand<::lls::WORLD_ADMIN>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::lls::WORLD_EVENT>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::lls::WORLD_SETUP>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::lls::WORLD_VALUE>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::lls::ACT_WHEELED>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::lls::ACT_SAILING>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::lls::ACT_AIRSHIP>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::lls::ACT_ENGINES>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::lls::ACTION_CHAR>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::lls::CLIENT_ASKS>(queue, commandName, commandArgs)
		;
	return failed ? -1 : 0;
}
