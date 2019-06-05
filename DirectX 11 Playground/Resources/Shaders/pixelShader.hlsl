cbuffer lightBuffer : register(b0)
{
	float ambientLightIntensity;
	float3 ambientLightColour;
	float bias;
}

struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float weight : TEXCOORD1;
	float4 lightPos : TEXCOORD2;
};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D shadowMap : TEXTURE : register(t1);
SamplerState objSamplerState : SAMPLER : register(s0);
SamplerState shadowSamplerState : SAMPLER : register(s1);

float ShadowCalculation(float4 fragPosLightSpace)
{
    // perform perspective divide
    float3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = shadowMap.Sample(objSamplerState, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    return shadow;
}  

float4 main(PS_IN input) : SV_TARGET
{
	// perform perspective divide
    float3 projCoords = input.lightPos.xyz / input.lightPos.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
	projCoords.y = -projCoords.y;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = shadowMap.Sample(shadowSamplerState, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth-bias > closestDepth ? 0.5 : 1.0;
	
	float visibilty = shadow;

	float3 col = objTexture.Sample(objSamplerState, input.tex);
	col *= max(dot(input.norm, float3(0,1,0)), ambientLightIntensity);
	float fogVal = saturate((1.0 - (input.pos.z/input.pos.w)));
	col = lerp(col, float3(0.3,0.3,0.3), pow(fogVal, 3));
	col *= visibilty;

	return float4(col, 1);
}