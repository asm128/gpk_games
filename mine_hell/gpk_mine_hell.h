#include "gpk_image.h"
#include "gpk_ptr.h"

#ifndef GPK_MINE_HELL_H_66546478
#define GPK_MINE_HELL_H_66546478

namespace gpkg
{
#pragma pack(push, 1)	// 0.5 would be much better, but I suppose this will do as it isn't even necessary for our small struct
	// Holds the state for a single cell
	struct SMineHellCell {
		bool									Show					: 1;
		bool									Mine					: 1;
		bool									Flag					: 1;
		bool									What					: 1;
		bool									Boom					: 1;
	};

	struct SMineHellState {
		::gpk::SRange<uint64_t>					Time					= {};
		::gpk::SCoord2<uint32_t>				BlastCoord				= {(uint32_t)-1, (uint32_t)-1};
		::gpk::SCoord2<uint32_t>				BoardSize				= {32, 32};	// Note that it's quite easy to hit stack overflow with more than 20 x 20 cells per block
		::gpk::SCoord2<uint32_t>				BlockSize				= {16, 16};	// Note that it's quite easy to hit stack overflow with more than 20 x 20 cells per block
		uint32_t								MineCount				= 10;
		//char									PlayerId[127]			= {};
		bool									Blast					: 1;
		bool									BlockBased				: 1;

												SMineHellState			() : Blast(false), BlockBased(true) {}		// it's faster to initialize these now than check if they're false by default
	};
	//static constexpr const size_t asdsasdasdjnwlef = sizeof(SMineHellState);
#pragma pack(pop)

	// Holds the board state
	struct SMineHell {
		::gpk::img<::gpkg::SMineHellCell>	Board					;	// Single-block board mode (deprecated)
		::gpkg::SMineHellState					GameState				= {};

		::gpk::aobj<::gpk::pobj<::gpk::img<::gpkg::SMineHellCell>>>
												BoardBlocks				;	// New implementation with support for large boards.

		::gpk::error_t							GetCell					(const ::gpk::SCoord2<uint32_t> & cellCoord, ::gpkg::SMineHellCell ** out_cell);					// These functions return the cell
		::gpk::error_t							GetCell					(const ::gpk::SCoord2<uint32_t> & cellCoord, const ::gpkg::SMineHellCell ** out_cell)	const;	// These functions return the cell

		::gpk::error_t							GetMines				(::gpk::view_bit<uint64_t> & out_Cells)	const;	// These functions return the amount of mines in the board.
		::gpk::error_t							GetFlags				(::gpk::view_bit<uint64_t> & out_Cells)	const;	// These functions return the amount of flags in the board.
		::gpk::error_t							GetHolds				(::gpk::view_bit<uint64_t> & out_Cells)	const;	// These functions return the amount of holds in the board.
		::gpk::error_t							GetShows				(::gpk::view_bit<uint64_t> & out_Cells)	const;	// These functions return the amount of hides in the board.
		::gpk::error_t							GetHints				(::gpk::view2d<uint8_t> & out_Cells)	const;	// These functions return the amount of hints in the board.
		::gpk::error_t							GetBlast				(::gpk::SCoord2<uint32_t> & out_coord)	const;	// Returns 1 if blast was found, 0 if not.

		::gpk::error_t							Flag					(const ::gpk::SCoord2<uint32_t> cell);	// Set/Clear a flag on a given tile
		::gpk::error_t							Hold					(const ::gpk::SCoord2<uint32_t> cell);	// Set/Clear a question mark on a given tile
		::gpk::error_t							Step					(const ::gpk::SCoord2<uint32_t> cell);	// Step on a given tile
		::gpk::error_t							Start					(const ::gpk::SCoord2<uint32_t> boardMetrics, const uint32_t mineCount);	// Start a new game with a board of boardMetrics size and mineCount mine count.

		::gpk::error_t							Save					(::gpk::apod<char_t> & bytes);	// Write game state to array
		::gpk::error_t							Load					(::gpk::vcb		bytes);	// Read game state from view


	};
} // namespace

#endif // GPK_MINE_HELL_H_66546478