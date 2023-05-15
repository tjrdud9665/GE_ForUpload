

#include "BRDF.hlsl"
#include "LightingCommon.hlsl"

#define ENABLE_POINT_LIGHTS 1
#define ENABLE_POINT_LIGHTS_SHADOW 1

#define ENABLE_SPOT_LIGHTS 1
#define ENABLE_SPOT_LIGHTS_SHADOW 1

#define ENABLE_DIRECTIONAL_LIGHTS 1

struct PSIn
{
	float4 position		 : SV_POSITION;
	float2 uv			 : TEXCOORD0;
};


// CBUFFERS
//-----------------------------------------------------------------------------------------------------------------------------------------
cbuffer SceneVariables : register(b5) // frame constants
{
	matrix matView;
	matrix matViewToWorld;
	matrix matProjInverse;
	
	//float2 pointShadowMapDimensions;
	float2 spotShadowMapDimensions;
	float  directionalShadowMapDimension;
    float dummy;
	matrix directionalProj;	
	
    SceneLighting Lights;
};





// TEXTURES & SAMPLERS
//-----------------------------------------------------------------------------------------------------------------------------------------
Texture2D texDiffuseRoughnessMap;
Texture2D texSpecularMetalnessMap;
Texture2D texNormals;
Texture2D texEmissiveMap;
Texture2D texDepth;

Texture2D texAmbient;

TextureCubeArray texPointShadowMaps;
Texture2DArray   texSpotShadowMaps;
Texture2DArray   texDirectionalShadowMaps;

SamplerState sShadowSampler;
SamplerState sLinearSampler;


float4 PSMain(PSIn In) : SV_TARGET
{

	ShadowTestPCFData pcfTest;

	// base indices for indexing shadow views
	const int pointShadowsBaseIndex = 0;	// omnidirectional cubemaps are sampled based on light dir, texture is its own array
	const int spotShadowsBaseIndex = 0;		
	const int directionalShadowBaseIndex = spotShadowsBaseIndex + Lights.numSpotCasters;
	const float2 dirShadowMapDimensions = directionalShadowMapDimension.xx;

	// lighting & surface parameters (View Space Lighting)
    const float nonLinearDepth = texDepth.Sample(sLinearSampler, In.uv).r;
	
 
	//For PS Optimize
    if (nonLinearDepth >= 1.0f)
        discard;
	
	const float3 P = ViewSpacePosition(nonLinearDepth, In.uv, matProjInverse);
	const float3 N = texNormals.Sample(sLinearSampler, In.uv);
	//const float3 T = normalize(In.tangent);
	const float3 V = normalize(- P);
	
	const float3 Pw = mul(matViewToWorld, float4(P, 1)).xyz;	// world position of the shaded pixel
	const float3 Pcam = float3(matViewToWorld._14, matViewToWorld._24, matViewToWorld._34);// world position of the camera
	pcfTest.viewDistanceOfPixel = length(Pw - Pcam);

	const float4 diffuseRoughness  = texDiffuseRoughnessMap.Sample(sLinearSampler, In.uv);
	const float4 specularMetalness = texSpecularMetalnessMap.Sample(sLinearSampler, In.uv);

    BRDF_Surface s;
    s.N = N;
	s.diffuseColor = diffuseRoughness.rgb;
	s.specularColor = specularMetalness.rgb;
	s.roughness = diffuseRoughness.a;
	s.metalness = specularMetalness.a;
	
    
   

    float3 IdIs = texAmbient.Sample(sLinearSampler, In.uv).rgb;
	
    float3 Ie = texEmissiveMap.Sample(sLinearSampler, In.uv);
	
    
	
//-- POINT LIGHTS --------------------------------------------------------------------------------------------------------------------------
#if ENABLE_POINT_LIGHTS
	// brightness default: 300
	for (int i = 0; i < Lights.numPointLights; ++i)		
	{		
        const float3 Lv = mul(matView, float4(point_lights[i].position, 1));
		const float3 Wi       = normalize(Lv - P);
        const float D = length(point_lights[i].position - Pw);
		const float NdotL	  = saturate(dot(s.N, Wi));
        const float3 radiance = AttenuationBRDF(point_lights[i].attenuation.xy, D)
			* point_lights[i].color 
			* point_lights[i].brightness;
		
        if (D < point_lights[i].range)
			IdIs += BRDF(Wi, s, V, P) * radiance * NdotL;		
	}
#endif
#if ENABLE_POINT_LIGHTS_SHADOW
	for (int l = 0; l < Lights.numPointCasters; ++l)
	{
        const float3 Lw = point_casters[l].position;
		const float3 Lv       = mul(matView, float4(Lw, 1));
		const float3 Wi       = normalize(Lv - P);
        const float D = length(point_casters[l].position - Pw);
		const float3 radiance = 
			AttenuationBRDF(point_casters[l].attenuation.xy, D)
			* point_casters[l].color 
			* point_casters[l].brightness;

		pcfTest.NdotL = saturate(dot(s.N, Wi));
        pcfTest.depthBias = point_casters[l].depthBias;

        if (D < point_casters[l].range)
		{
			const float3 shadowing = OmnidirectionalShadowTestPCF(
				pcfTest,
				texPointShadowMaps,
				sShadowSampler,
				spotShadowMapDimensions,
				l,
				(Lw - Pw),
				point_casters[l].range
			);
			IdIs += BRDF(Wi, s, V, P) * radiance * shadowing * pcfTest.NdotL;
		}
	}
#endif



//-- SPOT LIGHTS ---------------------------------------------------------------------------------------------------------------------------
#if ENABLE_SPOT_LIGHTS
    for (int j = 0; j < Lights.numSpots; ++j)
    {
        const float3 Lv = mul(matView, float4(spots[j].position, 1));
        const float3 Wi = normalize(Lv - P);
        const float3 radiance = SpotlightIntensity(spots[j], Pw) * spots[j].color * spots[j].brightness * SPOTLIGHT_BRIGHTNESS_SCALAR;
        const float NdotL = saturate(dot(s.N, Wi));
        IdIs += BRDF(Wi, s, V, P) * radiance * NdotL;
    }
#endif
//#if ENABLE_SPOT_LIGHTS_SHADOW
//	for (int k = 0; k < Lights.numSpotCasters; ++k)
//	{
//		const matrix matShadowView = Lights.shadowViews[spotShadowsBaseIndex + k];
//		pcfTest.lightSpacePos  = mul(matShadowView, float4(Pw, 1));
//		const float3 Lv        = mul(matView, float4(Lights.spot_casters[k].position, 1));
//		const float3 Wi        = normalize(Lv - P);
//		const float3 radiance  = SpotlightIntensity(Lights.spot_casters[k], Pw) * Lights.spot_casters[k].color * Lights.spot_casters[k].brightness * SPOTLIGHT_BRIGHTNESS_SCALAR;
//		pcfTest.NdotL          = saturate(dot(s.N, Wi));
//		pcfTest.depthBias      = Lights.spot_casters[k].depthBias;
//		const float3 shadowing = ShadowTestPCF(pcfTest, texSpotShadowMaps, sShadowSampler, spotShadowMapDimensions, k);
//		IdIs += BRDF(Wi, s, V, P) * radiance * shadowing * pcfTest.NdotL;
//	}
//#endif



//-- DIRECTIONAL LIGHT ----------------------------------------------------------------------------------------------------------------------
#if ENABLE_DIRECTIONAL_LIGHTS
    if (Lights.directional.enabled != 0)
    {
        pcfTest.lightSpacePos = mul(Lights.shadowViewDirectional, float4(Pw, 1));
        const float3 Lv = mul(matView, float4(Lights.directional.lightDirection, 0.0f));
        const float3 Wi = normalize(-Lv);
        const float3 radiance
			= Lights.directional.color
			* Lights.directional.brightness;
        pcfTest.NdotL = saturate(dot(s.N, Wi));
        pcfTest.depthBias = Lights.directional.depthBias;        
        const float shadowing = (Lights.directional.shadowing == 0)
		? 1.0f
			: ShadowTestPCF_Directional(
				pcfTest
				, texDirectionalShadowMaps
				, sShadowSampler
				, directionalShadowMapDimension
				, 0
				, directionalProj
			);

        IdIs += BRDF(Wi, s, V, P) * radiance * shadowing * pcfTest.NdotL;
    }
#endif




	//const float3 illumination = Ie + IdIs;
    const float3 illumination = Ie + IdIs;
	return float4(illumination, 1);
}