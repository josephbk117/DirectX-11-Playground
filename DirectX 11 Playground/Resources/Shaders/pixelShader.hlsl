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

	float nDotL = dot(input.norm, -input.lightDir);

	float modBias = max((bias + 0.005) * - nDotL, bias);
    float closestDepth = shadowMap.Sample(shadowSamplerState, projCoords.xy).r;

	float shadow = 0.0;
	float2 texelSize = 1.0 / dim(shadowMap);

	const int KERNEL_SIZE = 1;

	[unroll]
	for (int x = -KERNEL_SIZE; x <= KERNEL_SIZE; ++x)
	{
		for (int y = -KERNEL_SIZE; y <= KERNEL_SIZE; ++y)
		{
			float closestDepth = shadowMap.Sample(shadowSamplerState, projCoords.xy + float2(x, y) * texelSize).r;
			float shadowVal = currentDepth - modBias > closestDepth ? 0.5 : 1.0;
			shadow += shadowVal;
		}
	}
	shadow /= pow(((KERNEL_SIZE*2.0)+1.0), 2);
	
	float visibilty = shadow;

	float3 col = objTexture.Sample(objSamplerState, input.tex);
	col *= max(nDotL, ambientLightIntensity);
	float fogVal = 1.0 - (saturate(input.pos.z/input.pos.w) * 0.5 + 0.5);
	col = lerp(col, ambientLightColour, pow(fogVal, 3));
	col *= visibilty;
	return float4(col, 1);
}