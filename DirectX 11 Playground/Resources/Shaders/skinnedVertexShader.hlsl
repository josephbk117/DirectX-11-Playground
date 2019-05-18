#pragma pack_matrix( row_major )
#define MAX_JOINTS 50
#define MAX_WEIGHTS 3

cbuffer animatedPerObjBuffer : register(b0)
{
	float4x4 mvpMatrix;
	float4x4 worldMatrix;
	float4x4 joints[MAX_JOINTS];
}

struct VS_IN
{
	float3 pos : POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	uint3 ids : JOINT_IDS;
	float3 weights: WEIGHTS;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
};

VS_OUT main(VS_IN input)
{
	VS_OUT o;

	float4 totalLocalPos = {0.0,0.0,0.0,0.0};
	float4 totalNormal = {0.0,0.0,0.0,0.0};

	for(uint i = 0; i < MAX_WEIGHTS; i++)
	{
		float4x4 jointTransform = joints[input.ids[i]];
		float4 posePosition = mul(jointTransform, float4(input.pos.xyz, 1.0));
		totalLocalPos += posePosition * input.weights[i];

		float4 worldNormal = mul(jointTransform, float4(input.norm.xyz, 0.0));
		totalNormal += worldNormal * input.weights[i];
	}

	o.pos = mul(totalLocalPos, mvpMatrix);
	o.tex = input.tex;
	o.norm = normalize(mul(float4(input.norm, 0), worldMatrix));
	return o;
}