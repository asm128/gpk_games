#include "gpk_game.h"

#include "ssiege_world_view.h"

#include "gpk_engine_planetary_system.h"

#ifndef SSIEGE_GAME_H
#define SSIEGE_GAME_H

namespace ssg
{
#pragma pack(push, 1)
#pragma pack(pop)
	//GDEFINE_ENUM_TYPE(SCENE_TYPE, uint8_t);Likely not needed
	//GDEFINE_ENUM_VALUE(SCENE_TYPE, OuterSpace	, 0);
	//GDEFINE_ENUM_VALUE(SCENE_TYPE, Planetary	, 1);
	//GDEFINE_ENUM_VALUE(SCENE_TYPE, Submarine	, 2);
	//struct SGameObject {
	//	int16_t							Definition			=  0;
	//	int16_t							Modifier			=  0;
	//	int16_t							Level				=  1;
	//	int16_t							Owner				= -1;
	//};

	struct SSiegeGame {
		::gpk::SGame					Global				= {};
		::gpk::SEnginePlanetarySystem	SolarSystem			= {};
		::ssg::SWorldView				World				= {};
		::gpk::SCamera					Camera				= {};

		::gpk::eid_t					Controlled			= ::gpk::EID_INVALID;
		::gpk::aobj<::gpk::SPlayer>		Players;

		::gpk::eid_t					ShipCore			= ::gpk::EID_INVALID;
		::gpk::eid_t					ShipRing			= ::gpk::EID_INVALID;
		::gpk::aeid						ShipEngines			= {};
		::gpk::aeid						ShipWeapons			= {};

		::gpk::SEngine					Engine				= {};

		::gpk::error_t					Save				(::gpk::au0_t & output)	const	{ 
			gpk_necs(Global.Save(output));
			gpk_necs(SolarSystem.Save(output));
			gpk_necs(World.Save(output));
			gpk_necs(Engine.Save(output));
			return 0;
		}

		::gpk::error_t					Load				(::gpk::vcu0_t & input)	{ 
			gpk_necs(Global.Load(input));
			gpk_necs(SolarSystem.Load(input));
			gpk_necs(World.Load(input));
			gpk_necs(Engine.Load(input));
			return 0;
		}

	};

	::gpk::error_t					ssiegeGameSetup		(::ssg::SSiegeGame & world);
	::gpk::error_t					ssiegeGameUpdate	(::ssg::SSiegeGame & world, ::gpk::vpobj<::ssg::EventSSiege> inputEvents, ::gpk::apobj<::ssg::EventSSiege> & outputEvents, double secondsElapsed);
	::gpk::error_t					ssiegeGameDraw		(::ssg::SSiegeGame & world
		, ::gpk::rtbgra8d32			& backBuffer
		, const ::gpk::n3f2_t		& cameraPosition
		, const ::gpk::n3f2_t		& cameraTarget
		, const ::gpk::n3f2_t		& cameraUp = {0, 1, 0}
		, const ::gpk::minmaxf2_t	& nearFar = {.1f, 10000.f}
		);
} // namespace

#endif // SSIEGE_GAME_H