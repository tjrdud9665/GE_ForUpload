
//cbuffer perModel
//{
//    matrix worldViewProj;
//}

struct VSIn
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD0;
};

struct PSIn
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

PSIn VSMain(VSIn In)
{
	PSIn Out;
    Out.position = float4(In.position, 1);
	//mul(worldViewProj, float4(In.position, 1));
	Out.texCoord = In.texCoord;
	return Out;
}