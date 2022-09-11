#include "gpk_galaxy_hell_physics.h"
#include "gpk_enum.h"

#ifndef GPK_GALAXY_HELL_WEAPON_H
#define GPK_GALAXY_HELL_WEAPON_H

namespace ghg 
{
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

		int											SpawnForcedDirected	(double stabilityFactor, const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speedDebris, float brightness)	{
			static constexpr const double					randUnit			= ::gpk::math_2pi / RAND_MAX;
			::gpk::SCoord3<float>							finalDirection		= {0, 1, 0};
			finalDirection.RotateX(rand() * randUnit);
			finalDirection.RotateY(rand() * randUnit);
			finalDirection.Normalize();
			return SpawnForced(position, ::gpk::interpolate_linear(finalDirection, direction, stabilityFactor), speedDebris, brightness);
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

		::gpk::error_t								Save(::gpk::array_pod<byte_t> & output) const { 
			Particles.Save(output);
			::gpk::viewWrite(::gpk::view_array<const ::gpk::SCoord3<float>	>{PositionDraw	}, output);
			::gpk::viewWrite(::gpk::view_array<const ::gpk::SCoord3<float>	>{PositionPrev	}, output);
			::gpk::viewWrite(::gpk::view_array<const float					>{Brightness	}, output);
			return 0; 
		}
		::gpk::error_t								Load(::gpk::view_array<const byte_t> & input) { 
			uint32_t												bytesRead				= 0;
			Particles.Load(input);
			::gpk::view_array<const ::gpk::SCoord3<float>	>	readPositionPrev	= {}; bytesRead	= ::gpk::viewRead(readPositionPrev	, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; PositionPrev	= readPositionPrev	;
			::gpk::view_array<const ::gpk::SCoord3<float>	>	readPositionDraw	= {}; bytesRead	= ::gpk::viewRead(readPositionDraw	, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; PositionDraw	= readPositionDraw	;
			::gpk::view_array<const float					>	readBrightness		= {}; bytesRead	= ::gpk::viewRead(readBrightness	, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; Brightness		= readBrightness	;
			return 0;
		}

	};

#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE(WEAPON_DAMAGE, uint16_t);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Pierce		, 0x0);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Impact		, 0x1);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Wave			, 0x2);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Burn			, 0x4);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Shock			, 0x8);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Magnetic		, 0x10);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Radiation		, 0x20);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Poison		, 0x40);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Plasma		, 0x80);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Light			, 0x100);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Sound			, 0x200);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, EMP			, 0x400);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Gravity		, 0x800);
	GDEFINE_ENUM_VALUE(WEAPON_DAMAGE, Pow			, 0x1000);

	GDEFINE_ENUM_TYPE(WEAPON_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Gun			, 0);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Cannon		, 1);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Rocket		, 2);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Missile		, 3);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Torpedo		, 4);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Shotgun		, 5);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Bomb		, 6);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Mothership	, 7);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Shield		, 8);

	GDEFINE_ENUM_TYPE(WEAPON_LOAD, uint8_t);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Bullet		, 0);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Shell		, 1);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Ray			, 2);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Cannonball	, 3);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Rocket		, 4);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Missile		, 5);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Wave		, 6);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Flare		, 7);
	
	struct SWeapon {
		WEAPON_TYPE									Type				= WEAPON_TYPE_Gun;
		WEAPON_LOAD									Load				= WEAPON_LOAD_Bullet;
		WEAPON_DAMAGE								DamageType			= WEAPON_DAMAGE_Pierce;
		uint8_t										ParticleCount		= 1;
		double										MaxDelay			= .1;
		double										Delay				= 0;
		double										Stability			= 1.0;
		float										Speed				= 150;
		int32_t										Damage				= 1;

		int											Create				(::ghg::SShots & shots, const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speed, float brightness)	{
			if(Delay < MaxDelay)
				return 0;
			Delay										= 0;
			return shots.SpawnForced(position, direction, speed, brightness);
		}

		int											SpawnDirected		(::ghg::SShots & shots, double stabilityFactor, const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speedDebris, float brightness)	{
			if(Delay < MaxDelay)
				return 0;
			Delay										= 0;
			return shots.SpawnForcedDirected(Stability * stabilityFactor, position, direction, speedDebris, brightness);
		}

		int											SpawnDirected		(::ghg::SShots & shots, uint32_t countShots, double stabilityFactor, const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speedDebris, float brightness)	{
			if(Delay < MaxDelay || 0 == countShots)
				return 0;
			int32_t											indexFirst			= -1;
			if(countShots)
				indexFirst									= shots.SpawnForcedDirected(Stability * stabilityFactor, position, direction, speedDebris, brightness);
			for(uint32_t iDebris = 0; iDebris < (countShots - 1); ++iDebris)
				shots.SpawnForcedDirected(Stability * stabilityFactor, position, direction, speedDebris, brightness);
			Delay										= 0;
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