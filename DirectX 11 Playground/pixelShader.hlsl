struct PS_IN
{
	float4 pos : SV_POSITION;
	float3 col : COLOR;
};

float4 main(PS_IN input) : SV_TARGET
{
	return float4(input.col,1);
}