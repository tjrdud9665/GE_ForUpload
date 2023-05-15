

struct PointLightBuffer
{
	float3 vLightPosition;
	float fFarPlane;
};

cbuffer perLight : register(b11)
{
	PointLightBuffer cbLight;
};

struct PSIn
{
	float4 svPosition     : SV_POSITION;
	float4 worldPosition  : POSITION0;
	
#ifdef INSTANCED
	uint instanceID			: SV_InstanceID;
#endif
};


float PSMain(PSIn In) : SV_Depth
{
	const float depth = length(cbLight.vLightPosition - In.worldPosition);
	return depth / cbLight.fFarPlane;
}