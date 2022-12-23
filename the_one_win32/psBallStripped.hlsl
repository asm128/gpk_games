#include "PixelShaderShared.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET
{
	float2								relativeToCenter			= input.world.uv - float2(.5f, .5f);
	relativeToCenter.x				*= 2;
	float4								materialColor				= Diffuse;
	bool								shade						= false;
	float								ambientFactor				= LIGHT_FACTOR_AMBIENT;
	const float3						lightVecW					= normalize(LightPosition.xyz - input.world.position);
	const float3						normal						= normalize(input.world.normal);
	const float							diffuseFactor				= dot(normal, lightVecW);
	if(distance(float2(0.0f, 0.0f), relativeToCenter) >= .175f) {
		materialColor									
			= (relativeToCenter.y >  .20f) ? GPK_WHITE
			: (relativeToCenter.y < -.20f) ? GPK_WHITE
			: Diffuse
			;
		shade							= true;
	}
	else {
		const float2							fetchCoord					= 
			{ relativeToCenter.x * 2.f + .5f
			, relativeToCenter.y * 4.f + .5f
			};
		float4									surfacecolor				= texDiffuse.Sample(samplerLinear, fetchCoord);
	
		if((surfacecolor.r + surfacecolor.g + surfacecolor.b) != 0) {
			materialColor										= GPK_WHITE;
			shade												= uint(rand(input.world.uv * diffuseFactor) * 10) % 2;
			ambientFactor										= .35f;
		}
		else {
			materialColor										= GPK_BLACK;
			shade												= true;
		} 
	}
	if(shade) {
		if(diffuseFactor < 0) {
			ambientFactor										+= rand(input.world.uv * diffuseFactor) * diffuseFactor * -.25f;
			materialColor										= float4(saturate(materialColor * ambientFactor).xyz, 1.0f);
		}
		else {
			const float4											specular					= lightCalcSpecular(GPK_WHITE, GPK_WHITE, CameraPositionAndAngle.xyz, LIGHT_FACTOR_SPECULAR_POWER, input.world.position, normal, lightVecW);
			const float4											diffuse						= materialColor * max(0.0, diffuseFactor);
			const float4											ambient						= materialColor * ambientFactor;
			materialColor										= float4(saturate(ambient + diffuse + specular).xyz, 1.0f);
		}
	}
	return materialColor;
}