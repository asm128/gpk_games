#include "gpk_array.h"
#include "gpk_enum.h"

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
		SHAPE_TYPE											ShapeType				;
		uint8_t												Padding0				;
		uint16_t											Padding1				;
	};

	struct SEntity {
		int32_t												Parent					;
		int32_t												Geometry				;
		int32_t												Transform				;
		int32_t												Image					;
		int32_t												Body					;
		SEntityFlags										Flags					;
	};
#pragma pack(pop)

	struct SEntitySystem {
		::gpk::array_pod<::ghg::SEntity>					Entities				= {};
		::gpk::array_obj<::gpk::array_pod<uint32_t>>		EntityChildren			= {};

		::gpk::error_t										Create					(const ::ghg::SEntity & newEntity, ::gpk::view_array<const uint32_t> entityChildren) {
			EntityChildren.push_back(entityChildren);
			return Entities.push_back(newEntity);
		}

		::gpk::error_t										Create					() {
			EntityChildren.push_back({});
			return Entities.push_back({});
		}

		//::gpk::error_t										CreateSquare			() {
		//	SEntity													& squareEntity			= Entities[Create()];
		//	return EntityChildren.push_back(entityChildren);
		//}

		//::gpk::error_t										CreateCircle	(const ::ghg::SEntity & newEntity, gpk::AXIS axis);
		//::gpk::error_t										CreateRing		(const ::ghg::SEntity & newEntity, gpk::AXIS axis, float inset, float outset);
		//::gpk::error_t										CreateCube		(const ::ghg::SEntity & newEntity);
		//::gpk::error_t										CreateSphere	(const ::ghg::SEntity & newEntity, uint8_t stacks, uint8_t slices);
		//::gpk::error_t										CreateCylinder	(const ::ghg::SEntity & newEntity, uint8_t stacks, uint8_t slices, float radiusYMin, float radiusYMax);
		//::gpk::error_t										CreateTorus		(const ::ghg::SEntity & newEntity, uint8_t stacks, uint8_t slices, float radiusCircle, float radiusCylinder);

		::gpk::error_t										Save					(::gpk::array_pod<byte_t> & output) const { 
			gpk_necs(::gpk::viewWrite(Entities, output));
			for(uint32_t iEntity = 0; iEntity < Entities.size(); ++iEntity) 
				gpk_necall(::gpk::viewWrite(::gpk::view_array<const uint32_t>{EntityChildren[iEntity]}, output), "iEntity: %i", iEntity);
			info_printf("Saved %s, %i", "Entities"					, EntityChildren.size());
			return 0; 
		}
		::gpk::error_t										Load					(::gpk::view_array<const byte_t> & input) { 
			gpk_necs(::gpk::loadView(input, Entities));
			gpk_necall(EntityChildren.resize(Entities.size()), "size: %i", Entities.size());
			for(uint32_t iEntity = 0; iEntity < Entities.size(); ++iEntity)
				gpk_necall(::gpk::loadView(input, EntityChildren[iEntity]), "iEntity: %i", iEntity);

			return 0; 
		}
	};
}

#endif // GPK_GALAXY_HELL_ENTITY_H