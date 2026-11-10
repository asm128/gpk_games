#include "lls_event_animal.h"
#include "lls_event_world.h"
#include "lls_event_vehicle.h"
#include "lls_event_user.h"

#ifndef LLS_EVENT_H_23701
#define LLS_EVENT_H_23701

namespace lls
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (LLS_EVENT, uint8_t);
	GDEFINE_ENUM_VALUE(LLS_EVENT, WORLD_EVENT, 0);
	GDEFINE_ENUM_VALUE(LLS_EVENT, WORLD_ADMIN, 1);
	GDEFINE_ENUM_VALUE(LLS_EVENT, ACTION_CHAR, 2);
	GDEFINE_ENUM_VALUE(LLS_EVENT, ACT_WHEELED, 3);
	GDEFINE_ENUM_VALUE(LLS_EVENT, ACT_AIRSHIP, 4);
	GDEFINE_ENUM_VALUE(LLS_EVENT, ACT_SAILING, 5);
	GDEFINE_ENUM_VALUE(LLS_EVENT, ACT_ENGINES, 6);
	GDEFINE_ENUM_VALUE(LLS_EVENT, CLIENT_ASKS, 7);
	GDEFINE_ENUM_VALUE(LLS_EVENT, WORLD_SETUP, 8);
	GDEFINE_ENUM_VALUE(LLS_EVENT, WORLD_VALUE, 9);
	typedef	::gpk::SEvent		<LLS_EVENT>	EventLLS;
	typedef	::gpk::SEventView	<LLS_EVENT>	EViewLLS;

	tplt<tpnm _tChildEvent>
	stin	gpk::err_t	eventExtractAndHandle		(const EventLLS & parentEvent, const ::std::function<gpk::err_t(const ::gpk::SEventView<_tChildEvent>&)> & funcHandleChild) {
		return ::gpk::eventExtractAndHandle<_tChildEvent, LLS_EVENT>(parentEvent, funcHandleChild);
	}

#define HANDLE_LLS_EVENT(_result, _state, _llsEventName, _outputEvents) \
	es_if_failed(_result = ::lls::eventExtractAndHandle<::lls::_llsEventName>(eventToProcess, [&_state, &_outputEvents, &eventToProcess](auto ev){ \
		return ::lls::handle##_llsEventName(_state, ev, _outputEvents); \
	}))

#define CASE_LLS_EVENT(_result, _state, _llsEventName, _outputEvents) \
	case ::lls::LLS_EVENT_##_llsEventName: \
		HANDLE_LLS_EVENT(_result, _state, _llsEventName, _outputEvents); \
		break; 
#pragma pack(pop)
} // namespace

#endif // LLS_EVENT_H_23701