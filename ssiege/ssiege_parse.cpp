#include "ssiege_parse.h"
#include "ssiege_event_args.h"

#define warning_unhandled_command(command, commandArgs)	warning_printf("Unhandled command for '%s': '%s' (args:'%s').", ::gpk::get_enum_namep(command), ::gpk::get_value_namep(command), ::gpk::toString(commandArgs).begin())
#define warning_not_implemented(command, commandArgs)	warning_printf("Implement for '%s'! '%s' (args:'%s').", ::gpk::get_enum_namep(command), ::gpk::get_value_namep(command), ::gpk::toString(commandArgs).begin())

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssg::EventSSiege> & /*queue*/, ::ssg::ADMIN_WORLD command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssg::ADMIN_WORLD_Create	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::ADMIN_WORLD_Delete	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::ADMIN_WORLD_Rename	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::ADMIN_WORLD_Rotate	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::ADMIN_WORLD_Resize	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::ADMIN_WORLD_Reskin	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::ADMIN_WORLD_Deform	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::ADMIN_WORLD_Locate	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::ADMIN_WORLD_Relocate	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ADMIN_WORLD_Generate	: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssg::EventSSiege> & /*queue*/, ::ssg::WORLD_EVENT command, ::gpk::vcc commandArgs) { 
	::ssg::SArgsEvent interpetedArgs	= {};
	switch(command) {
	case ::ssg::WORLD_EVENT_Rain			: warning_not_implemented(command, commandArgs); break;
	case ::ssg::WORLD_EVENT_Thunderstorm	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::WORLD_EVENT_Earthquake	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::WORLD_EVENT_Tornado		: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::WORLD_EVENT_Wave			: warning_not_implemented(command, commandArgs); break;
	case ::ssg::WORLD_EVENT_Lightning		: warning_not_implemented(command, commandArgs); break;
	case ::ssg::WORLD_EVENT_Wildfire		: warning_not_implemented(command, commandArgs); break;
	case ::ssg::WORLD_EVENT_Flood			: warning_not_implemented(command, commandArgs); break;	
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssg::EventSSiege> & /*queue*/, ::ssg::WORLD_VALUE command, ::gpk::vcc commandArgs) { 
	::ssg::SArgsEvent interpetedArgs	= {};
	switch(command) {
	case ::ssg::WORLD_VALUE_TimeCreated			: warning_not_implemented(command, commandArgs); break;
	case ::ssg::WORLD_VALUE_TimeOffset			: warning_not_implemented(command, commandArgs); break;
	case ::ssg::WORLD_VALUE_DaylightOffsetMinutes	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::WORLD_VALUE_DaylightRatioExtra	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::WORLD_VALUE_TimeLastSaved			: warning_not_implemented(command, commandArgs); break;
	case ::ssg::WORLD_VALUE_WorldSize				: warning_not_implemented(command, commandArgs); break;
	case ::ssg::WORLD_VALUE_TimeScale				: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssg::EventSSiege> & /*queue*/, ::ssg::WORLD_SETUP command, ::gpk::vcc commandArgs) { 
	::ssg::SArgsEvent interpetedArgs	= {};
	switch(command) {
	case ::ssg::WORLD_SETUP_BlockSize		: warning_not_implemented(command, commandArgs); break;
	case ::ssg::WORLD_SETUP_MaxPlayers	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::WORLD_SETUP_Seed			: warning_not_implemented(command, commandArgs); break;	
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssg::EventSSiege> & /*queue*/, ::ssg::CHAR_ACTION command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssg::CHAR_ACTION_Walk	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Turn	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Hide	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Plow	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Dash	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Jump	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Duck	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Warp	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Pick	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Grab	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Take	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Wear	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Rush	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Read	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Use	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Climb	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Throw	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Sleep	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Shoot	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Steal	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Study	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Spawn	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Dance	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Taunt	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::CHAR_ACTION_Hello	: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssg::EventSSiege> & /*queue*/, ::ssg::SAIL_ACTION command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssg::SAIL_ACTION_Brake		: warning_not_implemented(command, commandArgs); break;
	case ::ssg::SAIL_ACTION_Handbrake	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::SAIL_ACTION_Reverse	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::SAIL_ACTION_Turn		: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssg::EventSSiege> & /*queue*/, ::ssg::WHEELED_ACT command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssg::WHEELED_ACT_Brake		: warning_not_implemented(command, commandArgs); break;
	case ::ssg::WHEELED_ACT_Handbrake	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::WHEELED_ACT_Reverse	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::WHEELED_ACT_Turn		: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssg::EventSSiege> & /*queue*/, ::ssg::AIRSHIP_ACT command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssg::AIRSHIP_ACT_Brake		: warning_not_implemented(command, commandArgs); break;
	case ::ssg::AIRSHIP_ACT_Handbrake	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::AIRSHIP_ACT_Reverse	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::AIRSHIP_ACT_Turn		: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssg::EventSSiege> & /*queue*/, ::ssg::ENGINE_ACTS command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssg::ENGINE_ACTS_Off		: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ENGINE_ACTS_On		: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ENGINE_ACTS_Accelerate: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ENGINE_ACTS_Decelerate: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ENGINE_ACTS_Nitro		: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssg::EventSSiege> & queue, ::ssg::CLIENT_ASKS command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssg::CLIENT_ASKS_Join: 
	case ::ssg::CLIENT_ASKS_Quit: 
		gpk_necs(::gpk::eventEnqueueChild<::ssg::SSG_EVENT>(queue, ::ssg::SSG_EVENT_CLIENT_ASKS, command, commandArgs)); 
		break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

tplt<tpnm _tEnum>
static	::gpk::error_t	parseCommand		(::gpk::apobj<::ssg::EventSSiege> & queue, ::gpk::vcc commandName, ::gpk::vcc commandArgs) { 
	const _tEnum				command				= ::gpk::get_value<_tEnum>(commandName);
	if(::gpk::get_value_count<_tEnum>() <= command)
		return -1;  /// having an error message here makes it too verbose
	info_printf("Command matched for %s: %s(args:%s).", ::gpk::get_enum_namep(command), ::gpk::get_value_namep(command), ::gpk::toString(commandArgs).begin());
	gpk_necs(::interpretArgsAndEnqueue(queue, command, commandArgs));
	return command;
}

::gpk::error_t			ssg::parseCommandLine	(::gpk::apobj<::ssg::EventSSiege> & queue, ::gpk::vcc inputLine) { 
	::gpk::vcc					commandName, commandArgs;
	gpk_necs(inputLine.slice(inputLine, 1));
	ginfo_if(errored(::gpk::split(::gpk::vcs{" "}, inputLine, commandName, commandArgs)), "No arguments provided for command %s.", ::gpk::toString(commandArgs).begin());
	bool						failed
		=  -1 == ::parseCommand<::ssg::ADMIN_WORLD>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::WORLD_EVENT>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::WORLD_SETUP>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::WORLD_VALUE>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::WHEELED_ACT>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::SAIL_ACTION>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::AIRSHIP_ACT>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::ENGINE_ACTS>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::CHAR_ACTION>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::CLIENT_ASKS>(queue, commandName, commandArgs)
		;
	return failed ? -1 : 0;
}
