#include "PixelShaderShared.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET {
	float3												lightVecW					= normalize(LightPosition.xyz - input.world.position);
	float4												diffuse						= lightCalcDiffuse(float4(0.25f, .25f, .25f, 1), normalize(input.world.normal), lightVecW);
	return float4(saturate(diffuse).xyz, diffuse.a);
}
