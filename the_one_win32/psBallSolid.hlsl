#include "PixelShaderShared.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET
{
	//const ::gpk::SCoord2<uint32_t>							& surfaceSize							= inPS.Surface.metrics();
	//::gpk::SCoord2<float>								relativeToCenter			= ::gpk::SCoord2<float>{inPS.WeightedUV.x, inPS.WeightedUV.y} - ::gpk::SCoord2<float>{.5f, .5f};
	//relativeToCenter.x								*= 2;
	//
	//::gpk::SColorFloat									materialColor;
	//bool												shade						= false;
	//float												ambientFactor				= ::LIGHT_FACTOR_AMBIENT;
	//if((::gpk::SCoord2<float>{0.0f, 0.0f} - relativeToCenter).LengthSquared() >= .0225f) {
	//	materialColor									= inPS.Material.Color.Diffuse;
	//	shade											= true;
	//}
	//else {
	//	const ::gpk::SCoord2<uint32_t>							fetchCoord					= 
	//		{ (uint32_t)(relativeToCenter.x * 2.f * surfaceSize.x + surfaceSize.x / 2)
	//		, (uint32_t)(relativeToCenter.y * 4.f * surfaceSize.y + surfaceSize.y / 2)
	//		};
	//	const ::gpk::SColorBGRA									surfacecolor				= inPS.Surface
	//		[fetchCoord.y % surfaceSize.y]
	//		[fetchCoord.x % surfaceSize.x]
	//		;
	//	if(surfacecolor != PIXEL_BLACK_NUMBER) {
	//		materialColor									= ::gpk::WHITE;
	//		shade											= rand() % 2;
	//		ambientFactor									= .65f;
	//	}
	//	else {
	//		materialColor									= gpk::BLACK;
	//		shade											= true;
	//	}
	//}
	//
	//if(shade) {
	//	const ::gpk::SCoord3<float>								lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	//	double													diffuseFactor				= inPS.WeightedNormal.Dot(lightVecW);
	//	if(diffuseFactor < 0) {
	//		ambientFactor									+= (rand() % 256) / 255.0f * float(diffuseFactor) * -.25f;
	//		materialColor									= (materialColor * ambientFactor).Clamp();
	//	}
	//	else {
	//		const ::gpk::SColorFloat								specular					= ::gpk::lightCalcSpecular(constants.CameraPosition, ::LIGHT_FACTOR_SPECULAR_POWER, gpk::WHITE, ::gpk::WHITE, inPS.WeightedPosition, inPS.WeightedNormal, lightVecW);
	//		const ::gpk::SColorFloat								diffuse						= materialColor * ::gpk::max(0.0, diffuseFactor);
	//		const ::gpk::SColorFloat								ambient						= materialColor * ambientFactor;
	//		materialColor									= ::gpk::SColorFloat(ambient + diffuse + specular).Clamp();
	//	}
	//}
	//outputPixel										= materialColor;
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}