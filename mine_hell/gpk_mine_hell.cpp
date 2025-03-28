#include "gpk_mine_hell.h"

#include "gpk_chrono.h"
#include "gpk_encoding.h"
#include "gpk_deflate.h"
#include "gpk_array_static.h"

static	::gpk::n2u32	getBlockCount							(const ::gpk::n2u32 boardMetrics, const ::gpk::n2u8 blockMetrics)	{
	return
		{ boardMetrics.x / blockMetrics.x + one_if(boardMetrics.x % blockMetrics.x)
		, boardMetrics.y / blockMetrics.y + one_if(boardMetrics.y % blockMetrics.y)
		};
}

static	::gpk::n2u32	getBlockFromCoord						(const ::gpk::n2u32 boardMetrics, const ::gpk::n2u8 blockMetrics)	{
	return
		{ boardMetrics.x / blockMetrics.x
		, boardMetrics.y / blockMetrics.y
		};
}

static	::gpk::n2u32	getLocalCoordFromCoord					(const ::gpk::n2u32 boardMetrics, const ::gpk::n2u8 blockMetrics)	{
	return
		{ boardMetrics.x % blockMetrics.x
		, boardMetrics.y % blockMetrics.y
		};
}

::gpk::error_t			gpkg::SMineHell::GetCell				(const ::gpk::n2u32 & cellCoord, ::gpkg::SMineHellCell ** out_cell)			{
	ree_if(cellCoord.x > GameState.BoardSize.x || cellCoord.y > GameState.BoardSize.y, "Invalid cell: %u, %u. Max: %u, %u.", cellCoord.x, cellCoord.y, GameState.BoardSize.x, GameState.BoardSize.y);
	if(false == GameState.BlockBased)
		*out_cell				= &Board[cellCoord.y][cellCoord.x];
	else {
		const ::gpk::n2u32			cellBlock							= ::getBlockFromCoord(cellCoord, GameState.BlockSize);
		const ::gpk::n2u32			cellPosition						= ::getLocalCoordFromCoord(cellCoord, GameState.BlockSize);
		const ::gpk::n2u32			blockCount							= ::getBlockCount(GameState.BoardSize, GameState.BlockSize);
		const uint32_t				blockIndex							= cellBlock.y * blockCount.x + cellBlock.x;
		if(0 == BoardBlocks[blockIndex].get_ref())
			BoardBlocks[blockIndex]->resize(GameState.BlockSize, {});
		*out_cell				= &(*BoardBlocks[blockIndex])[cellPosition.y][cellPosition.x];
	}
	return 0;
}

::gpk::error_t			gpkg::SMineHell::GetCell	(const ::gpk::n2u32 & cellCoord, const ::gpkg::SMineHellCell ** out_cell)		const	{
	ree_if(cellCoord.x > GameState.BoardSize.x || cellCoord.y > GameState.BoardSize.y, "Invalid cell: %u, %u. Max: %u, %u.", cellCoord.x, cellCoord.y, GameState.BoardSize.x, GameState.BoardSize.y);
	if(false == GameState.BlockBased)
		*out_cell				= &Board[cellCoord.y][cellCoord.x];
	else {
		const ::gpk::n2u32			cellBlock				= ::getBlockFromCoord(cellCoord, GameState.BlockSize);
		const ::gpk::n2u32			cellPosition			= ::getLocalCoordFromCoord(cellCoord, GameState.BlockSize);
		const ::gpk::n2u32			blockCount				= ::getBlockCount(GameState.BoardSize, GameState.BlockSize);
		const uint32_t				blockIndex				= cellBlock.y * blockCount.x + cellBlock.x;
		*out_cell				= (0 == BoardBlocks[blockIndex].get_ref()) ? 0 : &(*BoardBlocks[blockIndex])[cellPosition.y][cellPosition.x];
	}
	return 0;
}

::gpk::error_t			gpkg::SMineHell::GetBlast	(::gpk::n2u32 & out_coord)	const {
	if(GameState.BlockBased)
		out_coord				= GameState.BlastCoord;
	else {
		const ::gpk::n2u32			gridMetrix					= Board.metrics();
		for(uint32_t y = 0; y < gridMetrix.y; ++y) {
			for(uint32_t x = 0; x < gridMetrix.x; ++x) {
				const uint8_t				value						=	Board[y][x].Boom ? 1 : 0;
				if(value > 0) {
					out_coord				= {x, y};
					return 1;
				}
			}
		}
	}
	return 0;
}

static	::gpk::error_t	assignGridBool	(::gpk::vbitu64 & out_Cells, const ::gpk::n2u32 & gridMetrix, const ::std::function<bool(const ::gpkg::SMineHellCell & _cellValue)> & funcTestCell, const ::std::function<void(const ::gpk::n2u32 & cellCoord, const ::gpkg::SMineHellCell ** out_cell)> funcGetCell)	{
	uint32_t					total			= 0;
	for(uint32_t y = 0; y < gridMetrix.y; ++y) {
		const uint32_t				offset			= y * gridMetrix.x;
		for(uint32_t x = 0; x < gridMetrix.x; ++x) {
			const ::gpkg::SMineHellCell	* cellValue		= 0;
			funcGetCell({x ,y}, &cellValue);
			uint8_t						value			= cellValue ? one_if(funcTestCell(*cellValue)) : 0;
			out_Cells[offset + x]	= value;
			if(value)
				total					+= value;
		}
	}
	return total;
}

::gpk::error_t			gpkg::SMineHell::GetMines	(::gpk::vbitu64 & out_Cells)		const	{ const ::gpk::n2u32 gridMetrix = (GameState.BlockBased) ? GameState.BoardSize : Board.metrics(); return ::assignGridBool(out_Cells, gridMetrix, [](auto cellValue) { return cellValue.Mine; }, [this](auto coord, auto cell) { GetCell(coord, cell); } ); }
::gpk::error_t			gpkg::SMineHell::GetFlags	(::gpk::vbitu64 & out_Cells)		const	{ const ::gpk::n2u32 gridMetrix = (GameState.BlockBased) ? GameState.BoardSize : Board.metrics(); return ::assignGridBool(out_Cells, gridMetrix, [](auto cellValue) { return cellValue.Flag; }, [this](auto coord, auto cell) { GetCell(coord, cell); } ); }
::gpk::error_t			gpkg::SMineHell::GetHolds	(::gpk::vbitu64 & out_Cells)		const	{ const ::gpk::n2u32 gridMetrix = (GameState.BlockBased) ? GameState.BoardSize : Board.metrics(); return ::assignGridBool(out_Cells, gridMetrix, [](auto cellValue) { return cellValue.What; }, [this](auto coord, auto cell) { GetCell(coord, cell); } ); }
::gpk::error_t			gpkg::SMineHell::GetShows	(::gpk::vbitu64 & out_Cells)		const	{ const ::gpk::n2u32 gridMetrix = (GameState.BlockBased) ? GameState.BoardSize : Board.metrics(); return ::assignGridBool(out_Cells, gridMetrix, [](auto cellValue) { return cellValue.Show; }, [this](auto coord, auto cell) { GetCell(coord, cell); } ); }
::gpk::error_t			gpkg::SMineHell::GetHints	(::gpk::grid<uint8_t> & out_Cells)	const	{ const ::gpk::n2u32 gridMetrix = (GameState.BlockBased) ? GameState.BoardSize : Board.metrics();
	for(int32_t y = 0; y < (int32_t)gridMetrix.y; ++y)
	for(int32_t x = 0; x < (int32_t)gridMetrix.x; ++x) {
		const ::gpkg::SMineHellCell	* cellValue					= 0;
		GetCell({(uint32_t)x, (uint32_t)y}, &cellValue);
		//if(false == cellValue->Show)
		//	continue;
		if(cellValue && cellValue->Mine)
			continue;
		uint8_t						& out						= out_Cells[y][x]	= 0;
		::gpk::n2i32				coordToTest					= {};	// actually by making this uint32_t we could easily change all the conditions to be coordToTest.i < gridMetrix.i. However, I'm too lazy to start optimizing what's hardly the bottleneck
		const ::gpkg::SMineHellCell	* cellValueToTest			= 0;
		coordToTest	= {x - 1, y - 1	};	if(coordToTest.y >= 0 && coordToTest.x >= 0)										{ if(-1 != GetCell(coordToTest.u2_t(), &cellValueToTest) && 0 != cellValueToTest && cellValueToTest->Mine) ++out; }
		coordToTest	= {x	, y - 1	};	if(coordToTest.y >= 0)																{ if(-1 != GetCell(coordToTest.u2_t(), &cellValueToTest) && 0 != cellValueToTest && cellValueToTest->Mine) ++out; }
		coordToTest	= {x + 1, y - 1	};	if(coordToTest.y >= 0 && coordToTest.x < (int32_t)gridMetrix.x)						{ if(-1 != GetCell(coordToTest.u2_t(), &cellValueToTest) && 0 != cellValueToTest && cellValueToTest->Mine) ++out; }
		coordToTest	= {x - 1, y		};	if(coordToTest.x >= 0)																{ if(-1 != GetCell(coordToTest.u2_t(), &cellValueToTest) && 0 != cellValueToTest && cellValueToTest->Mine) ++out; }
		coordToTest	= {x + 1, y		};	if(coordToTest.x < (int32_t)gridMetrix.x)											{ if(-1 != GetCell(coordToTest.u2_t(), &cellValueToTest) && 0 != cellValueToTest && cellValueToTest->Mine) ++out; }
		coordToTest	= {x - 1, y + 1	};	if(coordToTest.y < (int32_t)gridMetrix.y && coordToTest.x >= 0)						{ if(-1 != GetCell(coordToTest.u2_t(), &cellValueToTest) && 0 != cellValueToTest && cellValueToTest->Mine) ++out; }
		coordToTest	= {x	, y + 1	};	if(coordToTest.y < (int32_t)gridMetrix.y)											{ if(-1 != GetCell(coordToTest.u2_t(), &cellValueToTest) && 0 != cellValueToTest && cellValueToTest->Mine) ++out; }
		coordToTest	= {x + 1, y + 1	};	if(coordToTest.y < (int32_t)gridMetrix.y && coordToTest.x < (int32_t)gridMetrix.x)	{ if(-1 != GetCell(coordToTest.u2_t(), &cellValueToTest) && 0 != cellValueToTest && cellValueToTest->Mine) ++out; }
	}
	return 0;
}

static	::gpk::error_t	uncoverCell						(::gpk::grid<::gpkg::SMineHellCell> & board, const ::gpk::grid<uint8_t> & hints, const ::gpk::n2u32 & cellOffset, const ::gpk::n2u32 & boardSize, const ::gpk::n2i32 localCellCoord, ::gpk::apod<::gpk::n2u32> & outOfRangeCells);
static	::gpk::error_t	uncoverCellIfNeeded				(::gpk::grid<::gpkg::SMineHellCell> & board, const ::gpk::grid<uint8_t> & hints, const ::gpk::n2u32 & cellOffset, const ::gpk::n2u32 & boardSize, const ::gpk::n2i32 localCellCoord, ::gpk::apod<::gpk::n2u32> & outOfRangeCells) {
	const ::gpkg::SMineHellCell	cellToTest						= board[localCellCoord.y][localCellCoord.x];
	if(false == cellToTest.Mine && false == cellToTest.Show)
		::uncoverCell(board, hints, cellOffset, boardSize, localCellCoord, outOfRangeCells);
	return 0;
}

static	::gpk::error_t	enqueueCellIfNeeded				(const ::gpk::n2u32 & cellOffset, const ::gpk::n2u32 & boardSize, const ::gpk::n2i32 localCellCoord, ::gpk::apod<::gpk::n2u32> & outOfRangeCells) {
	const ::gpk::n2u32			globalCoordToTest				= cellOffset + localCellCoord.u2_t();
	if(::gpk::in_range(globalCoordToTest, {{}, boardSize}) && 0 > ::gpk::find(globalCoordToTest, {outOfRangeCells.begin(), outOfRangeCells.size()}))
		gpk_necs(outOfRangeCells.push_back(globalCoordToTest));
	return 0;
}


constexpr ::gpk::array_static<const ::gpk::n2i32, 8>	SURROUNDING_CELLS_OFFSETS = 
	{ ::gpk::n2i32{0 - 1, 0 - 1} // top left
	, ::gpk::n2i32{0 + 0, 0 - 1} // top center
	, ::gpk::n2i32{0 + 1, 0 - 1} // top right
	, ::gpk::n2i32{0 - 1, 0 + 0} // center left
	, ::gpk::n2i32{0 + 1, 0 + 0} // center right
	, ::gpk::n2i32{0 - 1, 0 + 1} // bottom left
	, ::gpk::n2i32{0 + 0, 0 + 1} // bottom center
	, ::gpk::n2i32{0 + 1, 0 + 1} // bottom right
	};


static	::gpk::error_t	uncoverCell						(::gpk::grid<::gpkg::SMineHellCell> & block, const ::gpk::grid<uint8_t> & hints, const ::gpk::n2u32 & cellOffset, const ::gpk::n2u32 & boardSize, const ::gpk::n2i32 localCellCoord, ::gpk::apod<::gpk::n2u32> & outOfRangeCells) {
	::gpkg::SMineHellCell		& currentCell					= block[localCellCoord.y][localCellCoord.x];
	if(false == currentCell.Mine && false == currentCell.Show) {
		currentCell.Show		= true;
		const ::gpk::n2u32			gridMetrix						= block.metrics();
		const ::gpk::n2u32			globalCellCoord					= localCellCoord.u2_t() + cellOffset;
		if(::gpk::in_range(globalCellCoord, {{}, boardSize}) && 0 == hints[cellOffset.y + localCellCoord.y][cellOffset.x + localCellCoord.x]) {
			::gpk::n2i32				coordToTest						= {};	//
			coordToTest	= ::SURROUNDING_CELLS_OFFSETS[0] + localCellCoord; if(coordToTest.y >= 0 && coordToTest.x >= 0)											::uncoverCellIfNeeded(block, hints, cellOffset, boardSize, coordToTest, outOfRangeCells); else ::enqueueCellIfNeeded(cellOffset, boardSize, coordToTest, outOfRangeCells);
			coordToTest	= ::SURROUNDING_CELLS_OFFSETS[1] + localCellCoord; if(coordToTest.y >= 0)																::uncoverCellIfNeeded(block, hints, cellOffset, boardSize, coordToTest, outOfRangeCells); else ::enqueueCellIfNeeded(cellOffset, boardSize, coordToTest, outOfRangeCells);
			coordToTest	= ::SURROUNDING_CELLS_OFFSETS[2] + localCellCoord; if(coordToTest.y >= 0 && coordToTest.x < (int32_t)gridMetrix.x)						::uncoverCellIfNeeded(block, hints, cellOffset, boardSize, coordToTest, outOfRangeCells); else ::enqueueCellIfNeeded(cellOffset, boardSize, coordToTest, outOfRangeCells);
			coordToTest	= ::SURROUNDING_CELLS_OFFSETS[3] + localCellCoord; if(coordToTest.x >= 0)																::uncoverCellIfNeeded(block, hints, cellOffset, boardSize, coordToTest, outOfRangeCells); else ::enqueueCellIfNeeded(cellOffset, boardSize, coordToTest, outOfRangeCells);
			coordToTest	= ::SURROUNDING_CELLS_OFFSETS[4] + localCellCoord; if(coordToTest.x < (int32_t)gridMetrix.x)											::uncoverCellIfNeeded(block, hints, cellOffset, boardSize, coordToTest, outOfRangeCells); else ::enqueueCellIfNeeded(cellOffset, boardSize, coordToTest, outOfRangeCells);
			coordToTest	= ::SURROUNDING_CELLS_OFFSETS[5] + localCellCoord; if(coordToTest.y < (int32_t)gridMetrix.y && coordToTest.x >= 0)						::uncoverCellIfNeeded(block, hints, cellOffset, boardSize, coordToTest, outOfRangeCells); else ::enqueueCellIfNeeded(cellOffset, boardSize, coordToTest, outOfRangeCells);
			coordToTest	= ::SURROUNDING_CELLS_OFFSETS[6] + localCellCoord; if(coordToTest.y < (int32_t)gridMetrix.y)											::uncoverCellIfNeeded(block, hints, cellOffset, boardSize, coordToTest, outOfRangeCells); else ::enqueueCellIfNeeded(cellOffset, boardSize, coordToTest, outOfRangeCells);
			coordToTest	= ::SURROUNDING_CELLS_OFFSETS[7] + localCellCoord; if(coordToTest.y < (int32_t)gridMetrix.y && coordToTest.x < (int32_t)gridMetrix.x)	::uncoverCellIfNeeded(block, hints, cellOffset, boardSize, coordToTest, outOfRangeCells); else ::enqueueCellIfNeeded(cellOffset, boardSize, coordToTest, outOfRangeCells);
		}
	}
	return 0;
}

::gpk::error_t			gpkg::SMineHell::Flag			(const ::gpk::n2u32 cellCoord)	{ ::gpkg::SMineHellCell * cellData = 0; GetCell(cellCoord, &cellData); if(false == cellData->Show) { cellData->Flag = !cellData->Flag; cellData->What = false; } return 0; }
::gpk::error_t			gpkg::SMineHell::Hold			(const ::gpk::n2u32 cellCoord)	{ ::gpkg::SMineHellCell * cellData = 0; GetCell(cellCoord, &cellData); if(false == cellData->Show) { cellData->What = !cellData->What; cellData->Flag = false; } return 0; }
::gpk::error_t			gpkg::SMineHell::Step			(const ::gpk::n2u32 cellCoord)	{ ::gpkg::SMineHellCell * cellData = 0; GetCell(cellCoord, &cellData);
	if(false == cellData->Show) {
		::gpk::n2					boardMetrics					= GameState.BlockBased ? GameState.BoardSize : Board.metrics();
		if(cellData->Mine && false == cellData->Flag)
			cellData->Boom			= true;
		else if(false == cellData->Flag) {
			::gpk::imgu8				hints;
			hints.resize(boardMetrics, {});
			GetHints(hints.View);
			::gpk::apod<::gpk::n2u32>	outOfRangeCells;
			if(false == GameState.BlockBased)
				gpk_necs(::uncoverCell(Board.View, hints.View, {}, boardMetrics, cellCoord.s2_t(), outOfRangeCells));
			else {
				::gpk::n2u32				cellBlock						= getBlockFromCoord		(cellCoord		, GameState.BlockSize);
				::gpk::n2u32				cellPosition					= getLocalCoordFromCoord(cellCoord		, GameState.BlockSize);
				::gpk::n2u32				blockCount						= getBlockCount			(boardMetrics	, GameState.BlockSize);
				uint32_t					blockIndex						= cellBlock.y * blockCount.x + cellBlock.x;
				::gpk::n2i32				localCellCoord					= cellCoord.s2_t();
				localCellCoord			-= ::gpk::n2u32{cellBlock.x * GameState.BlockSize.x, cellBlock.y * GameState.BlockSize.y}.s2_t();
				if(0 == BoardBlocks[blockIndex].get_ref())
					gpk_necs(BoardBlocks[blockIndex]->resize(GameState.BlockSize, {}));
				gpk_necs(::uncoverCell(BoardBlocks[blockIndex]->View, hints.View, {cellBlock.x * GameState.BlockSize.x, cellBlock.y * GameState.BlockSize.y}, boardMetrics, localCellCoord, outOfRangeCells));
				for(uint32_t iCell = 0; iCell < outOfRangeCells.size(); ++iCell) {
					const ::gpk::n2u32			globalCellCoord					= outOfRangeCells[iCell];
					cellBlock				= getBlockFromCoord		(globalCellCoord, GameState.BlockSize);
					cellPosition			= getLocalCoordFromCoord(globalCellCoord, GameState.BlockSize);
					blockCount				= getBlockCount			(boardMetrics	, GameState.BlockSize);
					blockIndex				= cellBlock.y * blockCount.x + cellBlock.x;
					localCellCoord			= globalCellCoord.s2_t();
					localCellCoord			-= ::gpk::n2u32{cellBlock.x * GameState.BlockSize.x, cellBlock.y * GameState.BlockSize.y}.s2_t();
					if(0 == BoardBlocks[blockIndex].get_ref())
						gpk_necs(BoardBlocks[blockIndex]->resize(GameState.BlockSize, {}));
					gpk_necs(::uncoverCell(BoardBlocks[blockIndex]->View, hints.View, {cellBlock.x * GameState.BlockSize.x, cellBlock.y * GameState.BlockSize.y}, boardMetrics, localCellCoord, outOfRangeCells));
				}
			}
		}
		if(false == cellData->Boom) {	// Check if we won after uncovering the cell(s)
			::gpk::imgmonou64						cellsMines;
			::gpk::imgmonou64						cellsShows;
			gpk_necs(cellsMines.resize(boardMetrics));
			gpk_necs(cellsShows.resize(boardMetrics));
			const int32_t							totalShows						= GetShows(cellsShows.View);
			const int32_t							totalMines						= GetMines(cellsMines.View);
			if((boardMetrics.x * boardMetrics.y - totalShows) <= (uint32_t)totalMines && false == cellData->Boom)	// Win!
				GameState.Time.Count				= ::gpk::timeCurrent() - GameState.Time.Offset;
		}
		else {
			GameState.BlastCoord	= cellCoord;
			GameState.Blast			= true;
		}
	}
	return cellData->Boom ? 1 : 0;
}

::gpk::error_t			gpkg::SMineHell::Start		(const ::gpk::n2u32 boardMetrics, const uint32_t mineCount)	{
	GameState				= {};
	GameState.BoardSize		= {::gpk::max(3U, boardMetrics.x), ::gpk::max(3U, boardMetrics.y)};
	GameState.MineCount		= (uint32_t)::gpk::max(2.0, ::gpk::min((double)mineCount, ((boardMetrics.x * (double)boardMetrics.y) / 4) * 3));
	GameState.Time.Offset	= ::gpk::timeCurrent();
	if(false == GameState.BlockBased) { // Old small model (deprecated)
		gpk_necs(Board.resize(boardMetrics, {}));
		for(uint32_t iMine = 0; iMine < mineCount; ++iMine) {
			::gpk::n2u32				cellPosition					= {rand() % boardMetrics.x, rand() % boardMetrics.y};
			::gpkg::SMineHellCell		* mineData						= &Board[cellPosition.y][cellPosition.x];
			while(mineData->Mine) {
				cellPosition			= {rand() % boardMetrics.x, rand() % boardMetrics.y};
				mineData				= &Board[cellPosition.y][cellPosition.x];
			}
			mineData->Mine			= true;
		}
	}
	else { // Block-based model
		const ::gpk::n2u32			blockCount					= ::getBlockCount(boardMetrics, GameState.BlockSize);
		gpk_necs(BoardBlocks.resize(blockCount.x * blockCount.y));
		for(uint32_t iMine = 0; iMine < mineCount; ++iMine) {
			::gpk::n2u32				cellPosition					= {rand() % boardMetrics.x, rand() % boardMetrics.y};
			::gpkg::SMineHellCell		* cellData						= 0;
			GetCell(cellPosition, &cellData);
			while(cellData->Mine) {
				cellPosition			= {rand() % boardMetrics.x, rand() % boardMetrics.y};
				GetCell(cellPosition, &cellData);
			}
			cellData->Mine			= true;
		}
	}
	return 0;
}

::gpk::error_t			gpkg::SMineHell::Save			(::gpk::au8 & bytes)	{
	if(false == GameState.BlockBased) {
		::gpk::au8								rleDecoded						= {};
		gpk_necs(rleDecoded.append((const uint8_t*)&Board.metrics(), sizeof(::gpk::n2u32)) );
		gpk_necs(rleDecoded.append((const uint8_t*)Board.Texels.begin(), Board.Texels.size()));
		gpk_necs(::gpk::rleEncode(rleDecoded, bytes));
		gpk_necs(bytes.append((const uint8_t*)&GameState.Time, sizeof(::gpk::rangeu64)));
	}
	else {
		::gpk::au8								rleDecoded						= {};
		gpk_necs(rleDecoded.append((const uint8_t*)&GameState.BlockSize, sizeof(::gpk::n2u32)) );
		gpk_necs(rleDecoded.append((const uint8_t*)&BoardBlocks.size(), sizeof(uint32_t)) );
		for(uint32_t iBlock = 0; iBlock < BoardBlocks.size(); ++iBlock) {
			if(nullptr == BoardBlocks[iBlock].get_ref())
				gpk_necs(rleDecoded.push_back(0));
			else {
				gpk_necs(rleDecoded.push_back(1));
				gpk_necs(rleDecoded.append((const uint8_t*)BoardBlocks[iBlock]->Texels.begin(), BoardBlocks[iBlock]->Texels.size()));
			}
		}
		gpk_necs(::gpk::arrayDeflate(rleDecoded, bytes));
		info_printf("Game state saved. State size: %u. Deflated: %u.", rleDecoded.size(), bytes.size());
		gpk_necs(bytes.append((const uint8_t*)&GameState, sizeof(::gpkg::SMineHellState)));
	}
	return 0;
}

::gpk::error_t			gpkg::SMineHell::Load			(::gpk::vcu8 bytes)	{
	if(false == GameState.BlockBased) {
		GameState.Time			= *(::gpk::rangeu64*)&bytes[bytes.size() - sizeof(::gpk::rangeu64)];
		bytes					= {bytes.begin(), bytes.size() - szof(::gpk::rangeu64)};
		::gpk::au8					gameStateBytes					= {};
		gpk_necs(::gpk::arrayInflate(bytes, gameStateBytes));
		info_printf("Game state loaded. State size: %u. Compressed: %u.", gameStateBytes.size(), bytes.size());
		ree_if(gameStateBytes.size() < sizeof(::gpk::n2u32), "Invalid game state file format: %s.", "Invalid file size");
		const ::gpk::n2u32			boardMetrics					= *(::gpk::n2u32*)gameStateBytes.begin();
		gpk_necs(Board.resize(boardMetrics, {}));
		memcpy(Board.View.begin(), &gameStateBytes[sizeof(::gpk::n2u32)], Board.View.size());
		// --- The game is already loaded. Now, we need to get a significant return value from the data we just loaded.
		for(uint32_t y = 0; y < Board.metrics().y; ++y)
		for(uint32_t x = 0; x < Board.metrics().x; ++x)
			if(Board.View[y][x].Boom)
				return 1;

		::gpk::imgmonou64			cellsMines; gpk_necs(cellsMines.resize(Board.metrics()));
		::gpk::imgmonou64			cellsShows; gpk_necs(cellsShows.resize(Board.metrics()));
		const uint32_t				totalMines						= GetMines(cellsMines.View);
		const uint32_t				totalShows						= GetShows(cellsShows.View);
		::gpk::imgu8				hints;
		gpk_necs(hints.resize(Board.metrics(), 0));
		GetHints(hints.View);
		return ((boardMetrics.x * boardMetrics.y - totalShows) <= totalMines) ? 2 : 0;
	}
	else {
		GameState				= *(::gpkg::SMineHellState*)&bytes[bytes.size() - sizeof(::gpkg::SMineHellState)];
		bytes					= {bytes.begin(), bytes.size() - szof(::gpkg::SMineHellState)};
		::gpk::au8					gameStateBytes					= {};
		gpk_necs(::gpk::arrayInflate(bytes, gameStateBytes));
		info_printf("Game state loaded. State size: %u. Compressed: %u.", gameStateBytes.size(), bytes.size());
		ree_if(gameStateBytes.size() < sizeof(uint32_t), "Invalid game state file format: %s.", "Invalid file size");
		uint32_t					iByteOffset						= 0;
		GameState.BlockSize		= *(const ::gpk::n2u8*)&gameStateBytes[iByteOffset];
		iByteOffset				+= sizeof(::gpk::n2u32);
		const uint32_t				blockCount						= *(const uint32_t*)&gameStateBytes[iByteOffset];
		iByteOffset				+= sizeof(uint32_t);
		gpk_necall(BoardBlocks.resize(blockCount), "%s", "Corrupt file?");
		for(uint32_t iBlock = 0; iBlock < blockCount; ++iBlock) {
			const uint8_t				loadBlock						= *(const uint8_t*)&gameStateBytes[iByteOffset++];
			if(loadBlock) {
				gpk_necs(BoardBlocks[iBlock]->resize(GameState.BlockSize, {}));
				memcpy(BoardBlocks[iBlock]->View.begin(), &gameStateBytes[iByteOffset], BoardBlocks[iBlock]->View.size());
				iByteOffset							+= BoardBlocks[iBlock]->View.size();
			}
		}
		if(GameState.Blast)
			return 1;

		if(GameState.Time.Count)
			return 2;

		return 0;
	}
}
