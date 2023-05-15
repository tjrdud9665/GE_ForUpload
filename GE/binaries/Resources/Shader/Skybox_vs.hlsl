

// source: http://richardssoftware.net/Home/Post/25

struct VSIn
{
	float3 position : POSITION;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT0;
	float2 texCoord : TEXCOORD0;
};

struct PSIn
{
	float4 HPos : SV_POSITION;
	float3 LPos : POSITION;
};

cbuffer matrices {
	matrix worldViewProj;
	float fovH;
};

//#include "Panini.hlsl"

PSIn VSMain(VSIn In)
{
    const float4 projectedPos = mul(worldViewProj, float4(In.position, 1.0f)).xyww;
	
	PSIn Out;
	Out.LPos = In.position;	 // sample direction
	Out.HPos = projectedPos; // z=w makes depth=1 -> drawn at far plane
	return Out;
}