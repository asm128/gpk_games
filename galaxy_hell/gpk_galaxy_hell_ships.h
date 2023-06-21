#include "gpk_galaxy_hell_entity.h"
#include "gpk_galaxy_hell_weapon.h"
#include "gpk_rigidbody.h"

#include "gpk_geometry_lh.h"
#include "gpk_model.h"
#include "gpk_engine.h"
#include "gpk_camera.h"
#include "gpk_image_color.h"
#include "gpk_view_n3.h"

#ifndef GPK_GALAXY_HELL_SHIPS_H
#define GPK_GALAXY_HELL_SHIPS_H

namespace ghg
{
	enum CAMERA_MODE
		{ CAMERA_MODE_SKY			= 0
		, CAMERA_MODE_PERSPECTIVE
		, CAMERA_MODE_MAP
		, CAMERA_MODE_FOLLOW
		, CAMERA_MODE_FRONT
		, CAMERA_MODE_REAR
		, CAMERA_MODE_COUNT
		};

#pragma pack(push, 1)
	struct SShipSceneGlobal	{
		::gpk::m4f32			MatrixProjection	= {};
		::gpk::n3f32			LightVector			= {0, -12, 0};
		CAMERA_MODE				CameraMode			= CAMERA_MODE_PERSPECTIVE;

		::gpk::astatic<::gpk::SCameraPoints, ::ghg::CAMERA_MODE_COUNT>	
								Camera				= {};

		::gpk::error_t			CameraReset			() {
			Camera[CAMERA_MODE_SKY].Target		= {};
			Camera[CAMERA_MODE_SKY].Position	= {-0.000001f, 250, 0};
			//Camera[CAMERA_MODE_SKY].Up			= {0, 1, 0};

			Camera[CAMERA_MODE_MAP].Target		= {};
			Camera[CAMERA_MODE_MAP].Position	= {-0.000001f, 2500, 0};
			//Camera[CAMERA_MODE_MAP].Up			= {0, 1, 0};

			Camera[CAMERA_MODE_PERSPECTIVE].Target		= {};
			Camera[CAMERA_MODE_PERSPECTIVE].Position	= {-0.000001f, 220, 0};
			//Camera[CAMERA_MODE_PERSPECTIVE].Up		= {0, 1, 0};
			Camera[CAMERA_MODE_PERSPECTIVE].Position.RotateZ(::gpk::math_pi * .325);
			return 0;
		}
	};
#pragma pack(pop)

	struct SShipScene {
		::ghg::SShipSceneGlobal				Global;
		::gpk::aobj<::gpk::SGeometryQuads>	Geometry		= {};
		::gpk::aobj<::gpk::img8bgra>		Image			= {};
		::gpk::apod<::gpk::m4f32>			Transforms		= {}; // Ope per entity

		::gpk::error_t						Save			(::gpk::au8 & output) const { 
			gpk_necs(::gpk::savePOD		(output, Global));
			gpk_necs(::gpk::saveView	(output, Transforms));
			info_printf("Saved %s, %i", "Transforms"	, Transforms.size());
			return 0; 
		}

		::gpk::error_t						Load			(::gpk::vcu8 & input) { 
			gpk_necs(::gpk::loadPOD	(input, Global		));
			gpk_necs(::gpk::loadView(input, Transforms	));
			return 0; 
		}
	};

	::gpk::error_t			getLightArrays
		( const ::gpk::n3f32	& modelPosition
		, const ::gpk::vn3f32	& lightPointsWorld
		, const ::gpk::a8bgra	& lightColorsWorld
		, ::gpk::an3f32			& lightPointsModel
		, ::gpk::a8bgra			& lightColorsModel
		);

	GDEFINE_ENUM_TYPE (SHIP_GEOMETRY, uint8_t);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, LINE			, 0);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, TRIANGLE		, 1);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, RECTANGLE		, 2);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, CIRCLE		, 3);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, FRAME			, 4);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, RING			, 5);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, CUBE			, 6);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, SPHERE		, 7);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, CYLINDER		, 8);
//
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, ENGINE		, 100);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, GUN			, 101);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, WAFER			, 102);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, CANNON		, 103);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, SHOTGUN		, 104);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, WAFER_SHOTGUN	, 105);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, CANNONBALL	, 106);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, ROCKET		, 107);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, MISSILE		, 108);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, BULLET		, 109);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, SHRED			, 110);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, NITRO			, 111);

	GDEFINE_ENUM_TYPE(SHIP_PART_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Cargo		, 0);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Cannon		, 1);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Wafer		, 2);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Gun			, 3);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, ShotgunWafer	, 4);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Shotgun		, 5);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Coil			, 6);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Shield		, 7);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Silo			, 8);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Tractor		, 9);

	GDEFINE_ENUM_TYPE (SHIP_ACTION, uint8_t);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, spawn			, 0);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, move_left		, 1);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, move_right		, 2);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, move_up			, 3);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, move_down		, 4);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, rotate_x_up		, 5);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, rotate_x_down	, 6);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, rotate_z_front	, 7);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, rotate_z_back	, 8);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, rotate_y_left	, 9);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, rotate_y_right	, 10);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, turbo			, 11);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, bomb			, 12);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, lock			, 13);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, unlock			, 14);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, dash			, 15);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, vanish			, 16);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, explode			, 17);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, hit				, 18);

#pragma pack(push, 1)
	struct SOrbiter {
		uint32_t		Entity			= (uint32_t)-1;
		SHIP_PART_TYPE	Type			= SHIP_PART_TYPE_Cargo;
		int32_t			Health			= 0;
		uint32_t		MaxHealth		= 0;
		uint32_t		Weapon			= (uint32_t)-1;
		uint32_t		ActiveAction	= ~0U;
	};

	struct SShipScore {
		uint64_t		Score			= 0;
		uint64_t		Hits			= 0;
		uint64_t		Shots			= 0;
		uint64_t		Bullets			= 0;
		uint64_t		DamageDone		= 0;
		uint64_t		DamageReceived	= 0;
		uint64_t		HitsSurvived	= 0;
		uint32_t		OrbitersLost	= 0;
		uint32_t		KilledShips		= 0;
		uint32_t		KilledOrbiters	= 0;
	};

	struct SShipCore {
		// Const
		int32_t			Entity			;
		int32_t			Team			;
		int32_t			MaxNitro		;

		// Variable
		int32_t			Health			;
		double			Nitro			;
		uint32_t		AvailableNitros	;
		double			TimePlayed		;
	};
#pragma pack(pop)

	struct SShipManager {
		::gpk::apod<::ghg::SShipScore			>	ShipScores						= {}; // One per core
		::gpk::apod<::ghg::SShipCore			>	ShipCores						= {}; // One per core
		::gpk::aobj<::gpk::au16					>	ShipParts						= {}; // One per core, each one mapping to a list of orbiters.

		::gpk::apod<::ghg::SOrbiter				>	Orbiters						= {};	// One per orbiter	
		::gpk::apod<::ghg::SWeapon				>	Weapons							= {};	// One per orbiter	
		::gpk::aobj<::gpk::apod<::gpk::n3f32>	>	ShipOrbitersDistanceToTargets	= {};	// One per orbiter, each one mapping to a list of orbiters
		::gpk::aobj<::ghg::SShots				>	Shots							= {};	// One per weapon
		::gpk::aobj<::gpk::apod<SHIP_ACTION>	>	ShipOrbiterActionQueue			= {};

		::gpk::au32					ShipCoreToEntityMap	= {};
		::gpk::au32					ShipPartToEntityMap	= {};

		::gpk::au32					MeshMap				= {};

		::gpk::SEngine				Engine;

		::ghg::SEntitySystem		EntitySystem		= {};
		::ghg::SShipScene			Scene				= {};

		int32_t						Clear				()	{
			::gpk::clear
				( ShipScores
				, ShipCores
				, ShipParts
				, Orbiters
				, Weapons
				, Shots
				, ShipOrbiterActionQueue
				, Scene.Transforms
				, ShipCoreToEntityMap
				, ShipPartToEntityMap
			);
			Engine.Integrator.Clear();
			Engine.Entities.clear();

			EntitySystem				= {};
			return 0;
		}

		int32_t						GetShipHealth		(uint32_t iShipCore)				{ 
			int32_t							totalHealth			= 0;
			::gpk::vcu16					shipCoreParts		= ShipParts[iShipCore];
			for(uint32_t iShipCorePart = 0, countParts = shipCoreParts.size(); iShipCorePart < countParts; ++iShipCorePart)
				totalHealth += Orbiters[shipCoreParts[iShipCorePart]].Health;

			return totalHealth;
		}

		int32_t						GetTeamHealth		(int32_t teamId) {
			int32_t							totalHealth			= 0;
			for(uint32_t iShipCore = 0, countShips = ShipCores.size(); iShipCore < countShips; ++iShipCore) {
				if(ShipCores[iShipCore].Team != teamId) 
					continue;

				::gpk::vcu16					shipCoreParts		= ShipParts[iShipCore];
				for(uint32_t iShipCorePart = 0, countParts = shipCoreParts.size(); iShipCorePart < countParts; ++iShipCorePart) 
					totalHealth += Orbiters[shipCoreParts[iShipCorePart]].Health;
			}
			return totalHealth;
		}

		::gpk::SBodyCenter&					GetShipPivot		(const SShipCore & ship)			{ return Engine.Integrator.Centers[EntitySystem.Entities[ship.Entity].Body]; }
		const ::gpk::SBodyCenter&			GetShipPivot		(const SShipCore & ship)	const	{ return Engine.Integrator.Centers[EntitySystem.Entities[ship.Entity].Body]; }
		inline	::gpk::SBodyCenter&			GetShipPivot		(uint32_t indexShip)				{ return GetShipPivot(ShipCores[indexShip]); }
		inline	const ::gpk::SBodyCenter&	GetShipPivot		(uint32_t indexShip)		const	{ return GetShipPivot(ShipCores[indexShip]); }

		::gpk::n3f32&						GetShipPosition		(const SShipCore & ship)			{ return GetShipPivot(ship).Position; }
		const ::gpk::n3f32&					GetShipPosition		(const SShipCore & ship)	const	{ return GetShipPivot(ship).Position; }
		inline	::gpk::n3f32&				GetShipPosition		(uint32_t indexShip)				{ return GetShipPosition(ShipCores[indexShip]); }
		inline	const ::gpk::n3f32&			GetShipPosition		(uint32_t indexShip)		const	{ return GetShipPosition(ShipCores[indexShip]); }

		::gpk::error_t						GetShipPosition		(uint32_t iShip, ::gpk::n3f32 & output) const { output = GetShipPosition(iShip); return 0; }
		
		::gpk::SBodyCenter&			GetOrbiterTransform	(const SOrbiter & shipPart)		{ return Engine.Integrator.Centers[EntitySystem.Entities[shipPart.Entity + 1].Body]; }
		::gpk::SBodyForces&			GetShipOrbiterForces(const SOrbiter & shipPart)		{ return Engine.Integrator.Forces [EntitySystem.Entities[shipPart.Entity + 1].Body]; }


		::gpk::error_t				Save				(::gpk::au8 & output)	const	{ 
			gpk_necs(::gpk::saveView(output, ShipScores	));
			gpk_necs(::gpk::saveView(output, ShipCores	));
			uint32_t						totalEntityChildren	= 0;
			for(uint32_t iShipCore = 0; iShipCore < ShipCores.size(); ++iShipCore) {
				::gpk::vcu16					v		{ShipParts[iShipCore]};
				::gpk::saveView(output, v);
				totalEntityChildren += v.size();
			}

			info_printf("Saved %s, %i", "ShipScores	", ShipScores	.size());
			info_printf("Saved %s, %i", "ShipCores	", ShipCores	.size());
	
			gpk_necs(::gpk::saveView(output, Orbiters));
			info_printf("Saved %s, %i", "Orbiters", Orbiters.size());
			for(uint32_t iShipOrbiter = 0; iShipOrbiter < Orbiters.size(); ++iShipOrbiter) {
				gpk_necs(::gpk::saveView(output, ShipOrbitersDistanceToTargets	[iShipOrbiter]));
				gpk_necs(::gpk::saveView(output, ShipOrbiterActionQueue			[iShipOrbiter]));
			}

			gpk_necs(::gpk::saveView(output, Weapons));
			info_printf("Saved %s, %i", "Weapons", Weapons.size());
			for(uint32_t iWeapon = 0; iWeapon < Weapons.size(); ++iWeapon) 
				gpk_necs(Shots[iWeapon].Save(output));
			
			gpk_necs(Engine.Save(output));

			gpk_necs(EntitySystem	.Save(output));
			gpk_necs(Scene			.Save(output));
			return 0; 
		}
		::gpk::error_t				Load				(::gpk::vcu8 & input) { 
			ShipScores	.clear();
			ShipCores	.clear();
			gpk_necs(::gpk::loadView(input, ShipScores	));
			gpk_necs(::gpk::loadView(input, ShipCores	));
			gpk_necs(ShipParts.resize(ShipCores.size()));
			for(uint32_t iPart = 0; iPart < ShipParts.size(); ++iPart) 
				gpk_necall(::gpk::loadView(input, ShipParts[iPart]), "iWeapon: %i", iPart);

			Orbiters.clear();
			gpk_necs(::gpk::loadView(input, Orbiters));
			gpk_necs(ShipOrbitersDistanceToTargets	.resize(Orbiters.size()));
			gpk_necs(ShipOrbiterActionQueue			.resize(Orbiters.size()));
			for(uint32_t iShipOrbiter = 0; iShipOrbiter < Orbiters.size(); ++iShipOrbiter) {
				gpk_necall(::gpk::loadView(input, ShipOrbitersDistanceToTargets	[iShipOrbiter]), "iShipOrbiter: %i", iShipOrbiter);
				gpk_necall(::gpk::loadView(input, ShipOrbiterActionQueue		[iShipOrbiter]), "iShipOrbiter: %i", iShipOrbiter);
			}

			Weapons.clear();
			gpk_necs(::gpk::loadView(input, Weapons));
			Shots.resize(Weapons.size());
			for(uint32_t iWeapon = 0; iWeapon < Weapons.size(); ++iWeapon)
				gpk_necs(Shots[iWeapon].Load(input));

			gpk_necs(Engine.Load(input));

			gpk_necs(EntitySystem	.Load(input));
			gpk_necs(Scene			.Load(input));
			return 0; 
		}
	};

}

#endif // GPK_GALAXY_HELL_SHIPS_H