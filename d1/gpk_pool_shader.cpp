#include "gpk_pool_shader.h"
#include "gpk_engine_shader.h"
#include "gpk_raster_lh.h"

using ::gpk::get_value_namep, ::gpk::get_enum_namep, ::gpk::failed;
GPK_USING_TYPEINT();

stacxpr	float						LIGHT_FACTOR_AMBIENT			= .025f;
stacxpr	float						LIGHT_FACTOR_SPECULAR_POWER		= 30.0f;
stacxpr	::gpk::bgra8				PIXEL_BLACK_NUMBER				= ::gpk::bgra{0, 0, 0, 255};

::gpk::error_t							d1p::psTableCloth
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::bgra							& outputPixel
	) { 
	const ::gpk::n3f2_t							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::rgbaf							diffuse						= ::gpk::lightCalcDiffuse(::gpk::DARKGREEN, inPS.WeightedNormal, lightVecW);
	outputPixel								= ::gpk::rgbaf(diffuse).Clamp();
	return 0; 
}

::gpk::error_t							d1p::psTableCushion
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::bgra							& outputPixel
	) { 
	const ::gpk::n3f2_t							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::rgbaf							diffuse						= ::gpk::lightCalcDiffuse(::gpk::DARKGREEN, inPS.WeightedNormal, lightVecW);
	outputPixel								= ::gpk::rgbaf(diffuse).Clamp();
	return 0; 
}

::gpk::error_t							d1p::psPocket
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::bgra							& outputPixel
	) { 
	const ::gpk::n3f2_t							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::rgbaf							diffuse						= ::gpk::lightCalcDiffuse(::gpk::DARKGRAY, inPS.WeightedNormal, lightVecW);
	outputPixel								= ::gpk::rgbaf(diffuse).Clamp();
	return 0; 
}

::gpk::error_t							d1p::psStick
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::bgra							& outputPixel
	) { 
	::gpk::rgbaf								materialcolor				= ::gpk::BROWN + (::gpk::ORANGE * .5f);
	const ::gpk::n3f2_t							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::rgbaf							diffuse						= ::gpk::lightCalcDiffuse(materialcolor, inPS.WeightedNormal, lightVecW);
	const ::gpk::rgbaf							ambient						= materialcolor * ::LIGHT_FACTOR_AMBIENT;

	outputPixel								= ::gpk::rgbaf(ambient + diffuse).Clamp();
	return 0; 
}


::gpk::error_t							d1p::psBallCue				
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::bgra							& outputPixel
	) { 
	::gpk::n2f2_t									relativeToCenter			= ::gpk::n2f2_t{inPS.WeightedUV.x, inPS.WeightedUV.y} - ::gpk::n2f2_t{.5f, .5f};
	relativeToCenter.x						*= 2;

	const ::gpk::bgra							surfacecolor				
		= ((::gpk::n2f2_t{ 0.0f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: ((::gpk::n2f2_t{ 1.0f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: ((::gpk::n2f2_t{-1.0f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: ((::gpk::n2f2_t{ 0.5f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: ((::gpk::n2f2_t{-0.5f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: (( 0.5f - relativeToCenter.y) <  .05f) ? ::gpk::RED 
		: ((-0.5f - relativeToCenter.y) > -.05f) ? ::gpk::RED 
		: ::gpk::WHITE;
	::gpk::rgbaf								materialcolor				= surfacecolor;		
	const ::gpk::n3f2_t							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::rgbaf							specular					= ::gpk::lightCalcSpecular(constants.CameraPosition, ::LIGHT_FACTOR_SPECULAR_POWER, gpk::WHITE, ::gpk::WHITE, inPS.WeightedPosition, inPS.WeightedNormal, lightVecW);
	const ::gpk::rgbaf							diffuse						= ::gpk::lightCalcDiffuse(materialcolor, inPS.WeightedNormal, lightVecW);
	const ::gpk::rgbaf							ambient						= materialcolor * ::LIGHT_FACTOR_AMBIENT;

	outputPixel								= (0 == surfacecolor.g) ? ::gpk::RED : ::gpk::rgbaf(ambient + diffuse + specular).Clamp();
	return 0; 
}

::gpk::error_t							d1p::psBallSolid				
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::bgra							& outputPixel
	) { 
	const ::gpk::n2u2_t						& surfaceSize				= inPS.Surface.metrics();
	::gpk::n2f2_t									relativeToCenter			= ::gpk::n2f2_t{inPS.WeightedUV.x, inPS.WeightedUV.y} - ::gpk::n2f2_t{.5f, .5f};
	relativeToCenter.x						*= 2;

	::gpk::rgbaf								materialColor;
	bool										shade						= false;
	float										ambientFactor				= ::LIGHT_FACTOR_AMBIENT;
	if((::gpk::n2f2_t{0.0f, 0.0f} - relativeToCenter).LengthSquared() >= .0225f) {
		materialColor							= inPS.Material.Color.Diffuse;
		shade									= true;
	}
	else {
		const ::gpk::n2u2_t							fetchCoord					= 
			{ (uint32_t)(relativeToCenter.x * 2.f * surfaceSize.x + surfaceSize.x / 2)
			, (uint32_t)(relativeToCenter.y * 4.f * surfaceSize.y + surfaceSize.y / 2)
			};
		const ::gpk::bgra							surfacecolor				= inPS.Surface
			[fetchCoord.y % surfaceSize.y]
			[fetchCoord.x % surfaceSize.x]
			;
		if(surfacecolor != PIXEL_BLACK_NUMBER) {
			materialColor							= ::gpk::WHITE;
			shade									= rand() % 2;
			ambientFactor							= .65f;
		}
		else {
			materialColor							= gpk::BLACK;
			shade									= true;
		}
	}

	if(shade) {
		const ::gpk::n3f2_t							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
		double										diffuseFactor				= inPS.WeightedNormal.Dot(lightVecW);
		if(diffuseFactor < 0) {
			ambientFactor							+= (rand() % 256) / 255.0f * float(diffuseFactor) * -.25f;
			materialColor							= (materialColor * ambientFactor).Clamp();
		}
		else {
			const ::gpk::rgbaf							specular					= ::gpk::lightCalcSpecular(constants.CameraPosition, ::LIGHT_FACTOR_SPECULAR_POWER, gpk::WHITE, ::gpk::WHITE, inPS.WeightedPosition, inPS.WeightedNormal, lightVecW);
			const ::gpk::rgbaf							diffuse						= materialColor * ::gpk::max(0.0, diffuseFactor);
			const ::gpk::rgbaf							ambient						= materialColor * ambientFactor;
			materialColor							= ::gpk::rgbaf(ambient + diffuse + specular).Clamp();
		}
	}
	outputPixel								= materialColor;
	return 0; 
}

::gpk::error_t							d1p::psBallStripped			
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::bgra							& outputPixel
	) { 
	const ::gpk::n2u2_t							& surfaceSize				= inPS.Surface.metrics();
	::gpk::n2f2_t									relativeToCenter			= ::gpk::n2f2_t{inPS.WeightedUV.x, inPS.WeightedUV.y} - ::gpk::n2f2_t{.5f, .5f};
	relativeToCenter.x						*= 2;

	::gpk::rgbaf								materialColor;
	bool										shade						= false;
	float										ambientFactor				= ::LIGHT_FACTOR_AMBIENT;
	if((::gpk::n2f2_t{0.0f, 0.0f} - relativeToCenter).LengthSquared() >= .0225f) {
		materialColor							
			= (relativeToCenter.y >  .20f) ? ::gpk::WHITE
			: (relativeToCenter.y < -.20f) ? ::gpk::WHITE
			: inPS.Material.Color.Diffuse
			;
		shade									= true;
	}
	else {
		const ::gpk::n2u2_t							fetchCoord					= 
			{ (uint32_t)(relativeToCenter.x * 2.f * surfaceSize.x + surfaceSize.x / 2)
			, (uint32_t)(relativeToCenter.y * 4.f * surfaceSize.y + surfaceSize.y / 2)
			};
		const ::gpk::bgra							surfacecolor				= inPS.Surface
			[fetchCoord.y % surfaceSize.y]
			[fetchCoord.x % surfaceSize.x]
			;

		if(surfacecolor != PIXEL_BLACK_NUMBER) {
			materialColor							= ::gpk::WHITE;
			shade									= rand() % 2;
			ambientFactor							= .65f;
		}
		else {
			materialColor							= ::gpk::BLACK;
			shade									= true;
		} 
	}
	if(shade) {
		const ::gpk::n3f2_t							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
		double										diffuseFactor				= inPS.WeightedNormal.Dot(lightVecW);
		if(diffuseFactor < 0) {
			ambientFactor							+= (rand() % 256) * (1.0f / 255) * float(diffuseFactor) * -.25f;
			materialColor							= (materialColor * ambientFactor).Clamp();
		}
		else {
			const ::gpk::rgbaf							specular					= ::gpk::lightCalcSpecular(constants.CameraPosition, ::LIGHT_FACTOR_SPECULAR_POWER, gpk::WHITE, ::gpk::WHITE, inPS.WeightedPosition, inPS.WeightedNormal, lightVecW);
			const ::gpk::rgbaf							diffuse						= materialColor * ::gpk::max(0.0, diffuseFactor);
			const ::gpk::rgbaf							ambient						= materialColor * ambientFactor;
			materialColor							= ::gpk::rgbaf(ambient + diffuse + specular).Clamp();
		}
	}
	outputPixel									= materialColor;
	return 0; 
}
