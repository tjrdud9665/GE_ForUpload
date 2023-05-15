#pragma once
#include "RenderPass.h"

enum EAOQuality
{
	AO_QUALITY_LOW = 0, // 16 taps / px
	AO_QUALITY_MEDIUM,  // 32 taps / px
	AO_QUALITY_HIGH,    // 64 taps / px

	AO_QUALITY_NUM_OPTIONS
};
constexpr size_t VEC_SZ = 4;

template<size_t KERNEL_SIZE>
struct SSAOConstants
{
	Matrix matProjection;
	//-----------------------------
	Matrix matProjectionInverse;
	//-----------------------------
	SVector2 screenSize;
	float radius;
	float intensity;
	//-----------------------------
	std::array<float, KERNEL_SIZE* VEC_SZ> samples;
};



class AmbientOcclusionPass
{
	friend class Engine;
	friend class RenderManager;

public:
	AmbientOcclusionPass()
	{
		aoQuality = AO_QUALITY_HIGH;
	}
	// Pass interface
	void Initialize(class Renderer* pRenderer);
	void RenderAmbientOcclusion(class Renderer* pRenderer, const FTextureID texNormals, const SSceneView& sceneView) const;

	//FTextureID GetBlurredAOTexture(Renderer* pRenderer) const;

	//// draw functions
	void RenderOcclusion(class Renderer* pRenderer, const FTextureID texNormals, const SSceneView& sceneView) const;

	void GaussianBlurPass(Renderer* pRenderer, const FTextureID texAO) const;	// Gaussian 4x4 kernel


	EAOQuality aoQuality;

	// SSAO resources
	std::vector<SVector3>	sampleKernel[AO_QUALITY_NUM_OPTIONS];
	std::vector<SVector2>	noiseKernel;
	FTextureID			noiseTexture;
	FSamplerID			noiseSampler;

	// Regular SSAO resources -------------------------------
	FRenderTargetID		occlusionRenderTarget;
	FShaderID			SSAOShader[AO_QUALITY_NUM_OPTIONS];
	// Regular SSAO resources -------------------------------

	// Blur resources --------------------------------------
	FRenderTargetID			 gaussianBlurRenderTarget;

	FShaderID gaussianBlurShader;

public:
	static FTextureID whiteTexture4x4;
	static FTextureID blackTexture4x4;
};

