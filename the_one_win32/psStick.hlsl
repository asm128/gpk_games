#include "PixelShaderShared.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET
{
	//::gpk::SColorFloat									materialcolor				= ::gpk::BROWN + (::gpk::ORANGE * .5f);
	//const ::gpk::SCoord3<float>							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	//const ::gpk::SColorFloat							diffuse						= ::gpk::lightCalcDiffuse(materialcolor, inPS.WeightedNormal, lightVecW);
	//const ::gpk::SColorFloat							ambient						= materialcolor * ::LIGHT_FACTOR_AMBIENT;
	//
	//outputPixel										= ::gpk::SColorFloat(ambient + diffuse).Clamp();
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}