#include "gpk_event.h"

#ifndef CAMPP_EVENT_VEHICLE_H_23701
#define CAMPP_EVENT_VEHICLE_H_23701

namespace ssiege
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (ENGINE_ACTS, uint8_t);
	GDEFINE_ENUM_VALUE(ENGINE_ACTS, Off			, 0); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ENGINE_ACTS, On			, 1); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ENGINE_ACTS, Accelerate	, 2); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ENGINE_ACTS, Decelerate	, 3); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ENGINE_ACTS, Nitro		, 4); // Used with an SArgsEvent structure.
	typedef	::gpk::SEvent		<ENGINE_ACTS>	EventEngine;
	typedef	::gpk::SEventView	<ENGINE_ACTS>	EViewEngine;

	GDEFINE_ENUM_TYPE (WHEELED_ACT, uint8_t);
	GDEFINE_ENUM_VALUE(WHEELED_ACT, Turn		, 0); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(WHEELED_ACT, Brake		, 1); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(WHEELED_ACT, Reverse		, 2); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(WHEELED_ACT, Handbrake	, 3); // Used with an SArgsEvent structure.
	typedef	::gpk::SEvent		<WHEELED_ACT>	EventWheeled;
	typedef	::gpk::SEventView	<WHEELED_ACT>	EViewWheeled;

	GDEFINE_ENUM_TYPE (SAIL_ACTION, uint8_t);
	GDEFINE_ENUM_VALUE(SAIL_ACTION, Turn		, 0); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(SAIL_ACTION, Brake		, 1); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(SAIL_ACTION, Reverse		, 2); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(SAIL_ACTION, Handbrake	, 3); // Used with an SArgsEvent structure.
	typedef	::gpk::SEvent		<SAIL_ACTION>	EventSail;
	typedef	::gpk::SEventView	<SAIL_ACTION>	EViewSail;

	GDEFINE_ENUM_TYPE (AIRSHIP_ACT, uint8_t);
	GDEFINE_ENUM_VALUE(AIRSHIP_ACT, Turn		, 0); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(AIRSHIP_ACT, Brake		, 1); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(AIRSHIP_ACT, Reverse		, 2); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(AIRSHIP_ACT, Handbrake	, 3); // Used with an SArgsEvent structure.
	typedef	::gpk::SEvent		<AIRSHIP_ACT>	EventAircraft;
	typedef	::gpk::SEventView	<AIRSHIP_ACT>	EViewAircraft;
#pragma pack(pop)
} // namespace

#endif // CAMPP_EVENT_VEHICLE_H_23701