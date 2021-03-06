struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_IN input) : SV_TARGET
{
	float3 col = 1.0 - objTexture.Sample(objSamplerState, input.tex);
	return float4(col, 1);
}