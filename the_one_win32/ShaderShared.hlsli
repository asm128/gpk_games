cbuffer ModelConstantBuffer : register(b0) {
	matrix						Model;
	matrix						ModelInverseTranspose;
	matrix						MVP;
	float4						Diffuse;
	float4						Ambient;
	float4						Specular;
	float						SpecularPower;
};

cbuffer ViewProjectionConstantBuffer : register(b1) {
	matrix						View;
	matrix						Perspective;
	matrix						Screen;
	matrix						VP;
	matrix						VPS;

	float4						CameraPosition	; 
	float4						CameraFront		; 
	float4						LightPosition	; 
	float4						LightDirection	; 
	float4						Padding;
};

struct VertexShaderInput {
	float3				position	: POSITION;
	float3				normal		: NORMAL;
	float2				uv			: TEXCOORD;
};

struct PixelShaderInput {
	float4				position	: SV_POSITION;
	VertexShaderInput	world;
};