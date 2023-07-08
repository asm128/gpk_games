#include "gpk_event.h"

#ifndef SSIEGE_EVENT_WORLD_H_23701
#define SSIEGE_EVENT_WORLD_H_23701

namespace ssiege
{
#pragma pack(push, 1)
	// These events are sent for world contents manipulation
	GDEFINE_ENUM_TYPE (WORLD_ADMIN, uint8_t);
	GDEFINE_ENUM_VALUE(WORLD_ADMIN, Create		, 0);
	GDEFINE_ENUM_VALUE(WORLD_ADMIN, Delete		, 1);
	GDEFINE_ENUM_VALUE(WORLD_ADMIN, Rename		, 2);
	GDEFINE_ENUM_VALUE(WORLD_ADMIN, Rotate		, 3);
	GDEFINE_ENUM_VALUE(WORLD_ADMIN, Resize		, 4);
	GDEFINE_ENUM_VALUE(WORLD_ADMIN, Reskin		, 5);
	GDEFINE_ENUM_VALUE(WORLD_ADMIN, Deform		, 6);
	GDEFINE_ENUM_VALUE(WORLD_ADMIN, Locate		, 7);
	GDEFINE_ENUM_VALUE(WORLD_ADMIN, Relocate	, 8);
	GDEFINE_ENUM_VALUE(WORLD_ADMIN, Generate	, 9);
	GDEFINE_ENUM_VALUE(WORLD_ADMIN, Initialize	, 10); 
	GDEFINE_ENUM_VALUE(WORLD_ADMIN, Reset		, 11); 
	typedef	::gpk::SEvent		<WORLD_ADMIN>	EventAdmin;
	typedef	::gpk::SEventView	<WORLD_ADMIN>	EViewAdmin;

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

#endif // SSIEGE_EVENT_WORLD_H_23701