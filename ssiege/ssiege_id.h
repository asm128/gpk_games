#include "gpk_array_pod.h"

#ifndef SSIEGE_ID_H_23701
#define SSIEGE_ID_H_23701

namespace ssiege
{
#pragma pack(push, 1)
	typedef	uint32_t						ssiegeid_t;
	typedef	const ::ssiege::ssiegeid_t			cssiegeid_t;
	typedef	::gpk::apod<::ssiege::ssiegeid_t>	assiegeid;
	typedef	::gpk::view<::ssiege::ssiegeid_t>	vssiegeid;
	typedef	::gpk::view<cssiegeid_t>			vssiegecid;

	stacxpr ssiegeid_t	SSIEGEID_INVALID		= (ssiegeid_t)-1;
#pragma pack(pop)
} // namespace

#endif // SSIEGE_ID_H_23701