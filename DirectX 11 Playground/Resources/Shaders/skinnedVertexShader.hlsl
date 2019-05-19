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
	float weight : TEXCOORD1;
};

VS_OUT main(VS_IN input)
{
	VS_OUT o;

	float4x4 BoneTransform = joints[input.ids[0]] * input.weights[0];
    BoneTransform += joints[input.ids[1]] * input.weights[1];
    BoneTransform += joints[input.ids[2]] * input.weights[2];

	o.weight = input.weights[0];
	o.pos = mul(float4(input.pos, 1),BoneTransform);
	o.pos = mul(float4(o.pos.xyz, 1),mvpMatrix);
	o.tex = input.tex;
	o.norm = mul(float4(input.norm, 0), BoneTransform);
	o.norm = mul(float4(o.norm, 0), worldMatrix);
	return o;
}