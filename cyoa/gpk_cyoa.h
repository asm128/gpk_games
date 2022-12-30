#include "gpk_array.h"

#include <string>

#ifndef RGB_CYOA_H_28930749823
#define RGB_CYOA_H_28930749823

namespace gpkg 
{
	struct SJump {
		::std::string					Text;
		uint32_t						Jump;
	};

	struct SPage {
		::gpk::aobj<::std::string>	TextLines;
		::gpk::aobj<::gpkg::SJump>	PageJumps;
	};

	struct SGame {
		::gpk::aobj<::gpkg::SPage>	Pages				= {};
		uint32_t						CurrentPage			= 0;

		char							StoryFolder[4096]	= "test_story";
	};

	int								loadPage			(const char* folderName, ::gpkg::SPage & page, uint32_t pageIndex);
	int								validJump			(const ::gpk::aobj<::gpkg::SJump> & jumps, uint32_t indexToTest);
} 

#endif