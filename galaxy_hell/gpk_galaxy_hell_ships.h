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
		, CAMERA_MODE_FRONT
		, CAMERA_MODE_COUNT
		};

#pragma pack(push, 1)
	struct SShipSceneGlobal	{
		::gpk::SCoord3	<float>											LightVector						= {0, -12, 0};
		::gpk::SMatrix4	<float>											MatrixProjection				= {};
		CAMERA_MODE														CameraMode						= CAMERA_MODE_PERSPECTIVE;
		::gpk::array_static<::gpk::SCamera, ::ghg::CAMERA_MODE_COUNT>	Camera							= {};
	};
#pragma pack(pop)

	struct SShipScene {
		::ghg::SShipSceneGlobal								Global;
		//::gpk::SBufferManager								BufferManager					= {};
		::gpk::array_obj<::gpk::SGeometryQuads>				Geometry						= {};
		::gpk::array_obj<::gpk::SImage<::gpk::SColorBGRA>>	Image							= {};
		::gpk::array_pod<::gpk::SMatrix4<float>>			Transforms						= {}; // Ope per entity

		::gpk::error_t										Save(::gpk::array_pod<byte_t> & output) const { 
			::gpk::viewWrite(::gpk::view_array<const ::ghg::SShipSceneGlobal>{&Global, 1}, output);
			::gpk::viewWrite(::gpk::view_array<const ::gpk::SMatrix4<float>>{Transforms}, output);
			return 0; 
		}
		::gpk::error_t										Load(::gpk::view_array<const byte_t> & input) { 
			::gpk::view_array<const ::ghg::SShipSceneGlobal>		readGlobal				= {};
			::gpk::view_array<const ::gpk::SMatrix4<float>>			readTransforms			= {};
			int32_t													bytesRead				= 0;
			bytesRead = ::gpk::viewRead(readGlobal		, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; Global		= readGlobal[0];
			bytesRead = ::gpk::viewRead(readTransforms	, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; Transforms	= readTransforms;
			return bytesRead; 
		}
	};

	int													getLightArrays
		( const ::gpk::SCoord3<float>							& modelPosition
		, const ::gpk::array_pod<::gpk::SCoord3<float>>			& lightPointsWorld
		, const ::gpk::array_pod<::gpk::SColorBGRA>				& lightColorsWorld
		, ::gpk::array_pod<::gpk::SCoord3<float>>				& lightPointsModel
		, ::gpk::array_pod<::gpk::SColorBGRA>					& lightColorsModel
		);

	GDEFINE_ENUM_TYPE(SHIP_PART_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Cargo	, 0);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Cannon	, 1);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Wafer	, 2);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Gun		, 3);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Coil		, 4);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Shield	, 5);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Silo		, 6);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Tractor	, 7);

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
		int32_t														Entity							= -1;
		SHIP_PART_TYPE												Type							= SHIP_PART_TYPE_Cargo;
		int32_t														Health							= 0;
		uint32_t													MaxHealth						= 0;
		int32_t														Weapon							= -1;
		uint32_t													ActiveAction					= ~0U;
	};

	struct SShipScore {
		uint64_t													Score			= 0;
		uint64_t													Hits			= 0;
		uint64_t													DamageDone		= 0;
		uint64_t													DamageReceived	= 0;
		uint64_t													HitsSurvived	= 0;
		uint32_t													OrbitersLost	= 0;
		uint32_t													KilledShips		= 0;
		uint32_t													KilledOrbiters	= 0;
	};

	struct SShipCore {
		int32_t														Entity							;
		int32_t														Team							;
		int32_t														Health							;

		double														TimePlayed						;
	};
#pragma pack(pop)

	struct SShipState {
		::gpk::array_pod<::ghg::SShipScore			>				ShipScores						= {};
		::gpk::array_pod<::ghg::SShipCore			>				ShipCores						= {};
		::gpk::array_obj<::gpk::array_pod<uint32_t>	>				ShipParts						= {};
		::gpk::array_pod<::ghg::SOrbiter			>				Orbiters						= {};
		::gpk::array_pod<::ghg::SWeapon				>				Weapons							= {};
		::gpk::array_obj<::gpk::array_pod<::gpk::SCoord3<float>>>	ShipOrbitersDistanceToTargets	= {};
		::gpk::array_obj<::ghg::SShots				>				Shots							= {};	// one per weapon
		::gpk::array_obj<::gpk::array_pod<SHIP_ACTION>>				ShipOrbiterActionQueue			= {};
		::gpk::SIntegrator3											ShipPhysics						= {};

		::ghg::SEntitySystem										EntitySystem;
		::ghg::SShipScene											Scene						= {};

		int32_t														GetShipHealth				(uint32_t iShipCore)				{ 
			int32_t															totalHealth					= 0;
			::gpk::view_array<const uint32_t>								shipCoreParts				= ShipParts[iShipCore];
			for(uint32_t iShipCorePart = 0, countParts = shipCoreParts.size(); iShipCorePart < countParts; ++iShipCorePart)
				totalHealth += Orbiters[shipCoreParts[iShipCorePart]].Health;

			return totalHealth;
		}

		int32_t														GetTeamHealth(int32_t teamId) {
			int32_t															totalHealth					= 0;
			for(uint32_t iShipCore = 0, countShips = ShipCores.size(); iShipCore < countShips; ++iShipCore) {
				if(ShipCores[iShipCore].Team != teamId) 
					continue;

				::gpk::view_array<const uint32_t>								shipCoreParts				= ShipParts[iShipCore];
				for(uint32_t iShipCorePart = 0, countParts = shipCoreParts.size(); iShipCorePart < countParts; ++iShipCorePart) 
					totalHealth += Orbiters[shipCoreParts[iShipCorePart]].Health;
			}
			return totalHealth;
		}

		::gpk::SCoord3<float>&										GetShipPosition			(const SShipCore & ship)		{ return ShipPhysics.Transforms	[EntitySystem.Entities[ship.Entity].Body].Position; }

		::gpk::STransform3&											GetShipOrbiterTransform	(const SOrbiter & shipPart)	{ return ShipPhysics.Transforms	[EntitySystem.Entities[shipPart.Entity + 1].Body]; }
		::gpk::SForce3&												GetShipOrbiterForces	(const SOrbiter & shipPart)	{ return ShipPhysics.Forces		[EntitySystem.Entities[shipPart.Entity + 1].Body]; }

		inline	::gpk::SCoord3<float>&								GetShipPosition			(uint32_t indexShip)			{ return GetShipPosition(ShipCores[indexShip]); }

		::gpk::error_t												Save					(::gpk::array_pod<byte_t> & output) const { 
			::gpk::viewWrite(::gpk::view_array<const ::ghg::SShipScore	>{ShipScores	}, output);
			::gpk::viewWrite(::gpk::view_array<const ::ghg::SShipCore	>{ShipCores		}, output);
			for(uint32_t iShipCore = 0; iShipCore < ShipCores.size(); ++iShipCore) 
				::gpk::viewWrite(::gpk::view_array<const uint32_t>{ShipParts[iShipCore]}, output);

			::gpk::viewWrite(::gpk::view_array<const ::ghg::SOrbiter	>{Orbiters	}, output);
			for(uint32_t iShipOrbiter = 0; iShipOrbiter < Orbiters.size(); ++iShipOrbiter) {
				::gpk::viewWrite(::gpk::view_array<const ::gpk::SCoord3<float>>	{ShipOrbitersDistanceToTargets	[iShipOrbiter]}, output);
				::gpk::viewWrite(::gpk::view_array<const ::ghg::SHIP_ACTION>	{ShipOrbiterActionQueue			[iShipOrbiter]}, output);
			}

			::gpk::viewWrite(::gpk::view_array<const ::ghg::SWeapon>{Weapons}, output);
			for(uint32_t iWeapon = 0; iWeapon < Weapons.size(); ++iWeapon)
				Shots[iWeapon].Save(output);

			ShipPhysics	.Save(output);
			EntitySystem.Save(output);
			Scene		.Save(output);
			return 0; 
		}
		::gpk::error_t									Load				(::gpk::view_array<const byte_t> & input) { 
			::gpk::view_array<const ::ghg::SOrbiter	>		readShipOrbiters	= {};
			::gpk::view_array<const ::ghg::SWeapon		>		readWeapons		= {};
			::gpk::view_array<const ::ghg::SShipCore	>		readShipCores	= {};
			::gpk::view_array<const ::ghg::SShipScore	>		readShipScores	= {};
			int32_t												bytesRead		= 0;

			::gpk::clear(Shots, ShipParts, ShipOrbiterActionQueue, ShipOrbitersDistanceToTargets);

			bytesRead = ::gpk::viewRead(readShipScores	, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; ShipScores	= readShipScores;
			bytesRead = ::gpk::viewRead(readShipCores	, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; ShipCores	= readShipCores;
			for(uint32_t iWeapon = 0; iWeapon < ShipCores.size(); ++iWeapon) {
				::gpk::view_array<const uint32_t>					readShipCoresParts		= {};
				bytesRead = ::gpk::viewRead(readShipCoresParts, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
				ShipParts.push_back(readShipCoresParts);
			}

			bytesRead = ::gpk::viewRead(readShipOrbiters	, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; Orbiters	= readShipOrbiters;
			for(uint32_t iShipOrbiter = 0; iShipOrbiter < Orbiters.size(); ++iShipOrbiter) {
				::gpk::view_array<const ::gpk::SCoord3<float>>		readShipOrbitersDistanceToTargets	= {};
				::gpk::view_array<const ::ghg::SHIP_ACTION>			readShipActions		= {};
				bytesRead = ::gpk::viewRead(readShipOrbitersDistanceToTargets, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
				ShipOrbitersDistanceToTargets.push_back(readShipOrbitersDistanceToTargets);
				bytesRead = ::gpk::viewRead(readShipActions, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
				ShipOrbiterActionQueue.push_back(readShipActions);
			}

			bytesRead = ::gpk::viewRead(readWeapons		, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; Weapons	= readWeapons;
			for(uint32_t iWeapon = 0; iWeapon < Weapons.size(); ++iWeapon)
				Shots[Shots.push_back({})].Load(input);

			ShipPhysics		.Load(input);
			EntitySystem	.Load(input);
			Scene			.Load(input);
			return 0; 
		}
	};

}

#endif // GPK_GALAXY_HELL_SHIPS_H