#include "gpk_open_hell_tile.h"
#include "gpk_enum.h"

#ifndef OPEN_HELL_H_20221106
#define OPEN_HELL_H_20221106

namespace ohg
{
	GDEFINE_ENUM_TYPE(GAME_EVENT, uint8_t);
	GDEFINE_ENUM_VALUE(GAME_EVENT, CHUNK_LOAD, 0);

	struct SOpenHellEvent {
		GAME_EVENT						Event;
		::gpk::apod<uint8_t>			Data;
	};

	struct SPilot {
		uint32_t						Color;
		::gpk::SCoord3<uint32_t>		PlanetChunk;
		::gpk::SCoord3<uint32_t>		Position;
	};

	struct SOpenHell {
		::ohg::STileMap					TileMap;
	
		::gpk::aobj<::ohg::SPilot>	Pilots;

		::gpk::apod<SOpenHellEvent>		Events;


	};
} // namespace

#endif // OPEN_HELL_H_20221106