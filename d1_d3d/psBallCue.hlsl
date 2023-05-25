#include "PixelShaderShared.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET
{
	float2								relativeToCenter			= input.world.uv - float2(.5f, .5f);
	relativeToCenter.x				*= 2;
	
	float3								normal						= normalize(input.world.normal);
	const float4						surfacecolor				
		= (distance(float2( 0.0f, 0.0f), relativeToCenter) < .05f) ? float4(1, 0, 0, 1)
		: (distance(float2( 1.0f, 0.0f), relativeToCenter) < .05f) ? float4(1, 0, 0, 1)
		: (distance(float2(-1.0f, 0.0f), relativeToCenter) < .05f) ? float4(1, 0, 0, 1)
		: (distance(float2( 0.5f, 0.0f), relativeToCenter) < .05f) ? float4(1, 0, 0, 1)
		: (distance(float2(-0.5f, 0.0f), relativeToCenter) < .05f) ? float4(1, 0, 0, 1)
		: (( 0.5f - relativeToCenter.y) <  .05f) ? float4(1, 0, 0, 1)
		: ((-0.5f - relativeToCenter.y) > -.05f) ? float4(1, 0, 0, 1)
		: float4(1, 1, 1, 1);
	float3			lightVecW					= normalize(LightPosition.xyz - input.world.position);
	float4			specular					= lightCalcSpecular(float4(1, 1, 1, 1), float4(1, 1, 1, 1), CameraPositionAndAngle.xyz, LIGHT_FACTOR_SPECULAR_POWER, input.world.position, normal, lightVecW);
	float4			diffuse						= lightCalcDiffuse(surfacecolor, normal, lightVecW);
	float4			ambient						= surfacecolor * .1f;
	
	return (0 == surfacecolor.g) ? float4(1, 0, 0, 1) : float4((ambient + diffuse + specular).rgb, surfacecolor.a);
}