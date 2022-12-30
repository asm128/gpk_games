#include "ShaderShared.hlsli"

Texture2D				texDiffuse						: register( t0 );
SamplerState			samplerLinear					: register( s0 );

static const float4		GPK_BLACK						= float4(0.0f, 0.0f, 0.0f, 1.0f					);
static const float4		GPK_WHITE						= float4(1.0f, 1.0f, 1.0f, 1.0f					);
static const float4		GPK_RED							= float4(1.0f, 0.0f, 0.0f, 1.0f					);
static const float4		GPK_GREEN						= float4(0.0f, 1.0f, 0.0f, 1.0f					);
static const float4		GPK_BLUE						= float4(0.0f, 0.0f, 1.0f, 1.0f					);
static const float4		GPK_YELLOW						= float4(1.0f, 1.0f, 0.0f, 1.0f					);
static const float4		GPK_MAGENTA						= float4(1.0f, 0.0f, 1.0f, 1.0f					);
static const float4		GPK_CYAN						= float4(0.0f, 1.0f, 1.0f, 1.0f					);
static const float4		GPK_ORANGE						= float4(1.0f, 0.647f, 0.0f, 1.0f				);
static const float4		GPK_PANOCHE						= float4(0.25f, 0.5f, 1.0f, 1.0f				);
static const float4		GPK_PURPLE						= float4(0.5f, 0.1f, 1.0f, 1.0f					);
static const float4		GPK_TURQUOISE					= float4(0.2f, 1.0, 0.65f, 1.0f					);
static const float4		GPK_BROWN						= float4(0.4f, 0.223f, 0.0f, 1.0f				);
static const float4		GPK_GRAY						= float4(0.5f, 0.5f, 0.5f, 1.0f					);
static const float4		GPK_DARKGRAY					= float4(0.25f, 0.25f, 0.25f, 1.0f				);
static const float4		GPK_DARKRED						= float4(0.5f, 0.0f, 0.0f, 1.0f					);
static const float4		GPK_DARKGREEN					= float4(0.0f, 0.5f, 0.0f, 1.0f					);
static const float4		GPK_DARKBLUE					= float4(0.0f, 0.0f, 0.5f, 1.0f					);
static const float4		GPK_DARKYELLOW					= float4(0.5f, 0.5f, 0.0f, 1.0f					);
static const float4		GPK_DARKMAGENTA					= float4(0.5f, 0.0f, 0.5f, 1.0f					);
static const float4		GPK_DARKCYAN					= float4(0.0f, 0.5f, 0.5f, 1.0f					);
static const float4		GPK_DARKORANGE					= float4(1.0f, 0.5490196078431373f, 0.0f, 1.0f	);
static const float4		GPK_LIGHTGRAY					= float4(0.75f, 0.75f, 0.75f, 1.0f				);
static const float4		GPK_LIGHTRED					= float4(1.0f, 0.25f, 0.25f, 1.0f				);
static const float4		GPK_LIGHTGREEN					= float4(0.5f,  1.0f, 0.5f, 1.0f				);
static const float4		GPK_LIGHTBLUE					= float4(0.25f, 0.25f, 1.0f, 1.0f				);
static const float4		GPK_LIGHTYELLOW					= float4(1.0f, 1.0f, 0.25f, 1.0f				);
static const float4		GPK_LIGHTMAGENTA				= float4(1.0f, 0.25f, 1.0f, 1.0f				);
static const float4		GPK_LIGHTCYAN					= float4(0.25f, 1.0f, 1.0f, 1.0f				);
static const float4		GPK_LIGHTORANGE					= float4(1.0f, 0.780f, 0.25f, 1.0f				);

static const float		LIGHT_FACTOR_AMBIENT			= .025f;
static const float		LIGHT_FACTOR_SPECULAR_POWER		= 30.0f;

float4					lightCalcDiffuse				(float4 diffuseMaterial, float3 normalW, float3 lightVecW) {
	float						lightFactor						= max(dot(lightVecW, normalW), 0.0f);
	return lightFactor ? float4((diffuseMaterial * (float)lightFactor).rgb, diffuseMaterial.a) : float4(0, 0, 0, 1);
}

float4					lightCalcAmbient				(float4 ambientMaterial, float4 ambientLight) {
	return float4((ambientMaterial * ambientLight).rgb, ambientMaterial.a);
}

float					lightCalcAttenuation			(float3 attenuation012, float3 lightPosW, float3 posW) {
	float						d								= distance(lightPosW, posW);
	return attenuation012.x + attenuation012.y * d + attenuation012.z * d * d; // attenuation
}

float4					lightCalcSpecular				(float4 specularMaterial, float4 specularLight, float3 eyePosW, float specularPower, float3 posW, float3 normalW, float3 lightVecW) {
	const float3				pointToEye						= normalize(eyePosW - posW);
	const float3				reflected						= reflect(-lightVecW, normalW);
	const float					reflectedFactor					= max(dot(reflected, pointToEye), 0.0f);
	if(0 >= reflectedFactor) 
		return float4(0, 0, 0, specularMaterial.a);
	else {
		const float					factor							= pow(reflectedFactor, specularPower);
		return specularMaterial * specularLight * factor;
	}
}

float rand (float2 uv) {
	return frac(sin(dot(uv,float2(12.9898,78.233)))*43758.5453123);
}