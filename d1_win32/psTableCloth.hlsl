#include "PixelShaderShared.hlsli"
#include "ShaderColors.hlsli"

float4					main					(PixelShaderInput input) : SV_TARGET {
	float3						lightVecW				= normalize(LightPosition.xyz - input.world.position);
	float						diffuseFactor			= max(dot(lightVecW, normalize(input.world.normal)), 0.0f);
	float4						materialColor			
		= (input.world.uv.x > .2485 && input.world.uv.x < .2515) ? float4(GPK_WHITE.xyz, Diffuse.a)
		: Diffuse
		;

	float4						diffuse					= diffuseFactor ? float4((materialColor * diffuseFactor).rgb * max(.65f, rand(input.world.uv)), materialColor.a) : float4(0, 0, 0, 1);
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
