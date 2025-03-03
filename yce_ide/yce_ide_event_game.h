#include "gpk_event.h"

#ifndef SSG_EVENT_GAME_H
#define SSG_EVENT_GAME_H

namespace ssg
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

#endif // SSG_EVENT_GAME_H