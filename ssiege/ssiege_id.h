#include "gpk_array_pod.h"

#ifndef SSIEGE_ID_H_23701
#define SSIEGE_ID_H_23701

namespace ssiege
{
#pragma pack(push, 1)
	typedef	uint32_t						campid_t;
	typedef	const ::ssiege::campid_t			ccampid_t;
	typedef	::gpk::apod<::ssiege::campid_t>	acampid;
	typedef	::gpk::view<::ssiege::campid_t>	vcampid;
	typedef	::gpk::view<ccampid_t>			vcampcid;

	stacxpr campid_t	CAMPID_INVALID		= (campid_t)-1;
#pragma pack(pop)
} // namespace

#endif // SSIEGE_ID_H_23701