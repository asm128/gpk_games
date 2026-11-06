#include "ssiege_event_args.h"
#include "gpk_string_compose.h"

gpk::err_t			ssg::printArgsEvent		(const ::ssg::SArgsEvent & eventArgs, bool debugOutput) {
	if(debugOutput)
		info_printf(
			"\nCoordSource  : " N3_F2
			"\nCoordTarget  : " N3_F2
			"\nIdSource     : %i"
			"\nIdTarget     : %i"
			"\nValue.AsInt  : %i"
			"\nValue.AsFloat: %f"
			"\nId           : %i"
			, gpk_xyz(eventArgs.CoordSource)
			, gpk_xyz(eventArgs.CoordTarget)
			, eventArgs.IdSource		
			, eventArgs.IdTarget		
			, eventArgs.Value.AsInt		
			, eventArgs.Value.AsFloat	
			, eventArgs.Id	
			);
	else
		printf(
			"\nCoordSource  : " N3_F2
			"\nCoordTarget  : " N3_F2
			"\nIdSource     : %i"
			"\nIdTarget     : %i"
			"\nValue.AsInt  : %i"
			"\nValue.AsFloat: %f"
			"\nId           : %i"
			, gpk_xyz(eventArgs.CoordSource)
			, gpk_xyz(eventArgs.CoordTarget)
			, eventArgs.IdSource		
			, eventArgs.IdTarget		
			, eventArgs.Value.AsInt		
			, eventArgs.Value.AsFloat	
			, eventArgs.Id	
			);
	return 0;
}

gpk::err_t			ssg::printArgsEvent		(const ::ssg::SArgsEvent & eventArgs, ::gpk::asc_t & output) {
	sc_t						temp	[1024]				= {};
	sprintf_s(temp, 
		"\nCoordSource  : " N3_F2
		"\nCoordTarget  : " N3_F2
		"\nIdSource     : %i"
		"\nIdTarget     : %i"
		"\nValue.AsInt  : %i"
		"\nValue.AsFloat: %f"
		"\nId           : %i"
		, gpk_xyz(eventArgs.CoordSource)
		, gpk_xyz(eventArgs.CoordTarget)
		, eventArgs.IdSource		
		, eventArgs.IdTarget		
		, eventArgs.Value.AsInt		
		, eventArgs.Value.AsFloat	
		, eventArgs.Id	
		);
	if_fail_fe(::gpk::append_strings(output, temp));
	return 0;
}
