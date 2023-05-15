#include "gpk_galaxy_hell_entity.h"
#include "gpk_rigidbody.h"

#ifndef GPK_GALAXY_HELL_PLANET_H_20221106
#define GPK_GALAXY_HELL_PLANET_H_20221106

namespace ghg
{
	struct SSolarSystem {
		::gpk::n3<int64_t>					Sector;
		::gpk::n3<int64_t>					Position;
		::gpk::vcc								Name;
	};

	struct SGalaxy {
		::gpk::aobj<::ghg::SSolarSystem>	SolarSystems;
	};
} // namespace

#endif // GPK_GALAXY_HELL_PLANET_H_20221106