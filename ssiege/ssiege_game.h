#include "gpk_game.h"

#include "ssiege_world_view.h"

#include "gpk_engine_planetary_system.h"

#ifndef SSIEGE_GAME_H
#define SSIEGE_GAME_H

namespace ssg
{
#pragma pack(push, 1)
#pragma pack(pop)
	struct SSiegeGame {
		::gpk::SGame					Global			= {};
		::gpk::SEnginePlanetarySystem	SolarSystem		= {};
		::ssiege::SWorldView			World			= {};
		::gpk::SCamera					Camera			= {};

		::gpk::eid_t					Controlled		= ::gpk::EID_INVALID;
		::gpk::aobj<::gpk::SGamePlayer>	Players;

		::gpk::eid_t					ShipEntity		= ::gpk::EID_INVALID;

		::gpk::SEngine					Engine			= {};

		::gpk::error_t			Save			(::gpk::au8 & output)	const	{ 
			gpk_necs(Global.Save(output));
			//gpk_necs(SolarSystem.Save(output));
			gpk_necs(Engine.Save(output));
			return 0;
		}

		::gpk::error_t			Load				(::gpk::vcu8 & input)	{ 
			gpk_necs(Global.Load(input));
			//gpk_necs(SolarSystem.Load(input));
			gpk_necs(Engine.Load(input));
			return 0;
		}

	};

	::gpk::error_t			ssiegeGameSetup		(::ssg::SSiegeGame & world);
	::gpk::error_t			ssiegeGameUpdate	(::ssg::SSiegeGame & world, ::gpk::vpobj<::ssiege::EventSSiege> inputEvents, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents, double secondsElapsed);
	::gpk::error_t			ssiegeGameDraw		(::ssg::SSiegeGame & world
		, ::gpk::rtbgra8d32			& backBuffer
		, const ::gpk::n3f32		& cameraPosition
		, const ::gpk::n3f32		& cameraTarget
		, const ::gpk::n3f32		& cameraUp = {0, 1, 0}
		, const ::gpk::minmaxf32	& nearFar = {.1f, 10000.f}
		);
} // namespace

#endif // SSIEGE_GAME_H