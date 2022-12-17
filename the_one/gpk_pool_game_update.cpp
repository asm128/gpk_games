#include "gpk_pool_game.h"


static	::gpk::error_t		resolveCollision							
	( const ::gpk::SCoord3<double>	& initialVelocityA
	, const ::gpk::SCoord3<double>	& initialRotationA
	, const ::gpk::SCoord3<double>	& distanceDirection
	, double						& out_forceTransferRatioB	
	, ::gpk::SCoord3<float>			& out_finalRotationA
	, ::gpk::SCoord3<float>			& out_finalRotationB
	, ::gpk::SCoord3<float>			& out_finalVelocityA
	, ::gpk::SCoord3<float>			& out_finalVelocityB
	) {
	::gpk::SCoord3<double>			directionA			= initialVelocityA.Cast<double>();
	directionA.Normalize();
	out_forceTransferRatioB		= ::gpk::max(0.0f, (float)distanceDirection.Dot(directionA));
	if(0 >= out_forceTransferRatioB) {
		out_finalVelocityA			+= initialVelocityA.Cast<float>();
		out_finalRotationA			+= initialRotationA.Cast<float>();
		return 0;
	}
	if(1.0 <= out_forceTransferRatioB) {
		out_finalVelocityB			+= initialVelocityA.Cast<float>();
		out_finalRotationB			+= initialRotationA.Cast<float>();
		return 0;
	}

	bool							revert				= distanceDirection.Dot(directionA.RotateY(::gpk::math_pi_2)) >= 0; // ::revertCross(distanceDirection);
	double							speedA				= initialVelocityA.Length();
	const ::gpk::SCoord3<double>	vUp					= {0, revert ? -1 : 1.0f, 0};
	::gpk::SCoord3<double>			finalVelocityB		= distanceDirection * speedA * out_forceTransferRatioB;
	::gpk::SCoord3<double>			finalVelocityA		= ::gpk::SCoord3<double>{finalVelocityB}.Normalize().Cross(vUp).Normalize() * speedA * (1.0f - out_forceTransferRatioB);
	//const double					totalFinalSpeed		= finalVelocityA.Length() + finalVelocityB.Length();
	//const double					totalInitialSpeed	= initialVelocityA.Length();
	//if ((totalFinalSpeed - totalInitialSpeed) > 0.0001f) {
	//	warning_printf("Invalid resulting force: initial: %f, final: %f, gained: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed));
	//}
	//if ((totalFinalSpeed - totalInitialSpeed) < -0.0001f) {
	//	warning_printf("Invalid resulting force: initial: %f, final: %f, lost: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed) * -1);
	//}			

	out_finalVelocityA			+= finalVelocityA.Cast<float>();
	out_finalVelocityB			+= finalVelocityB.Cast<float>();

	//info_printf("Total initial rotation: %f", (float)(initialRotationA.Length()));

	const double					rotA				= initialRotationA.Length();
	const ::gpk::SCoord3<double>	finalRotationB		= initialRotationA.Cast<double>().Normalize() * rotA * out_forceTransferRatioB;
	const ::gpk::SCoord3<double>	finalRotationA		= initialRotationA.Cast<double>().Normalize() * rotA * (1.0 - out_forceTransferRatioB);
	//const double					finalRotA			= finalRotationB.Length();
	//const double					finalRotB			= finalRotationA.Length();
	//info_printf("Total final rotation: %f", (float)(finalRotA + finalRotB));
	out_finalRotationA			+= finalRotationA.Cast<float>();
	out_finalRotationB			+= finalRotationB.Cast<float>();
	return 0;
}

static	::gpk::error_t		collisionDetectSphere			(const ::gpk::SEngine & engine, uint32_t iEntityA, const ::gpk::SVirtualEntity & entityA, ::gpk::array_pod<::gpk::SContact> & contactsDetected) {
	const ::gpk::SCoord3<float>		& radiusA						= engine.Integrator.BoundingVolumes[entityA.RigidBody].HalfSizes;
	const ::gpk::SCoord3<float>		& positionA						= engine.Integrator.Centers[entityA.RigidBody].Position;

	::gpk::SContact					contactBall						= {};
	contactBall.EntityA			= iEntityA;
	for(uint32_t iEntityB = iEntityA + 1; iEntityB < engine.ManagedEntities.Entities.size(); ++iEntityB) {
		const ::gpk::SVirtualEntity		& entityB						= engine.ManagedEntities.Entities[iEntityB];
		if(entityB.RigidBody >= engine.Integrator.BodyFlags.size())
			continue;

		const ::gpk::SRigidBodyFlags	& flagsB						= engine.Integrator.BodyFlags[entityB.RigidBody];
		if(false == flagsB.Collides)
			continue;

		switch(flagsB.BVType) {
		case ::gpk::BOUNDING_TYPE_Sphere: {
			const ::gpk::SCoord3<float>		& radiusB						= engine.Integrator.BoundingVolumes[entityB.RigidBody].HalfSizes;
			const ::gpk::SCoord3<float>		& positionB						= engine.Integrator.Centers[entityB.RigidBody].Position;

			const float						maxDistance						= radiusA.x + radiusB.x;
			const float						collisionThreshold				= maxDistance * maxDistance;
			contactBall.Distance		= positionB - positionA;			
			const double					distanceSquared					= contactBall.Distance.LengthSquared();
			if(distanceSquared >= collisionThreshold) 
				continue;

			contactBall.EntityB			= iEntityB;
			contactBall.DistanceLength	= distanceSquared ? sqrt(distanceSquared) : 0.0f;
			contactsDetected.push_back(contactBall);
			} // case
			break;
		} // switch
	}
	return 0;
}

::gpk::error_t				gpk::collisionDetect			(const ::gpk::SEngine & engine, ::gpk::array_pod<::gpk::SContact> & contactsDetected) {
	for(uint32_t iEntityA = 0, countEntities = engine.ManagedEntities.Entities.size(); iEntityA < countEntities; ++iEntityA) {
		const ::gpk::SVirtualEntity		& entityA						= engine.ManagedEntities.Entities[iEntityA];
		if(entityA.RigidBody >= engine.Integrator.BodyFlags.size())
			continue;

		const ::gpk::SRigidBodyFlags	& flagsA						= engine.Integrator.BodyFlags[entityA.RigidBody];
		if(false == flagsA.Collides)
			continue;

		switch(flagsA.BVType) {
		case ::gpk::BOUNDING_TYPE_Sphere: ::collisionDetectSphere(engine, iEntityA, entityA, contactsDetected); break;
		} // switch
	}
	return 0;
}

::gpk::error_t				the1::poolGameUpdate			(::the1::SPoolGame & pool, double secondsElapsed) {
	::gpk::SEngine					& engine						= pool.Engine;
	for(uint32_t iBall = 0; iBall < pool.StateCurrent.BallCount; ++iBall) {
		pool.PositionDeltas[iBall].push_back({});
		pool.GetBallPosition(iBall, pool.PositionDeltas[iBall][pool.PositionDeltas[iBall].size() - 1].A);
	}

	double							step							= .005f;
	pool.LastFrameContactsBall		.clear();
	pool.LastFrameContactsCushion	.clear();
	::gpk::array_pod<::the1::SContactBall>	lastFrameContactsBatchBall;
	::gpk::array_pod<::the1::SContactBall>	lastFrameContactsBatchCushion;


	while(secondsElapsed > 0) { 
		double							secondsThisStep					= ::gpk::min(step, secondsElapsed);
		secondsElapsed				-= secondsThisStep;

		//for(uint32_t iBallA = 0; iBallA < pool.StartState.BallCount; ++iBallA) {
		//	::gpk::SBodyForces				& forces						= engine.Integrator.Forces		[engine.ManagedEntities.Entities[pool.StartState.Balls[iBallA].Entity].RigidBody];
		//	::gpk::SRigidBodyFrame			& bodyAccum						= engine.Integrator.BodyFrames	[engine.ManagedEntities.Entities[pool.StartState.Balls[iBallA].Entity].RigidBody];
		//	if(::gpk::SCoord3<float>{forces.Velocity}.Normalize().Dot(::gpk::SCoord3<float>{forces.Rotation.z, 0, -forces.Rotation.x}.Normalize()) < 0) {
		//		bodyAccum.AccumulatedForce += ::gpk::SCoord3<float>{forces.Rotation.z, 0, -forces.Rotation.x} * secondsElapsed;
		//	}
		//	if(forces.Rotation.y)
		//		forces.Velocity.RotateY(forces.Rotation.y * secondsElapsed * .05);
		//}
		engine.Update(secondsThisStep);


		lastFrameContactsBatchBall		.clear();
		::gpk::collisionDetect(pool.Engine, lastFrameContactsBatchBall);

		lastFrameContactsBatchCushion	.clear();

		for(uint32_t iContact = 0; iContact < lastFrameContactsBatchBall.size(); ++iContact) {
			::the1::SContactBall			& contact			= lastFrameContactsBatchBall[iContact];
			const ::gpk::SVirtualEntity		& entityA			= engine.ManagedEntities.Entities[contact.EntityA]; 
			const ::gpk::SVirtualEntity		& entityB			= engine.ManagedEntities.Entities[contact.EntityB]; 

			contact.Result.DistanceDirection	= contact.Distance;
			double							distanceLength		= contact.DistanceLength;
			contact.Result.DistanceDirection.Normalize();

			// Separate balls
			::gpk::SCoord3<float>			& positionA			= engine.Integrator.Centers[entityA.RigidBody].Position;
			::gpk::SCoord3<float>			& positionB			= engine.Integrator.Centers[entityB.RigidBody].Position;
			positionA					+= contact.Result.DistanceDirection * ::gpk::max(pool.StateCurrent.BallRadius * 2 - distanceLength, 0.0) * -.51f;
			positionB					+= contact.Result.DistanceDirection * ::gpk::max(pool.StateCurrent.BallRadius * 2 - distanceLength, 0.0) * .51f;


			// Calculate force transfer
			::gpk::SCoord3<float>			& velocityA			= engine.Integrator.Forces[entityA.RigidBody].Velocity;
			::gpk::SCoord3<float>			& velocityB			= engine.Integrator.Forces[entityB.RigidBody].Velocity;
			::gpk::SCoord3<float>			& rotationA			= engine.Integrator.Forces[entityA.RigidBody].Rotation;
			::gpk::SCoord3<float>			& rotationB			= engine.Integrator.Forces[entityB.RigidBody].Rotation;

			contact.Result.InitialVelocityA	= velocityA;
			contact.Result.InitialVelocityB	= velocityB;
			contact.Result.InitialRotationA	= rotationA;
			contact.Result.InitialRotationB	= rotationB;
			velocityB					= {};
			velocityA					= {};
			rotationB					= {};
			rotationA					= {};
			if(contact.Result.InitialVelocityA.LengthSquared() || contact.Result.InitialRotationA.LengthSquared()) {
				engine.Integrator.BodyFlags[entityB.RigidBody].Active = true;
				::gpk::SCoord3<float>			lvelocityB					= {};
				::gpk::SCoord3<float>			lvelocityA					= {};
				::resolveCollision(contact.Result.InitialVelocityA.Cast<double>(), contact.Result.InitialRotationA.Cast<double>(), contact.Result.DistanceDirection.Cast<double>(), contact.Result.ForceTransferRatioB, rotationA, rotationB, lvelocityA, lvelocityB);
				velocityB					+= lvelocityB;
				velocityA					+= lvelocityA;
			}

			if(contact.Result.InitialVelocityB.LengthSquared() || contact.Result.InitialRotationB.LengthSquared()) {
				engine.Integrator.BodyFlags[entityA.RigidBody].Active = true;
				::gpk::SCoord3<float>			lvelocityB					= {};
				::gpk::SCoord3<float>			lvelocityA					= {};
				::resolveCollision(contact.Result.InitialVelocityB.Cast<double>(), contact.Result.InitialRotationB.Cast<double>(), contact.Result.DistanceDirection.Cast<double>() * -1, contact.Result.ForceTransferRatioA, rotationB, rotationA, lvelocityB, lvelocityA);
				velocityB					+= lvelocityB;
				velocityA					+= lvelocityA;
				{
					const double					initialSpeedA				= contact.Result.InitialVelocityA.Length();
					const double					initialSpeedB				= contact.Result.InitialVelocityB.Length();
					const double					finalSpeedA					= velocityA.Length();
					const double					finalSpeedB					= velocityB.Length();
					const double					totalInitialSpeed			= initialSpeedA + initialSpeedB;
					const double					totalFinalSpeed				= finalSpeedA + finalSpeedB;
					if ((totalFinalSpeed - totalInitialSpeed) / totalInitialSpeed > 0.01f) {
						warning_printf("Invalid resulting force: initial: %f, final: %f, gained: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed));
					}
					if ((totalFinalSpeed - totalInitialSpeed) / totalInitialSpeed < -0.01f) {
						warning_printf("Invalid resulting force: initial: %f, final: %f, lost: %f", totalInitialSpeed, totalFinalSpeed, (totalFinalSpeed - totalInitialSpeed) * -1);
					}
				}
			}

			contact.Result.FinalVelocityA	= (velocityA *= pool.StateCurrent.Physics.DampingCollision);
			contact.Result.FinalVelocityB	= (velocityB *= pool.StateCurrent.Physics.DampingCollision);
			contact.Result.FinalRotationA	= (rotationA *= pool.StateCurrent.Physics.DampingCollision);
			contact.Result.FinalRotationB	= (rotationB *= pool.StateCurrent.Physics.DampingCollision);
		}

		pool.LastFrameContactsBall.append(lastFrameContactsBatchBall);

		const gpk::SCoord2<float>		tableHalfDimensions	= pool.StateCurrent.Table.Dimensions.Slate * .5f;
		for(uint32_t iBall = 0; iBall < pool.StateCurrent.BallCount; ++iBall) {
			const float						ballRadius			= pool.StateCurrent.BallRadius;
			const gpk::SCoord2<float>		ballLimits			= tableHalfDimensions - ::gpk::SCoord2<float>{ballRadius, ballRadius};
			const ::gpk::SVirtualEntity		& entityA			= engine.ManagedEntities.Entities[pool.StateCurrent.Ball[iBall].Entity]; 
			::gpk::SCoord3<float>			& positionA			= engine.Integrator.Centers[entityA.RigidBody].Position;
			::gpk::SRigidBodyFlags			& flags				= engine.Integrator.BodyFlags[entityA.RigidBody];
			bool							inPocket			= false;
			::gpk::SBodyForces				& forces			= engine.Integrator.Forces[entityA.RigidBody];
			for(uint32_t iPocket = 0; iPocket < 6; ++iPocket) {
				const ::gpk::SVirtualEntity		& entityPocket		= engine.ManagedEntities.Entities[pool.StateCurrent.Table.Pockets[iPocket].Entity];
				::gpk::SCoord3<float>			pocketPosition		= engine.Scene->ManagedRenderNodes.Transforms[entityPocket.RenderNode].World.GetTranslation();
				::gpk::SCoord3<float>			positionBall		= positionA;
				if(positionBall.y < 0) {
					inPocket					= true;
					forces.Acceleration			= {};
					forces.Velocity.x			= 0;
					forces.Velocity.z			= 0;
					flags.Falling				= true;
					break;
				}
				pocketPosition.y			= 0;
				positionBall.y				= 0;

				const float						pocketRadius		= pool.StateCurrent.Table.PocketRadius;
				const float						maxLength			= pocketRadius;// + ballRadius;
				::gpk::SCoord3<float>			distanceFromPocket	= positionA - pocketPosition;
				if(distanceFromPocket.LengthSquared() > maxLength * maxLength)
					continue;
				float							w					= (float)(distanceFromPocket.Length() - ballRadius) / (pocketRadius - ballRadius);
				inPocket					= true;
				forces.Acceleration.y		= -pool.StateCurrent.Physics.Gravity * ::gpk::max(0.0f, (1.0f - w));
				flags.Falling				= true;
				break;
			}
			if(inPocket)
				continue;

			::gpk::SCoord2<bool>				outOfBounds			= 
				{ (positionA.x	< -ballLimits.x) || (positionA.x > ballLimits.x)
				, (positionA.z	< -ballLimits.y) || (positionA.z > ballLimits.y)
				};
			if(outOfBounds.x 
				&& (positionA.z > -(tableHalfDimensions.y - pool.StateCurrent.Table.PocketRadius))
				&& (positionA.z <  (tableHalfDimensions.y - pool.StateCurrent.Table.PocketRadius))
			) {
				positionA.x					= (positionA.x	< -ballLimits.x) 
					? (-ballLimits.x) - (positionA.x + ballLimits.x)
					:   ballLimits.x  - (positionA.x - ballLimits.x)
					;
				forces.Velocity.x			*= -1;
				forces.Velocity				*= pool.StateCurrent.Physics.DampingCushion;
				forces.Rotation.z			*= -1;
			}
			if(outOfBounds.y 
			 &&  (positionA.x > -(tableHalfDimensions.x - pool.StateCurrent.Table.PocketRadius)
				&& positionA.x <  (tableHalfDimensions.x - pool.StateCurrent.Table.PocketRadius)
				&& (positionA.x < -pool.StateCurrent.Table.PocketRadius || positionA.x > pool.StateCurrent.Table.PocketRadius)
			)) {
				positionA.z					= (positionA.z	< -ballLimits.y) 
					? (-ballLimits.y) - (positionA.z + ballLimits.y)
					:   ballLimits.y  - (positionA.z - ballLimits.y)
					;
				forces.Velocity.z			*= -1;
				forces.Velocity				*= pool.StateCurrent.Physics.DampingCushion;
				forces.Rotation.x			*= -1;
			}
			if(false == flags.Falling) 
				forces.Velocity.y			=  0;
			else {
				if(positionA.y < pool.StateCurrent.BallRadius) {
					positionA.y					= (positionA.y - pool.StateCurrent.BallRadius) * -.95f + pool.StateCurrent.BallRadius;

					forces.Velocity.y			*= -1.0f;
					forces.Velocity.y			*= pool.StateCurrent.Physics.DampingGround;
					forces.Acceleration.y		= -pool.StateCurrent.Physics.Gravity;
				}
				else if(fabs(forces.Velocity.y) > 0.00075 || positionA.y > (pool.StateCurrent.BallRadius + pool.StateCurrent.BallRadius * .125f) || positionA.y < (pool.StateCurrent.BallRadius - pool.StateCurrent.BallRadius * .125f)) {
					forces.Acceleration.y		= -pool.StateCurrent.Physics.Gravity;
				}
				else {
					forces.Acceleration.y		=  0;
					forces.Velocity.y			=  0;
					positionA.y					= pool.StateCurrent.BallRadius;
					flags.Falling				= false;
				}
			}

		}
	}

	const float radius = pool.StateCurrent.BallRadius;
	const float diameter = radius * 2;
	for(uint32_t iBall = 0; iBall < pool.StateCurrent.BallCount; ++iBall) {
		::gpk::SLine3<float> & delta = pool.PositionDeltas[iBall][pool.PositionDeltas[iBall].size() - 1];
		pool.GetBallPosition(iBall, delta.B);
		if((delta.B - delta.A).LengthSquared() > 0.0000005) {
			::gpk::SBodyForces				& forces			= engine.Integrator.Forces[engine.ManagedEntities.Entities[pool.StateCurrent.Ball[iBall].Entity].RigidBody];

			::gpk::SCoord3<float>			rotationResult		= (delta.B - delta.A) / diameter * ::gpk::math_2pi;
			forces.Rotation				+= {rotationResult.z, 0, -rotationResult.x};
		}
		if(pool.PositionDeltas[iBall].size() > 10)
			for(uint32_t iDelta = 0; iDelta < pool.PositionDeltas[iBall].size(); ++iDelta) {
				if((delta.B - delta.A).LengthSquared() < 0.00005f) {
					pool.PositionDeltas[iBall].remove_unordered(iDelta--);
				}
			}
	}

	::gpk::SCoord3<float> ballPosition = {};
	pool.GetBallPosition(0, ballPosition);
	engine.SetPosition(pool.StateCurrent.Player[0].Stick.Entity, ballPosition);
	engine.SetOrientation(pool.StateCurrent.Player[0].Stick.Entity, ::gpk::SQuaternion<float>{}.CreateFromAxisAngle({0, 1}, -pool.StateCurrent.Player[0].Stick.Angle));
	return 0;
}