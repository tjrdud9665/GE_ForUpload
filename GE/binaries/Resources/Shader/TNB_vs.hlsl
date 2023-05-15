

struct VSIn
{
	float3 position : POSITION;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT0;
	float2 texCoord : TEXCOORD0;
};

struct GSIn
{
	float3 T : TANGENT;
	float3 N : NORMAL;
	float3 B : BINORMAL;
	float3 WorldPosition : POSITION;
};

cbuffer perObj
{
	matrix normalMatrix;
	matrix world;
};

GSIn VSMain(VSIn In)
{
	const float3 B = normalize(cross(In.normal, In.tangent));

	GSIn Out;
	Out.T	 = normalize(mul(normalMatrix, In.tangent));
	Out.N	 = normalize(mul(normalMatrix, In.normal));
	Out.B	 = normalize(mul(normalMatrix, B));
	Out.WorldPosition = mul(world, float4(In.position, 1)).xyz;
	return Out;
}