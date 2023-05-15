

struct PSIn
{
	float4 position		 : SV_POSITION;
	float2 uv			 : TEXCOORD0;
};

cbuffer SceneVariables
{
	float ambientFactor;
};


Texture2D tDiffuseRoughnessMap;
Texture2D tAmbientOcclusion;

Texture2D texDepth;

SamplerState sNearestSampler;
SamplerState sLinearSampler;


float4 PSMain(PSIn In) : SV_TARGET
{        

    const float nonLinearDepth = texDepth.Sample(sLinearSampler, In.uv).r;

    if (nonLinearDepth >= 1.0f)
        discard;
	
	const float3 diffuseColor     = tDiffuseRoughnessMap.Sample(sNearestSampler, In.uv).rgb;
	const float  ambientOcclusion = tAmbientOcclusion.Sample(sNearestSampler, In.uv).r;
	
    //* ambientOcclusion 
    const float3 Ia = diffuseColor * ambientFactor * ambientOcclusion;
    
	return float4(Ia, 1.0f);
}