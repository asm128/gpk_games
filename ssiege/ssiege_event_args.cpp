#include "ssiege_event_args.h"

::gpk::error_t			ssg::printArgsEvent		(const ::ssg::SArgsEvent & eventArgs, bool debugOutput) {
	if(debugOutput)
		info_printf(
			"\nCoordSource  : " N3_F32
			"\nCoordTarget  : " N3_F32
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
			"\nCoordSource  : " N3_F32
			"\nCoordTarget  : " N3_F32
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

::gpk::error_t			ssg::printArgsEvent		(const ::ssg::SArgsEvent & eventArgs, ::gpk::achar & output) {
	char						temp	[1024]				= {};
	sprintf_s(temp, 
		"\nCoordSource  : " N3_F32
		"\nCoordTarget  : " N3_F32
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
	gpk_necs(output.append_string(temp));
	return 0;
}
