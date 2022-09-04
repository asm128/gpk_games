#include "gpk_galaxy_hell_physics.h"
#include "gpk_enum.h"

#ifndef GPK_GALAXY_HELL_WEAPON_H
#define GPK_GALAXY_HELL_WEAPON_H

namespace ghg 
{
	GDEFINE_ENUM_TYPE(WEAPON_DAMAGE, uint8_t);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, PIERCE		,  0);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, IMPACT		,  1);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, WAVE			,  2);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, BURN			,  3);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, SHOCK			,  4);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, MAGNETIC		,  5);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, RADIATION		,  6);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, POISON		,  7);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, PLASMA		,  8);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, LIGHT			,  9);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, SOUND			, 10);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, PEM			, 11);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, GRAVITY		, 12);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, POW			, 13);

	GDEFINE_ENUM_TYPE(WEAPON_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, GUN			, 0);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, CANNON		, 1);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, ROCKET		, 2);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, MISSILE		, 3);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, TORPEDO		, 4);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, SHOTGUN		, 5);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, BOMB		, 6);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, MOTHERSHIP	, 7);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, SHIELD		, 8);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, COUNT		, 9);

	GDEFINE_ENUM_TYPE(WEAPON_LOAD, uint8_t);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, BULLET	, 0);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, SHELL	, 1);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, RAY		, 2);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, ROCKET	, 3);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, MISSILE	, 4);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, WAVE	, 5);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, FLARE	, 6);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, COUNT	, 7);


	struct SShots	{
		::gpk::array_pod<float>						Brightness			= {};
		::gpk::array_pod<::gpk::SCoord3<float>>		PositionPrev		= {};
		::gpk::array_pod<::gpk::SCoord3<float>>		PositionDraw		= {};
		::ghg::SParticles3							Particles;

		int											SpawnForced			(const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speed, float brightness)	{
			Particles.Create(position, direction, speed);
			PositionPrev.push_back(position);
			PositionDraw.push_back(position);
			return Brightness.push_back(brightness);
		}

		int											SpawnForcedDirected	(double noiseFactor, const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speedDebris, float brightness)	{
			static constexpr const double					randUnit			= ::gpk::math_2pi / RAND_MAX;
			::gpk::SCoord3<float>							finalDirection		= {0, 1, 0};
			finalDirection.RotateX(rand() * randUnit);
			finalDirection.RotateY(rand() * randUnit);
			finalDirection.Normalize();
			return SpawnForced(position, ::gpk::interpolate_linear(direction, finalDirection, noiseFactor), speedDebris, brightness);
		}

		int											Update				(float secondsLastFrame)	{
			static constexpr	const uint32_t				maxRange			= 200;
			static constexpr	const uint32_t				maxRangeSquared		= maxRange * maxRange;
			memcpy(PositionPrev.begin(), Particles.Position.begin(), Particles.Position.size() * sizeof(::gpk::SCoord3<float>));
			Particles.IntegrateSpeed(secondsLastFrame);
			for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
				if (Particles.Position[iShot].LengthSquared() > maxRangeSquared)
					Remove(iShot--);
			}
			return 0;
		}

		int											Remove				(uint32_t iShot)			{
			Particles.Remove(iShot);
			PositionPrev		.remove_unordered(iShot);
			PositionDraw		.remove_unordered(iShot);
			return Brightness	.remove_unordered(iShot);
		}

	};

	struct SWeapon {
		WEAPON_DAMAGE								DamageType			= WEAPON_DAMAGE_PIERCE;
		WEAPON_TYPE									Type				= WEAPON_TYPE_GUN;
		WEAPON_LOAD									Load				= WEAPON_LOAD_BULLET;
		double										Delay				= 0;
		int32_t										Damage				= 1;
		double										MaxDelay			= .1;
		SShots										Shots				= {};

		int											Create				(const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speed, float brightness)	{
			if(Delay < MaxDelay)
				return 0;
			Delay										= 0;
			return Shots.SpawnForced(position, direction, speed, brightness);
		}
		int											SpawnDirected		(double noiseFactor, const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speedDebris, float brightness)	{
			if(Delay < MaxDelay)
				return 0;
			Delay										= 0;
			return Shots.SpawnForcedDirected(noiseFactor, position, direction, speedDebris, brightness);
		}
		int											SpawnDirected		(uint32_t countShots, double noiseFactor, const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speedDebris, float brightness)	{
			if(Delay < MaxDelay || 0 == countShots)
				return 0;
			int32_t											indexFirst			= -1;
			if(countShots)
				indexFirst									= Shots.SpawnForcedDirected(noiseFactor, position, direction, speedDebris, brightness);
			for(uint32_t iDebris = 0; iDebris < (countShots - 1); ++iDebris)
				Shots.SpawnForcedDirected(noiseFactor, position, direction, speedDebris, brightness);
			Delay										= 0;
			return indexFirst;
		}
	};

	struct SWeaponScore {
		int32_t		WeaponType		= 0;
		int32_t		ImpactTime		= 0;
		int32_t		BulletsHit		= 0;
		int32_t		BulletsUsed		= 0;
		int32_t		BulletsLoaded	= 0;
		int32_t		ShotsUsed		= 0;
		int32_t		ShotsLoaded		= 0;
		int32_t		ReloadCount		= 0;
		int32_t		ReloadTime		= 0;
		int32_t		OverheatCount	= 0;
		int32_t		OverheatTime	= 0;
	};
	
} // namespace

#endif // GPK_GALAXY_HELL_WEAPON_H