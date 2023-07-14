#include "gpk_spaceship.h"
#include "gpk_weapon.h"
#include "gpk_galaxy_hell_entity.h"
#include "gpk_rigid_body.h"

#include "gpk_geometry_lh.h"
#include "gpk_model.h"
#include "gpk_engine.h"
#include "gpk_camera.h"
#include "gpk_img_color.h"
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


	struct SShipManager  {
		::gpk::SSpaceshipManager	SpaceshipManager;
		::gpk::au32					ShipCoreToEntityMap	= {};
		::gpk::au32					ShipPartToEntityMap	= {};

		::gpk::au32					MeshMap				= {};

		::gpk::SEngine				Engine;

		::ghg::SEntitySystem		EntitySystem		= {};
		::ghg::SShipScene			Scene				= {};

		int32_t						Clear				()	{
			SpaceshipManager.Clear();
			::gpk::clear
				( Scene.Transforms
				, ShipCoreToEntityMap
				, ShipPartToEntityMap
				, MeshMap
				);
			Engine.Integrator.Clear();
			Engine.Entities.clear();

			EntitySystem				= {};
			return 0;
		}

		::gpk::SBodyCenter&					GetShipPivot		(const ::gpk::SSpaceshipCore & ship)			{ return Engine.Integrator.Centers[EntitySystem.Entities[ship.Entity].Body]; }
		const ::gpk::SBodyCenter&			GetShipPivot		(const ::gpk::SSpaceshipCore & ship)	const	{ return Engine.Integrator.Centers[EntitySystem.Entities[ship.Entity].Body]; }
		inline	::gpk::SBodyCenter&			GetShipPivot		(uint32_t indexShip)				{ return GetShipPivot(SpaceshipManager.ShipCores[indexShip]); }
		inline	const ::gpk::SBodyCenter&	GetShipPivot		(uint32_t indexShip)		const	{ return GetShipPivot(SpaceshipManager.ShipCores[indexShip]); }

		::gpk::n3f32&						GetShipPosition		(const ::gpk::SSpaceshipCore & ship)			{ return GetShipPivot(ship).Position; }
		const ::gpk::n3f32&					GetShipPosition		(const ::gpk::SSpaceshipCore & ship)	const	{ return GetShipPivot(ship).Position; }
		inline	::gpk::n3f32&				GetShipPosition		(uint32_t indexShip)				{ return GetShipPosition(SpaceshipManager.ShipCores[indexShip]); }
		inline	const ::gpk::n3f32&			GetShipPosition		(uint32_t indexShip)		const	{ return GetShipPosition(SpaceshipManager.ShipCores[indexShip]); }

		::gpk::error_t						GetShipPosition		(uint32_t iShip, ::gpk::n3f32 & output) const { output = GetShipPosition(iShip); return 0; }
		
		::gpk::SBodyCenter&			GetOrbiterTransform	(const ::gpk::SSpaceshipOrbiter & shipPart)		{ return Engine.Integrator.Centers[EntitySystem.Entities[shipPart.Entity + 1].Body]; }
		::gpk::SBodyForces&			GetShipOrbiterForces(const ::gpk::SSpaceshipOrbiter & shipPart)		{ return Engine.Integrator.Forces [EntitySystem.Entities[shipPart.Entity + 1].Body]; }


		::gpk::error_t				Save				(::gpk::au8 & output)	const	{ 
			gpk_necs(SpaceshipManager.Save(output));

			gpk_necs(::gpk::saveView(output, ShipCoreToEntityMap));
			gpk_necs(::gpk::saveView(output, ShipPartToEntityMap));
		
			gpk_necs(Engine.Save(output));

			gpk_necs(EntitySystem	.Save(output));
			gpk_necs(Scene			.Save(output));
			return 0; 
		}
		::gpk::error_t				Load				(::gpk::vcu8 & input) { 
			gpk_necs(SpaceshipManager.Load(input));

			gpk_necs(::gpk::loadView(input, ShipCoreToEntityMap));
			gpk_necs(::gpk::loadView(input, ShipPartToEntityMap));

			gpk_necs(Engine.Load(input));

			gpk_necs(EntitySystem	.Load(input));
			gpk_necs(Scene			.Load(input));
			return 0; 
		}
	};

}

#endif // GPK_GALAXY_HELL_SHIPS_H