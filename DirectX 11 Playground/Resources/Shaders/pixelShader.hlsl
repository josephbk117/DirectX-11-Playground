cbuffer lightBuffer : register(b0)
{
	float ambientLightIntensity;
	float3 ambientLightColour;
}

struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float weight : TEXCOORD1;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_IN input) : SV_TARGET
{
	float3 col = objTexture.Sample(objSamplerState, input.tex);
	col *= max(dot(input.norm, float3(0,1,0)), ambientLightIntensity);
	float fogVal = saturate((1.0 - (input.pos.z/input.pos.w)));
	col = lerp(col, float3(0.3,0.3,0.3), pow(fogVal, 2));
	//col.r = input.weight;
	return float4(col,1);
}