#include "gpk_galaxy_hell_weapon.h"
#include "gpk_galaxy_hell_entity.h"
#include "gpk_rigidbody.h"

#include "gpk_geometry_lh.h"
#include "gpk_model.h"

#ifndef GPK_GALAXY_HELL_SHIPS_H
#define GPK_GALAXY_HELL_SHIPS_H

namespace ghg
{
	enum CAMERA_MODE
		{ CAMERA_MODE_SKY			= 0
		, CAMERA_MODE_PERSPECTIVE
		, CAMERA_MODE_FOLLOW
		//, CAMERA_MODE_FRONT
		, CAMERA_MODE_COUNT
		};

	struct SShipScene	{
		::gpk::array_pod<::gpk::SMatrix4<float>>				Transforms						= {}; // Ope per entity
		//::gpk::SBufferManager									BufferManager					= {};
		::gpk::array_obj<::gpk::SGeometryQuads>					Geometry						= {};
		::gpk::array_obj<::gpk::SImage<::gpk::SColorBGRA>>		Image							= {};
		::gpk::SCamera											Camera[CAMERA_MODE_COUNT]		= {};
		::gpk::SCoord3	<float>									LightVector						= {0, -12, 0};
		::gpk::SMatrix4	<float>									MatrixProjection				= {};
		CAMERA_MODE												CameraMode						= CAMERA_MODE_PERSPECTIVE;
	};

	GDEFINE_ENUM_TYPE(SHIP_PART_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, CANNON	, 0);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, WAFER	, 1);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, GUN		, 2);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, COIL		, 3);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, SHIELD	, 4);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, SILO		, 5);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, TRACTOR	, 6);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, COUNT	, 7);

	GDEFINE_ENUM_TYPE (SHIP_ACTION, uint8_t);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, SPAWN			, 0);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, MOVE_LEFT		, 1);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, MOVE_RIGHT		, 2);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, MOVE_UP			, 3);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, MOVE_DOWN		, 4);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, ROTATE_XUP		, 5);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, ROTATE_XDOWN	, 6);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, ROTATE_ZFRONT	, 7);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, ROTATE_ZBACK	, 8);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, ROTATE_YLEFT	, 9);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, ROTATE_YRIGHT	, 10);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, TURBO			, 11);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, BOMB			, 12);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, LOCK			, 13);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, UNLOCK			, 14);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, DASH			, 15);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, VANISH			, 16);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, EXPLODE			, 17);

	struct SShipPart {
		int32_t													Entity					= -1;
		int32_t													Type					= -1;
		int32_t													Health					= -1;
		::ghg::SWeapon											Weapon					= {};
		uint32_t												ActiveAction			= ~0U;
	};

	struct SShip {
		int32_t													Entity					;
		int32_t													Team					;
		int32_t													Health					;
		::gpk::array_obj<::ghg::SShipPart>						Parts					;
	};

	typedef ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>	TRenderTarget;

	struct SShipScore {
		::gpk::array_pod<::ghg::SWeaponScore>					WeaponScore;
	};

	struct SShipState {
		::ghg::SShipScene										Scene					= {};

		::ghg::SEntitySystem									EntitySystem;
		::gpk::array_obj<::ghg::SShip>							Ships					= {};
		::gpk::array_obj<::ghg::SShipScore>						ShipScores				= {};
		::gpk::SIntegrator3										ShipPhysics				= {};
		::gpk::array_obj<::gpk::array_pod<SHIP_ACTION>>			ShipActionQueue;
	};
}

#endif // GPK_GALAXY_HELL_SHIPS_H