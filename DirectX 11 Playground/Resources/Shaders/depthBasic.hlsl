struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float weight : TEXCOORD1;
};

float4 main(PS_IN input) : SV_TARGET
{
	float3 col = input.pos.z/input.pos.w;
	return float4(col,1);
}