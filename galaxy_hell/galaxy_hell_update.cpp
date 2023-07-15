#include "gpk_galaxy_hell.h"

#include "gpk_collision.h"

#include "gpk_line3.h"

#include <Windows.h>
#include <mmsystem.h>
#include <gpk_noise.h>

static	::gpk::error_t	applyDamage
	( const int32_t	weaponDamage
	, int32_t		& healthPart
	, int32_t		& healthParent
	) {
	const uint32_t				finalDamage			= ::gpk::min(weaponDamage, healthPart);
	healthPart				-= finalDamage;
	healthParent			-= finalDamage;
	return 0 >= healthPart;
}

static	::gpk::error_t	collisionDetect		(::gpk::SShots & shots, const ::gpk::n3f32 & modelPosition, ::gpk::apod<::gpk::n3f32> & collisionPoints)	{
	bool						detected			= false;
	collisionPoints.clear();
	for(uint32_t iShot = 0; iShot < shots.Particles.Position.size(); ++iShot) {
		const ::gpk::line3f32		shotSegment			= {shots.PositionPrev[iShot], shots.Particles.Position[iShot]};
		float						t					= 0;
		::gpk::n3f32				collisionPoint		= {};
		if( ::gpk::intersectRaySphere(shotSegment.A, (shotSegment.B - shotSegment.A).Normalize(), {1.2, modelPosition}, t, collisionPoint)
			&& t < 1
		) {
			detected				= true;
			collisionPoints.push_back(collisionPoint);
			shots.Remove(iShot);
			--iShot;
		}
	}
	return 0;
}

static	::gpk::error_t	handleCollisionPoint		(::ghg::SGalaxyHell & solarSystem, int32_t weaponDamage, ::gpk::SSpaceshipScore & attackerScore, ::gpk::SSpaceshipScore & damagedScore, ::gpk::SSpaceshipOrbiter & damagedPart, int32_t iAttackedPart, ::gpk::SSpaceshipCore & damagedShip, int32_t iAttackedShip, const ::gpk::n3f32 & sphereCenter, const ::gpk::n3f32 & collisionPoint)	{
	solarSystem.ShipState.SpaceshipManager.ShipOrbiterActionQueue[iAttackedShip]->push_back(::gpk::SHIP_ACTION_Hit);
	const ::gpk::n3f32			bounceVector				= (collisionPoint - sphereCenter).Normalize();
	solarSystem.DecoState.Debris.SpawnDirected(1 + weaponDamage / 10, 0.3, bounceVector, collisionPoint, 50, 1);
	attackerScore.Hits		+= 1;
	attackerScore.Score		+= weaponDamage;
	solarSystem.DecoState.ScoreParticles.Create(collisionPoint, bounceVector, 10.0f + weaponDamage * .001f, {weaponDamage, 1});
	attackerScore.DamageDone	+= weaponDamage;
	damagedScore.DamageReceived	+= weaponDamage;
	if(::applyDamage(weaponDamage, damagedPart.Health.Value, damagedShip.Health)) {	// returns true if health reaches zero
		attackerScore.Score			+= weaponDamage * 10;
		solarSystem.DecoState.ScoreParticles.Create(collisionPoint, bounceVector, 10.0f + weaponDamage * .001f, {weaponDamage * 10, 2});
		attackerScore.KilledOrbiters	+= 1;
		damagedScore.OrbitersLost		+= 1;
		const ::ghg::SGHEntity		& entityGeometry			= solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipPartEntity[iAttackedPart] + 1];
		const int32_t				indexMesh					= entityGeometry.Geometry;
		const uint32_t				countTriangles				= solarSystem.ShipState.Scene.Geometry[indexMesh].Triangles.size();
		::ghg::decoExplosionAdd(solarSystem.DecoState.Explosions, indexMesh, entityGeometry.Image, countTriangles, collisionPoint, 60);
		solarSystem.DecoState.Debris.SpawnSpherical(20, collisionPoint, 60, 2);
		if(0 >= damagedShip.Health) {
			attackerScore.Score		+= weaponDamage * 50;
			solarSystem.DecoState.ScoreParticles.Create(collisionPoint, bounceVector, 10.0f + weaponDamage * .001f, {weaponDamage * 50, 3});
			attackerScore.KilledShips	+= 1;
			const ::gpk::n3f32			& parentPosition			= solarSystem.ShipState.GetShipPosition(iAttackedShip);
			::ghg::decoExplosionAdd(solarSystem.DecoState.Explosions, indexMesh, entityGeometry.Image, countTriangles, parentPosition, 13);
			solarSystem.DecoState.Debris.SpawnSpherical(80, parentPosition, 13, 2.8f);
			solarSystem.PlayState.Slowing	= true;
			solarSystem.PlayState.TimeScale	= 1.0;
		}
		return 1;
	}
	else {
		if(0 < damagedShip.Health) 
			damagedScore.HitsSurvived	+= 1;
	}
	return 0;
}

static	::gpk::error_t	updateEntityTransforms		(uint32_t iEntity, const ::gpk::view<const ::ghg::SGHEntity> & entities, const ::gpk::view<const ::gpk::au32> & entitiesChildren, ::ghg::SShipScene & scene, ::gpk::SRigidBodyIntegrator & bodies)	{
	const ::ghg::SGHEntity		& entity					= entities[iEntity];
	if(-1 == entity.Body) {
		if(-1 == entity.Parent)
			scene.Transforms[iEntity].Identity();
		else
			scene.Transforms[iEntity]	= scene.Transforms[entity.Parent];
	}
	else {
		::gpk::m4f32				& matrixGlobal				= scene.Transforms[iEntity];
		bodies.GetTransform(entity.Body, matrixGlobal);
		if(-1 != entity.Parent)
			matrixGlobal			*= scene.Transforms[entity.Parent];
	}

	const ::gpk::au32			& entityChildren			= entitiesChildren[iEntity];
	for(uint32_t iChild = 0; iChild < entityChildren.size(); ++iChild) {
		const uint32_t				iChildEntity				= entityChildren[iChild];
		::updateEntityTransforms(iChildEntity, entities, entitiesChildren, scene, bodies);
	}
	return 0;
}

static	::gpk::error_t	updateShots				(::ghg::SGalaxyHell & solarSystem, double secondsLastFrame)	{
	for(uint32_t iShip = 0; iShip < solarSystem.ShipState.SpaceshipManager.ShipCores.size(); ++iShip) {
		::gpk::au16					& shipParts				= solarSystem.ShipState.SpaceshipManager.ShipParts[iShip];
		for(uint32_t iPart = 0; iPart < shipParts.size(); ++iPart) {
			::std::lock_guard			lockUpdate			(solarSystem.LockUpdate);
			solarSystem.ShipState.SpaceshipManager.Shots[solarSystem.ShipState.SpaceshipManager.Orbiters[shipParts[iPart]].Weapon].Update((float)secondsLastFrame);
		}
	}

	::gpk::apod<::gpk::n3f32>	collisionPoints;
	for(uint32_t iShipAttacker = 0; iShipAttacker < solarSystem.ShipState.SpaceshipManager.ShipCores.size(); ++iShipAttacker) {
		::gpk::SSpaceshipScore		& shipAttackerScore		= solarSystem.ShipState.SpaceshipManager.ShipScores	[iShipAttacker];
		::gpk::SSpaceshipCore		& shipAttacker			= solarSystem.ShipState.SpaceshipManager.ShipCores	[iShipAttacker];
		::gpk::au16					& shipAttackerParts		= solarSystem.ShipState.SpaceshipManager.ShipParts	[iShipAttacker];
		//::gpk::apod<::gpk::n3f32>	& shipDistancesToTarget	= solarSystem.ShipState.SpaceshipManager.ShipCoresDistanceToTargets	[iShipAttacker];
		for(uint32_t iPartAttacker = 0; iPartAttacker < shipAttackerParts.size(); ++iPartAttacker) {
			::gpk::SSpaceshipOrbiter	& shipPartAttacker		= solarSystem.ShipState.SpaceshipManager.Orbiters[shipAttackerParts[iPartAttacker]];

			for(uint32_t iShipAttacked = 0; iShipAttacked < solarSystem.ShipState.SpaceshipManager.ShipCores.size(); ++iShipAttacked) {
				::gpk::SSpaceshipCore		& shipAttacked			= solarSystem.ShipState.SpaceshipManager.ShipCores	[iShipAttacked];
				::gpk::au16					& shipAttackedParts		= solarSystem.ShipState.SpaceshipManager.ShipParts	[iShipAttacked];
				::gpk::SSpaceshipScore		& shipAttackedScore		= solarSystem.ShipState.SpaceshipManager.ShipScores	[iShipAttacked];
				if(shipAttacked.Health <= 0 || shipAttacker.Team == shipAttacked.Team) // avoid dead ships and ships of the same team
					continue;
				for(uint32_t iPartAttacked = 0; iPartAttacked < shipAttackedParts.size(); ++iPartAttacked) {
					int32_t attackedPart = shipAttackedParts[iPartAttacked];
					::gpk::SSpaceshipOrbiter	& shipPartAttacked		= solarSystem.ShipState.SpaceshipManager.Orbiters[attackedPart];
					if(shipPartAttacked.Health.Value <= 0)
						continue;
					const ::gpk::SWeapon		& weaponAttacker		= solarSystem.ShipState.SpaceshipManager.Weapons[shipPartAttacker.Weapon];
					const ::ghg::SGHEntity		& entity				= solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipPartEntity[attackedPart]];
					if(-1 != entity.Geometry) {
						const ::gpk::n3f32			attackedPosition		= solarSystem.ShipState.Scene.Transforms[solarSystem.ShipState.ShipPartEntity[attackedPart]].GetTranslation();
						::std::lock_guard			lockUpdate				(solarSystem.LockUpdate);
						::collisionDetect(solarSystem.ShipState.SpaceshipManager.Shots[shipPartAttacker.Weapon], attackedPosition, collisionPoints);
						for(uint32_t iCollisionPoint = 0; iCollisionPoint < collisionPoints.size(); ++iCollisionPoint)
							if(::handleCollisionPoint(solarSystem, weaponAttacker.Shot.Damage, shipAttackerScore, shipAttackedScore, shipPartAttacked, attackedPart, shipAttacked, iShipAttacked, attackedPosition, collisionPoints[iCollisionPoint]))	// returns true if part health reaches zero.
								break;
					}
					const ::gpk::au32			& entityChildren		= solarSystem.ShipState.EntitySystem.EntityChildren[solarSystem.ShipState.ShipPartEntity[attackedPart]];
					for(uint32_t iEntity = 0; iEntity < entityChildren.size(); ++iEntity) {
						::ghg::SGHEntity			& entityChild			= solarSystem.ShipState.EntitySystem.Entities[entityChildren[iEntity]];
						if(-1 == entityChild.Parent)
							continue;

						if(-1 == entityChild.Geometry)
							continue;

						const ::gpk::n3f32			attackedPosition		= solarSystem.ShipState.Scene.Transforms[entityChildren[iEntity]].GetTranslation();
						::std::lock_guard			lockUpdate				(solarSystem.LockUpdate);
						::collisionDetect(solarSystem.ShipState.SpaceshipManager.Shots[shipPartAttacker.Weapon], attackedPosition, collisionPoints);
						for(uint32_t iCollisionPoint = 0; iCollisionPoint < collisionPoints.size(); ++iCollisionPoint)
							if(::handleCollisionPoint(solarSystem, weaponAttacker.Shot.Damage, shipAttackerScore, shipAttackedScore, shipPartAttacked, attackedPart, shipAttacked, iShipAttacked, attackedPosition, collisionPoints[iCollisionPoint])) {	// returns true if part health reaches zero.
								break;
							}
					}
				}
			}

			::gpk::SWeapon	& weaponAttacker	= solarSystem.ShipState.SpaceshipManager.Weapons[shipPartAttacker.Weapon];
			::gpk::SShots	& shotsAttacker		= solarSystem.ShipState.SpaceshipManager.Shots[shipPartAttacker.Weapon];
			if(weaponAttacker.Shot.Type == ::gpk::WEAPON_LOAD_Rocket) {
				for(uint32_t iShot = 0; iShot < shotsAttacker.Particles.Position.size(); ++iShot) {
					const ::gpk::apod<::gpk::n3f32> & distances =  shotsAttacker.DistanceToTargets[iShot];
					if(0 == distances.size())
						continue;

					if(2.5f > shotsAttacker.Lifetime[iShot])
						continue;

					for(uint32_t iTarget = 0; iTarget < distances.size(); ++iTarget) {
						::gpk::n3f32			target		= distances[iTarget];
						target.Normalize();
						const uint32_t			targetSeed	= solarSystem.PlayState.Seed + iPartAttacker + iShot;
						const ::gpk::n3f32		& distance	= distances[::gpk::noise1DBase32(targetSeed) % distances.size()];
						if(distance.LengthSquared() > 10000) 
							continue;

						if(shotsAttacker.Particles.Direction[iShot].Dot(target) > 0) {
							shotsAttacker.Particles.Direction[iShot] = ::gpk::interpolate_linear(shotsAttacker.Particles.Direction[iShot], distance, ::gpk::min(secondsLastFrame * 2, 1.0)).Normalize();
							break;
						}
					}
				}
			}
			else if(weaponAttacker.Shot.Type == ::gpk::WEAPON_LOAD_Missile) {
				for(uint32_t iShot = 0; iShot < shotsAttacker.Particles.Position.size(); ++iShot) {
					const ::gpk::apod<::gpk::n3f32>	& distances =  shotsAttacker.DistanceToTargets[iShot];
					if(0 == distances.size())
						continue;

					if(2.5f > shotsAttacker.Lifetime[iShot])
						continue;

					const uint32_t					targetSeed	= solarSystem.PlayState.Seed + iPartAttacker + iShot;
					const ::gpk::n3f32				& distance	= distances[::gpk::noise1DBase32(targetSeed) % distances.size()];
					if(distance.LengthSquared() > 10000) 
						continue;

					shotsAttacker.Particles.Direction[iShot] = ::gpk::interpolate_linear(shotsAttacker.Particles.Direction[iShot], distance, ::gpk::min(secondsLastFrame * 1, 1.0)).Normalize();
				}
			}

		}
	}
	return 0;
}

static	::gpk::error_t	updateDistancesToTargets	(::ghg::SGalaxyHell & solarSystem, int32_t team, int32_t iShipPart, ::gpk::papod<::gpk::n3f32> & orbiterDistancesToTargets, ::gpk::SShots & shots)	{
	{
		::std::lock_guard			lockUpdate					(solarSystem.LockUpdate);
		orbiterDistancesToTargets.clear();
		for(uint32_t iShot = 0; iShot < shots.Particles.Position.size(); ++iShot)
			shots.DistanceToTargets[iShot].clear();
	}

	::gpk::apod<::gpk::SSpaceshipOrbiter>	& orbiters				= solarSystem.ShipState.SpaceshipManager.Orbiters				;
	::gpk::apod<::gpk::SSpaceshipCore>	& shipCores				= solarSystem.ShipState.SpaceshipManager.ShipCores				;
	::gpk::aobj<::gpk::au16>		& shipCoresParts		= solarSystem.ShipState.SpaceshipManager.ShipParts				;
	const ::gpk::n3f32				weaponPosition			= solarSystem.ShipState.Scene.Transforms[solarSystem.ShipState.ShipPartEntity[iShipPart]].GetTranslation();
	for(uint32_t iShip = 0; iShip < shipCores.size(); ++iShip) {
		::gpk::SSpaceshipCore				& ship					= shipCores[iShip];
		if(ship.Team == team || ship.Health <= 0)
			continue;

		for(uint32_t iPart = 0; iPart < shipCoresParts[iShip].size(); ++iPart) {
			::gpk::SSpaceshipOrbiter					& shipPartTarget		= orbiters[shipCoresParts[iShip][iPart]];
			if(0 >= shipPartTarget.Health.Value)
				continue;

			const ::gpk::n3f32				targetPosition			= solarSystem.ShipState.Scene.Transforms[solarSystem.ShipState.ShipPartEntity[shipCoresParts[iShip][iPart]]].GetTranslation();
			{
				::std::lock_guard				lockUpdate				(solarSystem.LockUpdate);
				orbiterDistancesToTargets->push_back(targetPosition - weaponPosition);
				for(uint32_t iShot = 0; iShot < shots.Particles.Position.size(); ++iShot) {
					shots.DistanceToTargets[iShot].push_back(targetPosition - shots.Particles.Position[iShot]);
				}
			}
		}
	}
	return 0;
}

static	::gpk::error_t	updateShipOrbiter			(::ghg::SGalaxyHell & solarSystem, int32_t team, ::gpk::SSpaceshipOrbiter & shipPart, int32_t iShip, int32_t iShipPart, ::gpk::papod<::gpk::n3f32> & shipPartDistancesToTargets, double secondsLastFrame)	{
	::gpk::SWeapon				& weapon					= solarSystem.ShipState.SpaceshipManager.Weapons	[shipPart.Weapon];
	::gpk::SShots				& shots						= solarSystem.ShipState.SpaceshipManager.Shots	[shipPart.Weapon];
	for(uint32_t iParticle = 0; iParticle < shots.Particles.Position.size(); ++iParticle)
		shots.Particles.Position[iParticle].x				-= (float)(solarSystem.PlayState.RelativeSpeedCurrent * secondsLastFrame * .2);

	;

	if(weapon.Shot.Type == ::gpk::WEAPON_LOAD_Rocket || weapon.Shot.Type == ::gpk::WEAPON_LOAD_Missile || weapon.Type == ::gpk::WEAPON_TYPE_Cannon) {
		updateDistancesToTargets(solarSystem, team, iShipPart, shipPartDistancesToTargets, shots); 
	}

	if(weapon.Trigger.Update(secondsLastFrame)) {}
	else {
		solarSystem.ShipState.SpaceshipManager.ShipScores[iShip].Shots	+= 1;
		solarSystem.ShipState.SpaceshipManager.ShipScores[iShip].Bullets	+= weapon.Shot.ParticleCount;

		const ::gpk::m4f32			& shipModuleMatrix			= solarSystem.ShipState.Scene.Transforms[solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipPartEntity[iShipPart] + 1].Transform];
		::gpk::n3f32				positionGlobal				= shipModuleMatrix.GetTranslation();
		::gpk::n3f32				targetDistance				= (shipPartDistancesToTargets && shipPartDistancesToTargets->size()) ? (*shipPartDistancesToTargets)[::gpk::noise1DBase32(solarSystem.PlayState.Seed + iShip * ::ghg::MAX_ORBITER_COUNT + iShipPart) % shipPartDistancesToTargets->size()] : ::gpk::n3f32{};
		::gpk::n3f32				targetPosition				= targetDistance + positionGlobal;
		if(weapon.Type == ::gpk::WEAPON_TYPE_Cannon) {
			if(1 < targetDistance.LengthSquared()) {
				::gpk::n3f32			direction					= targetDistance;
				direction.Normalize(); 
				{
					::std::lock_guard		lockUpdate			(solarSystem.LockUpdate);
					if(weapon.Shot.Type == ::gpk::WEAPON_LOAD_Rocket)
						weapon.Shoot(shots, positionGlobal, direction, 1);
					else if(weapon.Shot.Type == ::gpk::WEAPON_LOAD_Cannonball)
						weapon.Shoot(shots, positionGlobal, direction, 1);
					else if(weapon.Shot.Type == ::gpk::WEAPON_LOAD_Missile)
						weapon.Shoot(shots, positionGlobal, direction, 1);
				}

				if(solarSystem.PlayState.TimeStage > 1) {
					solarSystem.ShipState.Engine.Integrator.Forces[solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipPartEntity[iShipPart] + 1].Body].Rotation	= {};
					::gpk::m4f32			inverseTransform			= shipModuleMatrix.GetInverse();
					::gpk::SBodyCenter		& shipModuleTransform		= solarSystem.ShipState.GetOrbiterTransform(iShipPart);
					::gpk::n3f32			up							= {1, 0, 0};
					::gpk::n3f32			front						= {0, 1, 0};
					//const ::gpk::m4f32	& shipMatrix				= solarSystem.Scene.Transforms[solarSystem.Entities[solarSystem.Entities[shipPart.Entity].Parent].Transform];
					targetPosition		= inverseTransform.Transform(targetPosition);
					::gpk::quat<float>		q;
					q.LookAt(shipModuleTransform.Position, targetPosition, up, front);
					shipModuleTransform.Orientation	= q;
				}
			}
		}
		else if(weapon.Type == ::gpk::WEAPON_TYPE_Gun) {
			::gpk::n3f32				direction				= {team ? -1.0f : 1.0f, 0, 0};
			::std::lock_guard			lockUpdate				(solarSystem.LockUpdate);
			if(weapon.Shot.Type == ::gpk::WEAPON_LOAD_Ray)
				weapon.Shoot(shots, positionGlobal, direction, .75f);
			else
				weapon.Shoot(shots, positionGlobal, direction, 5.0f);
		}
		else if(weapon.Type == ::gpk::WEAPON_TYPE_Shotgun) {
			::gpk::n3f32				direction				= {team ? -1.0f : 1.0f, 0, 0};
			::std::lock_guard			lockUpdate				(solarSystem.LockUpdate);
			if(weapon.Shot.Type == ::gpk::WEAPON_LOAD_Ray)
				weapon.Shoot(shots, positionGlobal, direction, .75f);
			else
				weapon.Shoot(shots, positionGlobal, direction, 5.0f);
		}
	}
	return 0;
}

//stacxpr const double						UPDATE_STEP_TIME		= 0.012;

static	::gpk::error_t	shipsUpdate				(::ghg::SGalaxyHell & solarSystem, double secondsLastFrame)	{
	::updateShots(solarSystem, secondsLastFrame);

	bool						playing					= 0;
	for(uint32_t iShip = 0; iShip < solarSystem.ShipState.SpaceshipManager.ShipCores.size(); ++iShip) {
		::gpk::SSpaceshipCore			& ship					= solarSystem.ShipState.SpaceshipManager.ShipCores[iShip];
		if(0 >= ship.Health)
			continue;
		::gpk::au16					& shipParts				= solarSystem.ShipState.SpaceshipManager.ShipParts[iShip];
		::gpk::SBodyCenter			& shipTransform			= solarSystem.ShipState.Engine.Integrator.Centers[solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipCoreEntity[iShip]].Body];
		if(ship.Team) { 
			playing					= 1;

			shipTransform.Position.z	= (float)(sin(iShip + solarSystem.PlayState.TimeWorld) * (iShip * 5.0) * ((iShip % 2) ? -1 : 1));
			shipTransform.Position.x	= (float)((iShip * 5.0) - solarSystem.PlayState.Stage + 10 - (solarSystem.PlayState.RelativeSpeedCurrent * solarSystem.PlayState.RelativeSpeedCurrent * .0005 * ((solarSystem.PlayState.RelativeSpeedCurrent >= 0) ? 1 : -1)  ));
			double						timeWaveVertical					= .1;

			if(0 == (solarSystem.PlayState.Stage % 7)) {
				if(iShip % 2)
					shipTransform.Position.z	= float(cos(iShip + solarSystem.PlayState.TimeWorld) * ((solarSystem.ShipState.SpaceshipManager.ShipCores.size() - 1 - iShip) * 4.0) * ((iShip % 2) ? -1 : 1));
				else
					shipTransform.Position.z	= float(sin(iShip + solarSystem.PlayState.TimeWorld) * (iShip * 4.0) * ((iShip % 2) ? -1 : 1));
					 if(0 == (iShip % 2)) timeWaveVertical	= .65;
				else if(0 == (iShip % 3)) timeWaveVertical	= .80;
				else if(0 == (iShip % 7)) timeWaveVertical	= .80;
			}
			else if(0 == (solarSystem.PlayState.Stage % 5)) {
					shipTransform.Position.z	= float(cos(iShip + solarSystem.PlayState.TimeWorld) * ((solarSystem.ShipState.SpaceshipManager.ShipCores.size() - 1 - iShip) * 3.0) * ((iShip % 2) ? -1 : 1));
					 if(0 == (iShip % 2)) timeWaveVertical	= .50;
				else if(0 == (iShip % 3)) timeWaveVertical	= .75;
				else if(0 == (iShip % 7)) timeWaveVertical	= .80;
			}
			else if(0 == (solarSystem.PlayState.Stage % 3)) {
					shipTransform.Position.z	= float(cos(iShip + solarSystem.PlayState.TimeWorld) * ((solarSystem.ShipState.SpaceshipManager.ShipCores.size() - 1 - iShip) * 2.0) * ((iShip % 2) ? -1 : 1));
					 if(0 == (iShip % 2)) timeWaveVertical	= .25;
				else if(0 == (iShip % 3)) timeWaveVertical	= .50;
				else if(0 == (iShip % 7)) timeWaveVertical	= .75;
			}
			else {
					 if(0 == (iShip % 2)) timeWaveVertical	= .50;
				else if(0 == (iShip % 3)) timeWaveVertical	= .25;
				else if(0 == (iShip % 7)) timeWaveVertical	= .15;
			}
			shipTransform.Position.x		+= float(sin(solarSystem.PlayState.TimeWorld * timeWaveVertical * ::gpk::math_2pi) * ((iShip % 2) ? -1 : 1));
		}
		for(uint32_t iPart = 0; iPart < shipParts.size(); ++iPart) {
			::gpk::SSpaceshipOrbiter				& shipPart				= solarSystem.ShipState.SpaceshipManager.Orbiters[shipParts[iPart]];
			if(0 >= shipPart.Health.Value)
				continue;
			::updateShipOrbiter(solarSystem, ship.Team, shipPart, iShip, shipParts[iPart], solarSystem.ShipState.SpaceshipManager.WeaponDistanceToTargets[shipParts[iPart]], secondsLastFrame);
		}
	}
	return playing;
}

static	::gpk::error_t	processInput			(::ghg::SGalaxyHell & solarSystem, double secondsLastFrame, const ::gpk::SInput & input, ::gpk::vpobj<::gpk::SSystemEvent> frameEvents) {
	(void)frameEvents;


	::gpk::view<const ::ghg::SShipController>	controllers	= solarSystem.ShipControllers;
	//const bool				key_rotate_left			= input.KeyboardCurrent.KeyState[VK_NUMPAD8	];
	//const bool				key_rotate_right		= input.KeyboardCurrent.KeyState[VK_NUMPAD2	];
	//const bool				key_rotate_front		= input.KeyboardCurrent.KeyState[VK_NUMPAD6	];
	//const bool				key_rotate_back			= input.KeyboardCurrent.KeyState[VK_NUMPAD4	];
	//const bool				key_rotate_reset		= input.KeyboardCurrent.KeyState[VK_NUMPAD5	];
	const bool					key_camera_switch		= input.KeyboardCurrent.KeyState['C'		];
	const bool					key_camera_move_front	= input.KeyboardCurrent.KeyState[VK_HOME	];
	const bool					key_camera_move_back	= input.KeyboardCurrent.KeyState[VK_END		];
	const bool					key_camera_move_up		= input.KeyboardCurrent.KeyState['E'		];
	const bool					key_camera_move_down	= input.KeyboardCurrent.KeyState['Q'		];
	const bool					key_camera_reset		= input.KeyboardCurrent.KeyState['R'		];
	if(key_camera_reset)
		solarSystem.ShipState.Scene.Global.CameraReset();

	constexpr double			speed					= 40;
	if(solarSystem.ShipState.EntitySystem.Entities.size()) {
		for(uint32_t iPlayer = 0; iPlayer < solarSystem.PlayState.CountPlayers; ++iPlayer) {
			const ::ghg::SShipController	& shipController		= controllers[iPlayer];
			::gpk::SBodyCenter				& playerBody			= solarSystem.ShipState.Engine.Integrator.Centers[solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipCoreEntity[iPlayer]].Body];
			::gpk::SSpaceshipCore			& ship					= solarSystem.ShipState.SpaceshipManager.ShipCores[iPlayer];
			bool							turbo					= shipController.Turbo;
			double							speedMultiplier			= 1;
			if(ship.Nitro.Value < ship.Nitro.Limit)
				ship.Nitro.Value			+= (float)(secondsLastFrame * .1);
			if(ship.Nitro.Value > 0 && turbo) {
				ship.Nitro.Value			-= (float)secondsLastFrame;
				speedMultiplier				*= 2;
			}

			{
				solarSystem.PlayState.AccelerationControl	
					= shipController.Forward	? +1
					: shipController.Back		? -1
					: 0
					;

				playerBody.Position.x	+= (float)(secondsLastFrame * speed * speedMultiplier * solarSystem.PlayState.AccelerationControl);

				if(shipController.Left 	) { playerBody.Position.z += (float)(secondsLastFrame * speed * speedMultiplier); }
				if(shipController.Right	) { playerBody.Position.z -= (float)(secondsLastFrame * speed * speedMultiplier); }
			}

			//if(shipController.PointerDeltas.x) 
			//	playerBody.Orientation.y += (float)(shipController.PointerDeltas.x / 128.f);
			//if(shipController.PointerDeltas.y) 
			//	playerBody.Orientation.z += (float)(shipController.PointerDeltas.y / 128.f);

			playerBody.Orientation.Normalize();
		}
	}
	if(1 == solarSystem.PlayState.CountPlayers && solarSystem.ShipState.SpaceshipManager.ShipCores.size()) {
		::gpk::n3f32										shipPosition;
		solarSystem.ShipState.GetShipPosition(0, shipPosition);
		if(solarSystem.ShipState.Scene.Global.CameraMode == ::ghg::CAMERA_MODE_FOLLOW) {
			solarSystem.ShipState.Scene.Global.Camera[::ghg::CAMERA_MODE_FOLLOW].Position	= shipPosition + ::gpk::n3f32{-80.f, 25, 0};
			solarSystem.ShipState.Scene.Global.Camera[::ghg::CAMERA_MODE_FOLLOW].Target		= shipPosition + ::gpk::n3f32{1000.f, 0, 0};
			//solarSystem.ShipState.Scene.Global.Camera[::ghg::CAMERA_MODE_FOLLOW].Up			= {0, 1, 0};
		}
		else if(solarSystem.ShipState.Scene.Global.CameraMode == ::ghg::CAMERA_MODE_FRONT) {
			solarSystem.ShipState.Scene.Global.Camera[::ghg::CAMERA_MODE_FRONT].Position	= shipPosition + ::gpk::n3f32{0, 0, 0};
			solarSystem.ShipState.Scene.Global.Camera[::ghg::CAMERA_MODE_FRONT].Target		= shipPosition + ::gpk::n3f32{1, 0, 0};
			//solarSystem.ShipState.Scene.Global.Camera[::ghg::CAMERA_MODE_FRONT].Up			= {0, 1, 0};
		}
	}

	//------------------------------------------- Handle misc input
	::gpk::SCameraPoints		& camera				= solarSystem.ShipState.Scene.Global.Camera[solarSystem.ShipState.Scene.Global.CameraMode];
	if(controllers.size()) {
		if(key_camera_move_up	) camera.Position.y	+= (float)secondsLastFrame * (controllers[0].Turbo ? 20 : 10);
		if(key_camera_move_down	) camera.Position.y	-= (float)secondsLastFrame * (controllers[0].Turbo ? 20 : 10);
	}

	solarSystem.PlayState.CameraSwitchDelay	+= secondsLastFrame;
	if(key_camera_switch && solarSystem.PlayState.CameraSwitchDelay > .2) {
		solarSystem.PlayState.CameraSwitchDelay	= 0;

		solarSystem.ShipState.Scene.Global.CameraMode = (::ghg::CAMERA_MODE)((solarSystem.ShipState.Scene.Global.CameraMode + 1) % ((solarSystem.PlayState.CountPlayers > 1) ? (::ghg::CAMERA_MODE_PERSPECTIVE + 1) : ::ghg::CAMERA_MODE_COUNT));
	}

	if(camera.Position.y > 0.001f)
	if(camera.Position.y > 0.001f) if(key_camera_move_front) camera.Position.RotateZ(::gpk::math_pi * secondsLastFrame);
	if(camera.Position.x < 0.001f)
	if(camera.Position.x < 0.001f) if(key_camera_move_back) camera.Position.RotateZ(::gpk::math_pi * -secondsLastFrame);

	if(camera.Position.y < 0) camera.Position.y = 0.0001f;
	if(camera.Position.y < 0) camera.Position.y = 0.0001f;
	if(camera.Position.x > 0) camera.Position.x = -0.0001f;
	if(camera.Position.x > 0) camera.Position.x = -0.0001f;

	for(uint32_t iPlayer = 0; iPlayer < solarSystem.ShipControllers.size(); ++iPlayer) 
		solarSystem.ShipControllers[iPlayer].PointerDeltas	= {};

	return 0;
}

stacxpr	const double	UPDATE_STEP_TIME			= 0.012;

::gpk::error_t			ghg::solarSystemUpdate		(::ghg::SGalaxyHell & solarSystem, double actualSecondsLastFrame, const ::gpk::SInput & input, ::gpk::vpobj<::gpk::SSystemEvent> frameEvents)	{
	if(solarSystem.PlayState.Paused) 
		return 0;

	{
		::std::lock_guard			lockUpdate					(solarSystem.LockUpdate);
		for(uint32_t iShip = 0; iShip < solarSystem.ShipState.SpaceshipManager.ShipOrbiterActionQueue.size(); ++iShip)
			solarSystem.ShipState.SpaceshipManager.ShipOrbiterActionQueue[iShip].clear();
	}
	::gpk::n2u16				targetMetrics				= {}; 
	targetMetrics			= solarSystem.DrawCache.RenderTargetMetrics;

	double						secondsToProcess			= ::gpk::min(actualSecondsLastFrame, 0.15);
	for(uint32_t iShip = 0; iShip < solarSystem.ShipState.SpaceshipManager.ShipCores.size(); ++iShip) {
		::gpk::au16					& shipParts					= solarSystem.ShipState.SpaceshipManager.ShipParts[iShip];
		const uint32_t				iBody						= solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipCoreEntity[iShip]].Body;
		solarSystem.ShipState.Engine.Integrator.SetActive (iBody, true);
		solarSystem.ShipState.Engine.Integrator.Flags[iBody].UpdatedTransform	= false;
		for(uint32_t iPart = 0; iPart < shipParts.size(); ++iPart) {
			::gpk::SSpaceshipOrbiter				& shipPart					= solarSystem.ShipState.SpaceshipManager.Orbiters[shipParts[iPart]];
			::std::lock_guard			lockUpdate					(solarSystem.LockUpdate);
			memcpy(solarSystem.ShipState.SpaceshipManager.Shots[shipPart.Weapon].PositionDraw.begin(), solarSystem.ShipState.SpaceshipManager.Shots[shipPart.Weapon].Particles.Position.begin(), solarSystem.ShipState.SpaceshipManager.Shots[shipPart.Weapon].Particles.Position.size() * sizeof(::gpk::n3f32));
			solarSystem.ShipState.Engine.Integrator.SetActive(solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipPartEntity[shipParts[iPart]]].Body, true);
			solarSystem.ShipState.Engine.Integrator.Flags	 [solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipPartEntity[shipParts[iPart]]].Body].UpdatedTransform	= false;
		}
	}

	{
		::std::lock_guard			lockUpdate					(solarSystem.LockUpdate);
		::ghg::decoUpdate(solarSystem.DecoState, secondsToProcess, solarSystem.PlayState.RelativeSpeedCurrent, targetMetrics);
	}

	::processInput(solarSystem, secondsToProcess, input, frameEvents);
	if(solarSystem.PlayState.Slowing) {
		solarSystem.PlayState.TimeScale	-= secondsToProcess * .35;
		if(solarSystem.PlayState.TimeScale < .1)
			solarSystem.PlayState.Slowing	= false;
	}
	else {
		if(solarSystem.PlayState.TimeScale < .99)
			solarSystem.PlayState.TimeScale	= ::gpk::min(1.0, solarSystem.PlayState.TimeScale += secondsToProcess * .45);
	}
	secondsToProcess		*= solarSystem.PlayState.TimeScale;
	stacxpr	double				relativeAcceleration	= 20;
	if(solarSystem.PlayState.AccelerationControl > 0)
		solarSystem.PlayState.RelativeSpeedCurrent	+= secondsToProcess * relativeAcceleration;
	else if(solarSystem.PlayState.AccelerationControl < 0)
		solarSystem.PlayState.RelativeSpeedCurrent	-= secondsToProcess * relativeAcceleration;
	else { // == 0
		if(solarSystem.PlayState.RelativeSpeedCurrent > solarSystem.PlayState.RelativeSpeedTarget)
			solarSystem.PlayState.RelativeSpeedCurrent	-= secondsToProcess * relativeAcceleration;
		else if(solarSystem.PlayState.RelativeSpeedCurrent < solarSystem.PlayState.RelativeSpeedTarget)
			solarSystem.PlayState.RelativeSpeedCurrent	+= secondsToProcess * relativeAcceleration;
	}

	solarSystem.PlayState.TimeReal		+= actualSecondsLastFrame;
	solarSystem.PlayState.TimeRealStage	+= actualSecondsLastFrame;
	solarSystem.DecoState.AnimationTime	+= secondsToProcess;
	while(secondsToProcess > 0) {
		double						secondsLastFrame	= ::gpk::min(UPDATE_STEP_TIME, secondsToProcess);
		solarSystem.PlayState.TimeStage	+= secondsLastFrame;
		solarSystem.PlayState.TimeWorld	+= secondsLastFrame;

		solarSystem.ShipState.Engine.Integrator.Integrate(secondsLastFrame);

		bool						playing				= shipsUpdate(solarSystem, secondsLastFrame);
		if(false == playing) {
			playing					= false;
			for(uint32_t iExplosion = 0; iExplosion < solarSystem.DecoState.Explosions.size(); ++iExplosion) {
				if(solarSystem.DecoState.Explosions[iExplosion].Slices.size()) {
					playing					= true;
					break;
				}
			}
			if(false == playing)  {	// Set up enemy ships
				::ghg::stageSetup(solarSystem);
				playing					= true;
				break;
			}
		}

		for(uint32_t iEntity = 0; iEntity < solarSystem.ShipState.EntitySystem.Entities.size(); ++iEntity) {
			const ::ghg::SGHEntity		& entity			= solarSystem.ShipState.EntitySystem.Entities[iEntity];
			if(-1 == entity.Parent)	// process root entities
				::updateEntityTransforms(iEntity, solarSystem.ShipState.EntitySystem.Entities, solarSystem.ShipState.EntitySystem.EntityChildren, solarSystem.ShipState.Scene, solarSystem.ShipState.Engine.Integrator);
		}
		secondsToProcess		-= secondsLastFrame;
	}
	solarSystem.ShipState.Scene.Global.LightVector.Normalize();

	::gpk::m4f32				& matrixProjection		= solarSystem.ShipState.Scene.Global.MatrixProjection;

	//if(targetMetrics.x >= (double)targetMetrics.y)
		matrixProjection.FieldOfView(::gpk::math_pi * .25, targetMetrics.x / (double)targetMetrics.y, 0.1, 5000.0);
	//else
	//	matrixProjection.FieldOfView(::gpk::math_pi * .25, targetMetrics.y / (double)targetMetrics.x, 0.1, 5000.0);
	::gpk::m4f32				matrixViewport			= {};
	matrixViewport.ViewportLH(targetMetrics.u16());
	matrixProjection		*= matrixViewport;

	//solarSystem.Scene.LightVector	= solarSystem.Scene.LightVector.RotateY(secondsLastFrame * 2);
	return 0;
}
