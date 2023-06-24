#include "gpk_image.h"
#include "gpk_array_ptr.h"

#ifndef GPK_MINE_HELL_H_66546478
#define GPK_MINE_HELL_H_66546478

namespace gpkg
{
#pragma pack(push, 1)	// 0.5 would be much better, but I suppose this will do as it isn't even necessary for our small struct
	// Holds the state for a single cell
	struct SMineHellCell {
		bool			Show			: 1;
		bool			Mine			: 1;
		bool			Flag			: 1;
		bool			What			: 1;
		bool			Boom			: 1;
	};

	struct SMineHellState {
		::gpk::rangeu64	Time			= {};
		::gpk::n2u32	BlastCoord		= {(uint32_t)-1, (uint32_t)-1};
		::gpk::n2u32	BoardSize		= {32, 32};	
		::gpk::n2u8		BlockSize		= {16, 16};	// Note that it's quite easy to hit stack overflow with more than 20 x 20 cells per block
		uint32_t		MineCount		= 10;
		bool			Blast			: 1;
		bool			BlockBased		: 1;

						SMineHellState	() : Blast(false), BlockBased(true) {}		// it's faster to initialize these now than check if they're false by default
	};
	//static constexpr const size_t asdsasdasdjnwlef = sizeof(SMineHellState);
#pragma pack(pop)

	// Holds the board state
	struct SMineHell {
		::gpk::img<::gpkg::SMineHellCell>	Board					;	// Single-block board mode (deprecated)
		::gpkg::SMineHellState				GameState				= {};

		::gpk::apobj<::gpk::img<::gpkg::SMineHellCell>>
											BoardBlocks				;	// New implementation with support for large boards.

		::gpk::error_t						GetCell					(const ::gpk::n2u32 & cellCoord, ::gpkg::SMineHellCell			** out_cell);			
		::gpk::error_t						GetCell					(const ::gpk::n2u32 & cellCoord, const ::gpkg::SMineHellCell	** out_cell)	const;	

		::gpk::error_t						GetMines				(::gpk::vbitu64 & out_Cells)	const;	// Returns the amount of mines in the board.
		::gpk::error_t						GetFlags				(::gpk::vbitu64 & out_Cells)	const;	// Returns the amount of flags in the board.
		::gpk::error_t						GetHolds				(::gpk::vbitu64 & out_Cells)	const;	// Returns the amount of holds in the board.
		::gpk::error_t						GetShows				(::gpk::vbitu64 & out_Cells)	const;	// Returns the amount of hides in the board.

		::gpk::error_t						GetHints				(::gpk::gu8		& out_Cells)	const;	// Returns the amount of hints in the board.

		::gpk::error_t						GetBlast				(::gpk::n2u32	& out_coord)	const;	// Returns 1 if blast was found, 0 if not.

		::gpk::error_t						Flag					(const ::gpk::n2u32 cell);	// Set/Clear a flag on a given tile
		::gpk::error_t						Hold					(const ::gpk::n2u32 cell);	// Set/Clear a question mark on a given tile
		::gpk::error_t						Step					(const ::gpk::n2u32 cell);	// Step on a given tile

		::gpk::error_t						Start					(const ::gpk::n2u32 boardMetrics, const uint32_t mineCount);	// Start a new game with a board of boardMetrics size and mineCount mine count.

		::gpk::error_t						Save					(::gpk::au8 & bytes);	// Write game state to array
		::gpk::error_t						Load					(::gpk::vcu8 bytes);	// Read game state from view


	};
} // namespace

#endif // GPK_MINE_HELL_H_66546478