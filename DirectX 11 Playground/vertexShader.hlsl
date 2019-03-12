struct VS_IN
{
	float3 pos : POSITION;
	float2 tex : TEXCOORD;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

VS_OUT main(VS_IN input)
{
	VS_OUT o;
	o.pos = float4(input.pos, 1);
	o.tex = input.tex;
	return o;
}