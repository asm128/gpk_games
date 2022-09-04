#include "gpk_array.h"
#include "gpk_geometry_lh.h"

#ifndef GPK_GALAXY_HELL_ENTITY_H
#define GPK_GALAXY_HELL_ENTITY_H

namespace ghg {
	#pragma pack(push, 1)
	struct SEntityFlags {
		int32_t												Padding						:1;
	};
	#pragma pack(pop)

	struct SEntity {
		int32_t												Parent						;
		//::gpk::MODEL_NODE_TYPE								ModelType					;
		int32_t												Geometry					;
		int32_t												Transform					;
		int32_t												Image						;
		int32_t												Body						;
		SEntityFlags										Flags						;
	};

	struct SEntitySystem {
		::gpk::array_obj<::ghg::SEntity>								Entities						= {};
		::gpk::array_obj<::gpk::array_pod<uint32_t>>					EntityChildren					= {};

		::gpk::error_t													Create(const ::ghg::SEntity & newEntity, ::gpk::view_array<const uint32_t> entityChildren) {
			Entities.push_back(newEntity);
			return EntityChildren.push_back(entityChildren);
		}
	};
}

#endif // GPK_GALAXY_HELL_ENTITY_H