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
		::gpk::m4f2_t			MatrixProjection	= {};
		::gpk::n3f2_t			LightVector			= {0, -12, 0};
		CAMERA_MODE				CameraMode			= CAMERA_MODE_PERSPECTIVE;

		::gpk::astatic<::gpk::SCameraPoints, ::ghg::CAMERA_MODE_COUNT>	
								Camera				= {};

		gpk::err_t			CameraReset			() {
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
		::gpk::apod<::gpk::m4f2_t>			Transforms		= {}; // Ope per entity

		gpk::err_t						Save			(::gpk::au0_t & output) const { 
			if_fail_fe(::gpk::savePOD		(output, Global));
			if_fail_fe(::gpk::saveView	(output, Transforms));
			info_printf("Saved %s, %i", "Transforms"	, Transforms.size());
			return 0; 
		}

		gpk::err_t						Load			(::gpk::vcu0_t & input) { 
			if_fail_fe(::gpk::loadPOD	(input, Global		));
			if_fail_fe(::gpk::loadView(input, Transforms	));
			return 0; 
		}
	};

	gpk::err_t			getLightArrays
		( const ::gpk::n3f2_t	& modelPosition
		, const ::gpk::v3f2_t	& lightPointsWorld
		, const ::gpk::a8bgra	& lightColorsWorld
		, ::gpk::a3f2_t			& lightPointsModel
		, ::gpk::a8bgra			& lightColorsModel
		);


	struct SShipManager  {
		::gpk::SSpaceshipManager	SpaceshipManager;
		::gpk::SWeaponManager		WeaponManager;

		::gpk::aeid					ShipCoreEntity		= {};
		::gpk::aeid					ShipPartEntity		= {};

		::gpk::SEngine				Engine;
		::ghg::SEntitySystem		EntitySystem		= {};
		::ghg::SShipScene			Scene				= {};

		s2_t						Clear				()	{
			SpaceshipManager.Clear();
			WeaponManager.Clear();
			::gpk::clear(Scene.Transforms, ShipCoreEntity, ShipPartEntity);

			Engine.Integrator.Clear();
			Engine.Entities.clear();

			EntitySystem				= {};
			return 0;
		}

		inline	::gpk::SBodyCenter&			GetShipPivot		(u2_t indexShip)				{ return Engine.Integrator.Centers[EntitySystem.Entities[ShipCoreEntity[indexShip]].Body]; }
		inline	const ::gpk::SBodyCenter&	GetShipPivot		(u2_t indexShip)		const	{ return Engine.Integrator.Centers[EntitySystem.Entities[ShipCoreEntity[indexShip]].Body]; }

		inline	::gpk::n3f2_t&				GetShipPosition		(u2_t indexShip)				{ return GetShipPivot(indexShip).Position; }
		inline	const ::gpk::n3f2_t&			GetShipPosition		(u2_t indexShip)		const	{ return GetShipPivot(indexShip).Position; }

		gpk::err_t						GetShipPosition		(u2_t iShip, ::gpk::n3f2_t & output) const { output = GetShipPosition(iShip); return 0; }
		
		::gpk::SBodyCenter&					GetOrbiterTransform	(u2_t iPart)		{ return Engine.Integrator.Centers[EntitySystem.Entities[ShipPartEntity[iPart] + 1].Body]; }
		::gpk::SBodyForces&					GetShipOrbiterForces(u2_t iPart)		{ return Engine.Integrator.Forces [EntitySystem.Entities[ShipPartEntity[iPart] + 1].Body]; }


		gpk::err_t				Save				(::gpk::au0_t & output)	const	{ 
			if_fail_fe(SpaceshipManager.Save(output));
			if_fail_fe(WeaponManager.Save(output));

			if_fail_fe(::gpk::saveView(output, ShipCoreEntity));
			if_fail_fe(::gpk::saveView(output, ShipPartEntity));
		
			if_fail_fe(Engine.Save(output));

			if_fail_fe(EntitySystem	.Save(output));
			if_fail_fe(Scene			.Save(output));
			return 0; 
		}
		gpk::err_t				Load				(::gpk::vcu0_t & input) { 
			if_fail_fe(SpaceshipManager.Load(input));
			if_fail_fe(WeaponManager.Load(input));

			if_fail_fe(::gpk::loadView(input, ShipCoreEntity));
			if_fail_fe(::gpk::loadView(input, ShipPartEntity));

			if_fail_fe(Engine.Load(input));

			if_fail_fe(EntitySystem	.Load(input));
			if_fail_fe(Scene			.Load(input));
			return 0; 
		}
	};

}

#endif // GPK_GALAXY_HELL_SHIPS_H
