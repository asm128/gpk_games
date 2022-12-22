#include "PixelShaderShared.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET
{
	//::gpk::SCoord2<float>								relativeToCenter			= ::gpk::SCoord2<float>{inPS.WeightedUV.x, inPS.WeightedUV.y} - ::gpk::SCoord2<float>{.5f, .5f};
	//relativeToCenter.x								*= 2;
	//
	//const ::gpk::SColorBGRA								surfacecolor				
	//	= ((::gpk::SCoord2<float>{ 0.0f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
	//	: ((::gpk::SCoord2<float>{ 1.0f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
	//	: ((::gpk::SCoord2<float>{-1.0f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
	//	: ((::gpk::SCoord2<float>{ 0.5f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
	//	: ((::gpk::SCoord2<float>{-0.5f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
	//	: (( 0.5f - relativeToCenter.y) <  .05f) ? ::gpk::RED 
	//	: ((-0.5f - relativeToCenter.y) > -.05f) ? ::gpk::RED 
	//	: ::gpk::WHITE;
	//::gpk::SColorFloat									materialcolor				= surfacecolor;		
	//const ::gpk::SCoord3<float>							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	//const ::gpk::SColorFloat							specular					= ::gpk::lightCalcSpecular(constants.CameraPosition, ::LIGHT_FACTOR_SPECULAR_POWER, gpk::WHITE, ::gpk::WHITE, inPS.WeightedPosition, inPS.WeightedNormal, lightVecW);
	//const ::gpk::SColorFloat							diffuse						= ::gpk::lightCalcDiffuse(materialcolor, inPS.WeightedNormal, lightVecW);
	//const ::gpk::SColorFloat							ambient						= materialcolor * ::LIGHT_FACTOR_AMBIENT;
	//
	//outputPixel										= (0 == surfacecolor.g) ? ::gpk::RED : ::gpk::SColorFloat(ambient + diffuse + specular).Clamp();
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}