#include "PixelShaderShared.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET {
	float4												materialcolor				= saturate(GPK_BROWN + (GPK_ORANGE * .5f));
	float3												lightVecW					= normalize(LightPosition.xyz - input.world.position);
	float4												diffuse						= lightCalcDiffuse(materialcolor, normalize(input.world.normal), lightVecW);
	float4												ambient						= materialcolor * LIGHT_FACTOR_AMBIENT;
	return float4(saturate(ambient + diffuse).xyz, diffuse.a);
}
