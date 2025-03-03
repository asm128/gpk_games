#include "gpk_event.h"

#ifndef SSG_EVENT_ANIMAL_H_23701
#define SSG_EVENT_ANIMAL_H_23701

namespace ssg
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (ACTION_CHAR, uint8_t);
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Walk	, 0); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Turn	, 1); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Hide	, 2); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Plow	, 3); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Dash	, 4); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Jump	, 5); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Duck	, 6); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Warp	, 7); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Pick	, 8); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Grab	, 9); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Take	,10); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Wear	,11); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Rush	,12); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Read	,13); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Use		,14); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Climb	,15); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Throw	,16); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Sleep	,17); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Shoot	,18); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Dance	,19); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Taunt	,20); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Hello	,21); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Steal	,22); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Enter	,23); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Leave	,24); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Study	,25); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(ACTION_CHAR, Spawn	,26); // Used with an SArgsEvent structure.
	typedef	::gpk::SEvent		<ACTION_CHAR>	EventMe;
	typedef	::gpk::SEventView	<ACTION_CHAR>	EViewMe;
#pragma pack(pop)
} // namespace

#endif // SSG_EVENT_ANIMAL_H_23701