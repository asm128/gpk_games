#include "ShaderShared.hlsli"

cbuffer ModelViewProjectionConstantBuffer : register(b0) {
	matrix mvp;
	matrix model;
	matrix modelInverseTranspose;
	matrix view;
	matrix projection;
};

PixelShaderInput			main			(VertexShaderInput input) {
	PixelShaderInput				output;
	//output.position				= mul(float4(input.position, 1.0f), mul(model, mul(view, projection)));
	output.position				= mul(float4(input.position, 1.0f), mvp);

	output.world.position		= mul(float4(input.position, 1.0f), model).xyz;
	output.world.normal			= mul(float4(input.normal, 0.0f), modelInverseTranspose).xyz;
	output.world.uv				= input.uv;
	return output;
}
