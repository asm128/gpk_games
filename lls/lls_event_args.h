#include "lls_id.h"

#include "gpk_n3.h"

#ifndef LLS_EVENT_ARGS_H_23701
#define LLS_EVENT_ARGS_H_23701

namespace lls
{
#pragma pack(push, 1)
	struct SArgsEvent { // TODO: This will do for now. Maybe we can use a json format later on for more flexibility customizing for each event type.
		llsid_t				IdSource			= {}; 
		llsid_t				IdTarget			= {}; 
		::gpk::n3f2_t			CoordSource			= {}; 
		::gpk::n3f2_t			CoordTarget			= {}; 

		union UValue { 
			int32_t		AsInt; 
			float		AsFloat; 
		}						Value				= {};

		llsid_t				Id					= {};
	};
#pragma pack(pop)

	::gpk::error_t			printArgsEvent		(const ::lls::SArgsEvent & eventArgs, bool debugOutput = false);
	::gpk::error_t			printArgsEvent		(const ::lls::SArgsEvent & eventArgs, ::gpk::asc_t & output);
} // namespace

#endif // LLS_EVENT_ARGS_H_23701