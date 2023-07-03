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
	double						dealiased;
	int32_t						indexProperty;
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "mass"					)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.Mass					= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "diameter"				)) else { bodyData.Diameter				= (uint32_t)jsonData[indexProperty]->Token->Value; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "density"				)) else { bodyData.Density				= (uint32_t)jsonData[indexProperty]->Token->Value; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "gravity"				)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.Gravity				= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "escape_velocity"		)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.EscapeVelocity		= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "rotation_period"		)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.RotationPeriod		= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "length_of_day"			)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.LengthOfDay			= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "distance_from_sun"		)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.DistanceFromSun		= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "perihelion"				)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.Perihelion			= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "aphelion"				)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.Aphelion				= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "orbital_period"			)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.OrbitalPeriod		= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "orbital_velocity"		)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.OrbitalVelocity		= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "orbital_inclination"	)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.OrbitalInclination	= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "orbital_eccentricity"	)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.OrbitalEccentricity	= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "obliquity_to_orbit"		)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.ObliquityToOrbit		= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "mean_temperature"		)) else { bodyData.MeanTemperature		= (uint32_t)jsonData[indexProperty]->Token->Value; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "surface_pressure"		)) else { bodyData.SurfacePressure		= (uint32_t)jsonData[indexProperty]->Token->Value; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "number_of_moons"		)) else { bodyData.NumberOfMoons		= (uint32_t)jsonData[indexProperty]->Token->Value; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "ring_system"			)) else { bodyData.RingSystem			= ::gpk::char2bool(jsonData.View[indexProperty]); }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "global_magnetic_field"	)) else { bodyData.GlobalMagneticField	= ::gpk::char2bool(jsonData.View[indexProperty]); }
	return 0; 
}

static	::gpk::error_t	loadStar		(::ssiege::SDetailStar & bodyData, const ::gpk::SJSONReader & jsonData, int32_t jsonIndexStellarBody) { 
	double						dealiased;
	int32_t						indexProperty;
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "mass"					)) else { bodyData.Mass					= (uint32_t)jsonData[indexProperty]->Token->Value; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "gm"						)) else { bodyData.Gm					= (uint32_t)jsonData[indexProperty]->Token->Value; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "volume"					)) else { bodyData.Volume				= (uint32_t)jsonData[indexProperty]->Token->Value; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "volumetric_mean_radius"	)) else { bodyData.VolumetricMeanRadius	= (uint32_t)jsonData[indexProperty]->Token->Value; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "mean_density"			)) else { bodyData.MeanDensity			= (uint32_t)jsonData[indexProperty]->Token->Value; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "surface_gravity"		)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.SurfaceGravity		= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "escape_velocity"		)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.EscapeVelocity		= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "ellipticity"			)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.Ellipticity			= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "moment_of_inertia"		)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.MomentOfInertia		= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "visual_magnitude"		)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.VisualMagnitude		= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "absolute_magnitude"		)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.AbsoluteMagnitude	= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "luminosity"				)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.Luminosity			= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "mass_conversion_rate"	)) else { bodyData.MassConversionRate	= (uint32_t)jsonData[indexProperty]->Token->Value; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "mean_energy_production"	)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.MeanEnergyProduction	= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "surface_emission"		)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.SurfaceEmission		= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "central_pressure"		)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.CentralPressure		= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "central_temperature"	)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.CentralTemperature	= (float)dealiased; }
	ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody, "central_density"		)) else { memcpy(&dealiased, &jsonData[indexProperty]->Token->Value, sizeof(double)); bodyData.CentralDensity		= (float)dealiased; }
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
	::gpk::SJSONFile			jsonFile;
	gpk_necs(::gpk::jsonFileRead(jsonFile, jsonFilePath));
	return ::ssiege::solarSystemSetup(solarSystem, engine, jsonFile.Reader);
}
