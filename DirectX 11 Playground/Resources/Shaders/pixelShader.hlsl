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
	float3 col = objTexture.Sample(objSamplerState, input.tex);
	col *= dot(input.norm, float3(0,1,0)) * 0.5 + 0.5;
	return float4(col,1);
}