#include "ssiege_parse.h"
#include "ssiege_event_args.h"

#define warning_unhandled_command(command, commandArgs)	warning_printf("Unhandled command for '%s': '%s' (args:'%s').", ::gpk::get_enum_namep(command), ::gpk::get_value_namep(command), ::gpk::toString(commandArgs).begin())
#define warning_not_implemented(command, commandArgs)	warning_printf("Implement for '%s'! '%s' (args:'%s').", ::gpk::get_enum_namep(command), ::gpk::get_value_namep(command), ::gpk::toString(commandArgs).begin())

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssiege::EventSSiege> & /*queue*/, ::ssiege::ADMIN_WORLD command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssiege::ADMIN_WORLD_Create	: warning_not_implemented(command, commandArgs); break;	
	case ::ssiege::ADMIN_WORLD_Delete	: warning_not_implemented(command, commandArgs); break;	
	case ::ssiege::ADMIN_WORLD_Rename	: warning_not_implemented(command, commandArgs); break;	
	case ::ssiege::ADMIN_WORLD_Rotate	: warning_not_implemented(command, commandArgs); break;	
	case ::ssiege::ADMIN_WORLD_Resize	: warning_not_implemented(command, commandArgs); break;	
	case ::ssiege::ADMIN_WORLD_Reskin	: warning_not_implemented(command, commandArgs); break;	
	case ::ssiege::ADMIN_WORLD_Deform	: warning_not_implemented(command, commandArgs); break;	
	case ::ssiege::ADMIN_WORLD_Locate	: warning_not_implemented(command, commandArgs); break;	
	case ::ssiege::ADMIN_WORLD_Relocate	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::ADMIN_WORLD_Generate	: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssiege::EventSSiege> & /*queue*/, ::ssiege::WORLD_EVENT command, ::gpk::vcc commandArgs) { 
	::ssiege::SArgsEvent interpetedArgs	= {};
	switch(command) {
	case ::ssiege::WORLD_EVENT_Rain			: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::WORLD_EVENT_Thunderstorm	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::WORLD_EVENT_Earthquake	: warning_not_implemented(command, commandArgs); break;	
	case ::ssiege::WORLD_EVENT_Tornado		: warning_not_implemented(command, commandArgs); break;	
	case ::ssiege::WORLD_EVENT_Wave			: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::WORLD_EVENT_Lightning		: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::WORLD_EVENT_Wildfire		: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::WORLD_EVENT_Flood			: warning_not_implemented(command, commandArgs); break;	
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssiege::EventSSiege> & /*queue*/, ::ssiege::WORLD_VALUE command, ::gpk::vcc commandArgs) { 
	::ssiege::SArgsEvent interpetedArgs	= {};
	switch(command) {
	case ::ssiege::WORLD_VALUE_TimeCreated			: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::WORLD_VALUE_TimeOffset			: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::WORLD_VALUE_DaylightOffsetMinutes	: warning_not_implemented(command, commandArgs); break;	
	case ::ssiege::WORLD_VALUE_DaylightRatioExtra	: warning_not_implemented(command, commandArgs); break;	
	case ::ssiege::WORLD_VALUE_TimeLastSaved			: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::WORLD_VALUE_WorldSize				: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::WORLD_VALUE_TimeScale				: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssiege::EventSSiege> & /*queue*/, ::ssiege::WORLD_SETUP command, ::gpk::vcc commandArgs) { 
	::ssiege::SArgsEvent interpetedArgs	= {};
	switch(command) {
	case ::ssiege::WORLD_SETUP_BlockSize		: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::WORLD_SETUP_MaxPlayers	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::WORLD_SETUP_Seed			: warning_not_implemented(command, commandArgs); break;	
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssiege::EventSSiege> & /*queue*/, ::ssiege::CHAR_ACTION command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssiege::CHAR_ACTION_Walk	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Turn	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Hide	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Plow	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Dash	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Jump	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Duck	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Warp	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Pick	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Grab	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Take	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Wear	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Rush	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Read	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Use	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Climb	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Throw	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Sleep	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Shoot	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Steal	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Study	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Spawn	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Dance	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Taunt	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::CHAR_ACTION_Hello	: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssiege::EventSSiege> & /*queue*/, ::ssiege::SAIL_ACTION command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssiege::SAIL_ACTION_Brake		: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::SAIL_ACTION_Handbrake	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::SAIL_ACTION_Reverse	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::SAIL_ACTION_Turn		: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssiege::EventSSiege> & /*queue*/, ::ssiege::WHEELED_ACT command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssiege::WHEELED_ACT_Brake		: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::WHEELED_ACT_Handbrake	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::WHEELED_ACT_Reverse	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::WHEELED_ACT_Turn		: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssiege::EventSSiege> & /*queue*/, ::ssiege::AIRSHIP_ACT command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssiege::AIRSHIP_ACT_Brake		: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::AIRSHIP_ACT_Handbrake	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::AIRSHIP_ACT_Reverse	: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::AIRSHIP_ACT_Turn		: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssiege::EventSSiege> & /*queue*/, ::ssiege::ENGINE_ACTS command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssiege::ENGINE_ACTS_Off		: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::ENGINE_ACTS_On		: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::ENGINE_ACTS_Accelerate: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::ENGINE_ACTS_Decelerate: warning_not_implemented(command, commandArgs); break;
	case ::ssiege::ENGINE_ACTS_Nitro		: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssiege::EventSSiege> & queue, ::ssiege::CLIENT_ASKS command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssiege::CLIENT_ASKS_Join: 
	case ::ssiege::CLIENT_ASKS_Quit: 
		gpk_necs(::gpk::eventEnqueueChild<::ssiege::SSIEGE_EVENT>(queue, ::ssiege::SSIEGE_EVENT_CLIENT_ASKS, command, commandArgs)); 
		break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

tplt<tpnm _tEnum>
static	::gpk::error_t	parseCommand		(::gpk::apobj<::ssiege::EventSSiege> & queue, ::gpk::vcc commandName, ::gpk::vcc commandArgs) { 
	const _tEnum				command				= ::gpk::get_value<_tEnum>(commandName);
	if(::gpk::get_value_count<_tEnum>() <= command)
		return -1;  /// having an error message here makes it too verbose
	info_printf("Command matched for %s: %s(args:%s).", ::gpk::get_enum_namep(command), ::gpk::get_value_namep(command), ::gpk::toString(commandArgs).begin());
	gpk_necs(::interpretArgsAndEnqueue(queue, command, commandArgs));
	return command;
}

::gpk::error_t			ssiege::parseCommandLine	(::gpk::apobj<::ssiege::EventSSiege> & queue, ::gpk::vcc inputLine) { 
	::gpk::vcc					commandName, commandArgs;
	gpk_necs(inputLine.slice(inputLine, 1));
	ginfo_if(errored(::gpk::split(::gpk::vcs{" "}, inputLine, commandName, commandArgs)), "No arguments provided for command %s.", ::gpk::toString(commandArgs).begin());
	bool						failed
		=  -1 == ::parseCommand<::ssiege::ADMIN_WORLD>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssiege::WORLD_EVENT>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssiege::WORLD_SETUP>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssiege::WORLD_VALUE>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssiege::WHEELED_ACT>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssiege::SAIL_ACTION>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssiege::AIRSHIP_ACT>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssiege::ENGINE_ACTS>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssiege::CHAR_ACTION>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssiege::CLIENT_ASKS>(queue, commandName, commandArgs)
		;
	return failed ? -1 : 0;
}
