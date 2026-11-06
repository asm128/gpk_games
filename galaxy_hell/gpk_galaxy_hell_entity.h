#include "gpk_enum.h"
#include "gpk_apod_serialize.h"

#ifndef GPK_GALAXY_HELL_ENTITY_H
#define GPK_GALAXY_HELL_ENTITY_H

namespace ghg 
{
	usns gpk;
#pragma pack(push, 1)
	struct SGHEntity {
		s2_t						Parent					= -1;
		s2_t						Geometry				= -1;
		s2_t						Transform				= -1;
		s2_t						Image					= -1;
		s2_t						Body					= -1;
	};
#pragma pack(pop)

	struct SEntitySystem {
		::gpk::apod<::ghg::SGHEntity>	Entities				= {};
		::gpk::aobj<::gpk::au2_t>		EntityChildren			= {};

		gpk::err_t				Create					(const ::ghg::SGHEntity & newEntity, ::gpk::vcu2_t entityChildren = {})	{ EntityChildren.push_back(entityChildren); return Entities.push_back(newEntity); }
		gpk::err_t				Create					()																		{ Create({}); }

		gpk::err_t				Save					(::gpk::au0_t & output) const { 
			if_fail_fe(::gpk::saveView(output, Entities));
			for(u2_t iEntity = 0; iEntity < Entities.size(); ++iEntity) 
				if_fail_fef(::gpk::saveView(output, ::gpk::vcu2_t{EntityChildren[iEntity]}), "iEntity: %i", iEntity);
			info_printf("Saved %s, %i", "Entities"					, EntityChildren.size());
			return 0; 
		}

		gpk::err_t				Load					(::gpk::vcu0_t & input) { 
			if_fail_fe(::gpk::loadView(input, Entities));
			if_fail_fef(EntityChildren.resize(Entities.size()), "size: %i", Entities.size());
			for(u2_t iEntity = 0; iEntity < Entities.size(); ++iEntity)
				if_fail_fef(::gpk::loadView(input, EntityChildren[iEntity]), "iEntity: %i", iEntity);

			return 0; 
		}		
		// 11 6688 8367
	};
}

#endif // GPK_GALAXY_HELL_ENTITY_H