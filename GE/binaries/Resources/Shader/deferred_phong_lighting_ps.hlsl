#include "Phong.hlsl"
#include "LightingCommon.hlsl"

#define BLINN_PHONG

struct PSIn
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};


// CBUFFERS
//---------------------------------------------------------
cbuffer SceneVariables
{
    matrix matView;
    matrix matViewToWorld;
    matrix matProjInverse;
	
    float2 spotShadowMapDimensions;
    float2 pad;
	
    SceneLighting sceneLightData;
};
TextureCubeArray texPointShadowMaps;
Texture2DArray texSpotShadowMaps;
Texture2DArray texDirectionalShadowMaps;





// TEXTURES & SAMPLERS
//---------------------------------------------------------
Texture2D texDiffuseRoughnessMap;
Texture2D texSpecularMetalnessMap;
Texture2D texNormals;
Texture2D texDepth;

SamplerState sShadowSampler;
SamplerState sLinearSampler;

//StructuredBuffer<TestInfo> test;


float4 PSMain(PSIn In) : SV_TARGET
{	
    float3 diffuse = texDiffuseRoughnessMap.Sample(sLinearSampler, In.uv).rgb;
	
    const float3 P = ViewSpacePosition(texDepth.Sample(sLinearSampler, In.uv).r, In.uv, matProjInverse);   
    
    return float4(diffuse, 1);  
    
	
    
 //   ShadowTestPCFData pcfTest;
 //   pcfTest.depthBias = 0.0000005f;
	

	////// base indices for indexing shadow views
 //   const int pointShadowsBaseIndex = 0; // omnidirectional cubemaps are sampled based on light dir, texture is its own array
 //   const int spotShadowsBaseIndex = 0;
 //   const int directionalShadowBaseIndex = spotShadowsBaseIndex + sceneLightData.numSpotCasters; // currently unused

	////// lighting & surface parameters (View Space Lighting)
 //   const float3 P = ViewSpacePosition(texDepth.Sample(sLinearSampler, In.uv).r, In.uv, matProjInverse);
 //   const float3 N = normalize(texNormals.Sample(sLinearSampler, In.uv).xyz);
 //   const float3 V = normalize(-P);
	
 //   const float3 Pw = mul(matViewToWorld, float4(P, 1)).xyz;
 //   const float3 Vw = mul(matViewToWorld, float4(V, 0)).xyz;
 //   const float3 Nw = mul(matViewToWorld, float4(N, 0)).xyz;

 //   const float4 diffuseRoughness = texDiffuseRoughnessMap.Sample(sLinearSampler, In.uv);
 //   const float4 specularMetalness = texSpecularMetalnessMap.Sample(sLinearSampler, In.uv);

 //   PHONG_Surface s;
 //   s.N = Nw;
 //   s.diffuseColor = diffuseRoughness.rgb;
 //   s.specularColor = specularMetalness.rgb;
 //   s.shininess = diffuseRoughness.a; // shininess is stored in alpha channel

 //    float3 IdIs = float3(0.0f, 0.0f, 0.0f); // diffuse & specular
	
	////// POINT Lights w/o shadows
 //   for (int i = 0; i < sceneLightData.numPointLights; ++i)
 //   {        
 //       float3 Lw = normalize(point_lights[i].position - Pw);
 //       float NdotL = saturate(dot(Nw, Lw));
 //       IdIs +=
	//	Phong(s, Lw, Vw, point_lights[i].color)
	//	* AttenuationPhong(point_lights[i].attenuation.xy, length(point_lights[i].position - Pw))
	//	* point_lights[i].brightness 
	//	* NdotL
	//	* POINTLIGHT_BRIGHTNESS_SCALAR_PHONG;
 //   }
    
	
	////// SPOT Lights w/o shadows
 ////   for (int j = 0; j < sceneLightData.numSpots; ++j)
 ////   {
 ////       float3 Lw = normalize(sceneLightData.spots[j].position - Pw);
 ////       float NdotL = saturate(dot(Nw, Lw));
 ////       IdIs +=
	////	Phong(s, Lw, Vw, sceneLightData.spots[j].color)
	////	* SpotlightIntensity(sceneLightData.spots[j], Pw)
	////	* sceneLightData.spots[j].brightness 
	////	* NdotL
	////	* SPOTLIGHT_BRIGHTNESS_SCALAR_PHONG;
 ////   }

	////// SPOT Lights w/ shadows
 ////   for (int k = 0; k < sceneLightData.numSpotCasters; ++k)
 ////   {
 ////       const matrix matShadowView = sceneLightData.shadowViews[spotShadowsBaseIndex + k];
 ////       pcfTest.lightSpacePos = mul(matShadowView, float4(Pw, 1));
 ////       float3 Lw = normalize(sceneLightData.spot_casters[k].position - Pw);
 ////       pcfTest.NdotL = saturate(dot(Nw, Lw));
 ////       IdIs +=
	////	Phong(s, Lw, Vw, sceneLightData.spot_casters[k].color)
	////	* SpotlightIntensity(sceneLightData.spot_casters[k], Pw)
	////	* ShadowTestPCF(pcfTest, texSpotShadowMaps, sShadowSampler, spotShadowMapDimensions, k)
	////	* sceneLightData.spot_casters[k].brightness 
	////	* pcfTest.NdotL
	////	* SPOTLIGHT_BRIGHTNESS_SCALAR_PHONG;
 ////   }


 //   const float3 illumination = IdIs;
 //   return float4(illumination, 1);
}