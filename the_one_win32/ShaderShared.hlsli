
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