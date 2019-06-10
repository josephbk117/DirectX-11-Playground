cbuffer unlitBasicBuf : register(b0)
{
	float4 colour;
}

struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_IN input) : SV_TARGET
{
	float3 col = lerp(objTexture.Sample(objSamplerState, input.tex), colour.rgb, colour.a);
	return colour;
}