#include "gpk_event.h"

#ifndef SSG_EVENT_WORLD_H_23701
#define SSG_EVENT_WORLD_H_23701

namespace ssg
{
#pragma pack(push, 1)
	// These events are sent for world contents manipulation
	GDEFINE_ENUM_TYPE (ADMIN_GAME, uint8_t);
	GDEFINE_ENUM_VALUE(ADMIN_GAME, Show			, 0);
	GDEFINE_ENUM_VALUE(ADMIN_GAME, Hide			, 1);
	GDEFINE_ENUM_VALUE(ADMIN_GAME, Initialize	, 2); 
	GDEFINE_ENUM_VALUE(ADMIN_GAME, Reset		, 3); 
	typedef	::gpk::SEvent		<ADMIN_GAME>	EventAdminGame;
	typedef	::gpk::SEventView	<ADMIN_GAME>	EViewAdminGame;

	// These events are sent for world contents manipulation
	GDEFINE_ENUM_TYPE (ADMIN_WORLD, uint8_t);
	GDEFINE_ENUM_VALUE(ADMIN_WORLD, Create		, 0);
	GDEFINE_ENUM_VALUE(ADMIN_WORLD, Delete		, 1);
	GDEFINE_ENUM_VALUE(ADMIN_WORLD, Rename		, 2);
	GDEFINE_ENUM_VALUE(ADMIN_WORLD, Rotate		, 3);
	GDEFINE_ENUM_VALUE(ADMIN_WORLD, Resize		, 4);
	GDEFINE_ENUM_VALUE(ADMIN_WORLD, Reskin		, 5);
	GDEFINE_ENUM_VALUE(ADMIN_WORLD, Deform		, 6);
	GDEFINE_ENUM_VALUE(ADMIN_WORLD, Locate		, 7);
	GDEFINE_ENUM_VALUE(ADMIN_WORLD, Relocate	, 8);
	GDEFINE_ENUM_VALUE(ADMIN_WORLD, Generate	, 9);
	GDEFINE_ENUM_VALUE(ADMIN_WORLD, Initialize	, 10); 
	GDEFINE_ENUM_VALUE(ADMIN_WORLD, Reset		, 11); 
	typedef	::gpk::SEvent		<ADMIN_WORLD>	EventAdminWorld;
	typedef	::gpk::SEventView	<ADMIN_WORLD>	EViewAdminWorld;

	GDEFINE_ENUM_TYPE (ADMIN_CHARS, uint8_t);
	GDEFINE_ENUM_VALUE(ADMIN_CHARS, Spawn	, 12); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ADMIN_CHARS, Kill	, 13); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ADMIN_CHARS, Revive	, 14); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ADMIN_CHARS, Heal	, 15); // Used with an SArgsEvent structure.
	typedef	::gpk::SEvent		<ADMIN_CHARS>	EventAdminChar;
	typedef	::gpk::SEventView	<ADMIN_CHARS>	EViewAdminChar;

	GDEFINE_ENUM_TYPE (ADMIN_USERS, uint8_t);
	GDEFINE_ENUM_VALUE(ADMIN_USERS, Spawn	, 12); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ADMIN_USERS, Kill	, 13); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ADMIN_USERS, Revive	, 14); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ADMIN_USERS, Heal	, 15); // Used with an SArgsEvent structure.
	typedef	::gpk::SEvent		<ADMIN_USERS>	EventAdminUser;
	typedef	::gpk::SEventView	<ADMIN_USERS>	EViewAdminUser;

	// These events trigger or report effects that are spawned by the world 
	GDEFINE_ENUM_TYPE (WORLD_EVENT, uint8_t);
	GDEFINE_ENUM_VALUE(WORLD_EVENT, Rain		, 0);	// Takes an SArgsEvent structure. 0 + effect id: Stop, 1: Start drizzle, 2: Start rain, 3: Start storm, 4: Start storm.
	GDEFINE_ENUM_VALUE(WORLD_EVENT, Thunderstorm, 1);	// Takes an SArgsEvent structure. 0 + effect id: Stop, 1: Start intra-cloud, 2: Start cloud to cloud, 3: Start cloud to ground.
	GDEFINE_ENUM_VALUE(WORLD_EVENT, Earthquake	, 2);	// Takes an SArgsEvent structure. 0 + effect id: Stop, 1 to 10: Ritcher scale. 
	GDEFINE_ENUM_VALUE(WORLD_EVENT, Tornado		, 3);	// Takes an SArgsEvent structure. 0 + effect id: Stop, 1 to 10: Ritcher scale. 
	GDEFINE_ENUM_VALUE(WORLD_EVENT, Wave		, 4);	// Takes an SArgsEvent structure. 0 + effect id: Stop, 1: Small , 2: Medium , 3: Large, 4: Tsunami.
	GDEFINE_ENUM_VALUE(WORLD_EVENT, Lightning	, 5);	// Takes an SArgsEvent structure. 0 + effect id: Stop, 1: Small , 2: Medium , 3: Large.
	GDEFINE_ENUM_VALUE(WORLD_EVENT, Wildfire	, 6);	// Takes an SArgsEvent structure. 0 + effect id: Stop, 1: Small , 2: Medium , 3: Large.
	GDEFINE_ENUM_VALUE(WORLD_EVENT, Flood		, 7);	// Takes an SArgsEvent structure. 0 + effect id: Stop, 1: Small , 2: Medium , 3: Large.
	typedef	::gpk::SEvent		<WORLD_EVENT>	EventWorld;
	typedef	::gpk::SEventView	<WORLD_EVENT>	EViewWorld;

	// These events change or report the world configuration settings
	GDEFINE_ENUM_TYPE (WORLD_SETUP, uint8_t); 
	GDEFINE_ENUM_VALUE(WORLD_SETUP, Seed		, 0); 
	GDEFINE_ENUM_VALUE(WORLD_SETUP, MaxPlayers	, 1); 
	GDEFINE_ENUM_VALUE(WORLD_SETUP, BlockSize	, 2); 
	typedef	::gpk::SEvent		<WORLD_SETUP>	EventWorldSetup;
	typedef	::gpk::SEventView	<WORLD_SETUP>	EViewWorldSetup;
	
	// These events change or report the world current state values
	GDEFINE_ENUM_TYPE (WORLD_VALUE, uint8_t); 
	GDEFINE_ENUM_VALUE(WORLD_VALUE, TimeCreated				, 0); 
	GDEFINE_ENUM_VALUE(WORLD_VALUE, TimeOffset				, 1); 
	GDEFINE_ENUM_VALUE(WORLD_VALUE, DaylightOffsetMinutes	, 2); 
	GDEFINE_ENUM_VALUE(WORLD_VALUE, DaylightRatioExtra		, 3); 
	GDEFINE_ENUM_VALUE(WORLD_VALUE, TimeLastSaved			, 4); 
	GDEFINE_ENUM_VALUE(WORLD_VALUE, WorldSize				, 5); 
	GDEFINE_ENUM_VALUE(WORLD_VALUE, TimeScale				, 6); 
	typedef	::gpk::SEvent		<WORLD_VALUE>	EventWorldValue;
	typedef	::gpk::SEventView	<WORLD_VALUE>	EViewWorldValue;
#pragma pack(pop)
} // namespace

#endif // SSG_EVENT_WORLD_H_23701