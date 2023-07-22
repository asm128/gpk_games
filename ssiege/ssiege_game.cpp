#include "ssiege_game_handle.h"
#include "ssiege_game.h"
#include "gpk_timer.h"

stacxpr	float			SCALE_SHIP				= .001f;	// 

stacxpr	float			SHIP_CORE_RADIUS		= SCALE_SHIP * .5f;	// the ship core diameter represents the size unit
// 
stacxpr	float			SHIP_RING_HEIGHT		= SHIP_CORE_RADIUS * 2;							// 
stacxpr	float			SHIP_RING_RADIUS_MAX	= SHIP_CORE_RADIUS * 10;						// 
stacxpr	gpk::minmaxf32	SHIP_RING_RADIUS		= {SHIP_RING_RADIUS_MAX - SHIP_CORE_RADIUS, SHIP_RING_RADIUS_MAX};	// 
stacxpr	float			SHIP_RING_BORDER		= SHIP_RING_RADIUS.Length();	// 
stacxpr	float			SHIP_BASE_BOX_SIDE		= ::gpk::max(SHIP_RING_HEIGHT, SHIP_RING_BORDER);	// 
stacxpr	float			SHIP_BASE_BOX			= SHIP_BASE_BOX_SIDE + SHIP_BASE_BOX_SIDE * .5f;	// 
stacxpr	float			SHIP_BASE_IRON			= SHIP_BASE_BOX * .125f;		// 
stacxpr	float			SHIP_ARM_LENGTH			= SHIP_RING_RADIUS_MAX;	// 
stacxpr	::gpk::n3f32	SHIP_ARM_SIZE			= {SHIP_CORE_RADIUS * .5f, SHIP_ARM_LENGTH, SHIP_CORE_RADIUS * .5f};	// 
stacxpr	uint32_t		DEFAULT_ARM_COUNT		= 6;

static	::gpk::error_t	createShipCore		(::gpk::SEngine & engine) {
	::gpk::SParamsSphere		sphere				= {};
	sphere.Radius			= ::SHIP_CORE_RADIUS;
	::gpk::eid_t				iShip; 
	gpk_necs(iShip = engine.CreateSphere(sphere, "Ship Core"));
	engine.SetRotation(iShip, ::gpk::n3f32{0, 0, float(::gpk::math_pi_2 * .15)});
	return iShip;
}

static	::gpk::error_t	createShipRing		(::gpk::SEngine & engine) {
	::gpk::SParamsRing			ring				= {};
	ring.Origin.y			= ::SHIP_RING_HEIGHT * .5f;
	ring.Height				= ::SHIP_RING_HEIGHT;
	ring.RadiusYMin			= ::SHIP_RING_RADIUS;
	ring.RadiusYMax			= ::SHIP_RING_RADIUS;
	ring.CellCount.x		= 32;

	::gpk::eid_t				iShipRing; 
	gpk_necs(iShipRing = engine.CreateRing(ring, "Ship Ring"));
	engine.SetRotation(iShipRing, ::gpk::n3f32{0, float(::gpk::math_pi * .25),});
	return iShipRing; 
}

static	::gpk::error_t	createShipCraneBase		(::gpk::SEngine & engine) {
	::gpk::eid_t				iArmRoot;
	{	// construct the grip around the ship ring
		::gpk::SParamsBox			base					= {};
		base.HalfSizes.From(::SHIP_BASE_BOX * .5f);
		base.Origin				= base.HalfSizes;
		gpk_necs(iArmRoot = engine.CreateBox(base, "Crane Base"));
		engine.SetColorDiffuse(iArmRoot, {1, 1, 1, .5f});
		//engine.SetHidden(iArmRoot, true);
	}
	for(uint32_t iAxis = 0; iAxis < 3; ++iAxis)
	for(uint32_t iIron = 0; iIron < 4; ++iIron) {
		::gpk::eid_t				iArmBaseBorder;
		::gpk::SParamsBox			iron					= {};
		const uint32_t				iRow					= iIron / 2;
		const uint32_t				iCol					= iIron % 2;
		iron.HalfSizes.SetAxis  (iAxis, ::SHIP_BASE_BOX  * .5f);
		iron.HalfSizes.SetOthers(iAxis, ::SHIP_BASE_IRON * .5f);
		iron.Origin				= iron.HalfSizes;
		gpk_necs(iArmBaseBorder = engine.CreateBox(iron, "Crane Base Iron"));
	
		::gpk::n3f32				position				= {};
		position.SetAxis((iAxis + 1) % 3, (::SHIP_BASE_BOX * .5f - ::SHIP_BASE_IRON * .5f) * (iRow ? 1 : -1));
		position.SetAxis((iAxis + 2) % 3, (::SHIP_BASE_BOX * .5f - ::SHIP_BASE_IRON * .5f) * (iCol ? 1 : -1));
		engine.SetPosition(iArmBaseBorder, position);
		gpk_necs(engine.Entities.SetParent(iArmBaseBorder, iArmRoot));
	}
	return iArmRoot;
}

static	::gpk::error_t	createShipCrane		(::gpk::SEngine & engine) {
	::gpk::eid_t				iCrane;
	
	gpk_necs(iCrane = ::createShipCraneBase(engine));	

	// construct the arm itself
	::gpk::SParamsCylinderWall	arm					= {};
	arm.Radius				= {::SHIP_ARM_SIZE.x * .5f, ::SHIP_ARM_SIZE.x * .5f};
	arm.Height				= SHIP_ARM_LENGTH;
	::gpk::eid_t				iArm;
	gpk_necs(iArm = engine.CreateDisc(arm, "Ship Crane Arm Right?"));
	engine.SetPosition(iArm, ::gpk::n3f32{::SHIP_BASE_BOX * .5f, -SHIP_BASE_BOX * .5f, -SHIP_BASE_BOX * .5f});
	gpk_necs(engine.Entities.SetParent(iArm, iCrane));
	gpk_necs(iArm = engine.CreateDisc(arm, "Ship Crane Arm Left?"));
	engine.SetPosition(iArm, ::gpk::n3f32{::SHIP_BASE_BOX * .5f, -SHIP_BASE_BOX * .5f, SHIP_BASE_BOX * .5f});
	gpk_necs(engine.Entities.SetParent(iArm, iCrane));
	
	return iCrane; 
}

////static	::gpk::error_t	shipEngineCreate	(::gpk::SEngine & engine) {
////	::gpk::SParamsRing			ring			= {};
////	ring.Height				= .0005f;
////	ring.RadiusYMin.Max		*= .005f;
////	ring.RadiusYMin.Min		*= .005f;
////	ring.RadiusYMax.Max		*= .005f;
////	ring.RadiusYMax.Min		*= .005f;
////
////	::gpk::eid_t				iShipRing; 
////	gpk_necs(iShipRing = engine.CreateRing(ring));
////	engine.SetRotation(iShipRing, ::gpk::n3f32{0, float(::gpk::math_pi * .25),});
////	return iShipRing; 
////}

static	::gpk::error_t	shipCreate			(::ssg::SSiegeGame & world) { 
	gpk_necs(world.ShipCore = ::createShipCore(world.Engine));
	gpk_necs(world.ShipRing = ::createShipRing(world.Engine));
	for(uint32_t iArm = 0; iArm < ::DEFAULT_ARM_COUNT; ++iArm) {
		::gpk::eid_t				iArmRoot			= ::createShipCrane(world.Engine);
		::gpk::n3f32				position			= {::SHIP_RING_RADIUS.Middle()};
		position.RotateY(::gpk::math_2pi * (1.0f / ::DEFAULT_ARM_COUNT) * iArm);
		world.Engine.SetOrientation(iArmRoot, ::gpk::quatf32{}.MakeFromEuler(0, -(float)::gpk::math_2pi * (1.0f / ::DEFAULT_ARM_COUNT) * iArm, 0).Normalize());
		world.Engine.SetPosition(iArmRoot, position);
		world.Engine.Entities.SetParent(iArmRoot, world.ShipCore);
	}
	
	world.Engine.SetPosition	(world.ShipCore, (world.Camera.Position * .95) + ::gpk::n3f32{-.005f});
	world.Engine.SetOrientation	(world.ShipCore, ::gpk::quatf32{}.MakeFromEuler(::gpk::math_pi_2 * .5, .25f, ::gpk::math_pi_2));
	world.Engine.Entities.SetParent(world.ShipRing, world.ShipCore);

	return 0; 
}

::gpk::error_t			ssg::ssiegeGameSetup	(::ssg::SSiegeGame & world) {
	gpk_necs(::gpk::planetarySystemSetup(world.SolarSystem, world.Engine, "gpk_solar_system.json"));
	gpk_necs(::shipCreate(world));
	return 0;
}

::gpk::error_t			ssg::ssiegeGameUpdate	(::ssg::SSiegeGame & world, ::gpk::vpobj<::ssiege::EventSSiege> inputEvents, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents, double secondsElapsed) {
	::gpk::FBool<::gpk::pobj<::ssiege::EventSSiege> &, ::gpk::apobj<::ssiege::EventSSiege> &>	funcHandleEvent 
		= [&world](::gpk::pobj<::ssiege::EventSSiege> & _eventToProcess, ::gpk::apobj<::ssiege::EventSSiege> & worldOutputEvents) { 
			if(!_eventToProcess)
				return false;

			const ::ssiege::EventSSiege	& eventToProcess		= *_eventToProcess;
			info_printf("%s", ::gpk::get_value_namep(eventToProcess.Type));

			::gpk::error_t			result			= 0; 
			switch(eventToProcess.Type) {
			case ::ssiege::SSIEGE_EVENT_CHAR_ACTION: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::CHAR_ACTION>(eventToProcess, [&world, &worldOutputEvents, &eventToProcess](auto ev){ return ::ssg::handleCHAR_ACTION(world, ev, worldOutputEvents); })); break; }
			case ::ssiege::SSIEGE_EVENT_ADMIN_WORLD: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::ADMIN_WORLD>(eventToProcess, [&world, &worldOutputEvents, &eventToProcess](auto ev){ return ::ssg::handleADMIN_WORLD(world, ev, worldOutputEvents); })); break; }
			case ::ssiege::SSIEGE_EVENT_WORLD_EVENT: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::WORLD_EVENT>(eventToProcess, [&world, &worldOutputEvents, &eventToProcess](auto ev){ return ::ssg::handleWORLD_EVENT(world, ev, worldOutputEvents); })); break; }
			case ::ssiege::SSIEGE_EVENT_CLIENT_ASKS: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::CLIENT_ASKS>(eventToProcess, [&world, &worldOutputEvents, &eventToProcess](auto ev){ return ::ssg::handleCLIENT_ASKS(world, ev, worldOutputEvents); })); break; }
			case ::ssiege::SSIEGE_EVENT_WORLD_SETUP: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::WORLD_SETUP>(eventToProcess, [&world, &worldOutputEvents, &eventToProcess](auto ev){ return ::ssg::handleWORLD_SETUP(world, ev, worldOutputEvents); })); break; }
			case ::ssiege::SSIEGE_EVENT_WORLD_VALUE: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::WORLD_VALUE>(eventToProcess, [&world, &worldOutputEvents, &eventToProcess](auto ev){ return ::ssg::handleWORLD_VALUE(world, ev, worldOutputEvents); })); break; }
			default: 
				gpk_warning_unhandled_event(eventToProcess); 
				break;
			}
			return bool(result == 1);
		};

	inputEvents.for_each([&outputEvents, &funcHandleEvent](::gpk::pobj<::ssiege::EventSSiege> & _eventToProcess){ 
		if(funcHandleEvent(_eventToProcess, outputEvents))
			_eventToProcess.clear();
	});

	gpk_necs(world.Engine.Update(secondsElapsed));


	return 0;
}

::gpk::error_t			ssg::ssiegeGameDraw				
	( ::ssg::SSiegeGame			& world
	, ::gpk::rtbgra8d32			& backBuffer
	, const ::gpk::n3f32		& cameraPosition
	, const ::gpk::n3f32		& cameraTarget
	, const ::gpk::n3f32		& cameraUp
	, const ::gpk::minmaxf32	& nearFar
	) {
	::gpk::STimer				timer				= {};
	::gpk::SEngine				& engine			= world.Engine;

	gpk_necs(::gpk::drawScene(backBuffer.Color.View, backBuffer.DepthStencil.View, engine.Scene->RenderCache, *engine.Scene, cameraPosition, cameraTarget, cameraUp, nearFar));

	timer.Frame();
	info_printf("Render scene in %f seconds", timer.LastTimeSeconds);
	return 0;
}
