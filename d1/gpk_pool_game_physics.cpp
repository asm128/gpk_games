#include "gpk_pool_game_update.h"

static	::gpk::err_t	resolveCollision							
	( const ::gpk::n3f3_t	& initialVelocityA
	, const ::gpk::n3f3_t	& initialRotationA
	, const ::gpk::n3f3_t	& distanceDirection
	, double			& out_forceTransferRatioB	
	, ::gpk::n3f2_t		& out_finalRotationA
	, ::gpk::n3f2_t		& out_finalRotationB
	, ::gpk::n3f2_t		& out_finalVelocityA
	, ::gpk::n3f2_t		& out_finalVelocityB
	) {
	::gpk::n3f3_t						directionA			= initialVelocityA.f3();
	directionA.Normalize();
	out_forceTransferRatioB		= ::gpk::max(0.0f, (float)distanceDirection.Dot(directionA));
	if(0 >= out_forceTransferRatioB) {
		out_finalVelocityA			+= initialVelocityA.f2();
		out_finalRotationA			+= initialRotationA.f2();
		return 0;
	}
	if(1.0 <= out_forceTransferRatioB) {
		out_finalVelocityB			+= initialVelocityA.f2();
		out_finalRotationB			+= initialRotationA.f2();
		return 0;
	}

	const double					speedA				= initialVelocityA.Length();
	if(speedA) {
		bool							revert				= distanceDirection.Dot(directionA.RotateY(::gpk::math_pi_2)) >= 0;
		const ::gpk::n3f3_t				vUp					= {0, revert ? -1 : 1.0f, 0};
		::gpk::n3f3_t						finalVelocityB		= distanceDirection * speedA * out_forceTransferRatioB;
		::gpk::n3f3_t						finalVelocityA		= ::gpk::n3f3_t{finalVelocityB}.Normalize().Cross(vUp).Normalize() * speedA * (1.0f - out_forceTransferRatioB);
		out_finalVelocityA			+= finalVelocityA.f2();
		out_finalVelocityB			+= finalVelocityB.f2();
	//	const double					totalFinalSpeed		= finalVelocityA.Length() + finalVelocityB.Length();
	//	const double					totalInitialSpeed	= initialVelocityA.Length();
	//	gwarn_if((totalFinalSpeed - totalInitialSpeed) >  0.0001f, "Invalid resulting force: initial: %f, final: %f, gained: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed));
	//	gwarn_if((totalFinalSpeed - totalInitialSpeed) < -0.0001f, "Invalid resulting force: initial: %f, final: %f, lost: %f"  , totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed) * -1);
	//	info_printf("Total initial rotation: %f", (float)(initialRotationA.Length()));
	}

	const double					rotA				= initialRotationA.Length();
	if(rotA) {
		const ::gpk::n3f3_t				initialRotationANormalized	= initialRotationA * (1.0 / rotA);
		const ::gpk::n3f3_t				finalRotationB		= initialRotationANormalized * rotA * out_forceTransferRatioB;
		const ::gpk::n3f3_t				finalRotationA		= initialRotationANormalized * rotA * (1.0 - out_forceTransferRatioB);
		out_finalRotationA			+= finalRotationA.f2();
		out_finalRotationB			+= finalRotationB.f2();
		//const double					finalRotA			= finalRotationB.Length();
		//const double					finalRotB			= finalRotationA.Length();
		//info_printf("Total final rotation: %f", (float)(finalRotA + finalRotB));
	}
	return 0;
}

// this shoulnd't exist really but it will be here until we handle ball collisions properly
static	::gpk::err_t	handleBallContact	(::d1p::SPoolGame & pool, const ::gpk::SContact & contact, ::gpk::SContactResult & contactResult) {
	::gpk::SEngine					& engine			= pool.Engine;
	const ::gpk::SEntity		& entityA			= engine.Entities[contact.EntityA]; 
	const ::gpk::SEntity		& entityB			= engine.Entities[contact.EntityB]; 

	contactResult.DistanceDirection	= contact.CenterDistance;
	double							distanceLength		= contact.DistanceLength;
	contactResult.DistanceDirection.Normalize();

	// Separate balls
	::gpk::n3f2_t						& positionA			= engine.Integrator.Centers[entityA.RigidBody].Position;
	::gpk::n3f2_t						& positionB			= engine.Integrator.Centers[entityB.RigidBody].Position;
	positionA					+= contactResult.DistanceDirection * ::gpk::max(pool.MatchState.Board.BallRadius * 2 - distanceLength, 0.0) * -.51f;
	positionB					+= contactResult.DistanceDirection * ::gpk::max(pool.MatchState.Board.BallRadius * 2 - distanceLength, 0.0) * .51f;


	// Calculate force transfer
	::gpk::n3f2_t						& velocityA			= engine.Integrator.Forces[entityA.RigidBody].Velocity;
	::gpk::n3f2_t						& velocityB			= engine.Integrator.Forces[entityB.RigidBody].Velocity;
	::gpk::n3f2_t						& rotationA			= engine.Integrator.Forces[entityA.RigidBody].Rotation;
	::gpk::n3f2_t						& rotationB			= engine.Integrator.Forces[entityB.RigidBody].Rotation;

	contactResult.InitialVelocityA	= velocityA;
	contactResult.InitialVelocityB	= velocityB;
	contactResult.InitialRotationA	= rotationA;
	contactResult.InitialRotationB	= rotationB;
	velocityB					= {};
	velocityA					= {};
	rotationB					= {};
	rotationA					= {};
	if(contactResult.InitialVelocityA.LengthSquared() || contactResult.InitialRotationA.LengthSquared()) {
		engine.Integrator.SetActive(entityB.RigidBody, true);
		::gpk::n3f2_t						lvelocityB					= {};
		::gpk::n3f2_t						lvelocityA					= {};
		::resolveCollision(contactResult.InitialVelocityA.f3(), contactResult.InitialRotationA.f3(), contactResult.DistanceDirection.f3(), contactResult.ForceTransferRatioB, rotationA, rotationB, lvelocityA, lvelocityB);
		velocityB					+= lvelocityB;
		velocityA					+= lvelocityA;
	}

	if(contactResult.InitialVelocityB.LengthSquared() || contactResult.InitialRotationB.LengthSquared()) {
		engine.Integrator.SetActive(entityA.RigidBody, true);
		::gpk::n3f2_t						lvelocityB					= {};
		::gpk::n3f2_t						lvelocityA					= {};
		::resolveCollision(contactResult.InitialVelocityB.f3(), contactResult.InitialRotationB.f3(), contactResult.DistanceDirection.f3() * -1, contactResult.ForceTransferRatioA, rotationB, rotationA, lvelocityB, lvelocityA);
		velocityB					+= lvelocityB;
		velocityA					+= lvelocityA;
		//{
		//	const double					initialSpeedA				= contactResult.InitialVelocityA.Length();
		//	const double					initialSpeedB				= contactResult.InitialVelocityB.Length();
		//	const double					finalSpeedA					= velocityA.Length();
		//	const double					finalSpeedB					= velocityB.Length();
		//	const double					totalInitialSpeed			= initialSpeedA + initialSpeedB;
		//	const double					totalFinalSpeed				= finalSpeedA + finalSpeedB;
		//	if ((totalFinalSpeed - totalInitialSpeed) / totalInitialSpeed > 0.01f) {
		//		warning_printf("Invalid resulting force: initial: %f, final: %f, gained: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed));
		//	}
		//	if ((totalFinalSpeed - totalInitialSpeed) / totalInitialSpeed < -0.01f) {
		//		warning_printf("Invalid resulting force: initial: %f, final: %f, lost: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed) * -1);
		//	}
		//}
	}
	contactResult.FinalVelocityA	= (velocityA *= (pool.MatchState.Physics.Damping.Collision * (1.0f / 255.0f)));
	contactResult.FinalVelocityB	= (velocityB *= (pool.MatchState.Physics.Damping.Collision * (1.0f / 255.0f)));
	contactResult.FinalRotationA	= (rotationA *= (pool.MatchState.Physics.Damping.Collision * (1.0f / 255.0f)));
	contactResult.FinalRotationB	= (rotationB *= (pool.MatchState.Physics.Damping.Collision * (1.0f / 255.0f)));

	return 0;
}

static	::gpk::err_t	handlePockets	
	( ::d1p::SPoolGame					& pool
	, u0_t 							iBall
	, ::gpk::SBodyFlags					& flags	
	, ::gpk::SBodyForces				& forces
	, const ::gpk::n3f2_t					& positionA
	, ::gpk::apobj<::d1p::SEventPool>	& outputEvents
	//, double					secondsElapsed
	) {
	u0_t 							inPocket				= 0;
	::gpk::SEngine					& engine				= pool.Engine;
	const ::d1p::SPoolBoard			& board					= pool.MatchState.Board;
	const float						ballRadius				= board.BallRadius;
	const float						pocketRadius			= board.Table.PocketRadius;
	const float						maxDistance				= pocketRadius + ballRadius;
	const float						maxDistanceSquared		= maxDistance * maxDistance;
	::gpk::n3f2_t						positionBall			= positionA;
	positionBall.y				= 0;
	for(u0_t  iPocket = 0; iPocket < 6; ++iPocket) {
		const ::gpk::SEntity		& entityPocket			= engine.Entities[pool.Entities.Pockets[iPocket]];
		::gpk::n3f2_t					pocketPosition			= engine.Scene->RenderNodes.Transforms[entityPocket.RenderNode].Model.GetTranslation();
		pocketPosition.y			= 0;

		::gpk::n3f2_t						distanceFromPocket		= positionA - pocketPosition;
		if(distanceFromPocket.LengthSquared() > maxDistanceSquared)
			continue;

		if(positionA.y < 0) {
			inPocket					= 1 + iPocket;
			forces.Acceleration			= {};
			forces.Velocity.x			= 0;
			forces.Velocity.z			= 0;
			flags.Falling				= true;

			const ::d1p::SArgsBall			eventData			= {pool.MatchState.TotalSeconds, u1_t(pool.TurnHistory.size() - 1), {iBall, iPocket}};
			{ if_fail_fe(::gpk::eventEnqueueChild(outputEvents, ::d1p::POOL_EVENT_BALL_EVENT, ::d1p::BALL_EVENT_Pocketed, eventData)); }
			pool.MatchState.SetPocketed(eventData.Event.Pocketed.Ball);
			break;
		}

		inPocket					= 1 + iPocket;
		if(false == board.Table.InPlayingSurface(positionBall.xz())) {
			float							w						= (float)(distanceFromPocket.Length() - ballRadius) / (pocketRadius - ballRadius);
			forces.Acceleration.y		= -(pool.MatchState.Physics.Gravity * .001f) * ::gpk::max(0.0f, (1.0f - w));
			flags.Falling				= true;
		}
		break;
	}
	return inPocket;
}

// this shoulnd't exist really but it will be here until we handle cushion collisions properly
static	::gpk::err_t	handleBoundaries				(::d1p::SPoolGame & pool, float ballRadius, ::gpk::n3f2_t & positionA, ::gpk::SBodyForces & forcesA, const ::d1p::SPoolTable & tableDimensions, const gpk::n2f2_t & tableHalfDimensions) {
	::gpk::n2f2_t				ballLimits						= tableHalfDimensions - ::gpk::n2f2_t{ballRadius, ballRadius};
	::gpk::n3f2_t				pocketPosition					= {};
	double						pocketRadius					= tableDimensions.PocketRadius;
	double						pocketRadiusSquared				= pocketRadius * pocketRadius;
	for(u0_t  iPocket = 0; iPocket < ::d1p::MAX_POCKETS; ++iPocket) {
		pool.GetPocketPosition(iPocket, pocketPosition);
		if((positionA - pocketPosition).LengthSquared() < pocketRadiusSquared + ballRadius * ballRadius)
			ballLimits	+= ::gpk::n2f2_t{(float)pocketRadius, (float)pocketRadius};
	}

	::gpk::n2<bool>					outOfBounds						= 
		{ (positionA.x < -ballLimits.x) || (positionA.x > ballLimits.x)
		, (positionA.z < -ballLimits.y) || (positionA.z > ballLimits.y)
		};
	if(outOfBounds.x 
	//	&& (positionA.z > -(tableHalfDimensions.y - tableDimensions.PocketRadius))
	//	&& (positionA.z <  (tableHalfDimensions.y - tableDimensions.PocketRadius))
	) {
		positionA.x					= (positionA.x < -ballLimits.x) 
			? (-ballLimits.x) - (positionA.x + ballLimits.x)
			:   ballLimits.x  - (positionA.x - ballLimits.x)
			;
		forcesA.Velocity.x			*= -1;
		forcesA.Velocity			*= pool.MatchState.Physics.Damping.Cushion * (1.0f / 255.0f);
		forcesA.Rotation.z			*= -1;
	}
	if(outOfBounds.y 
	//	&& (	positionA.x > -(tableHalfDimensions.x - tableDimensions.PocketRadius)
	//		&& (positionA.x <  (tableHalfDimensions.x - tableDimensions.PocketRadius))
	//		&& (positionA.x <  -tableDimensions.PocketRadius || positionA.x > tableDimensions.PocketRadius)
	//	)
	) {
		positionA.z					= (positionA.z	< -ballLimits.y) 
			? (-ballLimits.y) - (positionA.z + ballLimits.y)
			:   ballLimits.y  - (positionA.z - ballLimits.y)
			;
		forcesA.Velocity.z			*= -1;
		forcesA.Velocity			*= pool.MatchState.Physics.Damping.Cushion * (1.0f / 255.0f);
		forcesA.Rotation.x			*= -1;
	}
	return 0;
}

static	::gpk::err_t	handleFalling					(::d1p::SPoolGame & pool, u2_t  iRigidBody, ::gpk::n3f2_t & positionA, ::gpk::SBodyFlags & flagsA, ::gpk::SBodyForces & forcesA, ::gpk::SBodyMass & massA) {
	if(positionA.y < pool.MatchState.Board.BallRadius) { // if the ball is falling through the top of the table we make it bounce up
		positionA.y				= (positionA.y - pool.MatchState.Board.BallRadius) * -.95f + pool.MatchState.Board.BallRadius;

		forcesA.Velocity.y		*= -1.0f;
		forcesA.Velocity.y		*= pool.MatchState.Physics.Damping.Ground * (1.0f / 255.0f);
		pool.Engine.Integrator.Frames[iRigidBody].AccumulatedForce.y	= -float(pool.MatchState.Physics.Gravity * .001f * massA.GetMass());
	}
	else if(fabs(forcesA.Velocity.y) > 0.0000075 || positionA.y > pool.MatchState.Board.BallRadius) { // if the ball is falling, we continue falling
		pool.Engine.Integrator.Frames[iRigidBody].AccumulatedForce.y	= -float(pool.MatchState.Physics.Gravity * .001f * massA.GetMass());
	} 
	else { // else, no velocity, so we disable falling
		forcesA.Acceleration.y	=  0;
		forcesA.Velocity.y		=  0;
		positionA.y				= pool.MatchState.Board.BallRadius;
		flagsA.Falling			= false;
	}
	return 0;
}

static	::gpk::err_t	handlePocketsAndBoundaries		(::d1p::SPoolGame & pool, u0_t  iBall, const ::d1p::SPoolTable & tableDimensions, const gpk::n2f2_t & tableHalfDimensions, ::gpk::apobj<::d1p::SEventPool> & outputEvents) {
	::gpk::SEngine				& engine						= pool.Engine;
	const float					ballRadius						= pool.MatchState.Board.BallRadius;
	const ::gpk::SEntity		& entityA						= engine.Entities[pool.Entities.Balls[iBall]]; 
	::gpk::n3f2_t				& positionA						= engine.Integrator.Centers		[entityA.RigidBody].Position;
	::gpk::SBodyForces			& forcesA						= engine.Integrator.Forces		[entityA.RigidBody];
	::gpk::SBodyFlags			& flagsA						= engine.Integrator.Flags		[entityA.RigidBody];
	::gpk::SBodyMass			& massA							= engine.Integrator.Masses		[entityA.RigidBody];
	
	u0_t 						inPocket						= (u0_t )::handlePockets(pool, (u0_t )iBall, flagsA, forcesA, positionA, outputEvents);
	if(inPocket)
		return inPocket;

	{ if_fail_fe(::handleBoundaries(pool, ballRadius, positionA, forcesA, tableDimensions, tableHalfDimensions)); }

	//if(flagsA.Falling) // skip handling falling when not needed to avoid loading this function for no reason
		{ if_fail_fe(::handleFalling(pool, entityA.RigidBody, positionA, flagsA, forcesA, massA)); }

	return 0;
}

::gpk::err_t			d1p::poolGamePhysicsUpdate		(::d1p::SPoolGame & pool, ::gpk::apobj<::d1p::SEventPool> & outputEvents, double secondsElapsed) {
	::gpk::apod<::gpk::SContact>	lastFrameContactsBatchBall;
	::gpk::apod<::gpk::SContact>	lastFrameContactsBatchCushion;
	
	::gpk::SEngine					& engine					= pool.Engine;
	double							step						= .001f;

	const ::d1p::SPoolTable			& tableDimensions			= pool.MatchState.Board.Table;
	const gpk::n2f2_t				tableHalfDimensions			= tableDimensions.Slate * .5f;
	while(secondsElapsed > 0) { 
		double							secondsThisStep				= ::gpk::min(step, secondsElapsed);
		secondsElapsed				-= secondsThisStep;
		engine.Update(secondsThisStep);

		lastFrameContactsBatchBall.clear();
		lastFrameContactsBatchCushion.clear();
		{ if_fail_fe(::gpk::collisionDetect(engine, pool.MatchState.TotalSeconds += secondsThisStep, lastFrameContactsBatchBall)); }

		for(u2_t  iContact = 0; iContact < lastFrameContactsBatchBall.size(); ++iContact) {
			::gpk::SContact						& ballContact					= lastFrameContactsBatchBall[iContact];
			const u0_t 						iBallA						= pool.EntityToBall((u1_t)ballContact.EntityA);
			//if(pool.MatchState.IsPocketed(iBallA)) 
			//	continue;
			const u0_t 						iBallB						= pool.EntityToBall((u1_t)ballContact.EntityB);
			//if(pool.MatchState.IsPocketed(iBallB))
			//	continue;
			{ if_fail_fe(::handleBallContact(pool, ballContact, ballContact.Result)); }
			if(ballContact.Result.ForceTransferRatioB > 0 || ballContact.Result.ForceTransferRatioA > 0) {
				const ::d1p::SArgsBall				eventContact					= 
					{ pool.MatchState.TotalSeconds
					, u1_t(pool.TurnHistory.size() - 1)
					, {iBallA, iBallB}
					};
				{ if_fail_fe(::gpk::eventEnqueueChild(outputEvents, ::d1p::POOL_EVENT_BALL_EVENT, ::d1p::BALL_EVENT_ContactBall, eventContact)); }
			}

		}

		{ if_fail_fe(pool.LastFrameContactsBall.append(lastFrameContactsBatchBall)); }
		
		for(u0_t  iBall = 0; iBall < pool.MatchState.CountBalls; ++iBall) {
			if(false == pool.MatchState.IsPocketed(iBall))
				{ if_fail_fe(::handlePocketsAndBoundaries(pool, iBall, tableDimensions, tableHalfDimensions, outputEvents)); }
		}

		//for(u2_t  iBallA = 0; iBallA < pool.StateStart.CountBalls; ++iBallA) {
		//	::gpk::SBodyForces				& forces						= engine.Integrator.Forces		[engine.Entities[pool.StateStart.Ball[iBallA].Entity].RigidBody];
		//	::gpk::SBodyFrame			& bodyAccum						= engine.Integrator.Frames	[engine.Entities[pool.StateStart.Ball[iBallA].Entity].RigidBody];
		//	if(::gpk::n3f2_t{forces.Velocity}.Normalize().Dot(::gpk::n3f2_t{forces.Rotation.z, 0, -forces.Rotation.x}.Normalize()) < 0) {
		//		bodyAccum.AccumulatedForce += ::gpk::n3f2_t{forces.Rotation.z, 0, -forces.Rotation.x} * secondsElapsed;
		//	}
		//	if(forces.Rotation.y)
		//		forces.Velocity.RotateY(forces.Rotation.y * secondsElapsed * .05);
		//}
	} 
	return 0;
}

