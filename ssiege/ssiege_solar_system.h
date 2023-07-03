#include "gpk_engine.h"
#include "gpk_json.h"

#ifndef CAMPP_SOLAR_SYSTEM_H_23701
#define CAMPP_SOLAR_SYSTEM_H_23701

namespace ssiege
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE(STELLAR_BODY, uint8_t);
	GDEFINE_ENUM_VALUE(STELLAR_BODY, STAR		, 0);
	GDEFINE_ENUM_VALUE(STELLAR_BODY, PLANET		, 1);
	GDEFINE_ENUM_VALUE(STELLAR_BODY, MOON		, 2);
	GDEFINE_ENUM_VALUE(STELLAR_BODY, ASTEROID	, 3);
	GDEFINE_ENUM_VALUE(STELLAR_BODY, COMET		, 4);

	struct SDetailStar {
		uint32_t					Mass					; // 1988500	// (10^24 kg)		
		uint32_t					Gm						; // 132712		// (x 106 km3/s2)	
		uint32_t					Volume					; // 1412000	// (1012 km3)		
		uint32_t					VolumetricMeanRadius	; // 695700		// (km)			
		uint32_t					MeanDensity				; // 1408		// (kg/m3)			
		float						SurfaceGravity			; // 274.0		// (eq.) (m/s2)	
		float						EscapeVelocity			; // 617.6		// (km/s)			
		float						Ellipticity				; // 0.00005	
		float						MomentOfInertia			; // 0.070		// (I/MR2)	
		float						VisualMagnitude			; // -26.74		// V(1,0)	
		float						AbsoluteMagnitude		; // 4.83		
		float						Luminosity				; // 382.8		// (1024 J/s)
		uint32_t					MassConversionRate		; // 4260		// (106 kg/s)
		float						MeanEnergyProduction	; // 0.1925		// (10-3 J/kg s)
		float						SurfaceEmission			; // 62.94		// (106 J/m2s)
		float						CentralPressure			; // 2.477		// x 1011	bar
		float						CentralTemperature		; // 1.571		// x 107	K
		float						CentralDensity			; // 1.622		// x 105	kg/m3
	};

	struct SDetailPlanet {
		int32_t						Parent					;
		float						Mass					; // 0.330		// (10^24 kg)	
		uint32_t					Diameter				; // 4879		// (km)
		uint32_t					Density					; // 5427		// (kg/m3)
		float						Gravity					; // 3.7		// (m/s^2)
		float						EscapeVelocity			; // 4.3		// (km/s)
		float						RotationPeriod			; // 1407.6		// (hours)	
		float						LengthOfDay				; // 4222.6		// (hours)
		float						DistanceFromSun			; // 57.9		// (10^6 km)
		float						Perihelion				; // 46.0		// (10^6 km)
		float						Aphelion				; // 69.8		// (10^6 km)
		float						OrbitalPeriod			; // 88.0		// (days)	
		float						OrbitalVelocity			; // 47.4		// (km/s)
		float						OrbitalInclination		; // 7.0		// (degrees)	
		float						OrbitalEccentricity		; // 0.205
		float						ObliquityToOrbit		; // 0.034		// (degrees)
		uint32_t					MeanTemperature			; // 167		// (Celsius)
		uint32_t					SurfacePressure			; // 0			// (Bars)
		uint32_t					NumberOfMoons			; // 0			// 
		bool						RingSystem				; // false
		bool						GlobalMagneticField		; // true
		float						RadiusPolar				;	// (km)
		float						RadiusEquatorial		;	// (km)
	};

	struct SCelestialBody {
		union UBody {
			SDetailPlanet	Planet	;
			SDetailStar		Star	;
		}							Detail;
	};
#pragma pack(pop)

	stacxpr	size_t				s0					= sizeof(SDetailPlanet);
	stacxpr	size_t				s1					= sizeof(SDetailStar);

	typedef	::gpk::SLinearPODMap<::gpk::vcc, SCelestialBody>	SCelestialBodyMap;

	struct SSolarSystem {
		SCelestialBodyMap			Body;
		::gpk::apod<STELLAR_BODY>	Type;
		::gpk::ai32					Parent;
		::gpk::ai32					Entity;
	};

	::gpk::error_t				solarSystemSetup		(::ssiege::SSolarSystem & solarSystem, ::gpk::SEngine & engine, const ::gpk::SJSONReader & jsonData);
	::gpk::error_t				solarSystemSetup		(::ssiege::SSolarSystem & solarSystem, ::gpk::SEngine & engine, ::gpk::vcc jsonFilePath);

} // namespace

#endif // CAMPP_SOLAR_SYSTEM_H_23701