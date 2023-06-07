#include "gpk_particle.h"
#include "gpk_enum.h"

#ifndef GPK_GALAXY_HELL_WEAPON_H
#define GPK_GALAXY_HELL_WEAPON_H

namespace ghg 
{
	struct SShots	{
		::gpk::aobj<::gpk::apod<::gpk::n3f32>>	DistanceToTargets	= {};
		::gpk::af32					Lifetime			= {};
		::gpk::af32					Brightness			= {};
		::gpk::apod<::gpk::n3f32>	PositionDraw		= {};
		::gpk::apod<::gpk::n3f32>	PositionPrev		= {};
		::gpk::SParticles3			Particles			= {};

		int							Remove				(uint32_t iShot)			{
			Particles.Remove(iShot);
			DistanceToTargets	.remove_unordered(iShot);
			Lifetime			.remove_unordered(iShot);
			Brightness			.remove_unordered(iShot);
			PositionPrev		.remove_unordered(iShot);
			return PositionDraw	.remove_unordered(iShot);
		}

		int							SpawnForced			(const ::gpk::n3f32 & position, const ::gpk::n3f32 & direction, float speed, float brightness, float lifetime)	{
			PositionDraw.push_back(position);
			PositionPrev.push_back(position);
			Brightness	.push_back(brightness);
			Lifetime	.push_back(lifetime);
			DistanceToTargets[DistanceToTargets.push_back({})].push_back(direction);
			return Particles.Create(position, direction, speed);
		}

		int							SpawnForcedDirected	(double stabilityFactor, const ::gpk::n3f & position, const ::gpk::n3f & direction, float speedDebris, float brightness, float lifetime)	{
			stacxpr	double					randUnit			= ::gpk::math_2pi / RAND_MAX;
			::gpk::n3f						finalDirection		= {0, 1, 0};
			finalDirection.RotateX(rand() * randUnit);
			finalDirection.RotateY(rand() * randUnit);
			finalDirection.Normalize();
			return SpawnForced(position, ::gpk::interpolate_linear(finalDirection, direction, stabilityFactor), speedDebris, brightness, lifetime);
		}

		::gpk::error_t				Update				(float secondsLastFrame)	{
			stacxpr	uint32_t				maxRange			= 200;
			stacxpr	uint32_t				maxRangeSquared		= maxRange * maxRange;
			memcpy(PositionPrev.begin(), Particles.Position.begin(), Particles.Position.size() * sizeof(::gpk::n3f));
			Particles.IntegrateSpeed(secondsLastFrame);
			for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
				Lifetime[iShot] -= secondsLastFrame;
				if(0 >= Lifetime[iShot])
					Remove(iShot--);
				//if (Particles.Position[iShot].LengthSquared() > maxRangeSquared)
				//	Remove(iShot--);
			}
			return 0;
		}

		::gpk::error_t				Save				(::gpk::au8 & output)	const	{ 
			gpk_necs(Particles.Save(output));
			gpk_necs(::gpk::saveView(output, PositionDraw	));
			gpk_necs(::gpk::saveView(output, PositionPrev	));
			gpk_necs(::gpk::saveView(output, Brightness		));
			gpk_necs(::gpk::saveView(output, Lifetime		));
			for(uint32_t iParticle = 0; iParticle < Particles.Position.size(); ++iParticle) 
				gpk_necs(::gpk::saveView(output, DistanceToTargets[iParticle]));
			
			return 0; 
		}

		::gpk::error_t				Load				(::gpk::vcu8 & input)			{ 
			gpk_necs(Particles.Load(input));
			gpk_necs(::gpk::loadView(input, PositionPrev	));
			gpk_necs(::gpk::loadView(input, PositionDraw	));
			gpk_necs(::gpk::loadView(input, Brightness		));
			gpk_necs(::gpk::loadView(input, Lifetime		));
			gpk_necs(DistanceToTargets.resize(Particles.Position.size()));
			for(uint32_t iParticle = 0; iParticle < DistanceToTargets.size(); ++iParticle)
				gpk_necs(::gpk::loadView(input, DistanceToTargets[iParticle]));

			return 0;
		}
	};

#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE(WEAPON_DAMAGE, uint16_t);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Pierce	, 0x0);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Impact	, 0x1);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Wave		, 0x2);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Burn		, 0x4);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Shock		, 0x8);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Magnetic	, 0x10);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Radiation	, 0x20);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Poison	, 0x40);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Plasma	, 0x80);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Light		, 0x100);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Sound		, 0x200);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, EMP		, 0x400);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Gravity	, 0x800);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Health	, 0x1000);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Speed		, 0x2000);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Power		, 0x4000);

	GDEFINE_ENUM_TYPE(WEAPON_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Gun			, 0);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Cannon		, 1);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Rocket		, 2);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Missile		, 3);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Torpedo		, 4);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Shotgun		, 5);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Mothership	, 6);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Shield		, 7);

	GDEFINE_ENUM_TYPE(WEAPON_LOAD, uint8_t);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Bullet		, 0);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Shell		, 1);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Ray			, 2);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Cannonball	, 3);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Rocket		, 4);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Missile		, 5);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Wave		, 6);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Flare		, 7);

	GDEFINE_ENUM_TYPE(WEAPON_EFFECT, uint8_t);
	GDEFINE_ENUM_VALUE(WEAPON_EFFECT, Pierce	, 0);

	// One per value combination
	struct SWeaponLoad {
		WEAPON_LOAD		Type			;//= WEAPON_LOAD_Bullet;
		WEAPON_DAMAGE	DamageType		;//= WEAPON_DAMAGE_Pierce;
		uint8_t			ParticleCount	;//= 1;
		float			Delay			;//= .1;
		int32_t			Damage			;//= 1;
		float			Lifetime		;//= 0;
		float			Speed			;//= 150;
		float			Weight			;//= 150;
		//uint32_t		Color			;

	};

	// One per value combinatiom
	struct SWeaponType {
		WEAPON_TYPE		Type				;//= WEAPON_TYPE_Gun;
		WEAPON_DAMAGE	DamageType			;//= WEAPON_DAMAGE_Pierce;
		int32_t			DamageMultiplier	;//= 1;
		float			Cooldown			;//= 1;
		float			Stability			;//= 1.0;
		float			ShotLifetime		;//= 0;
		float			SpeedMultiplier		;//= 150;
	};

	// One per orbiter
	struct SWeaponState {
		bool			CoolingDown			;//= false;
		float			Overheat			;//= 0;
	};

	// One per orbiter
	struct SWeapon {
		WEAPON_TYPE		Type				;//= WEAPON_TYPE_Gun;
		WEAPON_LOAD		Load				;//= WEAPON_LOAD_Bullet;
		WEAPON_DAMAGE	DamageType			;//= WEAPON_DAMAGE_Pierce;
		uint8_t			ParticleCount		;//= 1;
		float			MaxDelay			;//= .1;
		float			Delay				;//= 0;
		float			Cooldown			;//= 1;
		float			OverheatPerShot		;//= 0;
		float			Stability			;//= 1.0;
		float			Speed				;//= 150;
		int32_t			Damage				;//= 1;
		float			ShotLifetime		;//= 0;
		bool			CoolingDown			;//= false;
		float			Overheat			;//= 0;

		int				Create				(::ghg::SShots & shots, const ::gpk::n3f32 & position, const ::gpk::n3f32 & direction, float speed, float brightness, float lifetime)	{
			if(Delay < MaxDelay)
				return 0;

			Delay			= 0;
			Overheat		+= OverheatPerShot;
			return shots.SpawnForced(position, direction, speed, brightness, lifetime);
		}

		int				SpawnDirected		(::ghg::SShots & shots, double stabilityFactor, const ::gpk::n3f32 & position, const ::gpk::n3f32 & direction, float speedDebris, float brightness, float lifetime)	{
			if(Delay < MaxDelay)
				return 0;

			Delay			= 0;
			Overheat		+= OverheatPerShot;
			return shots.SpawnForcedDirected(Stability * stabilityFactor, position, direction, speedDebris, brightness, lifetime);
		}

		int				SpawnDirected		(::ghg::SShots & shots, uint32_t countShots, double stabilityFactor, const ::gpk::n3f32 & position, const ::gpk::n3f32 & direction, float speedDebris, float brightness, float lifetime)	{
			if(Delay < MaxDelay || 0 == countShots)
				return 0;

			int32_t				indexFirst			= -1;
			if(countShots)
				indexFirst		= shots.SpawnForcedDirected(Stability * stabilityFactor, position, direction, speedDebris, brightness, lifetime);

			for(uint32_t iDebris = 0; iDebris < (countShots - 1); ++iDebris)
				shots.SpawnForcedDirected(Stability * stabilityFactor, position, direction, speedDebris, brightness, lifetime);

			Delay			= 0;
			Overheat		+= OverheatPerShot;
			return indexFirst;
		}
	};

	struct SWeaponScore {
		int64_t		ImpactTime		= 0;
		int64_t		BulletsHit		= 0;
		int64_t		BulletsUsed		= 0;
		int64_t		BulletsLoaded	= 0;
		int64_t		ShotsUsed		= 0;
		int64_t		ShotsLoaded		= 0;
		int64_t		ReloadCount		= 0;
		int64_t		ReloadTime		= 0;
		int64_t		OverheatCount	= 0;
		int64_t		OverheatTime	= 0;
	};
#pragma pack(pop)
} // namespace

#endif // GPK_GALAXY_HELL_WEAPON_H