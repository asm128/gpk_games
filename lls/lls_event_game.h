#include "gpk_event.h"

#ifndef LLS_EVENT_GAME_H
#define LLS_EVENT_GAME_H

namespace lls
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (ADMIN_GAME, uint8_t);
	GDEFINE_ENUM_VALUE(ADMIN_GAME, Initialize	, 0);
	GDEFINE_ENUM_VALUE(ADMIN_GAME, Hide			, 1);
	GDEFINE_ENUM_VALUE(ADMIN_GAME, Seek			, 2); 
	GDEFINE_ENUM_VALUE(ADMIN_GAME, Reset		, 3); 
	typedef	::gpk::SEvent		<ADMIN_GAME>	EventAdminGame;
	typedef	::gpk::SEventView	<ADMIN_GAME>	EViewAdminGame;
#pragma pack(pop)
} // namespace

#endif // LLS_EVENT_GAME_H