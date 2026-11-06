#include "gpk_array.h"
#include "gpk_array_static.h"

#ifndef RGB_CYOA_H_28930749823
#define RGB_CYOA_H_28930749823

namespace gpkg 
{
	usns gpk;

	struct SJump {
		string			Text;
		u2_t			Jump;
	};

	struct SPage {
		aobj<string>	TextLines;
		aobj<SJump>		PageJumps;
	};
	struct SGame {
		aobj<SPage>			Pages				= {};
		u2_t				CurrentPage			= 0;

		vcst_t				StoryFolder			= "test_story";
	};

	gpk::err_t				loadPage			(sc_c * folderName, SPage & page, u2_t pageIndex);
	gpk::err_t				validJump			(const aobj<SJump> & jumps, u2_t indexToTest);
} 

#endif
