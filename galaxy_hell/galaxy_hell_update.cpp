#include "gpk_galaxy_hell.h"

#include "gpk_collision.h"

#include <Windows.h>
#include <mmsystem.h>

static	int											applyDamage
	( const int32_t						weaponDamage
	, int32_t							& healthPart
	, int32_t							& healthParent
	) {
	const uint32_t											finalDamage					= ::gpk::min(weaponDamage, healthPart);
	healthPart											-= finalDamage;
	healthParent										-= finalDamage;
	return 0 >= healthPart;
}

static	int											collisionDetect		(::ghg::SShots & shots, const ::gpk::SCoord3<float> & modelPosition, ::gpk::array_pod<::gpk::SCoord3<float>> & collisionPoints)	{
	bool													detected			= false;
	collisionPoints.clear();
	for(uint32_t iShot = 0; iShot < shots.Particles.Position.size(); ++iShot) {
		const ::gpk::SLine3<float>								shotSegment			= {shots.PositionPrev[iShot], shots.Particles.Position[iShot]};
		float													t					= 0;
		::gpk::SCoord3<float>									collisionPoint		= {};
		if( ::gpk::intersectRaySphere(shotSegment.A, (shotSegment.B - shotSegment.A).Normalize(), {1.2, modelPosition}, t, collisionPoint)
			&& t < 1
		) {
			detected											= true;
			collisionPoints.push_back(collisionPoint);
			shots.Remove(iShot);
			--iShot;
		}
	}
	return 0;
}

static	int											handleCollisionPoint	(::ghg::SGalaxyHell & solarSystem, int32_t weaponDamage, uint32_t & attackerScore, ::ghg::SShipPart& damagedPart, ::ghg::SShipCore & damagedShip, int32_t iAttackedShip, const ::gpk::SCoord3<float> & sphereCenter, const ::gpk::SCoord3<float> & collisionPoint)	{
	solarSystem.ShipState.ShipPartActionQueue[iAttackedShip].push_back(::ghg::SHIP_ACTION_hit);
	const ::gpk::SCoord3<float>								bounceVector				= (collisionPoint - sphereCenter).Normalize();
	solarSystem.DecoState.Debris.SpawnDirected(5, 0.3, bounceVector, collisionPoint, 50, 1);
	attackerScore										+= 1;
	if(::applyDamage(weaponDamage, damagedPart.Health, damagedShip.Health)) {	// returns true if health reaches zero
		attackerScore										+= 10;
		const ::ghg::SEntity									& entityGeometry			= solarSystem.ShipState.EntitySystem.Entities[damagedPart.Entity + 1];
		const int32_t											indexMesh					= entityGeometry.Geometry;
		const uint32_t											countTriangles				= solarSystem.ShipState.Scene.Geometry[indexMesh].Triangles.size();
		::ghg::decoExplosionAdd(solarSystem.DecoState.Explosions, indexMesh, countTriangles, collisionPoint, 60);
		solarSystem.DecoState.Debris.SpawnSpherical(30, collisionPoint, 60, 2);
		if(0 >= damagedShip.Health) {
			attackerScore										+= 50;
			const ::gpk::SCoord3<float>								& parentPosition			= solarSystem.ShipState.GetShipPosition(damagedShip);
			::ghg::decoExplosionAdd(solarSystem.DecoState.Explosions, indexMesh, countTriangles, parentPosition, 13);
			solarSystem.DecoState.Debris.SpawnSpherical(150, parentPosition, 13, 2.8f);
			solarSystem.PlayState.Slowing						= true;
			solarSystem.PlayState.TimeScale						= 1.0;
		}
		return 1;
	}
	return 0;
}

static	int											updateEntityTransforms		(uint32_t iEntity, ::gpk::array_obj<::ghg::SEntity> & entities, ::gpk::array_obj<::gpk::array_pod<uint32_t>> & entitiesChildren, ::ghg::SShipScene & scene, ::gpk::SIntegrator3 & bodies)	{
	const ::ghg::SEntity									& entity					= entities[iEntity];
	if(-1 == entity.Body) {
		if(-1 == entity.Parent)
			scene.Transforms[iEntity].Identity();
		else
			scene.Transforms[iEntity]							= scene.Transforms[entity.Parent];
	}
	else {
		::gpk::SMatrix4<float>									& matrixGlobal					= scene.Transforms[iEntity];
		bodies.GetTransform(entity.Body, matrixGlobal);
		if(-1 != entity.Parent)
			matrixGlobal										*= scene.Transforms[entity.Parent];
	}

	const ::gpk::array_pod<uint32_t>						& entityChildren			= entitiesChildren[iEntity];
	for(uint32_t iChild = 0; iChild < entityChildren.size(); ++iChild) {
		const uint32_t											iChildEntity				= entityChildren[iChild];
		::updateEntityTransforms(iChildEntity, entities, entitiesChildren, scene, bodies);
	}
	return 0;
}

static	int											updateShots				(::ghg::SGalaxyHell & solarSystem, double secondsLastFrame)	{
	for(uint32_t iShip = 0; iShip < solarSystem.ShipState.ShipCores.size(); ++iShip) {
		::gpk::array_pod<uint32_t>									& shipParts				= solarSystem.ShipState.ShipCoresParts[iShip];
		for(uint32_t iPart = 0; iPart < shipParts.size(); ++iPart)
			solarSystem.ShipState.Shots[solarSystem.ShipState.ShipParts[shipParts[iPart]].Weapon].Update((float)secondsLastFrame);
	}

	::gpk::array_pod<::gpk::SCoord3<float>>					collisionPoints;
	for(uint32_t iShipAttacker = 0; iShipAttacker < solarSystem.ShipState.ShipCores.size(); ++iShipAttacker) {
		::ghg::SShipCore										& shipAttacker					= solarSystem.ShipState.ShipCores					[iShipAttacker];
		::gpk::array_pod<uint32_t>								& shipAttackerParts				= solarSystem.ShipState.ShipCoresParts				[iShipAttacker];
		//::gpk::array_pod<::gpk::SCoord3<float>>					& shipDistancesToTarget			= solarSystem.ShipState.ShipCoresDistanceToTargets	[iShipAttacker];
		for(uint32_t iPartAttacker = 0; iPartAttacker < shipAttackerParts.size(); ++iPartAttacker) {
			::ghg::SShipPart										& shipPartAttacker		= solarSystem.ShipState.ShipParts[shipAttackerParts[iPartAttacker]];

			for(uint32_t iShipAttacked = 0; iShipAttacked < solarSystem.ShipState.ShipCores.size(); ++iShipAttacked) {
				::ghg::SShipCore										& shipAttacked					= solarSystem.ShipState.ShipCores[iShipAttacked];
				::gpk::array_pod<uint32_t>								& shipAttackedParts				= solarSystem.ShipState.ShipCoresParts[iShipAttacked];
				if(shipAttacked.Health <= 0 || shipAttacker.Team == shipAttacked.Team) // avoid dead ships and ships of the same team
					continue;
				for(uint32_t iPartAttacked = 0; iPartAttacked < shipAttackedParts.size(); ++iPartAttacked) {
					::ghg::SShipPart										& shipPartAttacked		= solarSystem.ShipState.ShipParts[shipAttackedParts[iPartAttacked]];
					if(shipPartAttacked.Health <= 0)
						continue;
					const ::ghg::SWeapon									& weaponAttacker			= solarSystem.ShipState.Weapons[shipPartAttacker.Weapon];
					const ::ghg::SEntity									& entity					= solarSystem.ShipState.EntitySystem.Entities[shipPartAttacked.Entity];
					if(-1 != entity.Geometry) {
						const ::gpk::SCoord3<float>								attackedPosition		= solarSystem.ShipState.Scene.Transforms[shipPartAttacked.Entity].GetTranslation();
						::collisionDetect(solarSystem.ShipState.Shots[shipPartAttacker.Weapon], attackedPosition, collisionPoints);
						for(uint32_t iCollisionPoint = 0; iCollisionPoint < collisionPoints.size(); ++iCollisionPoint)
							if(::handleCollisionPoint(solarSystem, weaponAttacker.Damage, shipAttacker.Score, shipPartAttacked, shipAttacked, iShipAttacked, attackedPosition, collisionPoints[iCollisionPoint]))	// returns true if part health reaches zero.
								break;
					}
					const ::gpk::array_pod<uint32_t>						& entityChildren			= solarSystem.ShipState.EntitySystem.EntityChildren[shipPartAttacked.Entity];
					for(uint32_t iEntity = 0; iEntity < entityChildren.size(); ++iEntity) {
						::ghg::SEntity											& entityChild				= solarSystem.ShipState.EntitySystem.Entities[entityChildren[iEntity]];
						if(-1 == entityChild.Parent)
							continue;

						if(-1 == entityChild.Geometry)
							continue;

						const ::gpk::SCoord3<float>								attackedPosition		= solarSystem.ShipState.Scene.Transforms[entityChildren[iEntity]].GetTranslation();
						::collisionDetect(solarSystem.ShipState.Shots[shipPartAttacker.Weapon], attackedPosition, collisionPoints);
						for(uint32_t iCollisionPoint = 0; iCollisionPoint < collisionPoints.size(); ++iCollisionPoint)
							if(int32_t score = ::handleCollisionPoint(solarSystem, weaponAttacker.Damage, shipAttacker.Score, shipPartAttacked, shipAttacked, iShipAttacked, attackedPosition, collisionPoints[iCollisionPoint])) {	// returns true if part health reaches zero.
								shipAttacker.Score += score;
								break;
							}
					}
				}
			}

			::ghg::SWeapon	& weaponAttacker	= solarSystem.ShipState.Weapons[shipPartAttacker.Weapon];
			::ghg::SShots	& shotsAttacker		= solarSystem.ShipState.Shots[shipPartAttacker.Weapon];
			if(weaponAttacker.Load == ::ghg::WEAPON_LOAD_Rocket) {
				for(uint32_t iShot = 0; iShot < shotsAttacker.Particles.Position.size(); ++iShot) {
					const ::gpk::array_pod<::gpk::SCoord3<float>> & distances =  shotsAttacker.DistanceToTargets[iShot];
					if(0 == distances.size())
						continue;

					if(2.5f > shotsAttacker.Lifetime[iShot])
						continue;

					for(uint32_t iTarget = iShot % distances.size(); iTarget < distances.size(); ++iTarget) {
						::gpk::SCoord3<float>			target = distances[iTarget];
						target.Normalize();
						if(shotsAttacker.Particles.Direction[iShot].Dot(target) > 0) {
							shotsAttacker.Particles.Direction[iShot] = ::gpk::interpolate_linear(shotsAttacker.Particles.Direction[iShot], distances[iTarget], ::gpk::min(secondsLastFrame * 2, 1.0)).Normalize();
							break;
						}
					}
				}
			}
			else if(weaponAttacker.Load == ::ghg::WEAPON_LOAD_Missile) {
				for(uint32_t iShot = 0; iShot < shotsAttacker.Particles.Position.size(); ++iShot) {
					const ::gpk::array_pod<::gpk::SCoord3<float>> & distances =  shotsAttacker.DistanceToTargets[iShot];
					if(0 == distances.size())
						continue;

					if(2.5f > shotsAttacker.Lifetime[iShot])
						continue;

					shotsAttacker.Particles.Direction[iShot] = ::gpk::interpolate_linear(shotsAttacker.Particles.Direction[iShot], distances[iShot % distances.size()], ::gpk::min(secondsLastFrame * 1, 1.0)).Normalize();
				}
			}

		}
	}
	return 0;
}

static	int											updateDistancesToTargets	(::ghg::SGalaxyHell & solarSystem, int32_t team, ::ghg::SShipPart & shipPart, ::gpk::array_pod<::gpk::SCoord3<float>> & shipPartDistancesToTargets, ::ghg::SShots & shots)	{
	shipPartDistancesToTargets.clear();
	for(uint32_t iShot = 0; iShot < shots.Particles.Position.size(); ++iShot)
		shots.DistanceToTargets[iShot].clear();

	::gpk::array_pod<::ghg::SShipPart			>				& shipParts						= solarSystem.ShipState.ShipParts					;
	::gpk::array_pod<::ghg::SShipCore			>				& shipCores						= solarSystem.ShipState.ShipCores					;
	::gpk::array_obj<::gpk::array_pod<uint32_t>	>				& shipCoresParts				= solarSystem.ShipState.ShipCoresParts				;
	const ::gpk::SCoord3<float>									weaponPosition					= solarSystem.ShipState.Scene.Transforms[shipPart.Entity].GetTranslation();
	for(uint32_t iShip = 0; iShip < shipCores.size(); ++iShip) {
		::ghg::SShipCore										& ship					= shipCores[iShip];
		if(ship.Team == team || ship.Health <= 0)
			continue;

		for(uint32_t iPart = 0; iPart < shipCoresParts[iShip].size(); ++iPart) {
			::ghg::SShipPart										& shipPartTarget				= shipParts[shipCoresParts[iShip][iPart]];
			if(0 >= shipPartTarget.Health)
				continue;

			const ::gpk::SCoord3<float>								targetPosition					= solarSystem.ShipState.Scene.Transforms[shipPartTarget.Entity].GetTranslation();
			shipPartDistancesToTargets.push_back(targetPosition - weaponPosition);
			for(uint32_t iShot = 0; iShot < shots.Particles.Position.size(); ++iShot) {
				shots.DistanceToTargets[iShot].push_back(targetPosition - shots.Particles.Position[iShot]);
			}
		}
	}
	return 0;
}


static	int											updateShipPart				(::ghg::SGalaxyHell & solarSystem, int32_t team, ::ghg::SShipPart & shipPart, ::gpk::array_pod<::gpk::SCoord3<float>> & shipPartDistancesToTargets, double secondsLastFrame)	{
	::ghg::SWeapon											& weapon					= solarSystem.ShipState.Weapons	[shipPart.Weapon];
	::ghg::SShots											& shots						= solarSystem.ShipState.Shots	[shipPart.Weapon];
	for(uint32_t iParticle = 0; iParticle < shots.Particles.Position.size(); ++iParticle)
		shots.Particles.Position[iParticle].x				-= (float)(solarSystem.PlayState.RelativeSpeedCurrent * secondsLastFrame * .2);

	weapon.Delay										+= (float)secondsLastFrame;

	if(weapon.Load == ::ghg::WEAPON_LOAD_Rocket || weapon.Load == ::ghg::WEAPON_LOAD_Missile || weapon.Load == ::ghg::WEAPON_LOAD_Cannonball) {
		updateDistancesToTargets(solarSystem, team, shipPart, shipPartDistancesToTargets, shots); 
	}

	weapon.Overheat										-= (float)secondsLastFrame;
	if(weapon.CoolingDown) {
		if(0 >= weapon.Overheat)
			weapon.CoolingDown									= false;
	}
	else {

		const ::gpk::SMatrix4<float>							& shipModuleMatrix			= solarSystem.ShipState.Scene.Transforms[solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Transform];
		::gpk::SCoord3<float>									positionGlobal				= shipModuleMatrix.GetTranslation();
		::gpk::SCoord3<float>									targetDistance				= shipPartDistancesToTargets.size() ? shipPartDistancesToTargets[0] : ::gpk::SCoord3<float>{};
		::gpk::SCoord3<float>									targetPosition				= targetDistance + positionGlobal;
		if(weapon.Type == ::ghg::WEAPON_TYPE_Cannon) {
			if(1 < targetDistance.LengthSquared()) {
				::gpk::SCoord3<float>									direction					= targetDistance;
				direction.Normalize();
				if(weapon.Load == ::ghg::WEAPON_LOAD_Rocket)
					weapon.SpawnDirected(shots, 1, positionGlobal, direction, weapon.Speed, 1, weapon.ShotLifetime);
				else if(weapon.Load == ::ghg::WEAPON_LOAD_Cannonball)
					weapon.SpawnDirected(shots, 1, positionGlobal, direction, weapon.Speed, 1, weapon.ShotLifetime);
				else if(weapon.Load == ::ghg::WEAPON_LOAD_Missile)
					weapon.SpawnDirected(shots, 1, positionGlobal, direction, weapon.Speed, 1, weapon.ShotLifetime);

				if(solarSystem.PlayState.TimeStage > 1) {
					solarSystem.ShipState.ShipPhysics.Forces[solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Body].Rotation	= {};
					::gpk::SMatrix4<float>									inverseTransform			= shipModuleMatrix.GetInverse();
					::gpk::STransform3										& shipModuleTransform		= solarSystem.ShipState.GetShipPartTransform(shipPart);
					::gpk::SCoord3<float>									up							= {1, 0, 0};
					::gpk::SCoord3<float>									front						= {0, 1, 0};
					//const ::gpk::SMatrix4<float>							& shipMatrix				= solarSystem.Scene.Transforms[solarSystem.Entities[solarSystem.Entities[shipPart.Entity].Parent].Transform];
					targetPosition										= inverseTransform.Transform(targetPosition);
					::gpk::SQuaternion<float>								q;
					q.LookAt(shipModuleTransform.Position, targetPosition, up, front);
					shipModuleTransform.Orientation						= q;
				}
			}
		}
		else if(weapon.Type == ::ghg::WEAPON_TYPE_Gun) {
			::gpk::SCoord3<float>									direction				= {team ? -1.0f : 1.0f, 0, 0};
			if(weapon.Load == ::ghg::WEAPON_LOAD_Ray)
				weapon.Create(shots, positionGlobal, direction, weapon.Speed, .75f, weapon.ShotLifetime);
			else
				weapon.SpawnDirected(shots, weapon.ParticleCount, positionGlobal, direction, weapon.Speed, 5.0f, weapon.ShotLifetime);
		}
		else if(weapon.Type == ::ghg::WEAPON_TYPE_Shotgun) {
			::gpk::SCoord3<float>									direction				= {team ? -1.0f : 1.0f, 0, 0};
			if(weapon.Load == ::ghg::WEAPON_LOAD_Ray)
				weapon.SpawnDirected(shots, weapon.ParticleCount, 1, positionGlobal, direction, weapon.Speed, .75f, weapon.ShotLifetime);
			else
				weapon.SpawnDirected(shots, weapon.ParticleCount, 1, positionGlobal, direction, weapon.Speed, 5.0f, weapon.ShotLifetime);
		}
	}
	if(weapon.Overheat >= weapon.Cooldown) {
		weapon.CoolingDown						= true;
	}
	return 0;
}

//static constexpr const double						UPDATE_STEP_TIME		= 0.012;

int													shipsUpdate				(::ghg::SGalaxyHell & solarSystem, double secondsLastFrame)	{
	bool													playing					= 0;
	{
		::std::lock_guard<::std::mutex>							lockUpdate			(solarSystem.LockUpdate);
		::updateShots(solarSystem, secondsLastFrame);
	}
	for(uint32_t iShip = 0; iShip < solarSystem.ShipState.ShipCores.size(); ++iShip) {
		::ghg::SShipCore										& ship					= solarSystem.ShipState.ShipCores[iShip];
		if(0 >= ship.Health)
			continue;
		::gpk::array_pod<uint32_t>								& shipParts				= solarSystem.ShipState.ShipCoresParts[iShip];
		::gpk::STransform3										& shipTransform			= solarSystem.ShipState.ShipPhysics.Transforms[solarSystem.ShipState.EntitySystem.Entities[ship.Entity].Body];
		if(ship.Team) { 
			playing												= 1;

			shipTransform.Position.z							= (float)(sin(iShip + solarSystem.PlayState.TimeWorld) * (iShip * 5.0) * ((iShip % 2) ? -1 : 1));
			shipTransform.Position.x							= (float)((iShip * 5.0) - solarSystem.PlayState.Stage + 10 - (solarSystem.PlayState.RelativeSpeedCurrent * solarSystem.PlayState.RelativeSpeedCurrent * .0005 * ((solarSystem.PlayState.RelativeSpeedCurrent >= 0) ? 1 : -1)  ));
			double													timeWaveVertical					= .1;

			if(0 == (solarSystem.PlayState.Stage % 7)) {
				if(iShip % 2)
					shipTransform.Position.z							= (float)(cos(iShip + solarSystem.PlayState.TimeWorld) * ((solarSystem.ShipState.ShipCores.size() - 1 - iShip) * 4.0) * ((iShip % 2) ? -1 : 1));
				else
					shipTransform.Position.z							= (float)(sin(iShip + solarSystem.PlayState.TimeWorld) * (iShip * 4.0) * ((iShip % 2) ? -1 : 1));
						if(0 == (iShip % 2)) timeWaveVertical	= .65;
				else if(0 == (iShip % 3)) timeWaveVertical	= .80;
				else if(0 == (iShip % 7)) timeWaveVertical	= .80;
			}
			else if(0 == (solarSystem.PlayState.Stage % 5)) {
					shipTransform.Position.z							= (float)(cos(iShip + solarSystem.PlayState.TimeWorld) * ((solarSystem.ShipState.ShipCores.size() - 1 - iShip) * 3.0) * ((iShip % 2) ? -1 : 1));
						if(0 == (iShip % 2)) timeWaveVertical	= .50;
				else if(0 == (iShip % 3)) timeWaveVertical	= .75;
				else if(0 == (iShip % 7)) timeWaveVertical	= .80;
			}
			else if(0 == (solarSystem.PlayState.Stage % 3)) {
					shipTransform.Position.z							= (float)(cos(iShip + solarSystem.PlayState.TimeWorld) * ((solarSystem.ShipState.ShipCores.size() - 1 - iShip) * 2.0) * ((iShip % 2) ? -1 : 1));
						if(0 == (iShip % 2)) timeWaveVertical	= .25;
				else if(0 == (iShip % 3)) timeWaveVertical	= .50;
				else if(0 == (iShip % 7)) timeWaveVertical	= .75;
			}
			else {
						if(0 == (iShip % 2)) timeWaveVertical	= .50;
				else if(0 == (iShip % 3)) timeWaveVertical	= .25;
				else if(0 == (iShip % 7)) timeWaveVertical	= .15;
			}
			shipTransform.Position.x								+= (float)(sin(solarSystem.PlayState.TimeWorld * timeWaveVertical * ::gpk::math_2pi) * ((iShip % 2) ? -1 : 1));
		}
		{
			::std::lock_guard<::std::mutex>							lockUpdate			(solarSystem.LockUpdate);
			for(uint32_t iPart = 0; iPart < shipParts.size(); ++iPart) {
				::ghg::SShipPart										& shipPart				= solarSystem.ShipState.ShipParts[shipParts[iPart]];
				if(0 >= shipPart.Health)
					continue;
				::updateShipPart(solarSystem, ship.Team, shipPart, solarSystem.ShipState.ShipPartsDistanceToTargets[shipParts[iPart]], secondsLastFrame);
			}
		}
	}
	return playing;
}


static int											processInput			(::ghg::SGalaxyHell & solarSystem, double actualSecondsLastFrame, double secondsLastFrame, const ::gpk::SInput & input, const ::gpk::view_array<::gpk::SSysEvent> & frameEvents) {
	(void)frameEvents;
	const bool												key_up					= input.KeyboardCurrent.KeyState['W'] || input.KeyboardCurrent.KeyState[VK_UP		];
	const bool												key_down				= input.KeyboardCurrent.KeyState['S'] || input.KeyboardCurrent.KeyState[VK_DOWN		];
	const bool												key_left				= input.KeyboardCurrent.KeyState['A'] || input.KeyboardCurrent.KeyState[VK_LEFT		];
	const bool												key_right				= input.KeyboardCurrent.KeyState['D'] || input.KeyboardCurrent.KeyState[VK_RIGHT	];
	const bool												key_turbo				= input.KeyboardCurrent.KeyState[VK_SHIFT	];
	const bool												key_rotate_left			= input.KeyboardCurrent.KeyState[VK_NUMPAD8	];
	const bool												key_rotate_right		= input.KeyboardCurrent.KeyState[VK_NUMPAD2	];
	const bool												key_rotate_front		= input.KeyboardCurrent.KeyState[VK_NUMPAD6	];
	const bool												key_rotate_back			= input.KeyboardCurrent.KeyState[VK_NUMPAD4	];
	const bool												key_rotate_reset		= input.KeyboardCurrent.KeyState[VK_NUMPAD5	];
	const bool												key_camera_switch		= input.KeyboardCurrent.KeyState['C'		];
	const bool												key_camera_move_front	= input.KeyboardCurrent.KeyState[VK_HOME	];
	const bool												key_camera_move_back	= input.KeyboardCurrent.KeyState[VK_END		];
	const bool												key_camera_move_up		= input.KeyboardCurrent.KeyState['E'		];
	const bool												key_camera_move_down	= input.KeyboardCurrent.KeyState['Q'		];

	//------------------------------------------- Handle input
	::gpk::SCamera											& camera				= solarSystem.ShipState.Scene.Global.Camera[solarSystem.ShipState.Scene.Global.CameraMode];
	if(key_camera_move_up	) camera.Position.y	+= (float)secondsLastFrame * (key_turbo ? 20 : 10);
	if(key_camera_move_down	) camera.Position.y	-= (float)secondsLastFrame * (key_turbo ? 20 : 10);
	solarSystem.PlayState.CameraSwitchDelay							+= actualSecondsLastFrame;
	if(key_camera_switch && solarSystem.PlayState.CameraSwitchDelay > .2) {
		solarSystem.PlayState.CameraSwitchDelay								= 0;

		solarSystem.ShipState.Scene.Global.CameraMode = (::ghg::CAMERA_MODE)((solarSystem.ShipState.Scene.Global.CameraMode + 1) % ::ghg::CAMERA_MODE_COUNT);
	}
	if(solarSystem.ShipState.EntitySystem.Entities.size()) {
		::gpk::STransform3										& playerBody			= solarSystem.ShipState.ShipPhysics.Transforms[solarSystem.ShipState.EntitySystem.Entities[0].Body];
		double													speed							= 10;
		{
				 if(key_up	) { playerBody.Position.x			+= (float)(secondsLastFrame * speed * (key_turbo ? 2 : 8)); solarSystem.PlayState.AccelerationControl	= +1; }
			else if(key_down) { playerBody.Position.x			-= (float)(secondsLastFrame * speed * (key_turbo ? 2 : 8)); solarSystem.PlayState.AccelerationControl	= -1; }
			else
				solarSystem.PlayState.AccelerationControl	= 0;

			if(key_left	) { playerBody.Position.z			+= (float)(secondsLastFrame * speed * (key_turbo ? 2 : 8)); }
			if(key_right) { playerBody.Position.z			-= (float)(secondsLastFrame * speed * (key_turbo ? 2 : 8)); }
		}
		if(solarSystem.ShipState.Scene.Global.CameraMode == ::ghg::CAMERA_MODE_FOLLOW) {
			solarSystem.ShipState.Scene.Global.Camera[::ghg::CAMERA_MODE_FOLLOW].Position	= playerBody.Position + ::gpk::SCoord3<float>{-80.f, 25, 0};
			solarSystem.ShipState.Scene.Global.Camera[::ghg::CAMERA_MODE_FOLLOW].Target	= playerBody.Position + ::gpk::SCoord3<float>{1000.f, 0, 0};
			solarSystem.ShipState.Scene.Global.Camera[::ghg::CAMERA_MODE_FOLLOW].Up		= {0, 1, 0};
		}
		if(key_rotate_reset)
			playerBody.Orientation.MakeFromEulerTaitBryan({0, 0, (float)-::gpk::math_pi_2});
		else {
			if(key_rotate_left	) playerBody.Orientation.z -= (float)(secondsLastFrame * (key_turbo ? 8 : 2));
			if(key_rotate_right	) playerBody.Orientation.z += (float)(secondsLastFrame * (key_turbo ? 8 : 2));
			if(key_rotate_front	) playerBody.Orientation.x -= (float)(secondsLastFrame * (key_turbo ? 8 : 2));
			if(key_rotate_back	) playerBody.Orientation.x += (float)(secondsLastFrame * (key_turbo ? 8 : 2));
		}

		playerBody.Orientation.Normalize();
	}


	if(camera.Position.y > 0.001f)
	if(camera.Position.y > 0.001f) if(key_camera_move_front) camera.Position.RotateZ(::gpk::math_pi * secondsLastFrame);
	if(camera.Position.x < 0.001f)
	if(camera.Position.x < 0.001f) if(key_camera_move_back) camera.Position.RotateZ(::gpk::math_pi * -secondsLastFrame);

	if(camera.Position.y < 0) camera.Position.y = 0.0001f;
	if(camera.Position.y < 0) camera.Position.y = 0.0001f;
	if(camera.Position.x > 0) camera.Position.x = -0.0001f;
	if(camera.Position.x > 0) camera.Position.x = -0.0001f;
	return 0;
}

static constexpr const double						UPDATE_STEP_TIME		= 0.012;

int													ghg::solarSystemUpdate				(::ghg::SGalaxyHell & solarSystem, double actualSecondsLastFrame, const ::gpk::SInput & input, const ::gpk::view_array<::gpk::SSysEvent> & frameEvents)	{
	if(solarSystem.PlayState.Paused) 
		return 0;

	for(uint32_t iShip = 0; iShip < solarSystem.ShipState.ShipPartActionQueue.size(); ++iShip)
		solarSystem.ShipState.ShipPartActionQueue[iShip].clear();

	::gpk::SCoord2<uint16_t>								targetMetrics = {}; 
	targetMetrics										= solarSystem.DrawCache.RenderTargetMetrics;

	for(uint32_t iSysEvent = 0; iSysEvent < frameEvents.size(); ++iSysEvent) {
		const ::gpk::SSysEvent									& eventToProcess					= frameEvents[iSysEvent];
		switch(eventToProcess.Type) {
		case ::gpk::SYSEVENT_RESIZE:
			break;
		}
	}

	double													secondsToProcess				= ::gpk::min(actualSecondsLastFrame, 0.15);

	{
		::std::lock_guard<::std::mutex>							lockUpdate			(solarSystem.LockUpdate);
		::ghg::decoUpdate(solarSystem.DecoState, secondsToProcess, solarSystem.PlayState.RelativeSpeedCurrent, targetMetrics);
	}

	::processInput(solarSystem, actualSecondsLastFrame, secondsToProcess, input, frameEvents);
	if(solarSystem.PlayState.Slowing) {
		solarSystem.PlayState.TimeScale						-= secondsToProcess * .35;
		if(solarSystem.PlayState.TimeScale < .1)
			solarSystem.PlayState.Slowing						= false;
	}
	else {
		if(solarSystem.PlayState.TimeScale < .99)
			solarSystem.PlayState.TimeScale						= ::gpk::min(1.0, solarSystem.PlayState.TimeScale += secondsToProcess * .45);
	}
	secondsToProcess									*= solarSystem.PlayState.TimeScale;
	static constexpr	const double	relativeAcceleration	= 20;
	if(solarSystem.PlayState.AccelerationControl == 0) {
		if(solarSystem.PlayState.RelativeSpeedCurrent > solarSystem.PlayState.RelativeSpeedTarget)
			solarSystem.PlayState.RelativeSpeedCurrent	-= secondsToProcess * relativeAcceleration;
		else if(solarSystem.PlayState.RelativeSpeedCurrent < solarSystem.PlayState.RelativeSpeedTarget)
			solarSystem.PlayState.RelativeSpeedCurrent	+= secondsToProcess * relativeAcceleration;
	}
	else if(solarSystem.PlayState.AccelerationControl > 0) {
		solarSystem.PlayState.RelativeSpeedCurrent	+= secondsToProcess * relativeAcceleration;
	}
	else if(solarSystem.PlayState.AccelerationControl < 0) {
		solarSystem.PlayState.RelativeSpeedCurrent	-= secondsToProcess * relativeAcceleration;
	}

	solarSystem.DecoState.AnimationTime					+= secondsToProcess;
	while(secondsToProcess > 0) {
		double													secondsLastFrame				= ::gpk::min(UPDATE_STEP_TIME, secondsToProcess);
		solarSystem.PlayState.TimeStage						+= secondsLastFrame;
		solarSystem.PlayState.TimeWorld						+= secondsLastFrame;

		solarSystem.ShipState.ShipPhysics.Integrate(secondsLastFrame);

		bool					playing				= shipsUpdate(solarSystem, secondsLastFrame);
		if(false == playing) {
			playing												= false;
			for(uint32_t iExplosion = 0; iExplosion < solarSystem.DecoState.Explosions.size(); ++iExplosion) {
				if(solarSystem.DecoState.Explosions[iExplosion].Slices.size()) {
					playing											= true;
					break;
				}
			}
			if(false == playing)  {	// Set up enemy ships
				::std::lock_guard<::std::mutex>							lockUpdate			(solarSystem.LockUpdate);
				::ghg::stageSetup(solarSystem);
				playing												= true;
				break;
			}
		}

		for(uint32_t iEntity = 0; iEntity < solarSystem.ShipState.EntitySystem.Entities.size(); ++iEntity) {
			const ::ghg::SEntity									& entity					= solarSystem.ShipState.EntitySystem.Entities[iEntity];
			if(-1 == entity.Parent)	// process root entities
				updateEntityTransforms(iEntity, solarSystem.ShipState.EntitySystem.Entities, solarSystem.ShipState.EntitySystem.EntityChildren, solarSystem.ShipState.Scene, solarSystem.ShipState.ShipPhysics);
		}
		secondsToProcess									-= UPDATE_STEP_TIME;
	}
	solarSystem.ShipState.Scene.Global.LightVector.Normalize();

	::gpk::SMatrix4<float>						& matrixProjection	= solarSystem.ShipState.Scene.Global.MatrixProjection;

	//if(targetMetrics.x >= (double)targetMetrics.y)
		matrixProjection.FieldOfView(::gpk::math_pi * .25, targetMetrics.x / (double)targetMetrics.y, 0.01, 500);
	//else
	//	matrixProjection.FieldOfView(::gpk::math_pi * .25, targetMetrics.y / (double)targetMetrics.x, 0.01, 500);
	::gpk::SMatrix4<float>						matrixViewport		= {};
	matrixViewport.ViewportLH(targetMetrics.Cast<uint32_t>());
	matrixProjection						*= matrixViewport;

	//solarSystem.Scene.LightVector									= solarSystem.Scene.LightVector.RotateY(secondsLastFrame * 2);
	return 0;
}
