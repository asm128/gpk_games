#include "yce_ide_event_animal.h"
#include "yce_ide_event_world.h"
#include "yce_ide_event_vehicle.h"
#include "yce_ide_event_user.h"

#ifndef SSG_EVENT_H_23701
#define SSG_EVENT_H_23701

namespace ssg
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (SSG_EVENT, uint8_t);
	GDEFINE_ENUM_VALUE(SSG_EVENT, WORLD_EVENT, 0);
	GDEFINE_ENUM_VALUE(SSG_EVENT, WORLD_ADMIN, 1);
	GDEFINE_ENUM_VALUE(SSG_EVENT, ACTION_CHAR, 2);
	GDEFINE_ENUM_VALUE(SSG_EVENT, ACT_WHEELED, 3);
	GDEFINE_ENUM_VALUE(SSG_EVENT, ACT_AIRSHIP, 4);
	GDEFINE_ENUM_VALUE(SSG_EVENT, ACT_SAILING, 5);
	GDEFINE_ENUM_VALUE(SSG_EVENT, ACT_ENGINES, 6);
	GDEFINE_ENUM_VALUE(SSG_EVENT, CLIENT_ASKS, 7);
	GDEFINE_ENUM_VALUE(SSG_EVENT, WORLD_SETUP, 8);
	GDEFINE_ENUM_VALUE(SSG_EVENT, WORLD_VALUE, 9);
	typedef	::gpk::SEvent		<SSG_EVENT>	EventSSiege;
	typedef	::gpk::SEventView	<SSG_EVENT>	EViewSSiege;

	tplt<tpnm _tChildEvent>
	stainli	::gpk::error_t	eventExtractAndHandle		(const EventSSiege & parentEvent, const ::std::function<::gpk::error_t(const ::gpk::SEventView<_tChildEvent>&)> & funcHandleChild) {
		return ::gpk::eventExtractAndHandle<_tChildEvent, SSG_EVENT>(parentEvent, funcHandleChild);
	}

#define HANDLE_SSG_EVENT(_result, _state, _ssgEventName, _outputEvents) \
	es_if_failed(_result = ::ssg::eventExtractAndHandle<::ssg::_ssgEventName>(eventToProcess, [&_state, &_outputEvents, &eventToProcess](auto ev){ \
		return ::ssg::handle##_ssgEventName(_state, ev, _outputEvents); \
	}))

#define CASE_SSG_EVENT(_result, _state, _ssgEventName, _outputEvents) \
	case ::ssg::SSG_EVENT_##_ssgEventName: \
		HANDLE_SSG_EVENT(_result, _state, _ssgEventName, _outputEvents); \
		break; 
#pragma pack(pop)
} // namespace

#endif // SSG_EVENT_H_23701