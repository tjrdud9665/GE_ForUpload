
#include "ShadingMath.hlsl"

struct PSIn
{
	float4 HPos : SV_POSITION;
	float3 LPos : POSITION;
};

Texture2D texSkybox;
SamplerState samWrap;

float4 PSMain(PSIn In) : SV_TARGET
{
	//return float4(In.LPos, 1);
	const float2 uv = SphericalSample(normalize(In.LPos));
//	return float4(texSkybox.Sample(samWrap, uv).xyz, 1);
    return float4(pow(texSkybox.Sample(samWrap, uv).xyz, 2.2f), 1);
	//return float4(uv, 0, 1);	// test
}
