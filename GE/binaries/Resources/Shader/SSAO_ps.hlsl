

#include "ShadingMath.hlsl"

// this helps removing the false occlusion due to 
// low tesselation (sponza arches)
#define SSAO_DEPTH_BIAS 1.2f	// bias to apply during depth testing
#define KERNEL_SIZE 64

struct PSIn
{
	float4 position	: SV_POSITION;
	float2 uv		: TEXCOORD0;
};


struct SceneVariables
{
	matrix matProjection;
	matrix matProjectionInv;
	float2 screenSize;
	float  radius;
	float  intensity;
	float3 samples[KERNEL_SIZE];
};

cbuffer cSceneVariables
{
    SceneVariables SSAO_constants;
};

Texture2D texViewSpaceNormals;
Texture2D<float2> texNoise;


Texture2D texDepth;


SamplerState sNoiseSampler;
SamplerState sPointSampler;
SamplerState sLinearSampler;

float PSMain(PSIn In) : SV_TARGET
{
	const float2 uv = In.uv;

	float3 N = texViewSpaceNormals.Sample(sPointSampler, uv).xyz;
	if(dot(N, N) < 0.00001) return 1.0f;
	N = normalize(N);


	const float zBufferSample = texDepth.Sample(sPointSampler, uv).x;	// non-linear depth

	const float3 P = ViewSpacePosition(zBufferSample, uv, SSAO_constants.matProjectionInv);

	// tile noise texture (4x4) over whole screen by scaling UV coords (textures wrap)
	const float2 noiseScale = SSAO_constants.screenSize / 4.0f;
	const float3 noise = float3(texNoise.Sample(sNoiseSampler, uv * noiseScale).xy, 0.0f);

	// Gramm-Schmidt process for orthogonal basis creation: https://en.wikipedia.org/wiki/Gram%E2%80%93Schmidt_process
	// in short: project noise on N vector, and subtract that projection from the noise vector
	//			 this would give us a vector perpendicular to N. see gif in link 
	const float3 T = normalize(noise - N * dot(noise, N));
	const float3 B = cross(T, N);
	const float3x3 TBN = float3x3(T, B, N);

	float occlusion = 0.0;

	[unroll]	// when unrolled, VGPR usage skyrockets and reduces #waves in flight
	for (int i = 0; i < KERNEL_SIZE; ++i)
	{
		float3 kernelSample = P + mul(SSAO_constants.samples[i], TBN) * SSAO_constants.radius; // From tangent to view-space

		// get the screenspace position of the sample
		float4 offset = float4(kernelSample, 1.0f);
		offset = mul(SSAO_constants.matProjection, offset);
		offset.xy = ((offset.xy / offset.w) * float2(1.0f, -1.0f)) * 0.5f + 0.5f; // [0, 1]
		
		// sample depth
		const float sampleDepth = LinearDepth(
			texDepth.Sample(sPointSampler, offset.xy).r
			, SSAO_constants.matProjection[2][3]
			, SSAO_constants.matProjection[2][2]);

		// range check & accumulate
		if (smoothstep(0.0f, 1.0f, SSAO_constants.radius / abs(P.z - sampleDepth)) > 0)
		{
			if (sampleDepth < kernelSample.z - SSAO_DEPTH_BIAS)
			{
				occlusion += 1.0f;

				//const float depthDistance = abs(sampleDepth - kernelSample.z);
				//occlusion += pow(depthDistance, -1.05f);

			}
		}
	}
	occlusion = 1.0 - (occlusion / KERNEL_SIZE);

	return pow(occlusion, SSAO_constants.intensity);
}