#include "gpk_event.h"

#ifndef SSIEGE_EVENT_GAME_H
#define SSIEGE_EVENT_GAME_H

namespace ssiege
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (ADMIN_GAME, uint8_t);
	GDEFINE_ENUM_VALUE(ADMIN_GAME, Initialize	, 0);
	GDEFINE_ENUM_VALUE(ADMIN_GAME, Hide			, 1);
	GDEFINE_ENUM_VALUE(ADMIN_GAME, Initialize	, 2); 
	GDEFINE_ENUM_VALUE(ADMIN_GAME, Reset		, 3); 
	typedef	::gpk::SEvent		<ADMIN_GAME>	EventAdminGame;
	typedef	::gpk::SEventView	<ADMIN_GAME>	EViewAdminGame;
#pragma pack(pop)
} // namespace

#endif // SSIEGE_EVENT_GAME_H