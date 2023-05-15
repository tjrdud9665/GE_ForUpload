#include "EnginePCH.h"
#include "AmbientOcclusionPass.h"
#include "Renderer.h"
#include <Assets/Mesh/Mesh.h>


constexpr size_t sAOQualityKernelSizeLookup[] =
{
	16, // AmbientOcclusionPass::EAOQuality::AO_QUALITY_LOW   
	32, // AmbientOcclusionPass::EAOQuality::AO_QUALITY_MEDIUM
	64, // AmbientOcclusionPass::EAOQuality::AO_QUALITY_HIGH  
};

template<size_t kernelSampleSize>
SSAOConstants<kernelSampleSize> FillAOConstBuffer(const AmbientOcclusionPass* pThis, const SSceneView& sceneView, Renderer* pRenderer)
{
	

	std::array<float, kernelSampleSize* VEC_SZ> samples;
	size_t idx = 0;
	for (const SVector3& v : pThis->sampleKernel[pThis->aoQuality])
	{
		samples[idx + 0] = v.x;
		samples[idx + 1] = v.y;
		samples[idx + 2] = v.z;
		samples[idx + 3] = -1.0f;
		idx += VEC_SZ;
	}
	SSAOConstants<kernelSampleSize> ret;

	ret.matProjectionInverse = sceneView.matProjInverse;
	ret.matProjection = sceneView.matProj;
	ret.screenSize = pRenderer->FrameRenderTargetDimensionsAsFloat2();

	ret.radius = sceneView.SSAOIntensity;
	ret.intensity = sceneView.SSAOIntensity;

	return ret;
}

void AmbientOcclusionPass::Initialize(Renderer* pRenderer)
{
	// CREATE SAMPLE KERNEL
	//--------------------------------------------------------------------
	constexpr size_t NOISE_KERNEL_SIZE = 4;

	for (size_t aoQuality = 0; aoQuality < AO_QUALITY_NUM_OPTIONS; ++aoQuality)
	{
		// src: https://john-chapman-graphics.blogspot.nl/2013/01/ssao-tutorial.html
		const size_t AO_KERNEL_SIZE = sAOQualityKernelSizeLookup[(EAOQuality)aoQuality];
		for (size_t i = 0; i < AO_KERNEL_SIZE; i++)
		{
			// get a random direction in tangent space, Z-up.
			// As the sample kernel will be oriented along the surface normal, 
			// the resulting sample vectors will all end up in the hemisphere.
			
			SVector3 sample = SVector3(SMathUtils::Random(-1, 1), SMathUtils::Random(-1, 1), SMathUtils::Random(0, 1));
			sample.Normalize();					// bring the sample to the hemisphere surface
			sample = sample * SMathUtils::Random(0.1f, 1);	// scale to distribute samples within the hemisphere

			// scale vectors with a power curve based on i to make samples close to center of the
			// hemisphere more significant. think of it as i selects where we sample the hemisphere
			// from, which starts from outer region of the hemisphere and as it increases, we 
			// sample closer to the normal direction. 
			float scale = static_cast<float>(i) / (AO_KERNEL_SIZE - 1);
			scale = SMathUtils::Lerp(0.1f, 1.0f, scale * scale);
			sample = sample * scale;

			sampleKernel[aoQuality].push_back(sample);
		}
	}

	// CREATE NOISE TEXTURE & SAMPLER
	//--------------------------------------------------------------------
	for (size_t i = 0; i < NOISE_KERNEL_SIZE * NOISE_KERNEL_SIZE; i++)
	{	// create a square noise texture using random directions
		SVector2 noise = SVector2(SMathUtils::Random(-1, 1), SMathUtils::Random(-1, 1));
		noise.Normalize();
		noiseKernel.push_back(noise);
	}
	STextureDesc texDesc = {};
	texDesc.Width = NOISE_KERNEL_SIZE;
	texDesc.Height = NOISE_KERNEL_SIZE;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.FileName= "noiseKernel";
	texDesc.pData = noiseKernel.data();
	texDesc.dataPitch = sizeof(noiseKernel.back()) * NOISE_KERNEL_SIZE;
	noiseTexture = pRenderer->CreateTexture2D(texDesc);

	{
		const float whiteValue = 1.0f;
		texDesc.Width = texDesc.Height = 4;
		std::vector<SVector4> white4x4 = std::vector<SVector4>(texDesc.Height * texDesc.Width, SVector4(whiteValue, whiteValue, whiteValue, 1));
		texDesc.FileName = "white4x4";
		texDesc.pData = white4x4.data();
		whiteTexture4x4 = pRenderer->CreateTexture2D(texDesc);
	}
	{
		const float blackValue = 0.0f;
		texDesc.Width = texDesc.Height = 4;
		std::vector<SVector4> black4x4 = std::vector<SVector4>(texDesc.Height * texDesc.Width, SVector4(blackValue, blackValue, blackValue, 1));
		texDesc.FileName = "black4x4";
		texDesc.pData = black4x4.data();
		blackTexture4x4 = pRenderer->CreateTexture2D(texDesc);
	}

	// The tiling of the texture causes the orientation of the kernel to be repeated and 
	// introduces regularity into the result. By keeping the texture size small we can make 
	// this regularity occur at a high frequency, which can then be removed with a blur step 
	// that preserves the low-frequency detail of the image.
	D3D11_SAMPLER_DESC noiseSamplerDesc = {};
	noiseSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	noiseSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	noiseSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	noiseSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	noiseSampler = pRenderer->CreateSamplerState(noiseSamplerDesc,"noiseSampler");

	// RENDER TARGET
	//--------------------------------------------------------------------
	
	SRenderTargetDesc rtDesc = {};
	rtDesc.TextureDesc.Width = pRenderer->FrameRenderTargetWidth();
	rtDesc.TextureDesc.Height = pRenderer->FrameRenderTargetHeight();
	rtDesc.TextureDesc.mipCount = 1;
	rtDesc.TextureDesc.arraySize = 1;
	rtDesc.TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtDesc.TextureDesc.Usage = ETextureUsage::RENDER_TARGET_RW;
	rtDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	occlusionRenderTarget = pRenderer->AddRenderTarget(rtDesc);
	gaussianBlurRenderTarget = pRenderer->AddRenderTarget(rtDesc);

	// SHADER
	//--------------------------------------------------------------------
	for (int32 i = 0; i < EAOQuality::AO_QUALITY_NUM_OPTIONS; ++i)
	{
		const SShaderDesc ssaoShaderDesc =
		{ "SSAO_Shader",
			SShaderStageDesc{ "FullscreenQuad_vs.hlsl", {} },
			SShaderStageDesc{ "SSAO_ps.hlsl", {
				{ "KERNEL_SIZE", std::to_string(sAOQualityKernelSizeLookup[i]) }
			} },
		};
		SSAOShader[i] = pRenderer->CreateShader(ssaoShaderDesc);
	}
	const SShaderDesc gaussianBlurShaderdesc =
	{ "GaussianBlur4x4",
		SShaderStageDesc{"FullscreenQuad_vs.hlsl" , {} },
		SShaderStageDesc{"GaussianBlur4x4_ps.hlsl", {} }
	};
	gaussianBlurShader = pRenderer->CreateShader(gaussianBlurShaderdesc);
}

void AmbientOcclusionPass::RenderAmbientOcclusion(class Renderer* pRenderer, const FTextureID texNormals, const SSceneView& sceneView) const
{
	// early out if we are not rendering anything into the G-Buffer
	if (sceneView.OpaqueList.empty() && sceneView.InstanceList.empty())
	{		
		pRenderer->BindRenderTarget(occlusionRenderTarget);
		pRenderer->UnbindDepthTarget();
		pRenderer->BeginRender(SClearCommand::Color(1, 1, 1, 1));		
		return;
	}	
	
	RenderOcclusion(pRenderer, texNormals, sceneView);
	const FTextureID texAO = pRenderer->GetRenderTargetTexture(occlusionRenderTarget);	
	GaussianBlurPass(pRenderer, texAO);
}

void AmbientOcclusionPass::RenderOcclusion(class Renderer* pRenderer, const FTextureID texNormals, const SSceneView& sceneView) const
{	

	const FTextureID depthTexture = pRenderer->GetDepthTargetTexture(pRenderer->GetDefaultDepthTarget());
	const FIAPair IABuffersQuad = pRenderer->GetBuiltInMesh(EGeometry::FULLSCREENQUAD).GetIAPair();	
	
	constexpr size_t AO_KERNEL_SIZE = sAOQualityKernelSizeLookup[EAOQuality::AO_QUALITY_HIGH];
	SSAOConstants<AO_KERNEL_SIZE> aoConsts = FillAOConstBuffer<AO_KERNEL_SIZE>(this, sceneView, pRenderer);

	pRenderer->SetShader(SSAOShader[aoQuality], true);
	pRenderer->BindRenderTarget(occlusionRenderTarget);
	pRenderer->UnbindDepthTarget();
	pRenderer->SetDepthStencilState(EDefaultDepthStencilState::DEPTH_STENCIL_DISABLED);
	pRenderer->SetSamplerState("sNoiseSampler", noiseSampler);
	pRenderer->SetSamplerState("sPointSampler", EDefaultSamplerState::POINT_SAMPLER);
	//pRenderer->SetSamplerState("sLinearSampler", EDefaultSamplerState::LINEAR_FILTER_SAMPLER);
	pRenderer->SetViewport(pRenderer->GetAASetting().ResolutionX, pRenderer->GetAASetting().ResolutionY);
	pRenderer->SetTexture("texViewSpaceNormals", texNormals);
	pRenderer->SetTexture("texNoise", noiseTexture);
	pRenderer->SetTexture("texDepth", depthTexture);
	pRenderer->SetConstantStruct("SSAO_constants", &aoConsts);
	pRenderer->SetVertexBuffer(IABuffersQuad.first);
	pRenderer->SetIndexBuffer(IABuffersQuad.second);
	pRenderer->Apply();
	pRenderer->DrawIndexed();
	
}

void AmbientOcclusionPass::GaussianBlurPass(Renderer* pRenderer, const FTextureID texAO) const
{
	const SVector2 texDimensions = pRenderer->FrameRenderTargetDimensionsAsFloat2();
	const FIAPair IABuffersQuad = pRenderer->GetBuiltInMesh(EGeometry::FULLSCREENQUAD).GetIAPair();
		

	pRenderer->SetShader(gaussianBlurShader, true);
	pRenderer->BindRenderTarget(gaussianBlurRenderTarget);
	pRenderer->SetTexture("tOcclusion", texAO);
	pRenderer->SetSamplerState("BlurSampler", EDefaultSamplerState::POINT_SAMPLER);
	pRenderer->SetConstant2f("inputTextureDimensions", texDimensions);
	pRenderer->SetVertexBuffer(IABuffersQuad.first);
	pRenderer->SetIndexBuffer(IABuffersQuad.second);
	pRenderer->Apply();
	pRenderer->DrawIndexed();


}

FTextureID AmbientOcclusionPass::whiteTexture4x4 = INDEX_NONE;

FTextureID AmbientOcclusionPass::blackTexture4x4 = INDEX_NONE;

