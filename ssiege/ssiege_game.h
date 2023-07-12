#include "ssiege_world.h"
#include "gpk_engine_planetary_system.h"
#include "gpk_engine_game.h"

#ifndef SSIEGE_GAME_H
#define SSIEGE_GAME_H

namespace ssiege
{
#pragma pack(push, 1)
#pragma pack(pop)
	struct SSiegeGame {
		::gpk::SGame					Global			= {};
		::gpk::SEnginePlanetarySystem	SolarSystem		= {};
		::ssiege::SWorldView			World			= {};
	};
} // namespace

#endif // SSIEGE_GAME_H