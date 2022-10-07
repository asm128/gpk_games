#include "gpk_array.h"

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
		int32_t												Geometry					;
		int32_t												Transform					;
		int32_t												Image						;
		int32_t												Body						;
		SEntityFlags										Flags						;
	};

	struct SEntitySystem {
		::gpk::array_pod<::ghg::SEntity>					Entities						= {};
		::gpk::array_obj<::gpk::array_pod<uint32_t>>		EntityChildren					= {};

		::gpk::error_t										Create(const ::ghg::SEntity & newEntity, ::gpk::view_array<const uint32_t> entityChildren) {
			Entities.push_back(newEntity);
			return EntityChildren.push_back(entityChildren);
		}
		::gpk::error_t										Save(::gpk::array_pod<byte_t> & output) const { 
			::gpk::viewWrite(::gpk::view_array<const ::ghg::SEntity>{Entities}, output);
			for(uint32_t iEntity = 0; iEntity < Entities.size(); ++iEntity) 
				::gpk::viewWrite(::gpk::view_array<const uint32_t>{EntityChildren[iEntity]}, output);
			info_printf("Saved %s, %i", "Entities"					, EntityChildren.size());
			return 0; 
		}
		::gpk::error_t										Load(::gpk::view_array<const byte_t> & input) { 
			gpk_necs(::gpk::loadView(input, Entities));
			EntityChildren.resize(Entities.size());
			for(uint32_t iEntity = 0; iEntity < Entities.size(); ++iEntity)
				gpk_necall(::gpk::loadView(input, EntityChildren[iEntity]), "iEntity: %i", iEntity);

			return 0; 
		}


	};
}

#endif // GPK_GALAXY_HELL_ENTITY_H