#include "ShaderShared.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET
{
	return float4(input.color, 1.0f);
}
