#include "ssiege_id.h"

#include "gpk_n3.h"

#ifndef SSG_EVENT_ARGS_H_23701
#define SSG_EVENT_ARGS_H_23701

namespace ssg
{
#pragma pack(push, 1)
	struct SArgsEvent { // TODO: This will do for now. Maybe we can use a json format later on for more flexibility customizing for each event type.
		ssiegeid_t				IdSource			= {}; 
		ssiegeid_t				IdTarget			= {}; 
		::gpk::n3f2_t			CoordSource			= {}; 
		::gpk::n3f2_t			CoordTarget			= {}; 

		union UValue { 
			int32_t		AsInt; 
			float		AsFloat; 
		}						Value				= {};

		ssiegeid_t				Id					= {};
	};
#pragma pack(pop)

	::gpk::error_t			printArgsEvent		(const ::ssg::SArgsEvent & eventArgs, bool debugOutput = false);
	::gpk::error_t			printArgsEvent		(const ::ssg::SArgsEvent & eventArgs, ::gpk::asc_t & output);
} // namespace

#endif // SSG_EVENT_ARGS_H_23701