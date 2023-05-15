

#include "ShadingMath.hlsl"

struct PSIn
{
	float4 HPos : SV_POSITION;
	float3 LPos : POSITION;
};

struct PSOut
{
	float4 cubeMapFace : SV_TARGET0;
};

Texture2D tEnvironmentMap;
SamplerState sLinear;

PSOut PSMain(PSIn In)
{
	PSOut cubeMap;
	const float2 uv = SphericalSample(normalize(In.LPos));
	cubeMap.cubeMapFace = tEnvironmentMap.Sample(sLinear, uv);
	return cubeMap;
}