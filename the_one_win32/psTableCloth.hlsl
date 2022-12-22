#include "PixelShaderShared.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET {
	float3												lightVecW					= normalize(LightPosition.xyz - input.world.position);
	float4												diffuse						= lightCalcDiffuse(float4(0, .25f, 0, 1), normalize(input.world.normal), lightVecW);
	float2												scaledUV					= float2(input.world.uv.x * 2, input.world.uv.y);
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
	else 
		return float4(saturate(diffuse).xyz, diffuse.a);
}
