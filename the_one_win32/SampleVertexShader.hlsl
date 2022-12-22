#include "ShaderShared.hlsli"

cbuffer ModelConstantBuffer : register(b0) {
	matrix						Model;
	matrix						ModelInverseTranspose;
	matrix						MVP;
};

PixelShaderInput			main			(VertexShaderInput input) {
	PixelShaderInput				output;
	output.position				= mul(float4(input.position, 1.0f), MVP);

	output.world.position		= mul(float4(input.position, 1.0f), Model).xyz;
	output.world.normal			= mul(float4(input.normal, 0.0f), ModelInverseTranspose).xyz;
	output.world.uv				= input.uv;
	return output;
}
