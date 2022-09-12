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

	struct SShipPart {
		int32_t											Entity							= -1;
		SHIP_PART_TYPE									Type							= SHIP_PART_TYPE_Cargo;
		int32_t											Health							= 0;
		uint32_t										MaxHealth						= 0;
		int32_t											Weapon							= -1;
		uint32_t										ActiveAction					= ~0U;
	};

	struct SShipCore {
		int32_t											Entity							;
		int32_t											Team							;
		int32_t											Health							;
		uint32_t										Score							;
		double											TimePlayed						;
	};

	struct SShipState {
		::ghg::SShipScene								Scene							= {};

		::ghg::SEntitySystem							EntitySystem;

		::gpk::array_pod<::ghg::SShipPart			>				ShipParts					= {};
		::gpk::array_pod<::ghg::SWeapon				>				Weapons						= {};
		::gpk::array_pod<::ghg::SShipCore			>				ShipCores					= {};
		::gpk::array_obj<::gpk::array_pod<uint32_t>	>				ShipCoresParts				= {};
		::gpk::array_obj<::gpk::array_pod<::gpk::SCoord3<float>>>	ShipPartsDistanceToTargets	= {};
		::gpk::array_obj<::ghg::SShots				>				Shots						= {};	// one per weapon
		::gpk::SIntegrator3											ShipPhysics					= {};
		::gpk::array_obj<::gpk::array_pod<SHIP_ACTION>>				ShipActionQueue;

		::gpk::SCoord3<float>&							GetShipPosition		(const SShipCore & ship)		{ return ShipPhysics.Transforms	[EntitySystem.Entities[ship.Entity].Body].Position; }

		::gpk::STransform3&								GetShipPartTransform(const SShipPart & shipPart)	{ return ShipPhysics.Transforms	[EntitySystem.Entities[shipPart.Entity + 1].Body]; }
		::gpk::SForce3&									GetShipPartForces	(const SShipPart & shipPart)	{ return ShipPhysics.Forces		[EntitySystem.Entities[shipPart.Entity + 1].Body]; }

		inline	::gpk::SCoord3<float>&					GetShipPosition(uint32_t indexShip)					{ return GetShipPosition(ShipCores[indexShip]); }

		::gpk::error_t									Save				(::gpk::array_pod<byte_t> & output) const { 
			Scene		.Save(output);
			EntitySystem.Save(output);
			ShipPhysics	.Save(output);

			::gpk::viewWrite(::gpk::view_array<const ::ghg::SWeapon		>{Weapons	}, output);
			for(uint32_t iWeapon = 0; iWeapon < Weapons.size(); ++iWeapon)
				Shots[iWeapon].Save(output);

			::gpk::viewWrite(::gpk::view_array<const ::ghg::SShipPart	>{ShipParts	}, output);
			for(uint32_t iShipPart = 0; iShipPart < ShipParts.size(); ++iShipPart)
				::gpk::viewWrite(::gpk::view_array<const ::gpk::SCoord3<float>>{ShipPartsDistanceToTargets[iShipPart]}, output);

			::gpk::viewWrite(::gpk::view_array<const ::ghg::SShipCore	>{ShipCores	}, output);
			for(uint32_t iShipCore = 0; iShipCore < ShipCores.size(); ++iShipCore) 
				::gpk::viewWrite(::gpk::view_array<const uint32_t>{ShipCoresParts[iShipCore]}, output);

			return 0; 
		}
		::gpk::error_t									Load				(::gpk::view_array<const byte_t> & input) { 
			Scene.Load(input);
			EntitySystem.Load(input);
			ShipPhysics.Load(input);

			::gpk::view_array<const ::ghg::SShipPart	>		readShipParts	= {};
			::gpk::view_array<const ::ghg::SWeapon		>		readWeapons		= {};
			::gpk::view_array<const ::ghg::SShipCore	>		readShipCores	= {};
			int32_t												bytesRead		= 0;

			bytesRead = ::gpk::viewRead(readWeapons		, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; Weapons	= readWeapons;
			::gpk::clear(Shots, ShipCoresParts, ShipActionQueue);
			for(uint32_t iWeapon = 0; iWeapon < Weapons.size(); ++iWeapon)
				Shots[Shots.push_back({})].Load(input);

			bytesRead = ::gpk::viewRead(readShipParts	, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; ShipParts	= readShipParts;
			for(uint32_t iShipPart = 0; iShipPart < ShipParts.size(); ++iShipPart) {
				::gpk::view_array<const ::gpk::SCoord3<float>>		readShipPartsDistanceToTargets	= {};
				bytesRead = ::gpk::viewRead(readShipPartsDistanceToTargets, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
				ShipPartsDistanceToTargets.push_back(readShipPartsDistanceToTargets);
			}

			bytesRead = ::gpk::viewRead(readShipCores	, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; ShipCores	= readShipCores;
			for(uint32_t iWeapon = 0; iWeapon < ShipCores.size(); ++iWeapon) {
				::gpk::view_array<const uint32_t>					readShipCoresParts		= {};
				bytesRead = ::gpk::viewRead(readShipCoresParts, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
				ShipCoresParts.push_back(readShipCoresParts);
			}
			ShipActionQueue.resize(ShipCores.size());
			return 0; 
		}
	};

}

#endif // GPK_GALAXY_HELL_SHIPS_H