#include "PixelShaderShared.hlsli"

float4			main					(PixelShaderInput input) : SV_TARGET {
	float3				lightVecW				= normalize(LightPosition.xyz - input.world.position);
	float				diffuseFactor			= max(dot(lightVecW, normalize(input.world.normal)), 0.0f);
	float4				diffuse					= diffuseFactor ? float4((Diffuse * (float)diffuseFactor).rgb * max(.65f, rand(input.world.uv)), Diffuse.a) : float4(0, 0, 0, 1);
	return float4(saturate(diffuse).xyz, diffuse.a);
}
