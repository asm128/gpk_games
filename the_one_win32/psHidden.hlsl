#include "PixelShaderShared.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 vDiffuse = texDiffuse.Sample( samplerLinear, input.world.uv );
	
	float fLighting = saturate( dot( normalize(float3(-.5f, -1, 0)), input.world.normal ) );
	fLighting = max( fLighting, .1f );
	
	return vDiffuse * fLighting;
}
