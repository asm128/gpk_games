#include "ssiege_parse.h"
#include "ssiege_event_args.h"

#define warning_unhandled_command(command, commandArgs)	warning_printf("Unhandled command for '%s': '%s' (args:'%s').", ::gpk::get_enum_namep(command), ::gpk::get_value_namep(command), ::gpk::toString(commandArgs).begin())
#define warning_not_implemented(command, commandArgs)	warning_printf("Implement for '%s'! '%s' (args:'%s').", ::gpk::get_enum_namep(command), ::gpk::get_value_namep(command), ::gpk::toString(commandArgs).begin())

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssg::EventSSiege> & /*queue*/, ::ssg::WORLD_ADMIN command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssg::WORLD_ADMIN_Create	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::WORLD_ADMIN_Delete	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::WORLD_ADMIN_Rename	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::WORLD_ADMIN_Rotate	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::WORLD_ADMIN_Resize	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::WORLD_ADMIN_Reskin	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::WORLD_ADMIN_Deform	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::WORLD_ADMIN_Locate	: warning_not_implemented(command, commandArgs); break;	
	case ::ssg::WORLD_ADMIN_Relocate	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::WORLD_ADMIN_Generate	: warning_not_implemented(command, commandArgs); break;
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

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssg::EventSSiege> & /*queue*/, ::ssg::ACTION_CHAR command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssg::ACTION_CHAR_Walk	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Turn	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Hide	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Plow	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Dash	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Jump	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Duck	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Warp	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Pick	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Grab	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Take	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Wear	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Rush	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Read	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Use	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Climb	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Throw	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Sleep	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Shoot	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Steal	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Study	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Spawn	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Dance	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Taunt	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACTION_CHAR_Hello	: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssg::EventSSiege> & /*queue*/, ::ssg::ACT_SAILING command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssg::ACT_SAILING_Brake		: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACT_SAILING_Handbrake	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACT_SAILING_Reverse	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACT_SAILING_Turn		: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssg::EventSSiege> & /*queue*/, ::ssg::ACT_WHEELED command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssg::ACT_WHEELED_Brake		: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACT_WHEELED_Handbrake	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACT_WHEELED_Reverse	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACT_WHEELED_Turn		: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssg::EventSSiege> & /*queue*/, ::ssg::ACT_AIRSHIP command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssg::ACT_AIRSHIP_Brake		: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACT_AIRSHIP_Handbrake	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACT_AIRSHIP_Reverse		: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACT_AIRSHIP_Turn		: warning_not_implemented(command, commandArgs); break;
	default: 
		warning_unhandled_command(command, commandArgs);
		return -1; 
	}
	return 0;
}

static	::gpk::error_t	interpretArgsAndEnqueue	(::gpk::apobj<::ssg::EventSSiege> & /*queue*/, ::ssg::ACT_ENGINES command, ::gpk::vcc commandArgs) { 
	switch(command) {
	case ::ssg::ACT_ENGINES_Off			: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACT_ENGINES_On			: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACT_ENGINES_Accelerate	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACT_ENGINES_Decelerate	: warning_not_implemented(command, commandArgs); break;
	case ::ssg::ACT_ENGINES_Nitro		: warning_not_implemented(command, commandArgs); break;
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
		=  -1 == ::parseCommand<::ssg::WORLD_ADMIN>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::WORLD_EVENT>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::WORLD_SETUP>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::WORLD_VALUE>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::ACT_WHEELED>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::ACT_SAILING>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::ACT_AIRSHIP>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::ACT_ENGINES>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::ACTION_CHAR>(queue, commandName, commandArgs)
		&& -1 == ::parseCommand<::ssg::CLIENT_ASKS>(queue, commandName, commandArgs)
		;
	return failed ? -1 : 0;
}
