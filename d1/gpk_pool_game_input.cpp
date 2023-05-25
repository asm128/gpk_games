#include "gpk_pool_game_update.h"

static	::gpk::error_t		shootCueBall			(::d1::SPoolGame & pool, const ::d1::SStickControl & activeStick, uint16_t activeStickEntity)		{
	::gpk::SEngine					& engine				= pool.Engine;
	::d1::SPoolTurnInfo				& activeTurn			= pool.ActiveTurn();
	activeTurn.Time.Shoot		= ::gpk::timeCurrentInMs();
	activeTurn.StickControl		= activeStick;
	pool.GetStickOrientation(activeTurn.Orientation);
	::gpk::n3f						velocity				= {activeStick.Velocity, 0, 0}; 
	velocity					= activeTurn.Orientation.RotateVector(velocity);
	engine.SetVelocity(pool.Entities.Balls[0], velocity);
	if(velocity.y)
		engine.Integrator.Flags[engine.Entities[pool.Entities.Balls[0]].RigidBody].Falling = true;

	engine.SetHidden(activeStickEntity, true);
	pool.MatchState.TurnState.NotInHand	= 1;
	return 1;
}

// This switches the turn state so the aiming mode gets disabled 
static	::gpk::error_t		processEventShoot		(::d1::SPoolGame & pool, ::gpk::apobj<::d1::SEventPool> & /*outputEvents*/)											{ 
	const d1::SStickControl			& activeStick			= pool.ActiveStick();
	if (activeStick.Velocity <= 0) 
		return 0;

	uint16_t						activeStickEntity		= pool.ActiveStickEntity();
	gpk_necs(::shootCueBall(pool, activeStick, activeStickEntity));
	return 1;
}

static	::gpk::error_t		processEventForce		(::d1::SPoolGame & pool, const ::d1::SArgsStickForce & stickEvent, ::gpk::apobj<::d1::SEventPool> & /*outputEvents*/)	{ 
	info_printf("%s", ::gpk::get_value_namep(stickEvent.Direction));
	float							directionMultiplier		= 1;
	switch(stickEvent.Direction) { 
	default						: gpk_warning_unhandled_value(stickEvent.Direction); break; 
	case ::gpk::AXIS_ORIGIN		: 
		pool.SetStickVelocity(stickEvent.Value); 
		break;
	case ::gpk::AXIS_X_NEGATIVE	: 
		directionMultiplier			= -1;
	case ::gpk::AXIS_X_POSITIVE	: 
		pool.AddStickVelocity(stickEvent.Value * directionMultiplier); 
		break; 
	}
	return 0;
}

static	::gpk::error_t		processEventMove		(::d1::SPoolGame & /*pool*/, const ::d1::SArgsStickMove & stickEvent, ::gpk::apobj<::d1::SEventPool> & /*outputEvents*/)	{ 
	info_printf("%s", ::gpk::get_value_namep(stickEvent.Direction));
	float							directionMultiplier		= 1;
	switch(stickEvent.Direction) { 
	default						: gpk_warning_unhandled_value(stickEvent.Direction); break; 
	case ::gpk::AXIS_ORIGIN		: break;
	case ::gpk::AXIS_X_NEGATIVE	: 
		directionMultiplier			= -1;
	case ::gpk::AXIS_X_POSITIVE	: 
		break;
	case ::gpk::AXIS_Y_NEGATIVE	: 
		directionMultiplier			= -1;
	case ::gpk::AXIS_Y_POSITIVE	: 
		break;
	}
	return 0;
}

static	::gpk::error_t		processEventTurn		(::d1::SPoolGame & pool, const ::d1::SArgsStickTurn & stickEvent, ::gpk::apobj<::d1::SEventPool> & /*outputEvents*/)	{ 
	info_printf("%s", ::gpk::get_value_namep(stickEvent.Direction));
	float							directionMultiplier		= 1;
	switch(stickEvent.Direction) { 
	default						: gpk_warning_unhandled_value(stickEvent.Direction); break; 
	//case ::gpk::AXIS_ORIGIN		: pool.ActiveStick().Angle = stickEvent.Value; break;
	case ::gpk::AXIS_X_NEGATIVE	: 
		directionMultiplier			= -1;
	case ::gpk::AXIS_X_POSITIVE	: {
		pool.ActiveStick().Angle	+= stickEvent.Value * directionMultiplier;

		const int32_t					activeStickEntity		= pool.ActiveStickEntity();
		const uint32_t					stickRigidBodyId		= pool.Engine.Entities[activeStickEntity].RigidBody;
		::gpk::SBodyCenter				& stickRigidBody		= pool.Engine.Integrator.Centers[stickRigidBodyId];
		stickRigidBody.Orientation.AddScaled({0, stickEvent.Value * directionMultiplier, 0}, 1).Normalize();
		pool.Engine.Integrator.Flags[stickRigidBodyId].UpdatedTransform = false;
	}
		break;
	case ::gpk::AXIS_Y_NEGATIVE	: 
		directionMultiplier			= -1;
	case ::gpk::AXIS_Y_POSITIVE	: {
		constexpr float					angleLimitY				= float(::gpk::math_pi_2 * .99);
		::d1::SStickControl				& activeStick			= pool.ActiveStick();
		float							angleY					= stickEvent.Value * directionMultiplier;
#if defined(GPK_DEBUG_ENABLED)
		if(activeStick.Pitch + angleY < -angleLimitY)
			angleY						= (-angleLimitY) - activeStick.Pitch;
#else
		if(activeStick.Pitch + angleY < 0)
			angleY						= -activeStick.Pitch;
#endif
		if((activeStick.Pitch + angleY) > angleLimitY)
			angleY						= angleLimitY - activeStick.Pitch;

		activeStick.Pitch			+= angleY;

		const int32_t					activeStickEntity		= pool.ActiveStickEntity();
		const uint32_t					stickRigidBodyId		= pool.Engine.Entities[activeStickEntity].RigidBody;
		::gpk::SBodyCenter				& stickRigidBody		= pool.Engine.Integrator.Centers[stickRigidBodyId];
		(stickRigidBody.Orientation *= ::gpk::quatf{}.CreateFromAxisAngle({0, 0, 1}, -angleY).Normalize()).Normalize();
		pool.Engine.Integrator.Flags[stickRigidBodyId].UpdatedTransform = false;
	}
		break;
	}
	return 0;
}

static	::gpk::error_t		processEventBall	(::d1::SPoolGame & pool, const ::d1::SArgsStickMove & stickEvent, ::gpk::apobj<::d1::SEventPool> & /*outputEvents*/)	{ 
	info_printf("%s", ::gpk::get_value_namep(stickEvent.Direction));
	float							directionMultiplier		= 1;
	rwws_if(pool.MatchState.TurnState.NotInHand);

	switch(stickEvent.Direction) { 
	default						: gpk_warning_unhandled_value(stickEvent.Direction); break; 
	case ::gpk::AXIS_ORIGIN		: break;
	case ::gpk::AXIS_X_NEGATIVE	: 
		directionMultiplier			= -1;
	case ::gpk::AXIS_X_POSITIVE	: 
		break;
	case ::gpk::AXIS_Y_NEGATIVE	: 
		directionMultiplier			= -1;
	case ::gpk::AXIS_Y_POSITIVE	: 
		break;
	}
	return 0;
}

static	::gpk::error_t		processInputEvent		(::d1::SPoolGame & pool, const ::d1::SEventPlayer & stickEvent, ::gpk::apobj<::d1::SEventPool> & outputEvents)		{ 
	if(pool.MatchState.TurnState.Active)
		return 0;

	switch(stickEvent.Type) {
	case ::d1::PLAYER_INPUT_Ball	: return ::processEventBall (pool, *(const ::d1::SArgsStickMove *)stickEvent.Data.begin(), outputEvents);
	case ::d1::PLAYER_INPUT_Move	: return ::processEventMove (pool, *(const ::d1::SArgsStickMove *)stickEvent.Data.begin(), outputEvents);
	case ::d1::PLAYER_INPUT_Turn	: return ::processEventTurn (pool, *(const ::d1::SArgsStickTurn *)stickEvent.Data.begin(), outputEvents);
	case ::d1::PLAYER_INPUT_Force	: return ::processEventForce(pool, *(const ::d1::SArgsStickForce*)stickEvent.Data.begin(), outputEvents);
	case ::d1::PLAYER_INPUT_Shoot	: return ::processEventShoot(pool, outputEvents); // once the shoot has been initiated, the stick controls deactivate until the play ends. 
	default: 
		gpk_warning_unhandled_event(stickEvent); 
		break; 
	}
	return 0;
}

::gpk::error_t				d1::processInputEvents		(::d1::SPoolGame & pool, ::gpk::view<const ::d1::SEventPlayer> inputEvents, ::gpk::apobj<::d1::SEventPool> & outputEvents)		{ 
	for(uint32_t iEvent = 0; iEvent < inputEvents.size(); ++iEvent) 
		gpk_necs(::processInputEvent(pool, inputEvents[iEvent], outputEvents));

	return 0;
}
