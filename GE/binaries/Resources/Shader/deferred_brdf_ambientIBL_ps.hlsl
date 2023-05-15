

#include "BRDF.hlsl"

struct PSIn
{
	float4 position	: SV_POSITION;
	float2 uv		: TEXCOORD0;
};

cbuffer SceneVariables
{
    matrix matViewInverse;
	matrix matProjInverse;
	float ambientFactor;
};

Texture2D tDiffuseRoughnessMap;
Texture2D tSpecularMetalnessMap;
Texture2D tNormalMap;
Texture2D tDepthMap;
Texture2D tAmbientOcclusion;
Texture2D tIrradianceMap;
TextureCube tPreFilteredEnvironmentMap;
Texture2D tBRDFIntegrationLUT;

SamplerState sWrapSampler;
SamplerState sEnvMapSampler;
SamplerState sNearestSampler;

float4 PSMain(PSIn In) : SV_TARGET
{
    const float4 kD_roughness = tDiffuseRoughnessMap.Sample(sNearestSampler, In.uv);
    const float4 kS_metalness = tSpecularMetalnessMap.Sample(sNearestSampler, In.uv);

	// view-space vectors
    const float3 Pv = ViewSpacePosition(tDepthMap.Sample(sNearestSampler, In.uv).r, In.uv, matProjInverse);
    const float3 Nv = tNormalMap.Sample(sNearestSampler, In.uv).rgb;
    const float3 Vv = normalize(-Pv);
    
	// world-space vectors
	const float3 Vw = normalize(mul(matViewInverse, float4(Vv, 0.0f)).xyz);
    const float3 Nw = mul(matViewInverse, float4(Nv, 0.0f)).xyz;
    const float3 Rw = reflect(-Vw, Nw);
    const float NdotV = max(dot(Nw, Vw), 0);

	// environment map
    const float2 equirectangularUV = SphericalSample(normalize(Nw));
    const float3 environmentIrradience = tIrradianceMap.Sample(sWrapSampler, equirectangularUV).rgb;
    const float3 environmentSpecular = tPreFilteredEnvironmentMap.SampleLevel(sEnvMapSampler, Rw, kD_roughness.a * MAX_REFLECTION_LOD).rgb;
    
	// ambient occl
	const float  ambientOcclusion = tAmbientOcclusion.Sample(sNearestSampler, In.uv).r;	

	// surface
	BRDF_Surface s;
    s.N = Nw;
    s.diffuseColor = kD_roughness.rgb;
    s.specularColor = kS_metalness.rgb;
    s.roughness = kD_roughness.a;
    s.metalness = kS_metalness.a;
    
	const float2 F0ScaleBias = tBRDFIntegrationLUT.Sample(sNearestSampler, float2(NdotV, 1.0f - s.roughness)).rg;

	const float ao = ambientOcclusion * ambientFactor;
    //environmentSpecular
    float3 IEnv = EnvironmentBRDF(s, Vw, ao, environmentIrradience, float3(1, 1, 1), F0ScaleBias);
    return float4(IEnv, 1.0f);
}