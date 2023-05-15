

struct PSIn
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

Texture2D inputTexture;

// todo get from app
SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;
};

cbuffer c{
	float isDepthTexture;
	int numChannels;
};

float4 PSMain(PSIn In) : SV_TARGET
{
	const float depthExponent = 300;
	float4 color = inputTexture.Sample(samAnisotropic, In.texCoord);
	if (isDepthTexture > 0)
	{
		color.yzw = float3(color.x, color.x, color.x);
		color = pow(color, float4(depthExponent, depthExponent, depthExponent, 1));
	}
	if (numChannels == 1)
		return float4(color.xxx, color.w);
	else
		return color;
}