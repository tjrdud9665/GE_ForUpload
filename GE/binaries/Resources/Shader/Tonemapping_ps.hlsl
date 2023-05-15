
#define ENABLE_TONEMAPPING 1

struct PSIn
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

Texture2D ColorTexture;
SamplerState Sampler;

cbuffer constants 
{
	float exposure;
};

// src
//	: reference	: https://learnopengl.com/#!Advanced-Lighting/HDR
//	: tonemap	: https://photo.stackexchange.com/questions/7630/what-is-tone-mapping-how-does-it-relate-to-hdr
//	: hdr		: https://www.slideshare.net/ozlael/hable-john-uncharted2-hdr-lighting
//	: hdr		: https://gamedev.stackexchange.com/questions/62836/does-hdr-rendering-have-any-benefits-if-bloom-wont-be-applied
//  : gamma		: http://blog.johnnovak.net/2016/09/21/what-every-coder-should-know-about-gamma/
float4 PSMain(PSIn In) : SV_TARGET
{
	const float3 color = ColorTexture.Sample(Sampler, In.texCoord);
	const float gamma = 1.0f / 2.2f;
	
#if ENABLE_TONEMAPPING

#ifdef HDR_TONEMAPPER
	float3 toneMappedColor =  float3(1, 1, 1) - exp(-color * exposure);
#else // LDR_TONEMAPPER
	float3 toneMappedColor = color / (color + float3(1, 1, 1));
#endif // HDR_TONEMAPPER
#else

	float3 toneMapped = color; // no tonemapping

#endif // ENABLE_TONEMAPPING


#ifdef SINGLE_CHANNEL
	toneMappedColor = pow(toneMappedColor.r, gamma).rrr;
#else
	toneMappedColor = pow(toneMappedColor, gamma.xxx);
#endif

	return float4(toneMappedColor, 1.0f);
}