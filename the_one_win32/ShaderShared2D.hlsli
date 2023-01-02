struct GUIVertexShaderInput {
	float3				position	: POSITION;
	float2				uv			: TEXCOORD;
};

struct GUIPixelShaderInput {
	float4				position	: SV_POSITION;
	float2				uv			: TEXCOORD;
};
