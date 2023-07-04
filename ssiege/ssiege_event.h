#include "ssiege_event_animal.h"
#include "ssiege_event_world.h"
#include "ssiege_event_vehicle.h"
#include "ssiege_event_user.h"

#ifndef SSIEGE_EVENT_H_23701
#define SSIEGE_EVENT_H_23701

namespace ssiege
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (SSIEGE_EVENT, uint8_t);
	GDEFINE_ENUM_VALUE(SSIEGE_EVENT, SAIL_ACTION, 0);
	GDEFINE_ENUM_VALUE(SSIEGE_EVENT, CHAR_ACTION, 1);
	GDEFINE_ENUM_VALUE(SSIEGE_EVENT, ENGINE_ACTS, 2);
	GDEFINE_ENUM_VALUE(SSIEGE_EVENT, AIRSHIP_ACT, 3);
	GDEFINE_ENUM_VALUE(SSIEGE_EVENT, WHEELED_ACT, 4);
	GDEFINE_ENUM_VALUE(SSIEGE_EVENT, WORLD_ADMIN, 5);
	GDEFINE_ENUM_VALUE(SSIEGE_EVENT, WORLD_EVENT, 6);
	GDEFINE_ENUM_VALUE(SSIEGE_EVENT, CLIENT_ASKS, 7);
	GDEFINE_ENUM_VALUE(SSIEGE_EVENT, WORLD_SETUP, 8);
	GDEFINE_ENUM_VALUE(SSIEGE_EVENT, WORLD_VALUE, 9);
	typedef	::gpk::SEvent		<SSIEGE_EVENT>	EventSSiege;
	typedef	::gpk::SEventView	<SSIEGE_EVENT>	EViewSSiege;

	template<typename _tChildEvent>
	stainli	::gpk::error_t	eventExtractAndHandle		(const EventSSiege & parentEvent, const ::std::function<::gpk::error_t(const ::gpk::SEventView<_tChildEvent>&)> & funcHandleChild) {
		return ::gpk::eventExtractAndHandle<_tChildEvent, SSIEGE_EVENT>(parentEvent, funcHandleChild);
	}
#pragma pack(pop)
} // namespace

#endif // SSIEGE_EVENT_H_23701