
struct PSIn
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

cbuffer perObject : register(b11)
{
	float3 color;
};

Texture2D textMap;
SamplerState samText;


float4 PSMain(PSIn In) : SV_TARGET
{
	const float2 uv = In.texCoord;
	float alpha = textMap.Sample(samText, uv).r;
    return float4(color, alpha);
}