#include "gpk_enum.h"
#include "gpk_apod_serialize.h"

#ifndef GPK_GALAXY_HELL_ENTITY_H
#define GPK_GALAXY_HELL_ENTITY_H

namespace ghg 
{
	GDEFINE_ENUM_TYPE(SHAPE_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Custom		, 0);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Rectangle	, 1);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Circle		, 2);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Ring			, 3);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Cube			, 4);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Sphere		, 5);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Cylinder		, 6);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Torus		, 7);

#pragma pack(push, 1)
	struct SEntityFlags {
		SHAPE_TYPE					ShapeType				;
		uint8_t						Padding0				;
		uint16_t					Padding1				;
	};

	struct SGHEntity {
		int32_t						Parent					= -1;
		int32_t						Geometry				= -1;
		int32_t						Transform				= -1;
		int32_t						Image					= -1;
		int32_t						Body					= -1;
		SEntityFlags				Flags					= {SHAPE_TYPE_Sphere};
	};
#pragma pack(pop)

	struct SEntitySystem {
		::gpk::apod<::ghg::SGHEntity>	Entities				= {};
		::gpk::aobj<::gpk::au32>	EntityChildren			= {};

		::gpk::error_t				Create					(const ::ghg::SGHEntity & newEntity, ::gpk::vcu32 entityChildren = {}) { EntityChildren.push_back(entityChildren); return Entities.push_back(newEntity); }
		::gpk::error_t				Create					()																{ Create({}); }

		::gpk::error_t				Save					(::gpk::au8 & output) const { 
			gpk_necs(::gpk::saveView(output, Entities));
			for(uint32_t iEntity = 0; iEntity < Entities.size(); ++iEntity) 
				gpk_necall(::gpk::saveView(output, ::gpk::vcu32{EntityChildren[iEntity]}), "iEntity: %i", iEntity);
			info_printf("Saved %s, %i", "Entities"					, EntityChildren.size());
			return 0; 
		}

		::gpk::error_t				Load					(::gpk::vcu8 & input) { 
			gpk_necs(::gpk::loadView(input, Entities));
			gpk_necall(EntityChildren.resize(Entities.size()), "size: %i", Entities.size());
			for(uint32_t iEntity = 0; iEntity < Entities.size(); ++iEntity)
				gpk_necall(::gpk::loadView(input, EntityChildren[iEntity]), "iEntity: %i", iEntity);

			return 0; 
		}		
		// 11 6688 8367
	};
}

#endif // GPK_GALAXY_HELL_ENTITY_H