#pragma pack_matrix( row_major )

cbuffer perObjBuffer : register(b0)
{
	float4x4 mvpMatrix;
	float4x4 worldMatrix;
}

cbuffer lightBuffer : register(b1)
{
	float4x4 lightMatrix;
}

struct VS_IN
{
	float3 pos : POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float weight : TEXCOORD1;
	float4 lightPos : TEXCOORD2;
};

VS_OUT main(VS_IN input)
{
	VS_OUT o;
	o.pos = mul(float4(input.pos, 1), mvpMatrix);
	float4 worldPos = mul(float4(input.pos, 1), worldMatrix);
	o.lightPos = mul(worldPos, lightMatrix);
	o.tex = input.tex;
	o.norm = normalize(mul(float4(input.norm, 0), worldMatrix));
	o.weight = 0;
	return o;
}