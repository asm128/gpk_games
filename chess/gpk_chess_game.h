#include "gpk_game.h"

#ifndef GPK_CHESS_GAME_H
#define GPK_CHESS_GAME_H

namespace gpk
{
	struct SChessGame {
		SChessBoard			Board;
		::gpk::SPlayer		Player;
	};

	::gpk::error_t					chessGameSetup		(::gpk::SChessGame & chess);
	//::gpk::error_t					chessGameUpdate		(::gpk::SChessGame & chess, ::gpk::vpobj<::gpk::EventChess> inputEvents, ::gpk::apobj<::gpk::EventChess> & outputEvents, double secondsElapsed);

} // namespace

#endif // GPK_CHESS_GAME_H
