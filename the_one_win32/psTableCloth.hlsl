#include "PixelShaderShared.hlsli"

float4					main					(PixelShaderInput input) : SV_TARGET {
	float3						lightVecW				= normalize(LightPosition.xyz - input.world.position);
	float						diffuseFactor			= max(dot(lightVecW, normalize(input.world.normal)), 0.0f);
	float4						diffuse					= diffuseFactor ? float4((Diffuse * (float)diffuseFactor).rgb * max(.65f, rand(input.world.uv)), Diffuse.a) : float4(0, 0, 0, 1);
	float2						scaledUV				= float2(input.world.uv.x * 2, input.world.uv.y);
	if( distance(scaledUV, float2(0, 0)) < .05
	 || distance(scaledUV, float2(1, 1)) < .05
	 || distance(scaledUV, float2(1, 0)) < .05
	 || distance(scaledUV, float2(2, 1)) < .05
	 || distance(scaledUV, float2(2, 0)) < .05
	 || distance(scaledUV, float2(0, 1)) < .05
	) {
		discard;
		return float4(0,0,0,0);
	}
	else {
		return float4(saturate(diffuse).xyz, diffuse.a);
	}
}
