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
	float3 lightDir : TEXCOORD3;
};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D shadowMap : TEXTURE : register(t1);
SamplerState objSamplerState : SAMPLER : register(s0);
SamplerState shadowSamplerState : SAMPLER : register(s1);

float2 dim(Texture2D textureObj)
{
	uint width;
	uint height;
	textureObj.GetDimensions(width, height);
	return float2(width, height);
}

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
    float3 projCoords = input.lightPos.xyz / input.lightPos.w;
    projCoords = projCoords * 0.5 + 0.5;
	projCoords.y = -projCoords.y;
	float currentDepth = projCoords.z;
	float modBias = max((bias + 0.005) * (1.0 - dot(input.norm, normalize(-input.lightDir))), bias);

    float closestDepth = shadowMap.Sample(shadowSamplerState, projCoords.xy).r;

	float shadow = 0.0;
	float2 texelSize = 1.0 / dim(shadowMap);
	[unroll]
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float closestDepth = shadowMap.Sample(shadowSamplerState, projCoords.xy + float2(x, y) * texelSize).r;
			float shadowVal = currentDepth - modBias > closestDepth ? 0.5 : 1.0;
			shadow += shadowVal;
		}
	}
	shadow /= 9.0;
	
	float visibilty = shadow;

	float3 col = objTexture.Sample(objSamplerState, input.tex);
	col *= max(dot(input.norm, float3(0,1,0)), ambientLightIntensity);
	float fogVal = saturate((1.0 - (input.pos.z/input.pos.w)));
	col = lerp(col, float3(0.3,0.3,0.3), pow(fogVal, 3));
	col *= visibilty;

	return float4(col, 1);
}