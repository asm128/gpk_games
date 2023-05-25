#include "ShaderShared2D.hlsli"

GUIPixelShaderInput			main			(GUIVertexShaderInput input) {
	GUIPixelShaderInput				output;
	output.position				= float4(input.position.xyz, 1);
	output.uv					= input.uv;
	return output;
}
