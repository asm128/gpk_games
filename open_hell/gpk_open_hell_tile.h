#include "gpk_coord.h"
#include "gpk_array_static.h"
#include "gpk_array.h"
#include "gpk_image.h"

#ifndef OPEN_HELL_TILE_H_20221106
#define OPEN_HELL_TILE_H_20221106

namespace ohg
{
#pragma pack(push, 1)
	static constexpr	::gpk::SCoord2<uint8_t>					DIMENSIONS_MAP_CHUNK		= {16, 16};
	static constexpr	::gpk::SCoord2<uint8_t>					DIMENSIONS_TILE_LIGHTMAP	= {16, 16};

	// 
	template<typename _tCell, ::gpk::SCoord2<uint8_t> _nDim>
	struct STileChunk {
		_tCell					Cells	[_nDim.y][_nDim.x];
	};

	typedef ::gpk::array_static<::gpk::SCoord2<uint16_t>, 4> ATileUV;

	struct STileUV {
		::gpk::array_static<::gpk::SCoord2<uint16_t>, 4> Top;
		::gpk::array_static<::gpk::SCoord2<uint16_t>, 4> Front;
		::gpk::array_static<::gpk::SCoord2<uint16_t>, 4> Right;
	};

	struct STileSkin {
		uint8_t		LightTop	;
		uint8_t		LightFront	;
		uint8_t		LightRight	;

		uint16_t	RevertDiag	: 1;
		uint16_t	Flags		: 15;
	};

	struct STileHeight {
		::gpk::array_static<uint8_t, 4>	Heights;
		uint8_t		SkinTop		;
		uint8_t		SkinFront	;
		uint8_t		SkinRight	;
	};
#pragma pack(pop)


	struct STileMap {
		::gpk::SCoord2<uint32_t>		Origin;
		::gpk::img<uint8_t>			Tiles;
	};


	struct STilePlanet {
		::gpk::img<uint8_t>			Tiles;
	};
} // namespace

#endif // OPEN_HELL_TILE_H_20221106