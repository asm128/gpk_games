#include "ssiege_event_animal.h"
#include "ssiege_event_world.h"
#include "ssiege_event_vehicle.h"
#include "ssiege_event_user.h"

#ifndef SSG_EVENT_H_23701
#define SSG_EVENT_H_23701

namespace ssg
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (SSG_EVENT, uint8_t);
	GDEFINE_ENUM_VALUE(SSG_EVENT, SAIL_ACTION, 0);
	GDEFINE_ENUM_VALUE(SSG_EVENT, CHAR_ACTION, 1);
	GDEFINE_ENUM_VALUE(SSG_EVENT, ENGINE_ACTS, 2);
	GDEFINE_ENUM_VALUE(SSG_EVENT, AIRSHIP_ACT, 3);
	GDEFINE_ENUM_VALUE(SSG_EVENT, WHEELED_ACT, 4);
	GDEFINE_ENUM_VALUE(SSG_EVENT, ADMIN_WORLD, 5);
	GDEFINE_ENUM_VALUE(SSG_EVENT, WORLD_EVENT, 6);
	GDEFINE_ENUM_VALUE(SSG_EVENT, CLIENT_ASKS, 7);
	GDEFINE_ENUM_VALUE(SSG_EVENT, WORLD_SETUP, 8);
	GDEFINE_ENUM_VALUE(SSG_EVENT, WORLD_VALUE, 9);
	typedef	::gpk::SEvent		<SSG_EVENT>	EventSSiege;
	typedef	::gpk::SEventView	<SSG_EVENT>	EViewSSiege;

	tplt<tpnm _tChildEvent>
	stainli	::gpk::error_t	eventExtractAndHandle		(const EventSSiege & parentEvent, const ::std::function<::gpk::error_t(const ::gpk::SEventView<_tChildEvent>&)> & funcHandleChild) {
		return ::gpk::eventExtractAndHandle<_tChildEvent, SSG_EVENT>(parentEvent, funcHandleChild);
	}
#pragma pack(pop)
} // namespace

#endif // SSG_EVENT_H_23701