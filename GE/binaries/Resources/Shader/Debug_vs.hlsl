
struct VSIn
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD0;
};

struct PSIn
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

cbuffer perTexture
{
	matrix screenSpaceTransformation;
};

PSIn VSMain(VSIn In)
{
	PSIn psIn;
	psIn.position = mul(screenSpaceTransformation, float4(float3(In.position.xy, 0.0f), 1.0f) );
	psIn.texCoord = In.texCoord;
	return psIn;
}