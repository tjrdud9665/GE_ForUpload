#ifndef _LIGHTING_COMMON_H
#define _LIGHTING_COMMON_H

#include "ShadingMath.hlsl"
// some manual tuning...
#define SPOTLIGHT_BRIGHTNESS_SCALAR 0.001f
#define SPOTLIGHT_BRIGHTNESS_SCALAR_PHONG 0.00028f
#define POINTLIGHT_BRIGHTNESS_SCALAR_PHONG 0.002f

//----------------------------------------------------------
// LIGHTS
//----------------------------------------------------------
struct PointLight
{ // 48 bytes
    float3 position;
    float range;
    float3 color;
    float brightness;
    float3 attenuation;
    float depthBias;
};

struct SpotLight
{ // 48 bytes
    float3 position;
    float outerConeAngle;
    float3 color;
    float brightness;
    float3 spotDir;
    float depthBias;
    float innerConeAngle;
    float dummy;
    float dummy1;
    float dummy2;
};

struct DirectionalLight
{ // 40 bytes
    float3 lightDirection;
    float brightness;
    float3 color;
    float depthBias;
    
    int shadowing;
    int enabled;
    
    float2 Padding;
};



// defines maximum number of dynamic lights  todo: shader defines
// don't forget to update CPU define too (RenderPasses.h)
#define NUM_POINT_LIGHT	100
#define NUM_SPOT_LIGHT	20

#define NUM_POINT_LIGHT_SHADOW	5
#define NUM_SPOT_LIGHT_SHADOW	5

#define LIGHT_INDEX_SPOT	0
#define LIGHT_INDEX_POINT	1

struct SceneLighting
{
	// non-shadow caster counts
    int numPointLights;
    int numSpots;
	// shadow caster counts
    int numPointCasters;
    int numSpotCasters;
	//----------------------------------------------
    DirectionalLight directional;
    matrix shadowViewDirectional;

};

StructuredBuffer<PointLight> point_lights;
StructuredBuffer<PointLight> point_casters;

StructuredBuffer<SpotLight> spots;
StructuredBuffer<SpotLight> spot_casters;

//StructuredBuffer<matrix> shadowViews;


	// bit 0: hasDiffuseMap
	// bit 1: hasNormalMap
	// bit 2: hasSpecularMap
	// bit 3: hasAlphaMask

int HasDiffuseMap(int textureConfig)
{
    return ((textureConfig & 1) > 0 ? 1 : 0);
}
int HasNormalMap(int textureConfig)
{
    return ((textureConfig & 2) > 0 ? 1 : 0);
}
int HasSpecularMap(int textureConfig)
{
    return ((textureConfig & 4) > 0 ? 1 : 0);
}
int HasAlphaMask(int textureConfig)
{
    return ((textureConfig & 8) > 0 ? 1 : 0);
}

//For BRDF_GGX
int HasRoughnessMap(int textureConfig)
{
    return ((textureConfig & 16) > 0 ? 1 : 0);
}
inline int HasMetallicMap(int textureConfig)
{
    return ((textureConfig & 32) > 0 ? 1 : 0);
}
int HasHeightMap(int textureConfig)
{
    return ((textureConfig & 64) > 0 ? 1 : 0);
}
int HasEmissiveMap(int textureConfig)
{
    return ((textureConfig & 128) > 0 ? 1 : 0);
}


struct SurfaceMaterial
{
    float3      Diffuse;
    float       Alpha;

    float3      Specular;
    float       Roughness;

    float       Metalness;
    float       Shininess;
    float2      UvScale;

    int         TextureConfig;          //Shift
    int         pad0, pad1, pad2;

    float3      EmissiveColor;
    float       EmissiveIntensity;
};







// SHADOW TESTS
//      //Percentage Closer Filtering
struct ShadowTestPCFData
{
	//-------------------------
    float4 lightSpacePos;
	//-------------------------
    float depthBias;
    float NdotL;
    float viewDistanceOfPixel; // of the shaded pixel
	//...
	//-------------------------
};

float OmnidirectionalShadowTest(
	in ShadowTestPCFData pcfTestLightData
	, TextureCubeArray shadowCubeMapArr
	, SamplerState shadowSampler
	, float2 shadowMapDimensions
	, int shadowMapIndex
	, float3 lightVectorWorldSpace
	, float range
)
{
    const float BIAS = pcfTestLightData.depthBias * tan(acos(pcfTestLightData.NdotL));
    float shadow = 0.0f;

    const float closestDepthInLSpace = shadowCubeMapArr.Sample(shadowSampler, float4(-lightVectorWorldSpace, shadowMapIndex)).x;
    const float closestDepthInWorldSpace = closestDepthInLSpace * range;
    shadow += (length(lightVectorWorldSpace) > closestDepthInWorldSpace + pcfTestLightData.depthBias) ? 1.0f : 0.0f;

    return 1.0f - shadow;
}
float OmnidirectionalShadowTestPCF(
	in ShadowTestPCFData pcfTestLightData
	, TextureCubeArray shadowCubeMapArr
	, SamplerState shadowSampler
	, float2 shadowMapDimensions
	, int shadowMapIndex
	, float3 lightVectorWorldSpace
	, float range
)
{
#define NUM_OMNIDIRECTIONAL_PCF_TAPS 20
    const float3 sampleOffsetDirections[NUM_OMNIDIRECTIONAL_PCF_TAPS] =
    {
        float3(1, 1, 1), float3(1, -1, 1), float3(-1, -1, 1), float3(-1, 1, 1),
	   float3(1, 1, -1), float3(1, -1, -1), float3(-1, -1, -1), float3(-1, 1, -1),
	   float3(1, 1, 0), float3(1, -1, 0), float3(-1, -1, 0), float3(-1, 1, 0),
	   float3(1, 0, 1), float3(-1, 0, 1), float3(1, 0, -1), float3(-1, 0, -1),
	   float3(0, 1, 1), float3(0, -1, 1), float3(0, -1, -1), float3(0, 1, -1)
    };

	// const float BIAS = pcfTestLightData.depthBias * tan(acos(pcfTestLightData.NdotL));
	// const float bias = 0.001f;

    float shadow = 0.0f;

	// parameters for determining shadow softness based on view distance to the pixel
    const float diskRadiusScaleFactor = 1.0f / 8.0f;
    const float diskRadius = (1.0f + (pcfTestLightData.viewDistanceOfPixel / range)) * diskRadiusScaleFactor;

	[unroll]
    for (int i = 0; i < NUM_OMNIDIRECTIONAL_PCF_TAPS; ++i)
    {
        const float4 cubemapSampleVec = float4(-(lightVectorWorldSpace + normalize(sampleOffsetDirections[i]) * diskRadius), shadowMapIndex);
        const float closestDepthInLSpace = shadowCubeMapArr.Sample(shadowSampler, cubemapSampleVec).x;
        const float closestDepthInWorldSpace = closestDepthInLSpace * range;
        shadow += (length(lightVectorWorldSpace) > closestDepthInWorldSpace + pcfTestLightData.depthBias) ? 1.0f : 0.0f;
    }
    shadow /= NUM_OMNIDIRECTIONAL_PCF_TAPS;
    return 1.0f - shadow;
}

// todo: ESM - http://www.cad.zju.edu.cn/home/jqfeng/papers/Exponential%20Soft%20Shadow%20Mapping.pdf
float ShadowTestPCF(in ShadowTestPCFData pcfTestLightData, Texture2DArray shadowMapArr, SamplerState shadowSampler, float2 shadowMapDimensions, int shadowMapIndex)
{
	// homogeneous position after interpolation
    const float3 projLSpaceCoords = pcfTestLightData.lightSpacePos.xyz / pcfTestLightData.lightSpacePos.w;

	// frustum check
    if (projLSpaceCoords.x < -1.0f || projLSpaceCoords.x > 1.0f ||
		projLSpaceCoords.y < -1.0f || projLSpaceCoords.y > 1.0f ||
		projLSpaceCoords.z < 0.0f || projLSpaceCoords.z > 1.0f
		)
    {
        return 0.0f;
    }


    const float BIAS = pcfTestLightData.depthBias * tan(acos(pcfTestLightData.NdotL));
    float shadow = 0.0f;

    const float2 texelSize = 1.0f / (shadowMapDimensions);
	
	// clip space [-1, 1] --> texture space [0, 1]
    const float2 shadowTexCoords = float2(0.5f, 0.5f) + projLSpaceCoords.xy * float2(0.5f, -0.5f); // invert Y
    const float pxDepthInLSpace = projLSpaceCoords.z;


	// PCF
    const int rowHalfSize = 2;
    for (int x = -rowHalfSize; x <= rowHalfSize; ++x)
    {
        for (int y = -rowHalfSize; y <= rowHalfSize; ++y)
        {
            float2 texelOffset = float2(x, y) * texelSize;
            float closestDepthInLSpace = shadowMapArr.Sample(shadowSampler, float3(shadowTexCoords + texelOffset, shadowMapIndex)).x;

			// depth check
            shadow += (pxDepthInLSpace - BIAS > closestDepthInLSpace) ? 1.0f : 0.0f;
        }
    }

    shadow /= (rowHalfSize * 2 + 1) * (rowHalfSize * 2 + 1);

    return 1.0 - shadow;
}



float ShadowTestPCF_Directional(
	in ShadowTestPCFData pcfTestLightData
	, Texture2DArray shadowMapArr
	, SamplerState shadowSampler
	, float2 shadowMapDimensions
	, int shadowMapIndex
	, in matrix lightProj
)
{
	// homogeneous position after interpolation
    const float3 projLSpaceCoords = pcfTestLightData.lightSpacePos.xyz / pcfTestLightData.lightSpacePos.w;

	//// frustum check
 //   if (projLSpaceCoords.x < -1.0f || projLSpaceCoords.x > 1.0f ||
	//	projLSpaceCoords.y < -1.0f || projLSpaceCoords.y > 1.0f ||
	//	projLSpaceCoords.z < 0.0f || projLSpaceCoords.z > 1.0f
	//	)
 //   {
 //       return 0.0f;
 //   }


    const float BIAS = pcfTestLightData.depthBias * tan(acos(pcfTestLightData.NdotL));
    float shadow = 0.0f;

    const float2 texelSize = 1.0f / (shadowMapDimensions);

	// clip space [-1, 1] --> texture space [0, 1]
    const float2 shadowTexCoords = float2(0.5f, 0.5f) + projLSpaceCoords.xy * float2(0.5f, -0.5f); // invert Y
    const float pxDepthInLSpace = projLSpaceCoords.z;


	// PCF
    const int rowHalfSize = 2;
    for (int x = -rowHalfSize; x <= rowHalfSize; ++x)
    {
        for (int y = -rowHalfSize; y <= rowHalfSize; ++y)
        {
            float2 texelOffset = float2(x, y) * texelSize;
            float closestDepthInLSpace = shadowMapArr.Sample(shadowSampler, float3(shadowTexCoords + texelOffset, shadowMapIndex)).x;

			// depth check
            const float linearCurrentPx = LinearDepth(pxDepthInLSpace, lightProj);
            const float linearClosestPx = LinearDepth(closestDepthInLSpace, lightProj);
            
            shadow += (pxDepthInLSpace - pcfTestLightData.depthBias > closestDepthInLSpace) ? 1.0f : 0.0f;
			

        }
    }

    shadow /= (rowHalfSize * 2 + 1) * (rowHalfSize * 2 + 1);

    return 1.0f - shadow;
}

//----------------------------------------------------------
// LIGHTING FUNCTIONS
//----------------------------------------------------------
inline float AttenuationBRDF(float2 coeffs, float dist)
{
    return 1.0f / (dist * dist); // quadratic attenuation (inverse square) is physically more accurate
}

inline float AttenuationPhong(float2 coeffs, float dist)
{
    return 1.0f / (
		1.0f
		+ coeffs[0] * dist
		+ coeffs[1] * dist * dist
		);
}



// LearnOpenGL: PBR Lighting
//
// You may still want to use the constant, linear, quadratic attenuation equation that(while not physically correct) 
// can offer you significantly more control over the light's energy falloff.
inline float Attenuation(float3 coeffs, float dist)
{
    return 1.0f / (
		coeffs[0]
		+ coeffs[1] * dist
		+ coeffs[2] * dist * dist
	);
}


float SpotlightIntensity(SpotLight l, float3 worldPos)
{
    const float3 pixelDirectionInWorldSpace = normalize(worldPos - l.position);
    const float3 spotDir = normalize(l.spotDir);
#if 1
    const float theta = acos(dot(pixelDirectionInWorldSpace, spotDir));

    if (theta > l.outerConeAngle)
        return 0.0f;
    if (theta <= l.innerConeAngle)
        return 1.0f;
    return 1.0f - (theta - l.innerConeAngle) / (l.outerConeAngle - l.innerConeAngle);
#else
	if (dot(spotDir, pixelDirectionInWorldSpace) < cos(l.outerConeAngle))
		return 0.0f;
	else
		return 1.0f;
#endif
}

float3 ShadowTestDebug(float3 worldPos, float4 lightSpacePos, float3 illumination, Texture2D shadowMap, SamplerState shadowSampler)
{
    float3 outOfFrustum = -illumination + float3(1, 1, 0);
    float3 inShadows = -illumination + float3(1, 0, 0);
    float3 noShadows = -illumination + float3(0, 1, 0);

    float3 projLSpaceCoords = lightSpacePos.xyz / lightSpacePos.w;
	//projLSpaceCoords.x *= -1.0f;

    float2 shadowTexCoords = float2(0.5f, 0.5f) + projLSpaceCoords.xy * float2(0.5f, -0.5f); // invert Y
    float pxDepthInLSpace = projLSpaceCoords.z;
    float closestDepthInLSpace = shadowMap.Sample(shadowSampler, shadowTexCoords).x;

    if (projLSpaceCoords.x < -1.0f || projLSpaceCoords.x > 1.0f ||
		projLSpaceCoords.y < -1.0f || projLSpaceCoords.y > 1.0f ||
		projLSpaceCoords.z < 0.0f || projLSpaceCoords.z > 1.0f
		)
    {
        return outOfFrustum;
    }

    if (pxDepthInLSpace > closestDepthInLSpace)
    {
        return inShadows;
    }

    return noShadows;
}

#endif