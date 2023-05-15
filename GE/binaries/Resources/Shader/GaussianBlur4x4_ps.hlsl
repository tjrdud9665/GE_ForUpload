struct PSIn
{
	float4 position : SV_POSITION;
	float2 uv		: TEXCOORD0;
};

Texture2D tOcclusion;
SamplerState BlurSampler;

cbuffer cb
{
	float2 inputTextureDimensions;
};

float PSMain(PSIn In) : SV_TARGET
{
	const float2 texelSize = 1.0f / inputTextureDimensions;

	float result = 0.0f;
	for (int x = -2; x < 2; ++x)
	{
		for (int y = -2; y < 2; ++y)
		{
			const float2 offset = float2(x, y) * texelSize;
			result += tOcclusion.Sample(BlurSampler, In.uv + offset).r;
		}
	}
	return result / 16;	// =4x4 kernel size
}