#include "ssiege_solar_system.h"
#include "gpk_stdstring.h"
#include "gpk_json_expression.h"

//static	::gpk::error_t	setupPlanet		(::ssiege::SSolarSystem & /*solarSystem*/, ::gpk::SEngine & /*engine*/, const ::gpk::SJSONReader & /*jsonData*/) { return 0; }
//static	::gpk::error_t	setupStar		(::ssiege::SSolarSystem & /*solarSystem*/, ::gpk::SEngine & /*engine*/, const ::gpk::SJSONReader & /*jsonData*/) { return 0; }

static	::gpk::error_t	printStar		(const ::ssiege::SDetailStar & body) {
	info_printf(
		" \nMass				: %u"
		" \nGm					: %u"
		" \nVolume				: %u"
		" \nVolumetricMeanRadius: %u"
		" \nMeanDensity			: %u"
		" \nSurfaceGravity		: %f"
		" \nEscapeVelocity		: %f"
		" \nEllipticity			: %f"
		" \nMomentOfInertia		: %f"
		" \nVisualMagnitude		: %f"
		" \nAbsoluteMagnitude	: %f"
		" \nLuminosity			: %f"
		" \nMassConversionRate	: %u"
		" \nMeanEnergyProduction: %f"
		" \nSurfaceEmission		: %f"
		" \nCentralPressure		: %f"
		" \nCentralTemperature	: %f"
		" \nCentralDensity		: %f"
		, body.Mass						
		, body.Gm							
		, body.Volume						
		, body.VolumetricMeanRadius		
		, body.MeanDensity				
		, body.SurfaceGravity				
		, body.EscapeVelocity				
		, body.Ellipticity				
		, body.MomentOfInertia			
		, body.VisualMagnitude			
		, body.AbsoluteMagnitude			
		, body.Luminosity					
		, body.MassConversionRate			
		, body.MeanEnergyProduction		
		, body.SurfaceEmission			
		, body.CentralPressure			
		, body.CentralTemperature			
		, body.CentralDensity				
	);
	return 0;
}

static	::gpk::error_t	printPlanet		(const ::ssiege::SDetailPlanet & body) {
	info_printf(
		" \nMass				: %f"
		" \nDiameter			: %u"
		" \nDensity				: %u"
		" \nGravity				: %f"
		" \nEscapeVelocity		: %f"
		" \nRotationPeriod		: %f"
		" \nLengthOfDay			: %f"
		" \nDistanceFromSun		: %f"
		" \nPerihelion			: %f"
		" \nAphelion			: %f"
		" \nOrbitalPeriod		: %f"
		" \nOrbitalVelocity		: %f"
		" \nOrbitalInclination	: %f"
		" \nOrbitalEccentricity	: %f"
		" \nObliquityToOrbit	: %f"
		" \nMeanTemperature		: %u"
		" \nSurfacePressure		: %u"
		" \nNumberOfMoons		: %u"
		" \nRingSystem			: %s"
		" \nGlobalMagneticField	: %s"
		" \nRadiusPolar			: %f"
		" \nRadiusEquatorial	: %f"
		, body.Mass				
		, body.Diameter			
		, body.Density				
		, body.Gravity				
		, body.EscapeVelocity		
		, body.RotationPeriod		
		, body.LengthOfDay			
		, body.DistanceFromSun		
		, body.Perihelion			
		, body.Aphelion			
		, body.OrbitalPeriod		
		, body.OrbitalVelocity		
		, body.OrbitalInclination	
		, body.OrbitalEccentricity	
		, body.ObliquityToOrbit	
		, body.MeanTemperature		
		, body.SurfacePressure		
		, body.NumberOfMoons		
		, ::gpk::bool2char(body.RingSystem			)
		, ::gpk::bool2char(body.GlobalMagneticField	)
		, body.RadiusPolar			
		, body.RadiusEquatorial	
	);
	return 0;
}

static	::gpk::error_t	loadPlanet		(::ssiege::SDetailPlanet & bodyData, const ::gpk::SJSONReader & jsonData, int32_t jsonIndexStellarBody) { 
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "mass"					, bodyData.Mass					));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "diameter"				, bodyData.Diameter				));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "density"				, bodyData.Density				));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "gravity"				, bodyData.Gravity				));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "escape_velocity"		, bodyData.EscapeVelocity		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "rotation_period"		, bodyData.RotationPeriod		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "length_of_day"		, bodyData.LengthOfDay			));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "distance_from_sun"	, bodyData.DistanceFromSun		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "perihelion"			, bodyData.Perihelion			));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "aphelion"				, bodyData.Aphelion				));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "orbital_period"		, bodyData.OrbitalPeriod		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "orbital_velocity"		, bodyData.OrbitalVelocity		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "orbital_inclination"	, bodyData.OrbitalInclination	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "orbital_eccentricity"	, bodyData.OrbitalEccentricity	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "obliquity_to_orbit"	, bodyData.ObliquityToOrbit		));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "mean_temperature"		, bodyData.MeanTemperature		));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "surface_pressure"		, bodyData.SurfacePressure		));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "number_of_moons"		, bodyData.NumberOfMoons		));
	ws_if_failed(::gpk::jsonObjectGetBoolean(jsonData, jsonIndexStellarBody, "ring_system"			, bodyData.RingSystem			));
	ws_if_failed(::gpk::jsonObjectGetBoolean(jsonData, jsonIndexStellarBody, "global_magnetic_field", bodyData.GlobalMagneticField	));
	return 0; 
}

static	::gpk::error_t	loadStar		(::ssiege::SDetailStar & bodyData, const ::gpk::SJSONReader & jsonData, int32_t jsonIndexStellarBody) { 
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "mass"						, bodyData.Mass					));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "gm"						, bodyData.Gm					));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "volume"					, bodyData.Volume				));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "volumetric_mean_radius"	, bodyData.VolumetricMeanRadius	));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "mean_density"				, bodyData.MeanDensity			));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "surface_gravity"			, bodyData.SurfaceGravity		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "escape_velocity"			, bodyData.EscapeVelocity		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "ellipticity"				, bodyData.Ellipticity			));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "moment_of_inertia"		, bodyData.MomentOfInertia		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "visual_magnitude"			, bodyData.VisualMagnitude		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "absolute_magnitude"		, bodyData.AbsoluteMagnitude	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "luminosity"				, bodyData.Luminosity			));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "mass_conversion_rate"		, bodyData.MassConversionRate	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "mean_energy_production"	, bodyData.MeanEnergyProduction	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "surface_emission"			, bodyData.SurfaceEmission		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "central_pressure"			, bodyData.CentralPressure		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "central_temperature"		, bodyData.CentralTemperature	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "central_density"			, bodyData.CentralDensity		));
	return 0; 
}

::gpk::error_t			ssiege::solarSystemSetup		(::ssiege::SSolarSystem & solarSystem, ::gpk::SEngine & engine, const ::gpk::SJSONReader & jsonData) {
	::gpk::avcc					stellarBodyNames			= {};
	::gpk::ai32					stellarBodyIndices;
	gpk_necs(::gpk::jsonObjectKeyList(jsonData, 0, stellarBodyIndices, stellarBodyNames));
	::gpk::vcvcc				jsonView					= jsonData.View;
	::gpk::avcc					bodyParentNames;

	float						furthest					= 0;

	for(uint32_t iPlanet = 0; iPlanet < stellarBodyIndices.size(); ++iPlanet) {
		const int32_t				jsonIndexStellarBody		= stellarBodyIndices[iPlanet];
		int32_t						indexProperty;

		if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody + 2, "parent")) 
			gpk_necs(bodyParentNames.push_back({}));
		else
			gpk_necs(bodyParentNames.push_back(jsonView[indexProperty]));
		
		{ // Load planet properties.
			::ssiege::STELLAR_BODY		bodyType					= ::ssiege::STELLAR_BODY_PLANET;
			ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody + 2, "type")) // ?? no type? shouldn't happen
			else {
				::gpk::vcc					strType						= jsonData.View[indexProperty]; 
				bodyType				= (::gpk::vcs("star") == strType) ? ::ssiege::STELLAR_BODY_STAR : ::ssiege::STELLAR_BODY_PLANET;
			}

			::ssiege::SCelestialBody		bodyProperties				= {};
			switch(bodyType) {
			case ::ssiege::STELLAR_BODY_STAR		: ws_if_failed(loadStar  (bodyProperties.Detail.Star  , jsonData, jsonIndexStellarBody + 2)); printStar  (bodyProperties.Detail.Star  ); break;
			case ::ssiege::STELLAR_BODY_PLANET	: ws_if_failed(loadPlanet(bodyProperties.Detail.Planet, jsonData, jsonIndexStellarBody + 2)); printPlanet(bodyProperties.Detail.Planet); break;
			}
			furthest	= ::gpk::max(furthest, bodyProperties.Detail.Planet.DistanceFromSun);
			gpk_necs(solarSystem.Type.push_back(bodyType));
			gpk_necs(solarSystem.Body.push_back(stellarBodyNames[iPlanet], bodyProperties));
		}
	}

	for(uint32_t iPlanet = 0; iPlanet < solarSystem.Body.Keys.size(); ++iPlanet)
		gpk_necs(solarSystem.Parent.push_back(solarSystem.Body.Keys.find(bodyParentNames[iPlanet])));

	for(uint32_t iPlanet = 0; iPlanet < solarSystem.Body.Keys.size(); ++iPlanet) {
		const ::ssiege::SCelestialBody	& bodyData				= solarSystem.Body.Values[iPlanet];
		const ::gpk::error_t			iEntity					= engine.CreateOrbiter
			( bodyData.Detail.Planet.Diameter
			, bodyData.Detail.Planet.Mass
			, bodyData.Detail.Planet.DistanceFromSun
			, bodyData.Detail.Planet.ObliquityToOrbit
			, bodyData.Detail.Planet.LengthOfDay
			, 24
			, bodyData.Detail.Planet.OrbitalPeriod
			, bodyData.Detail.Planet.OrbitalInclination
			, 1.0 / furthest * 2500
			);
		gpk_necs(solarSystem.Entity.push_back(iEntity));
	}
	return 0;
}

::gpk::error_t			ssiege::solarSystemSetup		(::ssiege::SSolarSystem & solarSystem, ::gpk::SEngine & engine, ::gpk::vcc jsonFilePath) {
	::gpk::SJSONFile			jsonFile						= {};
	gpk_necs(::gpk::jsonFileRead(jsonFile, jsonFilePath));
	return ::ssiege::solarSystemSetup(solarSystem, engine, jsonFile.Reader);
}
