

//cbuffer perFrame
//{
//	matrix view;
//	matrix	proj;
//}

cbuffer perModel
{
    matrix world;
	matrix normalMatrix;
	matrix worldViewProj;
}

struct VSIn
{
	float3 position : POSITION;
	float3 normal	: NORMAL;
	float2 texCoord : TEXCOORD0;
	float3 tangent	: TANGENT0;
};

struct PSIn
{
	float4 position : SV_POSITION;
	float3 normal	: NORMAL;
    float3 tangent	: TANGENT;
	float2 texCoord : TEXCOORD4;
};

PSIn VSMain(VSIn In)
{
	PSIn Out;
    Out.position = mul(worldViewProj, float4(In.position, 1));
    Out.normal   = mul(normalMatrix, In.normal);
    Out.tangent  = mul(normalMatrix, In.tangent);
    Out.texCoord = In.texCoord;
	
	return Out;
}