#include "PixelShaderShared.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET
{
	//const ::gpk::SCoord3<float>							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	//const ::gpk::SColorFloat								diffuse						= ::gpk::lightCalcDiffuse(::gpk::DARKGREEN, inPS.WeightedNormal, lightVecW);
	//outputPixel										= ::gpk::SColorFloat(diffuse).Clamp();
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}