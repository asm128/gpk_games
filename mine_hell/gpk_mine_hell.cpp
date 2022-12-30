#include "gpk_mine_hell.h"

#include "gpk_chrono.h"
#include "gpk_encoding.h"
#include "gpk_deflate.h"
#include "gpk_array_static.h"

static	::gpk::SCoord2<uint32_t>	getBlockCount				(const ::gpk::SCoord2<uint32_t> boardMetrics, const ::gpk::SCoord2<uint32_t> blockMetrics)	{
	return
		{ boardMetrics.x / blockMetrics.x + one_if(boardMetrics.x % blockMetrics.x)
		, boardMetrics.y / blockMetrics.y + one_if(boardMetrics.y % blockMetrics.y)
		};
}

static	::gpk::SCoord2<uint32_t>	getBlockFromCoord			(const ::gpk::SCoord2<uint32_t> boardMetrics, const ::gpk::SCoord2<uint32_t> blockMetrics)	{
	return
		{ boardMetrics.x / blockMetrics.x
		, boardMetrics.y / blockMetrics.y
		};
}

static	::gpk::SCoord2<uint32_t>	getLocalCoordFromCoord		(const ::gpk::SCoord2<uint32_t> boardMetrics, const ::gpk::SCoord2<uint32_t> blockMetrics)	{
	return
		{ boardMetrics.x % blockMetrics.x
		, boardMetrics.y % blockMetrics.y
		};
}

::gpk::error_t						gpkg::SMineHell::GetBlast	(::gpk::SCoord2<uint32_t> & out_coord)	const {
	if(GameState.BlockBased)
		out_coord							= GameState.BlastCoord;
	else {
		const ::gpk::SCoord2<uint32_t>	gridMetrix = Board.metrics();
		for(uint32_t y = 0; y < gridMetrix.y; ++y) {
			for(uint32_t x = 0; x < gridMetrix.x; ++x) {
				const uint8_t							value						=	Board[y][x].Boom ? 1 : 0;
				if(value > 0) {
					out_coord							= {x, y};
					return 1;
				}
			}
		}
	}
	return 0;
}

#define ASSIGN_GRID_BOOL(value_, total_) {										\
	for(uint32_t y = 0; y < gridMetrix.y; ++y) {								\
		const uint32_t offset = y * gridMetrix.x;								\
		for(uint32_t x = 0; x < gridMetrix.x; ++x) {							\
			const ::gpkg::SMineHellCell * cellValue = 0;							\
			GetCell({x ,y}, &cellValue);										\
			uint8_t	value =	(0 == cellValue) ? 0 : cellValue->value_ ? 1 : 0;	\
			out_Cells[offset + x] = value;										\
			if(value)															\
				total_ += value;												\
		}																		\
	}																			\
}

::gpk::error_t						gpkg::SMineHell::GetMines	(::gpk::view_bit<uint64_t> & out_Cells)	const	{ uint32_t total = 0;	const ::gpk::SCoord2<uint32_t> gridMetrix = (GameState.BlockBased) ? GameState.BoardSize : Board.metrics(); ASSIGN_GRID_BOOL(Mine, total); return total; }
::gpk::error_t						gpkg::SMineHell::GetFlags	(::gpk::view_bit<uint64_t> & out_Cells)	const	{ uint32_t total = 0;	const ::gpk::SCoord2<uint32_t> gridMetrix = (GameState.BlockBased) ? GameState.BoardSize : Board.metrics(); ASSIGN_GRID_BOOL(Flag, total); return total; }
::gpk::error_t						gpkg::SMineHell::GetHolds	(::gpk::view_bit<uint64_t> & out_Cells)	const	{ uint32_t total = 0;	const ::gpk::SCoord2<uint32_t> gridMetrix = (GameState.BlockBased) ? GameState.BoardSize : Board.metrics(); ASSIGN_GRID_BOOL(What, total); return total; }
::gpk::error_t						gpkg::SMineHell::GetShows	(::gpk::view_bit<uint64_t> & out_Cells)	const	{ uint32_t total = 0;	const ::gpk::SCoord2<uint32_t> gridMetrix = (GameState.BlockBased) ? GameState.BoardSize : Board.metrics(); ASSIGN_GRID_BOOL(Show, total); return total; }
::gpk::error_t						gpkg::SMineHell::GetHints	(::gpk::view2d<uint8_t> & out_Cells)	const	{						const ::gpk::SCoord2<uint32_t> gridMetrix = (GameState.BlockBased) ? GameState.BoardSize : Board.metrics();
	for(int32_t y = 0; y < (int32_t)gridMetrix.y; ++y)
	for(int32_t x = 0; x < (int32_t)gridMetrix.x; ++x) {
		const ::gpkg::SMineHellCell				* cellValue					= 0;
		GetCell({(uint32_t)x, (uint32_t)y}, &cellValue);
		//if(false == cellValue->Show)
		//	continue;
		if(cellValue && cellValue->Mine)
			continue;
		uint8_t									& out				= out_Cells[y][x]		= 0;
		::gpk::SCoord2<int32_t>					coordToTest			= {};	// actually by making this uint32_t we could easily change all the conditions to be coordToTest.i < gridMetrix.i. However, I'm too lazy to start optimizing what's hardly the bottleneck
		const ::gpkg::SMineHellCell				* cellValueToTest	= 0;
		coordToTest	= {x - 1, y - 1	};	if(coordToTest.y >= 0 && coordToTest.x >= 0)										{ if(-1 != GetCell(coordToTest.Cast<uint32_t>(), &cellValueToTest) && 0 != cellValueToTest && cellValueToTest->Mine) ++out; }
		coordToTest	= {x	, y - 1	};	if(coordToTest.y >= 0)																{ if(-1 != GetCell(coordToTest.Cast<uint32_t>(), &cellValueToTest) && 0 != cellValueToTest && cellValueToTest->Mine) ++out; }
		coordToTest	= {x + 1, y - 1	};	if(coordToTest.y >= 0 && coordToTest.x < (int32_t)gridMetrix.x)						{ if(-1 != GetCell(coordToTest.Cast<uint32_t>(), &cellValueToTest) && 0 != cellValueToTest && cellValueToTest->Mine) ++out; }
		coordToTest	= {x - 1, y		};	if(coordToTest.x >= 0)																{ if(-1 != GetCell(coordToTest.Cast<uint32_t>(), &cellValueToTest) && 0 != cellValueToTest && cellValueToTest->Mine) ++out; }
		coordToTest	= {x + 1, y		};	if(coordToTest.x < (int32_t)gridMetrix.x)											{ if(-1 != GetCell(coordToTest.Cast<uint32_t>(), &cellValueToTest) && 0 != cellValueToTest && cellValueToTest->Mine) ++out; }
		coordToTest	= {x - 1, y + 1	};	if(coordToTest.y < (int32_t)gridMetrix.y && coordToTest.x >= 0)						{ if(-1 != GetCell(coordToTest.Cast<uint32_t>(), &cellValueToTest) && 0 != cellValueToTest && cellValueToTest->Mine) ++out; }
		coordToTest	= {x	, y + 1	};	if(coordToTest.y < (int32_t)gridMetrix.y)											{ if(-1 != GetCell(coordToTest.Cast<uint32_t>(), &cellValueToTest) && 0 != cellValueToTest && cellValueToTest->Mine) ++out; }
		coordToTest	= {x + 1, y + 1	};	if(coordToTest.y < (int32_t)gridMetrix.y && coordToTest.x < (int32_t)gridMetrix.x)	{ if(-1 != GetCell(coordToTest.Cast<uint32_t>(), &cellValueToTest) && 0 != cellValueToTest && cellValueToTest->Mine) ++out; }
	}
	return 0;
}

static	::gpk::error_t				uncoverCell						(::gpk::view2d<::gpkg::SMineHellCell> & board, const ::gpk::view2d<uint8_t> & hints, const ::gpk::SCoord2<uint32_t> & cellOffset, const ::gpk::SCoord2<uint32_t> & boardSize, const ::gpk::SCoord2<int32_t> localCellCoord, ::gpk::apod<::gpk::SCoord2<uint32_t>> & outOfRangeCells);
static	::gpk::error_t				uncoverCellIfNeeded				(::gpk::view2d<::gpkg::SMineHellCell> & board, const ::gpk::view2d<uint8_t> & hints, const ::gpk::SCoord2<uint32_t> & cellOffset, const ::gpk::SCoord2<uint32_t> & boardSize, const ::gpk::SCoord2<int32_t> localCellCoord, ::gpk::apod<::gpk::SCoord2<uint32_t>> & outOfRangeCells) {
	const ::gpkg::SMineHellCell				cellToTest						= board[localCellCoord.y][localCellCoord.x];
	if(false == cellToTest.Mine && false == cellToTest.Show)
		::uncoverCell(board, hints, cellOffset, boardSize, localCellCoord, outOfRangeCells);
	return 0;
}

static	::gpk::error_t				enqueueCellIfNeeded				(const ::gpk::SCoord2<uint32_t> & cellOffset, const ::gpk::SCoord2<uint32_t> & boardSize, const ::gpk::SCoord2<int32_t> localCellCoord, ::gpk::apod<::gpk::SCoord2<uint32_t>> & outOfRangeCells) {
	const ::gpk::SCoord2<uint32_t>			globalCoordToTest				= cellOffset + localCellCoord.Cast<uint32_t>();
	if(::gpk::in_range(globalCoordToTest, {{}, boardSize}) && 0 > ::gpk::find(globalCoordToTest, {outOfRangeCells.begin(), outOfRangeCells.size()}))
		gpk_necall(outOfRangeCells.push_back(globalCoordToTest), "%s", "Out of memory?");
	return 0;
}


constexpr ::gpk::array_static<const ::gpk::SCoord2<int32_t>, 8>	SURROUNDING_CELLS_OFFSETS = 
	{ ::gpk::SCoord2<int32_t>{0 - 1, 0 - 1} // top left
	, ::gpk::SCoord2<int32_t>{0 + 0, 0 - 1} // top center
	, ::gpk::SCoord2<int32_t>{0 + 1, 0 - 1} // top right
	, ::gpk::SCoord2<int32_t>{0 - 1, 0 + 0} // center left
	, ::gpk::SCoord2<int32_t>{0 + 1, 0 + 0} // center right
	, ::gpk::SCoord2<int32_t>{0 - 1, 0 + 1} // bottom left
	, ::gpk::SCoord2<int32_t>{0 + 0, 0 + 1} // bottom center
	, ::gpk::SCoord2<int32_t>{0 + 1, 0 + 1} // bottom right
	};


static	::gpk::error_t				uncoverCell						(::gpk::view2d<::gpkg::SMineHellCell> & block, const ::gpk::view2d<uint8_t> & hints, const ::gpk::SCoord2<uint32_t> & cellOffset, const ::gpk::SCoord2<uint32_t> & boardSize, const ::gpk::SCoord2<int32_t> localCellCoord, ::gpk::apod<::gpk::SCoord2<uint32_t>> & outOfRangeCells) {
	::gpkg::SMineHellCell					& currentCell					= block[localCellCoord.y][localCellCoord.x];
	if(false == currentCell.Mine && false == currentCell.Show) {
		currentCell.Show					= true;
		const ::gpk::SCoord2<uint32_t>			gridMetrix						= block.metrics();
		const ::gpk::SCoord2<uint32_t>			globalCellCoord					= localCellCoord.Cast<uint32_t>() + cellOffset;
		if(::gpk::in_range(globalCellCoord, {{}, boardSize}) && 0 == hints[cellOffset.y + localCellCoord.y][cellOffset.x + localCellCoord.x]) {
			::gpk::SCoord2<int32_t>					coordToTest						= {};	//
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

::gpk::error_t						gpkg::SMineHell::Flag			(const ::gpk::SCoord2<uint32_t> cellCoord)	{ ::gpkg::SMineHellCell * cellData = 0; GetCell(cellCoord, &cellData); if(false == cellData->Show) { cellData->Flag = !cellData->Flag; cellData->What = false; } return 0; }
::gpk::error_t						gpkg::SMineHell::Hold			(const ::gpk::SCoord2<uint32_t> cellCoord)	{ ::gpkg::SMineHellCell * cellData = 0; GetCell(cellCoord, &cellData); if(false == cellData->Show) { cellData->What = !cellData->What; cellData->Flag = false; } return 0; }
::gpk::error_t						gpkg::SMineHell::Step			(const ::gpk::SCoord2<uint32_t> cellCoord)	{ ::gpkg::SMineHellCell * cellData = 0; GetCell(cellCoord, &cellData);
	if(false == cellData->Show) {
		::gpk::SCoord2							boardMetrics				= GameState.BlockBased ? GameState.BoardSize : Board.metrics();
		if(cellData->Mine && false == cellData->Flag)
			cellData->Boom						= true;
		else if(false == cellData->Flag) {
			::gpk::SImage<uint8_t>					hints;
			hints.resize(boardMetrics, {});
			GetHints(hints.View);
			::gpk::apod<::gpk::SCoord2<uint32_t>>	outOfRangeCells;
			if(false == GameState.BlockBased)
				gpk_necall(::uncoverCell(Board.View, hints.View, {}, boardMetrics, cellCoord.Cast<int32_t>(), outOfRangeCells), "%s", "Out of memory?");
			else {
				::gpk::SCoord2<uint32_t>				cellBlock						= getBlockFromCoord		(cellCoord		, GameState.BlockSize);
				::gpk::SCoord2<uint32_t>				cellPosition					= getLocalCoordFromCoord(cellCoord		, GameState.BlockSize);
				::gpk::SCoord2<uint32_t>				blockCount						= getBlockCount			(boardMetrics	, GameState.BlockSize);
				uint32_t								blockIndex						= cellBlock.y * blockCount.x + cellBlock.x;
				::gpk::SCoord2<int32_t>					localCellCoord					= cellCoord.Cast<int32_t>();
				localCellCoord						-= ::gpk::SCoord2<uint32_t>{cellBlock.x * GameState.BlockSize.x, cellBlock.y * GameState.BlockSize.y}.Cast<int32_t>();
				if(0 == BoardBlocks[blockIndex].get_ref())
					gpk_necall(BoardBlocks[blockIndex]->resize(GameState.BlockSize, {}), "%s", "Out of memory?");
				gpk_necall(::uncoverCell(BoardBlocks[blockIndex]->View, hints.View, {cellBlock.x * GameState.BlockSize.x, cellBlock.y * GameState.BlockSize.y}, boardMetrics, localCellCoord, outOfRangeCells), "%s", "Out of memory?");
				for(uint32_t iCell = 0; iCell < outOfRangeCells.size(); ++iCell) {
					const ::gpk::SCoord2<uint32_t>			globalCellCoord					= outOfRangeCells[iCell];
					cellBlock							= getBlockFromCoord		(globalCellCoord, GameState.BlockSize);
					cellPosition						= getLocalCoordFromCoord(globalCellCoord, GameState.BlockSize);
					blockCount							= getBlockCount			(boardMetrics	, GameState.BlockSize);
					blockIndex							= cellBlock.y * blockCount.x + cellBlock.x;
					localCellCoord						= globalCellCoord.Cast<int32_t>();
					localCellCoord						-= ::gpk::SCoord2<uint32_t>{cellBlock.x * GameState.BlockSize.x, cellBlock.y * GameState.BlockSize.y}.Cast<int32_t>();
					if(0 == BoardBlocks[blockIndex].get_ref())
						gpk_necall(BoardBlocks[blockIndex]->resize(GameState.BlockSize, {}), "%s", "Out of memory?");
					gpk_necall(::uncoverCell(BoardBlocks[blockIndex]->View, hints.View, {cellBlock.x * GameState.BlockSize.x, cellBlock.y * GameState.BlockSize.y}, boardMetrics, localCellCoord, outOfRangeCells), "%s", "Out of memory?");
				}
			}
		}
		if(false == cellData->Boom) {	// Check if we won after uncovering the cell(s)
			::gpk::SImageMonochrome<uint64_t>		cellsMines;
			::gpk::SImageMonochrome<uint64_t>		cellsShows;
			gpk_necall(cellsMines.resize(boardMetrics), "%s", "Out of memory?");
			gpk_necall(cellsShows.resize(boardMetrics), "%s", "Out of memory?");
			const int32_t							totalShows						= GetShows(cellsShows.View);
			const int32_t							totalMines						= GetMines(cellsMines.View);
			if((boardMetrics.x * boardMetrics.y - totalShows) <= (uint32_t)totalMines && false == cellData->Boom)	// Win!
				GameState.Time.Count				= ::gpk::timeCurrent() - GameState.Time.Offset;
		}
		else {
			GameState.BlastCoord				= cellCoord;
			GameState.Blast						= true;
		}
	}
	return cellData->Boom ? 1 : 0;
}

::gpk::error_t						gpkg::SMineHell::Start		(const ::gpk::SCoord2<uint32_t> boardMetrics, const uint32_t mineCount)	{
	GameState							= {};
	GameState.BoardSize					= {::gpk::max(3U, boardMetrics.x), ::gpk::max(3U, boardMetrics.y)};
	GameState.MineCount					= (uint32_t)::gpk::max(2.0, ::gpk::min((double)mineCount, ((boardMetrics.x * (double)boardMetrics.y) / 4) * 3));
	GameState.Time.Offset				= ::gpk::timeCurrent();
	if(false == GameState.BlockBased) { // Old small model (deprecated)
		gpk_necall(Board.resize(boardMetrics, {}), "Out of memory? Board size: {%u, %u}", boardMetrics.x, boardMetrics.y);
		for(uint32_t iMine = 0; iMine < mineCount; ++iMine) {
			::gpk::SCoord2<uint32_t>				cellPosition					= {rand() % boardMetrics.x, rand() % boardMetrics.y};
			::gpkg::SMineHellCell					* mineData						= &Board[cellPosition.y][cellPosition.x];
			while(mineData->Mine) {
				cellPosition						= {rand() % boardMetrics.x, rand() % boardMetrics.y};
				mineData							= &Board[cellPosition.y][cellPosition.x];
			}
			mineData->Mine						= true;
		}
	}
	else { // Block-based model
		const ::gpk::SCoord2<uint32_t>			blockCount					= ::getBlockCount(boardMetrics, GameState.BlockSize);
		gpk_necall(BoardBlocks.resize(blockCount.x * blockCount.y), "Out of memory? Board size: {%u, %u}", boardMetrics.x, boardMetrics.y);
		for(uint32_t iMine = 0; iMine < mineCount; ++iMine) {
			::gpk::SCoord2<uint32_t>				cellPosition					= {rand() % boardMetrics.x, rand() % boardMetrics.y};
			::gpkg::SMineHellCell					* cellData						= 0;
			GetCell(cellPosition, &cellData);
			while(cellData->Mine) {
				cellPosition						= {rand() % boardMetrics.x, rand() % boardMetrics.y};
				GetCell(cellPosition, &cellData);
			}
			cellData->Mine						= true;
		}
	}
	return 0;
}

::gpk::error_t						gpkg::SMineHell::Save			(::gpk::apod<char_t> & bytes)	{
	if(false == GameState.BlockBased) {
		::gpk::apod<byte_t>				rleDecoded						= {};
		gpk_necall(rleDecoded.append((const char*)&Board.metrics(), sizeof(::gpk::SCoord2<uint32_t>)) , "%s", "Out of memory?");
		gpk_necall(rleDecoded.append((const char*)Board.Texels.begin(), Board.Texels.size()), "%s", "Out of memory?");
		gpk_necall(::gpk::rleEncode(rleDecoded, bytes), "%s", "Out of memory?");
		gpk_necall(bytes.append((const char*)&GameState.Time, sizeof(::gpk::SRange<uint64_t>)), "%s", "Out of memory?");
	}
	else {
		::gpk::apod<byte_t>				rleDecoded						= {};
		gpk_necall(rleDecoded.append((const char*)&GameState.BlockSize, sizeof(::gpk::SCoord2<uint32_t>)) , "%s", "Out of memory?");
		gpk_necall(rleDecoded.append((const char*)&BoardBlocks.size(), sizeof(uint32_t)) , "%s", "Out of memory?");
		for(uint32_t iBlock = 0; iBlock < BoardBlocks.size(); ++iBlock) {
			if(nullptr == BoardBlocks[iBlock].get_ref())
				gpk_necall(rleDecoded.push_back(0), "%s", "Out of memory?");
			else {
				gpk_necall(rleDecoded.push_back(1), "%s", "Out of memory?");
				gpk_necall(rleDecoded.append((const char*)BoardBlocks[iBlock]->Texels.begin(), BoardBlocks[iBlock]->Texels.size()), "%s", "Out of memory?");
			}
		}
		gpk_necall(::gpk::arrayDeflate(rleDecoded, bytes), "%s", "Out of memory?");
		info_printf("Game state saved. State size: %u. Deflated: %u.", rleDecoded.size(), bytes.size());
		gpk_necall(bytes.append((const char*)&GameState, sizeof(::gpkg::SMineHellState)), "%s", "Out of memory?");
	}
	return 0;
}

::gpk::error_t						gpkg::SMineHell::Load			(::gpk::view_const_byte	bytes)	{
	if(false == GameState.BlockBased) {
		GameState.Time						= *(::gpk::SRange<uint64_t>*)&bytes[bytes.size() - sizeof(::gpk::SRange<uint64_t>)];
		bytes								= {bytes.begin(), bytes.size() - sizeof(::gpk::SRange<uint64_t>)};
		::gpk::apod<byte_t>				gameStateBytes					= {};
		gpk_necall(::gpk::arrayInflate(bytes, gameStateBytes), "%s", "Out of memory or corrupt data!");
		info_printf("Game state loaded. State size: %u. Compressed: %u.", gameStateBytes.size(), bytes.size());
		ree_if(gameStateBytes.size() < sizeof(::gpk::SCoord2<uint32_t>), "Invalid game state file format: %s.", "Invalid file size");
		const ::gpk::SCoord2<uint32_t>			boardMetrics					= *(::gpk::SCoord2<uint32_t>*)gameStateBytes.begin();
		gpk_necall(Board.resize(boardMetrics, {}), "Out of memory? Board size: {%u, %u}", boardMetrics.x, boardMetrics.y);
		memcpy(Board.View.begin(), &gameStateBytes[sizeof(::gpk::SCoord2<uint32_t>)], Board.View.size());
		// --- The game is already loaded. Now, we need to get a significant return value from the data we just loaded.
		for(uint32_t y = 0; y < Board.metrics().y; ++y)
		for(uint32_t x = 0; x < Board.metrics().x; ++x)
			if(Board.View[y][x].Boom)
				return 1;
		::gpk::SImageMonochrome<uint64_t>		cellsMines; gpk_necall(cellsMines.resize(Board.metrics())	, "%s", "Out of memory?");
		::gpk::SImageMonochrome<uint64_t>		cellsShows; gpk_necall(cellsShows.resize(Board.metrics())	, "%s", "Out of memory?");
		const uint32_t							totalMines						= GetMines(cellsMines.View);
		const uint32_t							totalShows						= GetShows(cellsShows.View);
		::gpk::SImage<uint8_t>					hints;
		gpk_necall(hints.resize(Board.metrics(), 0), "%s", "");
		GetHints(hints.View);
		return ((boardMetrics.x * boardMetrics.y - totalShows) <= totalMines) ? 2 : 0;
	}
	else {
		GameState							= *(::gpkg::SMineHellState*)&bytes[bytes.size() - sizeof(::gpkg::SMineHellState)];
		bytes								= {bytes.begin(), bytes.size() - sizeof(::gpkg::SMineHellState)};
		::gpk::apod<byte_t>				gameStateBytes					= {};
		gpk_necall(::gpk::arrayInflate(bytes, gameStateBytes), "%s", "Out of memory or corrupt data!");
		info_printf("Game state loaded. State size: %u. Compressed: %u.", gameStateBytes.size(), bytes.size());
		ree_if(gameStateBytes.size() < sizeof(uint32_t), "Invalid game state file format: %s.", "Invalid file size");
		uint32_t								iByteOffset						= 0;
		GameState.BlockSize					= *(const ::gpk::SCoord2<uint32_t>*)&gameStateBytes[iByteOffset];
		iByteOffset							+= sizeof(::gpk::SCoord2<uint32_t>);
		const uint32_t							blockCount						= *(const uint32_t*)&gameStateBytes[iByteOffset];
		iByteOffset							+= sizeof(uint32_t);
		gpk_necall(BoardBlocks.resize(blockCount), "%s", "Corrupt file?");
		for(uint32_t iBlock = 0; iBlock < blockCount; ++iBlock) {
			const byte_t							loadBlock						= *(const byte_t*)&gameStateBytes[iByteOffset++];
			if(loadBlock) {
				gpk_necall(BoardBlocks[iBlock]->resize(GameState.BlockSize, {}), "Out of memory? Board size: {%u, %u}", GameState.BlockSize.x, GameState.BlockSize.y);
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

::gpk::error_t									gpkg::SMineHell::GetCell				(const ::gpk::SCoord2<uint32_t> & cellCoord, ::gpkg::SMineHellCell ** out_cell)			{
	ree_if(cellCoord.x > GameState.BoardSize.x || cellCoord.y > GameState.BoardSize.y, "Invalid cell: %u, %u. Max: %u, %u.", cellCoord.x, cellCoord.y, GameState.BoardSize.x, GameState.BoardSize.y);
	if(false == GameState.BlockBased)
		*out_cell										= &Board[cellCoord.y][cellCoord.x];
	else {
		const ::gpk::SCoord2<uint32_t>						cellBlock							= ::getBlockFromCoord(cellCoord, GameState.BlockSize);
		const ::gpk::SCoord2<uint32_t>						cellPosition						= ::getLocalCoordFromCoord(cellCoord, GameState.BlockSize);
		const ::gpk::SCoord2<uint32_t>						blockCount							= ::getBlockCount(GameState.BoardSize, GameState.BlockSize);
		const uint32_t										blockIndex							= cellBlock.y * blockCount.x + cellBlock.x;
		if(0 == BoardBlocks[blockIndex].get_ref())
			BoardBlocks[blockIndex]->resize(GameState.BlockSize, {});
		*out_cell										= &(*BoardBlocks[blockIndex])[cellPosition.y][cellPosition.x];
	}
	return 0;
}

::gpk::error_t									gpkg::SMineHell::GetCell				(const ::gpk::SCoord2<uint32_t> & cellCoord, const ::gpkg::SMineHellCell ** out_cell)		const	{
	ree_if(cellCoord.x > GameState.BoardSize.x || cellCoord.y > GameState.BoardSize.y, "Invalid cell: %u, %u. Max: %u, %u.", cellCoord.x, cellCoord.y, GameState.BoardSize.x, GameState.BoardSize.y);
	if(false == GameState.BlockBased)
		*out_cell										= &Board[cellCoord.y][cellCoord.x];
	else {
		const ::gpk::SCoord2<uint32_t>						cellBlock							= ::getBlockFromCoord(cellCoord, GameState.BlockSize);
		const ::gpk::SCoord2<uint32_t>						cellPosition						= ::getLocalCoordFromCoord(cellCoord, GameState.BlockSize);
		const ::gpk::SCoord2<uint32_t>						blockCount							= ::getBlockCount(GameState.BoardSize, GameState.BlockSize);
		const uint32_t										blockIndex							= cellBlock.y * blockCount.x + cellBlock.x;
		*out_cell										= (0 == BoardBlocks[blockIndex].get_ref()) ? 0 : &(*BoardBlocks[blockIndex])[cellPosition.y][cellPosition.x];
	}
	return 0;
}

