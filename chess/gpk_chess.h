#include "gpk_chess_game.h"

#include "gpk_image.h"
#include "gpk_enum.h"
#include "gpk_frameinfo.h"
#include "gpk_camera.h"

#include "gpk_gui_control_list.h"
#include "gpk_gui_inputbox.h"
#include "gpk_dialog_controls.h"
#include "gpk_deflate.h"
#include "gpk_file.h"

#ifndef GPK_CHESS_H
#define GPK_CHESS_H

namespace gpk
{
	GDEFINE_ENUM_TYPE (CHESS_PIECE, u0_t);
	GDEFINE_ENUM_VALUE(CHESS_PIECE, None	, 0);
	GDEFINE_ENUM_VALUE(CHESS_PIECE, King	, 1);
	GDEFINE_ENUM_VALUE(CHESS_PIECE, Queen	, 2);
	GDEFINE_ENUM_VALUE(CHESS_PIECE, Rook	, 3);
	GDEFINE_ENUM_VALUE(CHESS_PIECE, Knight	, 4);
	GDEFINE_ENUM_VALUE(CHESS_PIECE, Bishop	, 5);
	GDEFINE_ENUM_VALUE(CHESS_PIECE, Pawn	, 6);

	struct SChessBoard {
		::gpk::img<u0_t>	Cells;
	};

	struct SChessApp {
		SChessBoard			Board;
	};


} // namespace 
#endif // GPK_CHESS_H
