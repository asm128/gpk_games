#include "ssiege_world_handle.h"
#include "gpk_timer.h"

::gpk::error_t			ssiege::worldViewUpdate	(::ssiege::SWorldView & world, ::gpk::vpobj<::ssiege::EventCampp> inputEvents, ::gpk::apobj<::ssiege::EventCampp> & outputEvents, double secondsElapsed) {
	::gpk::FBool<::gpk::pobj<::ssiege::EventCampp> &, ::gpk::apobj<::ssiege::EventCampp> &>	funcHandleEvent 
		= [&world](::gpk::pobj<::ssiege::EventCampp> & _eventToProcess, ::gpk::apobj<::ssiege::EventCampp> & worldOutputEvents) { 
			if(!_eventToProcess)
				return false;

			const ::ssiege::EventCampp	& eventToProcess		= *_eventToProcess;
			info_printf("%s", ::gpk::get_value_namep(eventToProcess.Type));

			::gpk::error_t			result			= 0; 
			switch(eventToProcess.Type) {
			case ::ssiege::CAMPP_EVENT_CHAR_ACTION: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::CHAR_ACTION>(eventToProcess, [&world, &worldOutputEvents, &eventToProcess](auto ev){ return ::ssiege::handleCHAR_ACTION(world, ev, worldOutputEvents); })); break; }
			case ::ssiege::CAMPP_EVENT_WORLD_ADMIN: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::WORLD_ADMIN>(eventToProcess, [&world, &worldOutputEvents, &eventToProcess](auto ev){ return ::ssiege::handleWORLD_ADMIN(world, ev, worldOutputEvents); })); break; }
			case ::ssiege::CAMPP_EVENT_WORLD_EVENT: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::WORLD_EVENT>(eventToProcess, [&world, &worldOutputEvents, &eventToProcess](auto ev){ return ::ssiege::handleWORLD_EVENT(world, ev, worldOutputEvents); })); break; }
			case ::ssiege::CAMPP_EVENT_CLIENT_ASKS: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::CLIENT_ASKS>(eventToProcess, [&world, &worldOutputEvents, &eventToProcess](auto ev){ return ::ssiege::handleCLIENT_ASKS(world, ev, worldOutputEvents); })); break; }
			case ::ssiege::CAMPP_EVENT_WORLD_SETUP: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::WORLD_SETUP>(eventToProcess, [&world, &worldOutputEvents, &eventToProcess](auto ev){ return ::ssiege::handleWORLD_SETUP(world, ev, worldOutputEvents); })); break; }
			case ::ssiege::CAMPP_EVENT_WORLD_VALUE: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::WORLD_VALUE>(eventToProcess, [&world, &worldOutputEvents, &eventToProcess](auto ev){ return ::ssiege::handleWORLD_VALUE(world, ev, worldOutputEvents); })); break; }
			default: 
				gpk_warning_unhandled_event(eventToProcess); 
				break;
			}
			return bool(result == 1);
		};

	inputEvents.for_each([&outputEvents, &funcHandleEvent](::gpk::pobj<::ssiege::EventCampp> & _eventToProcess){ 
		if(funcHandleEvent(_eventToProcess, outputEvents))
			_eventToProcess.clear();
	});

	gpk_necs(world.Engine.Update(secondsElapsed));


	return 0;
}

::gpk::error_t			ssiege::worldViewDraw				
	( ::ssiege::SWorldView		& world
	, ::gpk::rtbgra8d32			& backBuffer
	, const ::gpk::n3f32		& cameraPosition
	, const ::gpk::n3f32		& cameraTarget
	, const ::gpk::n3f32		& cameraUp
	, const ::gpk::minmaxf32	& nearFar
	) {
	::gpk::STimer				timer				= {};
	::gpk::SEngine				& engine			= world.Engine;

	const ::gpk::n2u16			offscreenMetrics	= backBuffer.Color.View.metrics16();

	::gpk::n3f32				cameraFront			= (cameraTarget - cameraPosition).Normalized();

	::gpk::SEngineSceneConstants	constants		= {};
	constants.CameraPosition	= cameraPosition;
	constants.CameraFront		= cameraFront;
	constants.LightPosition		= {0, 10, 0};
	constants.LightDirection	= {0, -1, 0};

	constants.View.LookAt(cameraPosition, cameraTarget, cameraUp);
	constants.Perspective.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar.Min, nearFar.Max);
	constants.Screen.ViewportLH(offscreenMetrics);
	constants.VP			= constants.View * constants.Perspective;
	constants.VPS			= constants.VP * constants.Screen;

	gpk_necs(::gpk::drawScene(backBuffer.Color.View, backBuffer.DepthStencil.View, engine.Scene->RenderCache, *engine.Scene, constants));

	timer.Frame();
	info_printf("Render scene in %f seconds", timer.LastTimeSeconds);
	return 0;
}
