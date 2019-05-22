cbuffer lightBuffer : register(b0)
{
	float ambientLightIntensity;
	float3 ambientLightColour;
	float4x4 lightMatrix;
}

struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float weight : TEXCOORD1;
	float4 worldPos : TEXCOORD2;
};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D shadowMap : TEXTURE : register(t1);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_IN input) : SV_TARGET
{	
	float4 shadowCoord = mul(lightMatrix, input.worldPos);
	float visibilty = 1.0;
	if(shadowMap.Sample(objSamplerState, shadowCoord.xy * 0.5 + 0.5).z <= shadowCoord.z)
		visibilty = 0.5;

	float3 col = objTexture.Sample(objSamplerState, input.tex);
	col *= max(dot(input.norm, float3(0,1,0)), ambientLightIntensity);
	float fogVal = saturate((1.0 - (input.pos.z/input.pos.w)));
	col = lerp(col, float3(0.3,0.3,0.3), pow(fogVal, 2));
	col *= visibilty;

	return float4(col,1);
}