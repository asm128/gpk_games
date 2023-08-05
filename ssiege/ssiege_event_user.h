#include "gpk_event.h"

#ifndef SSG_EVENT_USER_H_23701
#define SSG_EVENT_USER_H_23701

namespace ssg
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (CLIENT_ASKS, uint8_t); 
	GDEFINE_ENUM_VALUE(CLIENT_ASKS, Join, 0); // The args for this command is the player name or email or hash or token or whatever is needed to uniquely identify a player.
	GDEFINE_ENUM_VALUE(CLIENT_ASKS, Quit, 1); // The args for this command is the player name or email or hash or token or whatever is needed to uniquely identify a player.
	typedef	::gpk::SEvent		<CLIENT_ASKS>	EventClient;
	typedef	::gpk::SEventView	<CLIENT_ASKS>	EViewClient;
#pragma pack(pop)
} // namespace

#endif // SSG_EVENT_USER_H_23701