#include "gpk_array_pod.h"
#include "gpk_grid.h"

#ifndef GPK_CHESS_H
#define GPK_CHESS_H

namespace gpk
{
	struct SChessBoard {
		::gpk::au0_t		CellData;
		::gpk::grid<u0>		CellView;
	};
} // namespace 
#endif // GPK_CHESS_H
