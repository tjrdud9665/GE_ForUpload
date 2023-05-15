#include "LightingCommon.hlsl"
#include "ShadingMath.hlsl"

#define EPSILON 0.000000000001f

#define _DEBUG

//#define INSTANCED 1


struct PSIn
{
    float4 position : SV_POSITION;
    float3 viewPosition : POSITION0;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float2 uv : TEXCOORD1;
    
    float4 blendweight : BLENDWEIGHT;
    float4 blendindex : BLENDINDEX;
    
#ifdef INSTANCED
	uint instanceID			: SV_InstanceID;
#endif
};

struct PSOut	// G-BUFFER
{
    float4 diffuseRoughness : SV_TARGET0;
    float4 specularMetalness : SV_TARGET1;
    float4 normals : SV_TARGET2;
    float4 emissiveColor : SV_TARGET3;
};


cbuffer cbSurfaceMaterial : register(b11)
{

    SurfaceMaterial surfaceMaterial;                
 
};

#ifdef INSTANCED
    StructuredBuffer<SurfaceMaterial> InstancedSurfaceMaterial: register(t22);  
#endif



Texture2D texDiffuseMap;
Texture2D texNormalMap;
Texture2D texSpecularMap;
//Texture2D texAOMap;        // ?
Texture2D texMetallicMap;
Texture2D texRoughnessMap;
Texture2D texEmissiveMap;

Texture2D texAlphaMask;

SamplerState sNormalSampler;
SamplerState sAnisoSampler;




PSOut PSMain(PSIn In) : SV_TARGET
{
    float2 uv = In.uv * surfaceMaterial.UvScale;
    float alpha = 1.0f;
    
#ifdef INSTANCED        
   uv = In.uv * InstancedSurfaceMaterial[In.instanceID].UvScale;
   alpha = HasAlphaMask(InstancedSurfaceMaterial[In.instanceID].TextureConfig) > 0 ? texAlphaMask.Sample(sNormalSampler, uv).r : 1.0f;
    
   if (alpha < 0.01f)
       discard;
 #else
   alpha = HasAlphaMask(surfaceMaterial.TextureConfig) > 0 ? texAlphaMask.Sample(sNormalSampler, uv).r : 1.0f;
   if (alpha < 0.01f)
       discard;
    
#endif


    PSOut GBuffer;
    
    float BRDFOrPhong = 1;

	// lighting & surface parameters
    const float3 P = In.viewPosition;
    const float3 N = normalize(In.viewNormal);
    const float3 T = normalize(In.viewTangent);
    const float3 V = normalize(-P);
    
    int texConfing = 0;
    texConfing = surfaceMaterial.TextureConfig;    
    
    float metalness = surfaceMaterial.Metalness;
    float3 finalDiffuse = surfaceMaterial.Diffuse;
    float3 finalSpecular = surfaceMaterial.Specular;
    float roughnessORshininess = surfaceMaterial.Roughness * BRDFOrPhong
                                + surfaceMaterial.Shininess * (1.0f - BRDFOrPhong);
    float3 emissive = surfaceMaterial.EmissiveColor * surfaceMaterial.EmissiveIntensity;
    float3 finalNormal = N;

#ifdef INSTANCED    
    
	metalness            = InstancedSurfaceMaterial[In.instanceID].Metalness;
	finalDiffuse         = InstancedSurfaceMaterial[In.instanceID].Diffuse;
	finalSpecular        = InstancedSurfaceMaterial[In.instanceID].Specular;
	roughnessORshininess = InstancedSurfaceMaterial[In.instanceID].Roughness * BRDFOrPhong 
	                     + InstancedSurfaceMaterial[In.instanceID].Shininess * (1.0f - BRDFOrPhong);
	emissive             = InstancedSurfaceMaterial[In.instanceID].EmissiveColor * InstancedSurfaceMaterial[In.instanceID].EmissiveIntensity;
	finalNormal = N;    
    texConfing = InstancedSurfaceMaterial[In.instanceID].TextureConfig;   
#endif
    
    
    //Gamma Collections. pow(texDiffuseMap.Sample(sAnisoSampler, uv).xyz, 2.2f);
    const float3 sampledDiffuse = pow(texDiffuseMap.Sample(sAnisoSampler, uv).xyz, 2.2f);
    const float3 surfaceDiffuse = surfaceMaterial.Diffuse;

     finalDiffuse = HasDiffuseMap(texConfing) > 0
		? sampledDiffuse
		: surfaceDiffuse;

    finalNormal = HasNormalMap(texConfing) > 0
		? UnpackNormals(texNormalMap, sAnisoSampler, uv, N, T)
		: N;
    //Single Channel Requirements
     finalSpecular = HasSpecularMap(texConfing) > 0
		? texSpecularMap.Sample(sNormalSampler, uv).rrr
		: surfaceMaterial.Specular;

    roughnessORshininess = HasRoughnessMap(texConfing) > 0
		? texRoughnessMap.Sample(sNormalSampler, uv).ggg
		: surfaceMaterial.Roughness;

    metalness = HasMetallicMap(texConfing) > 0
		? texMetallicMap.Sample(sNormalSampler, uv)
		: surfaceMaterial.Metalness;

     emissive = (HasEmissiveMap(texConfing) > 0
		? texEmissiveMap.Sample(sNormalSampler, uv) * surfaceMaterial.EmissiveIntensity
		: surfaceMaterial.EmissiveColor) * surfaceMaterial.EmissiveIntensity;


    GBuffer.diffuseRoughness        = float4(finalDiffuse, roughnessORshininess);
    GBuffer.specularMetalness       = float4(finalSpecular, metalness);
    GBuffer.normals                 = float4(finalNormal, 1);
    GBuffer.emissiveColor           = float4(emissive, 1);    

    
    return GBuffer;
}