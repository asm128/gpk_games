#include "gpk_game.h"

#include "yce_ide_world_view.h"

#include "gpk_engine_planetary_system.h"

#ifndef SSIEGE_GAME_H
#define SSIEGE_GAME_H

namespace ssg
{
#pragma pack(push, 1)
#pragma pack(pop)
	//GDEFINE_ENUM_TYPE(SCENE_TYPE, u0_t );Likely not needed
	//GDEFINE_ENUM_VALUE(SCENE_TYPE, OuterSpace	, 0);
	//GDEFINE_ENUM_VALUE(SCENE_TYPE, Planetary	, 1);
	//GDEFINE_ENUM_VALUE(SCENE_TYPE, Submarine	, 2);
	//struct SGameObject {
	//	s1_t							Definition			=  0;
	//	s1_t							Modifier			=  0;
	//	s1_t							Level				=  1;
	//	s1_t							Owner				= -1;
	//};

	struct SSiegeGame {
		::gpk::SGame					Global				= {};
		::gpk::SEnginePlanetarySystem	SolarSystem			= {};
		::ssg::SWorldView			World				= {};
		::gpk::SCamera					Camera				= {};

		::gpk::eid_t					Controlled			= ::gpk::EID_INVALID;
		::gpk::aobj<::gpk::SPlayer>		Players;

		::gpk::eid_t					ShipCore			= ::gpk::EID_INVALID;
		::gpk::eid_t					ShipRing			= ::gpk::EID_INVALID;
		::gpk::aeid						ShipEngines			= {};
		::gpk::aeid						ShipWeapons			= {};

		::gpk::SEngine					Engine				= {};

		::gpk::error_t					Save				(::gpk::au8 & output)	const	{ 
			{ if_fail_fe(Global.Save(output)); }
			{ if_fail_fe(SolarSystem.Save(output)); }
			{ if_fail_fe(World.Save(output)); }
			{ if_fail_fe(Engine.Save(output)); }
			return 0;
		}

		::gpk::error_t					Load				(::gpk::vcu8 & input)	{ 
			{ if_fail_fe(Global.Load(input)); }
			{ if_fail_fe(SolarSystem.Load(input)); }
			{ if_fail_fe(World.Load(input)); }
			{ if_fail_fe(Engine.Load(input)); }
			return 0;
		}

	};

	::gpk::error_t					ssiegeGameSetup		(::ssg::SSiegeGame & world);
	::gpk::error_t					ssiegeGameUpdate	(::ssg::SSiegeGame & world, ::gpk::vpobj<::ssg::EventSSiege> inputEvents, ::gpk::apobj<::ssg::EventSSiege> & outputEvents, double secondsElapsed);
	::gpk::error_t					ssiegeGameDraw		(::ssg::SSiegeGame & world
		, ::gpk::rtbgra8d32			& backBuffer
		, const ::gpk::n3f32		& cameraPosition
		, const ::gpk::n3f32		& cameraTarget
		, const ::gpk::n3f32		& cameraUp = {0, 1, 0}
		, const ::gpk::minmaxf32	& nearFar = {.1f, 10000.f}
		);
} // namespace

#endif // SSIEGE_GAME_H