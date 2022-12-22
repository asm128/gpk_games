#include "ShaderShared.hlsli"

cbuffer ViewProjectionConstantBuffer : register(b0) {
	matrix						View;
	matrix						Perspective;
	matrix						Screen;
	matrix						VP;
	matrix						VPS;

	float3						CameraPosition	; 
	float3						CameraFront		; 
	float3						LightPosition	; 
	float3						LightDirection	; 
};

cbuffer ModelConstantBuffer : register(b1) {
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
