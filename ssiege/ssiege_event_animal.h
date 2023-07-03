#include "gpk_event.h"

#ifndef CAMPP_EVENT_ANIMAL_H_23701
#define CAMPP_EVENT_ANIMAL_H_23701

namespace ssiege
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (CHAR_ACTION, uint8_t);
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Walk	, 0); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Turn	, 1); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Hide	, 2); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Plow	, 3); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Dash	, 4); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Jump	, 5); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Duck	, 6); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Warp	, 7); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Pick	, 8); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Grab	, 9); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Take	,10); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Wear	,11); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Rush	,12); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Read	,13); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Use		,14); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Climb	,15); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Throw	,16); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Sleep	,17); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Shoot	,18); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Dance	,19); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Taunt	,20); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Hello	,21); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Steal	,22); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Enter	,23); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Leave	,24); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Study	,25); // Used with an SArgsEvent structure.
	GDEFINE_ENUM_VALUE(CHAR_ACTION, Spawn	,26); // Used with an SArgsEvent structure.
	typedef	::gpk::SEvent		<CHAR_ACTION>	EventMinime;
	typedef	::gpk::SEventView	<CHAR_ACTION>	EViewMinime;
#pragma pack(pop)
} // namespace

#endif // CAMPP_EVENT_ANIMAL_H_23701