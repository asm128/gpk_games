#include "gpk_array.h"
#include "gpk_array_static.h"

#ifndef RGB_CYOA_H_28930749823
#define RGB_CYOA_H_28930749823

namespace gpkg 
{
	GPK_USING_TYPEINT();

	struct SJump {
		::gpk::achar				Text;
		uint32_t					Jump;
	};

	struct SPage {
		::gpk::aachar				TextLines;
		::gpk::aobj<::gpkg::SJump>	PageJumps;
	};

	struct SGame {
		::gpk::aobj<::gpkg::SPage>	Pages				= {};
		uint32_t					CurrentPage			= 0;

		::gpk::astatic<sc_t, 4096>	StoryFolder			= "test_story";
	};

	::gpk::error_t				loadPage			(sc_c * folderName, ::gpkg::SPage & page, uint32_t pageIndex);
	::gpk::error_t				validJump			(const ::gpk::aobj<::gpkg::SJump> & jumps, uint32_t indexToTest);
} 

#endif