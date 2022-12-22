#include "gpk_pool_shader.h"
#include "gpk_engine_shader.h"
#include "gpk_raster_lh.h"

static constexpr float							LIGHT_FACTOR_AMBIENT			= .025f;
static constexpr float							LIGHT_FACTOR_SPECULAR_POWER		= 30.0f;
static constexpr ::gpk::SColorBGRA				PIXEL_BLACK_NUMBER				= ::gpk::SColorBGRA{0, 0, 0, 255};

::gpk::error_t										the1::psTableCloth
	( const ::gpk::SEngineSceneConstants					& constants
	, const ::gpk::SPSIn									& inPS
	, ::gpk::SColorBGRA										& outputPixel
	) { 
	const ::gpk::SCoord3<float>							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::SColorFloat							diffuse						= ::gpk::lightCalcDiffuse(::gpk::DARKGREEN, inPS.WeightedNormal, lightVecW);
	outputPixel										= ::gpk::SColorFloat(diffuse).Clamp();
	return 0; 
}

::gpk::error_t										the1::psPocket
	( const ::gpk::SEngineSceneConstants					& constants
	, const ::gpk::SPSIn									& inPS
	, ::gpk::SColorBGRA										& outputPixel
	) { 
	const ::gpk::SCoord3<float>							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::SColorFloat							diffuse						= ::gpk::lightCalcDiffuse(::gpk::DARKGRAY, inPS.WeightedNormal, lightVecW);
	outputPixel										= ::gpk::SColorFloat(diffuse).Clamp();
	return 0; 
}

::gpk::error_t										the1::psStick
	( const ::gpk::SEngineSceneConstants					& constants
	, const ::gpk::SPSIn									& inPS
	, ::gpk::SColorBGRA										& outputPixel
	) { 
	::gpk::SColorFloat									materialcolor				= ::gpk::BROWN + (::gpk::ORANGE * .5f);
	const ::gpk::SCoord3<float>							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::SColorFloat							diffuse						= ::gpk::lightCalcDiffuse(materialcolor, inPS.WeightedNormal, lightVecW);
	const ::gpk::SColorFloat							ambient						= materialcolor * ::LIGHT_FACTOR_AMBIENT;

	outputPixel										= ::gpk::SColorFloat(ambient + diffuse).Clamp();
	return 0; 
}


::gpk::error_t										the1::psBallCue				
	( const ::gpk::SEngineSceneConstants					& constants
	, const ::gpk::SPSIn									& inPS
	, ::gpk::SColorBGRA										& outputPixel
	) { 
	::gpk::SCoord2<float>								relativeToCenter			= ::gpk::SCoord2<float>{inPS.WeightedUV.x, inPS.WeightedUV.y} - ::gpk::SCoord2<float>{.5f, .5f};
	relativeToCenter.x								*= 2;

	const ::gpk::SColorBGRA								surfacecolor				
		= ((::gpk::SCoord2<float>{ 0.0f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: ((::gpk::SCoord2<float>{ 1.0f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: ((::gpk::SCoord2<float>{-1.0f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: ((::gpk::SCoord2<float>{ 0.5f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: ((::gpk::SCoord2<float>{-0.5f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: (( 0.5f - relativeToCenter.y) <  .05f) ? ::gpk::RED 
		: ((-0.5f - relativeToCenter.y) > -.05f) ? ::gpk::RED 
		: ::gpk::WHITE;
	::gpk::SColorFloat									materialcolor				= surfacecolor;		
	const ::gpk::SCoord3<float>							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::SColorFloat							specular					= ::gpk::lightCalcSpecular(constants.CameraPosition, ::LIGHT_FACTOR_SPECULAR_POWER, gpk::WHITE, ::gpk::WHITE, inPS.WeightedPosition, inPS.WeightedNormal, lightVecW);
	const ::gpk::SColorFloat							diffuse						= ::gpk::lightCalcDiffuse(materialcolor, inPS.WeightedNormal, lightVecW);
	const ::gpk::SColorFloat							ambient						= materialcolor * ::LIGHT_FACTOR_AMBIENT;

	outputPixel										= (0 == surfacecolor.g) ? ::gpk::RED : ::gpk::SColorFloat(ambient + diffuse + specular).Clamp();
	return 0; 
}

::gpk::error_t										the1::psBallSolid				
	( const ::gpk::SEngineSceneConstants					& constants
	, const ::gpk::SPSIn									& inPS
	, ::gpk::SColorBGRA										& outputPixel
	) { 
	const ::gpk::SCoord2<uint32_t>							& surfaceSize							= inPS.Surface.metrics();
	::gpk::SCoord2<float>								relativeToCenter			= ::gpk::SCoord2<float>{inPS.WeightedUV.x, inPS.WeightedUV.y} - ::gpk::SCoord2<float>{.5f, .5f};
	relativeToCenter.x								*= 2;

	::gpk::SColorFloat									materialColor;
	bool												shade						= false;
	float												ambientFactor				= ::LIGHT_FACTOR_AMBIENT;
	if((::gpk::SCoord2<float>{0.0f, 0.0f} - relativeToCenter).LengthSquared() >= .0225f) {
		materialColor									= inPS.Material.Color.Diffuse;
		shade											= true;
	}
	else {
		const ::gpk::SCoord2<uint32_t>							fetchCoord					= 
			{ (uint32_t)(relativeToCenter.x * 2.f * surfaceSize.x + surfaceSize.x / 2)
			, (uint32_t)(relativeToCenter.y * 4.f * surfaceSize.y + surfaceSize.y / 2)
			};
		const ::gpk::SColorBGRA									surfacecolor				= inPS.Surface
			[fetchCoord.y % surfaceSize.y]
			[fetchCoord.x % surfaceSize.x]
			;
		if(surfacecolor != PIXEL_BLACK_NUMBER) {
			materialColor									= ::gpk::WHITE;
			shade											= rand() % 2;
			ambientFactor									= .65f;
		}
		else {
			materialColor									= gpk::BLACK;
			shade											= true;
		}
	}

	if(shade) {
		const ::gpk::SCoord3<float>								lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
		double													diffuseFactor				= inPS.WeightedNormal.Dot(lightVecW);
		if(diffuseFactor < 0) {
			ambientFactor									+= (rand() % 256) / 255.0f * float(diffuseFactor) * -.25f;
			materialColor									= (materialColor * ambientFactor).Clamp();
		}
		else {
			const ::gpk::SColorFloat								specular					= ::gpk::lightCalcSpecular(constants.CameraPosition, ::LIGHT_FACTOR_SPECULAR_POWER, gpk::WHITE, ::gpk::WHITE, inPS.WeightedPosition, inPS.WeightedNormal, lightVecW);
			const ::gpk::SColorFloat								diffuse						= materialColor * ::gpk::max(0.0, diffuseFactor);
			const ::gpk::SColorFloat								ambient						= materialColor * ambientFactor;
			materialColor									= ::gpk::SColorFloat(ambient + diffuse + specular).Clamp();
		}
	}
	outputPixel										= materialColor;
	return 0; 
}

::gpk::error_t										the1::psBallStripped			
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::SColorBGRA						& outputPixel
	) { 
	const ::gpk::SCoord2<uint32_t>						& surfaceSize							= inPS.Surface.metrics();
	::gpk::SCoord2<float>								relativeToCenter			= ::gpk::SCoord2<float>{inPS.WeightedUV.x, inPS.WeightedUV.y} - ::gpk::SCoord2<float>{.5f, .5f};
	relativeToCenter.x								*= 2;

	::gpk::SColorFloat									materialColor;
	bool												shade						= false;
	float												ambientFactor				= ::LIGHT_FACTOR_AMBIENT;
	if((::gpk::SCoord2<float>{0.0f, 0.0f} - relativeToCenter).LengthSquared() >= .0225f) {
		materialColor									
			= (relativeToCenter.y >  .20f) ? ::gpk::WHITE
			: (relativeToCenter.y < -.20f) ? ::gpk::WHITE
			: inPS.Material.Color.Diffuse
			;
		shade											= true;
	}
	else {
		const ::gpk::SCoord2<uint32_t>							fetchCoord					= 
			{ (uint32_t)(relativeToCenter.x * 2.f * surfaceSize.x + surfaceSize.x / 2)
			, (uint32_t)(relativeToCenter.y * 4.f * surfaceSize.y + surfaceSize.y / 2)
			};
		const ::gpk::SColorBGRA									surfacecolor				= inPS.Surface
			[fetchCoord.y % surfaceSize.y]
			[fetchCoord.x % surfaceSize.x]
			;

		if(surfacecolor != PIXEL_BLACK_NUMBER) {
			materialColor										= ::gpk::WHITE;
			shade												= rand() % 2;
			ambientFactor										= .65f;
		}
		else {
			materialColor										= ::gpk::BLACK;
			shade												= true;
		} 
	}
	if(shade) {
		const ::gpk::SCoord3<float>								lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
		double													diffuseFactor				= inPS.WeightedNormal.Dot(lightVecW);
		if(diffuseFactor < 0) {
			ambientFactor									+= (rand() % 256) * (1.0f / 255) * float(diffuseFactor) * -.25f;
			materialColor									= (materialColor * ambientFactor).Clamp();
		}
		else {
			const ::gpk::SColorFloat								specular					= ::gpk::lightCalcSpecular(constants.CameraPosition, ::LIGHT_FACTOR_SPECULAR_POWER, gpk::WHITE, ::gpk::WHITE, inPS.WeightedPosition, inPS.WeightedNormal, lightVecW);
			const ::gpk::SColorFloat								diffuse						= materialColor * ::gpk::max(0.0, diffuseFactor);
			const ::gpk::SColorFloat								ambient						= materialColor * ambientFactor;
			materialColor									= ::gpk::SColorFloat(ambient + diffuse + specular).Clamp();
		}
	}
	outputPixel											= materialColor;
	return 0; 
}
