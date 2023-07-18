#include "ssiege_game_handle.h"
#include "ssiege_game.h"
#include "gpk_timer.h"

static	::gpk::error_t	shipCoreCreate			(::ssg::SSiegeGame & world) { 
	{	// Core
		::gpk::SParamsSphere		sphere					= {};
		sphere.Radius			*= .0005f;
		world.ShipCore			= world.Engine.CreateSphere(sphere);
	}
	{	// Ring
		::gpk::SParamsRing			ring					= {};
		ring.Height				= .0005f;
		ring.RadiusYMin.Max		*= .005f;
		ring.RadiusYMin.Min		*= .005f;
		ring.RadiusYMax.Max		*= .005f;
		ring.RadiusYMax.Min		*= .005f;
		world.ShipRing			= world.Engine.CreateRing(ring);
		world.Engine.SetRotation(world.ShipRing, ::gpk::n3f32{0, float(::gpk::math_pi * .25),});
	}

	world.Engine.SetPosition	(world.ShipCore, (world.Camera.Position * .95) + ::gpk::n3f32{-.005f});
	world.Engine.SetOrientation	(world.ShipCore, ::gpk::quatf32{}.MakeFromEuler(0, 0, ::gpk::math_pi_2));

	world.Engine.Entities.SetParent(world.ShipRing, world.ShipCore);

	return 0; 
}

::gpk::error_t			ssg::ssiegeGameSetup	(::ssg::SSiegeGame & world) {
	gpk_necs(::gpk::planetarySystemSetup(world.SolarSystem, world.Engine, "gpk_solar_system.json"));
	gpk_necs(::shipCoreCreate(world));
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
