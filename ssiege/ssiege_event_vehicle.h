#include "gpk_event.h"

#ifndef SSG_EVENT_VEHICLE_H_23701
#define SSG_EVENT_VEHICLE_H_23701

namespace ssg
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (ACT_ENGINES, uint8_t);
	GDEFINE_ENUM_VALUE(ACT_ENGINES, Off			, 0); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACT_ENGINES, On			, 1); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACT_ENGINES, Accelerate	, 2); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACT_ENGINES, Decelerate	, 3); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACT_ENGINES, Nitro		, 4); // Used with an SArgsEvent structure.
	typedef	::gpk::SEvent		<ACT_ENGINES>	EventEngine;
	typedef	::gpk::SEventView	<ACT_ENGINES>	EViewEngine;

	GDEFINE_ENUM_TYPE (ACT_WHEELED, uint8_t);
	GDEFINE_ENUM_VALUE(ACT_WHEELED, Turn		, 0); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACT_WHEELED, Brake		, 1); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACT_WHEELED, Reverse		, 2); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACT_WHEELED, Handbrake	, 3); // Used with an SArgsEvent structure.
	typedef	::gpk::SEvent		<ACT_WHEELED>	EventWheeled;
	typedef	::gpk::SEventView	<ACT_WHEELED>	EViewWheeled;

	GDEFINE_ENUM_TYPE (ACT_SAILING, uint8_t);
	GDEFINE_ENUM_VALUE(ACT_SAILING, Turn		, 0); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACT_SAILING, Brake		, 1); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACT_SAILING, Reverse		, 2); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACT_SAILING, Handbrake	, 3); // Used with an SArgsEvent structure.
	typedef	::gpk::SEvent		<ACT_SAILING>	EventSail;
	typedef	::gpk::SEventView	<ACT_SAILING>	EViewSail;

	GDEFINE_ENUM_TYPE (ACT_AIRSHIP, uint8_t);
	GDEFINE_ENUM_VALUE(ACT_AIRSHIP, Turn		, 0); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACT_AIRSHIP, Brake		, 1); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACT_AIRSHIP, Reverse		, 2); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACT_AIRSHIP, Handbrake	, 3); // Used with an SArgsEvent structure.
	typedef	::gpk::SEvent		<ACT_AIRSHIP>	EventAircraft;
	typedef	::gpk::SEventView	<ACT_AIRSHIP>	EViewAircraft;
#pragma pack(pop)
} // namespace

#endif // SSG_EVENT_VEHICLE_H_23701