
struct VertexShaderInput {
	float3				position	: POSITION;
	float3				normal		: NORMAL;
	float2				uv			: TEXCOORD;
};

struct PixelShaderInput
{
	float4				position	: SV_POSITION;
	VertexShaderInput	world;
};