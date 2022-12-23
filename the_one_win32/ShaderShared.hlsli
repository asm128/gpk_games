cbuffer ModelConstantBuffer : register(b0) {
	matrix						Model;
	matrix						ModelInverseTranspose;
	matrix						MVP;
	float4						Diffuse;
	float4						Ambient;
	float4						Specular;
	float4						EmissionaAndSpecularPower;	// .w or .a for the specular power
};

cbuffer ViewProjectionConstantBuffer : register(b1) {
	matrix						View;
	matrix						Perspective;
	matrix						Screen;
	matrix						VP;
	matrix						VPS;

	float4						CameraPositionAndAngle		;	// use .w for the angle
	float4						CameraFront					; 
	float4						LightPosition				; 
	float4						LightDirectionAndSpotPower	;	// use .w for the spot power
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