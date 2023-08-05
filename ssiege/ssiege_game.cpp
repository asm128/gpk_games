#include "ssiege_game_handle.h"
#include "ssiege_game.h"
#include "gpk_timer.h"

stacxpr	uint32_t		DEFAULT_ARM_COUNT		= 6;

stacxpr	float			SCALE_SHIP				= .001f;	// 

stacxpr	float			SHIP_CORE_RADIUS		= SCALE_SHIP * .5f;	// the ship core diameter represents the size unit
// 
stacxpr	float			SHIP_RING_HEIGHT		= SHIP_CORE_RADIUS * 2;							// 
stacxpr	float			SHIP_RING_RADIUS_MAX	= SHIP_CORE_RADIUS * 10;						// 
stacxpr	gpk::minmaxf32	SHIP_RING_RADIUS		= {SHIP_RING_RADIUS_MAX - SHIP_CORE_RADIUS, SHIP_RING_RADIUS_MAX};	// 
stacxpr	float			SHIP_RING_BORDER		= SHIP_RING_RADIUS.Length();	// 
stacxpr	float			SHIP_RING_THICKNESS		= ::gpk::max(SHIP_RING_HEIGHT, SHIP_RING_BORDER);	// 
stacxpr	float			SHIP_CRANE_BOX			= SHIP_RING_THICKNESS + SHIP_RING_THICKNESS * .5f;	// 
stacxpr	float			SHIP_CRANE_BOX_HALF		= SHIP_CRANE_BOX * .5f;	// 
stacxpr	float			SHIP_CRANE_IRON			= SHIP_CRANE_BOX_HALF * .125f;		// 
stacxpr	float			SHIP_CRANE_IRON_HALF	= SHIP_CRANE_IRON * .5f;		// 
stacxpr	float			SHIP_CRANE_WHEEL_RADIUS	= SHIP_CRANE_BOX_HALF * .5f;		// 
stacxpr	float			SHIP_CRANE_WHEEL_HEIGHT	= SHIP_CRANE_BOX_HALF * .5f;		// 
stacxpr	float			SHIP_ARM_RADIUS			= SHIP_CORE_RADIUS * .125f;	// 
stacxpr	float			SHIP_ARM_LENGTH			= SHIP_RING_RADIUS_MAX;	// 

static	::gpk::error_t	createShipCore		(::gpk::SEngine & engine) {
	::gpk::SParamsSphere		sphere				= {};
	sphere.Radius			= ::SHIP_CORE_RADIUS;
	::gpk::eid_t				iShip; 
	gpk_necs(iShip = engine.CreateSphere(sphere, "Ship Core"));
	engine.SetRotation(iShip, ::gpk::n3f32{0, 0, float(::gpk::math_pi_2 * .15)});
	return iShip;
}

static	::gpk::error_t	createShipRing		(::gpk::SEngine & engine) {
	::gpk::eid_t				iShipRing; 
	{
		::gpk::SParamsTube			ring				= {};
		ring.Origin.y			= ::SHIP_RING_HEIGHT * .5f;
		ring.Height				= ::SHIP_RING_HEIGHT;
		ring.RadiusYMin			= ::SHIP_RING_RADIUS;
		ring.RadiusYMax			= ::SHIP_RING_RADIUS;
		ring.CellCount.x		= 32;

		gpk_necs(iShipRing = engine.CreateTube(ring, "Ship Ring"));
	}
	{
		::gpk::SParamsCircle		energy				= {};
		energy.Radius			= ::SHIP_RING_RADIUS.Min;

		::gpk::eid_t				iRingEnergy; 
		gpk_necs(iRingEnergy = engine.CreateDisc(energy, "Ship Ring Energy"));
		engine.SetColorDiffuse(iRingEnergy, {.2f, .5f, 1, .5f});
		engine.Entities.SetParent(iRingEnergy, iShipRing);
	}
	engine.SetRotation(iShipRing, ::gpk::n3f32{0, float(::gpk::math_pi * .25),});
	return iShipRing; 
}

static	::gpk::error_t	createShipCraneBox		(::gpk::SEngine & engine) {
	::gpk::eid_t				iArmRoot;
	{	// construct the grip around the ship ring
		::gpk::SParamsBox			base					= {};
		base.HalfSizes.From(::SHIP_CRANE_BOX * .5f);
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
		iron.HalfSizes.SetAxis  (iAxis, ::SHIP_CRANE_BOX_HALF);
		iron.HalfSizes.SetOthers(iAxis, ::SHIP_CRANE_IRON_HALF);
		iron.Origin				= iron.HalfSizes;
		gpk_necs(iArmBaseBorder = engine.CreateBox(iron, "Crane Base Iron"));
	
		::gpk::n3f32				position				= {};
		position.SetAxis((iAxis + 1) % 3, (::SHIP_CRANE_BOX_HALF - SHIP_CRANE_IRON_HALF) * (iRow ? 1 : -1));
		position.SetAxis((iAxis + 2) % 3, (::SHIP_CRANE_BOX_HALF - SHIP_CRANE_IRON_HALF) * (iCol ? 1 : -1));
		engine.SetPosition(iArmBaseBorder, position);
		gpk_necs(engine.Entities.SetParent(iArmBaseBorder, iArmRoot));
	}
	return iArmRoot;
}

static	::gpk::error_t	createShipCraneBar	(::gpk::SEngine & engine, ::gpk::vcs entityName = "Crane Bar") {
	// construct the arm itself
	::gpk::SParamsCylinderWall	bar					= {};
	bar.Radius				= {::SHIP_ARM_RADIUS, ::SHIP_ARM_RADIUS};
	bar.Height				= ::SHIP_ARM_LENGTH;
	::gpk::eid_t				iBar;
	gpk_necall(iBar = engine.CreateCylinder(bar, entityName), "entityName: '%s'", ::gpk::toString(entityName).begin());
	engine.SetOrientation(iBar, ::gpk::quatf32{1, 0, 0, 1}.Normalize());
		;
	engine.SetPosition(iBar, {0, ::SHIP_CRANE_WHEEL_RADIUS * .5f, });
	return iBar;
}

static	::gpk::error_t	createShipCraneWheel(::gpk::SEngine & engine, ::gpk::vcs entityName = "Crane Wheel") {
	// construct the arm itself
	::gpk::SParamsCylinderWall	wheel				= {};
	wheel.Radius			= ::gpk::dim2<::gpk::minmaxf32>(::SHIP_CRANE_WHEEL_RADIUS);
	wheel.Height			= ::SHIP_CRANE_WHEEL_HEIGHT;
	::gpk::eid_t				iWheel;
	gpk_necall(iWheel = engine.CreateCylinder(wheel, entityName), "entityName: '%s'", ::gpk::toString(entityName).begin());
	engine.SetOrientation(iWheel, ::gpk::quatf32{1, 0, 0, 1}.Normalize());
	return iWheel;
}

static	::gpk::error_t	createShipCrane		(::gpk::SEngine & engine) {
	::gpk::eid_t				iCrane; 
	{
		::gpk::SParamsSphere		sphere				= {};
		sphere.Radius			= ::SHIP_CORE_RADIUS;
		gpk_necs(iCrane = engine.CreateSphere(sphere, "Ship Crane"));
	}
	engine.SetColorDiffuse(iCrane, {.25f, 1, 1, .5f});
	{
		::gpk::eid_t				iBox;
		gpk_necs(iBox = ::createShipCraneBox(engine));	
		gpk_necs(engine.Entities.SetParent(iBox, iCrane));
	}
	stacxpr	float				OFFSET_CORNER		= ::SHIP_CRANE_BOX_HALF + ::SHIP_CRANE_IRON_HALF;
	for(uint32_t iArm = 0; iArm < 2; ++iArm) {
		const bool					isLeft				= (1 + iArm) % 2;
		if(isLeft)
			continue;

		::gpk::eid_t				iWheel;
		{	// Create wheel
			gpk_necs(iWheel = ::createShipCraneWheel(engine, isLeft ? ::gpk::vcs{"Ship Crane Wheel Left?"} : ::gpk::vcs{"Ship Crane Wheel Right?"}));
			::gpk::n3f32				barPosition			= {OFFSET_CORNER, -::SHIP_CRANE_BOX_HALF, OFFSET_CORNER * (iArm ? 1 : -1)};
			engine.SetPosition(iWheel, barPosition);
			engine.SetRotation(iWheel, ::gpk::n3f32{0, 0, float(::gpk::math_pi_2 * .15)});
			gpk_necs(engine.Entities.SetParent(iWheel, iCrane));
		}
		::gpk::eid_t				iBar;	// Create bar
		if(isLeft) 
			iBar				= ::gpk::EID_INVALID;
		else {
			gpk_necs(iBar = ::createShipCraneBar(engine, iArm ? ::gpk::vcs{"Ship Crane Bar Right?"} : ::gpk::vcs{"Ship Crane Bar Left?"}));
			gpk_necs(engine.Entities.SetParent(iBar, iWheel));
		}
	}
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
	const int					armCount			= ::DEFAULT_ARM_COUNT;
	for(uint32_t iArm = 0; iArm < armCount; ++iArm) {
		::gpk::eid_t				iArmRoot			= ::createShipCrane(world.Engine);
		::gpk::n3f32				position			= {::SHIP_RING_RADIUS.Middle()};
		position.RotateY(::gpk::math_2pi * (1.0f / armCount) * iArm);
		world.Engine.SetOrientation(iArmRoot, ::gpk::quatf32{}.MakeFromEuler(0, -(float)::gpk::math_2pi * (1.0f / armCount) * iArm, 0).Normalize());
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

::gpk::error_t			ssg::ssiegeGameUpdate	(::ssg::SSiegeGame & world, ::gpk::vpobj<::ssg::EventSSiege> inputEvents, ::gpk::apobj<::ssg::EventSSiege> & outputEvents, double secondsElapsed) {
	::gpk::FBool<::gpk::pobj<::ssg::EventSSiege> &, ::gpk::apobj<::ssg::EventSSiege> &>	funcHandleEvent 
		= [&world](::gpk::pobj<::ssg::EventSSiege> & _eventToProcess, ::gpk::apobj<::ssg::EventSSiege> & worldOutputEvents) { 
			if(!_eventToProcess)
				return false;

			const ::ssg::EventSSiege	& eventToProcess		= *_eventToProcess;
			info_printf("%s", ::gpk::get_value_namep(eventToProcess.Type));

			::gpk::error_t			result			= 0; 
			switch(eventToProcess.Type) {
			CASE_SSG_EVENT(result, world, WORLD_EVENT, worldOutputEvents); 
			CASE_SSG_EVENT(result, world, WORLD_ADMIN, worldOutputEvents); 
			CASE_SSG_EVENT(result, world, ACT_SAILING, worldOutputEvents); 
			CASE_SSG_EVENT(result, world, ACTION_CHAR, worldOutputEvents); 
			CASE_SSG_EVENT(result, world, ACT_ENGINES, worldOutputEvents); 
			CASE_SSG_EVENT(result, world, ACT_AIRSHIP, worldOutputEvents); 
			CASE_SSG_EVENT(result, world, ACT_WHEELED, worldOutputEvents); 
			CASE_SSG_EVENT(result, world, CLIENT_ASKS, worldOutputEvents); 
			CASE_SSG_EVENT(result, world, WORLD_SETUP, worldOutputEvents); 
			CASE_SSG_EVENT(result, world, WORLD_VALUE, worldOutputEvents); 
			default: 
				gpk_warning_unhandled_event(eventToProcess); 
				break;
			}
			return bool(result == 1);
		};

	inputEvents.for_each([&outputEvents, &funcHandleEvent](::gpk::pobj<::ssg::EventSSiege> & _eventToProcess){ 
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
