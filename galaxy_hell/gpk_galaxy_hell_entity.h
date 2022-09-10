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
		::gpk::array_obj<::ghg::SEntity>					Entities						= {};
		::gpk::array_obj<::gpk::array_pod<uint32_t>>		EntityChildren					= {};

		::gpk::error_t										Create(const ::ghg::SEntity & newEntity, ::gpk::view_array<const uint32_t> entityChildren) {
			Entities.push_back(newEntity);
			return EntityChildren.push_back(entityChildren);
		}
		::gpk::error_t										Save(::gpk::array_pod<byte_t> & output) const { 
			//uint32_t entityCount = EntityChildren.size();
			//output.append(::gpk::view_array<const byte_t>{(const byte_t*)&entityCount, (uint32_t)sizeof(uint32_t)});
			::gpk::viewWrite(::gpk::view_array<const ::ghg::SEntity>{Entities}, output);
			for(uint32_t iEntity = 0; iEntity < Entities.size(); ++iEntity)
				::gpk::viewWrite(::gpk::view_array<const uint32_t>{EntityChildren[iEntity]}, output);
			return 0; 
		}
		::gpk::error_t										Load(::gpk::view_array<const byte_t> & input) { 
			::gpk::view_array<const ::ghg::SEntity>					entities;
			int32_t													readBytes		= ::gpk::viewRead(entities, input);
			input												= {input.begin() + readBytes, input.size() - readBytes};
			Entities											= entities;
			EntityChildren.clear();
			if(0 == entities.size()) {
				return readBytes;
			}
			for(uint32_t iEntity = 0; iEntity < Entities.size(); ++iEntity) {
				::gpk::view_array<const uint32_t>						entityChildren;
				readBytes											= ::gpk::viewRead(entityChildren, input);
				input												= {input.begin() + readBytes, input.size() - readBytes};
				EntityChildren.push_back(entityChildren);

			}
			return readBytes; 
		}


	};
}

#endif // GPK_GALAXY_HELL_ENTITY_H