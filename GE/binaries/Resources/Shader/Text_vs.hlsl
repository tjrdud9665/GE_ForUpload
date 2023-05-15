

cbuffer perModel
{
    matrix projection;
}

struct VSIn
{
	float4 positionAndUVs : POSITION;
};

struct PSIn
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

PSIn VSMain(VSIn In)
{
	PSIn Out;
	Out.position = mul(projection, float4(In.positionAndUVs.xy, 0, 1));
	Out.position.z = 0.001f;
	Out.texCoord = In.positionAndUVs.zw;
	return Out;
}