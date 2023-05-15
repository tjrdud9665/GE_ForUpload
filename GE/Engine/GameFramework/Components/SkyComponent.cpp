#include "EnginePCH.h"
#include "SkyComponent.h"
#include "CameraComponent.h"
#include "CameraManager.h"
#include "ASsets/Scene/Scene.h"
#include "Renderer/Renderer.h"
#include "Assets/Mesh/Mesh.h"
#include "Path/PathManager.h"
#include <AssetManager.h>


SkyComponent::SkyComponent()	
{

}

SkyComponent::SkyComponent(const SkyComponent& Other)
	:Super(Other)
	,m_SkyboxTexture(Other.m_SkyboxTexture)
	, m_EnvironmentTex(Other.m_EnvironmentTex)
{

}

SkyComponent::~SkyComponent()
{

}

void SkyComponent::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
	Renderer* pRenderer = g_Engine->Get<Renderer>();
	EnvironmentMap::Initialize(pRenderer);
	EnvironmentMap::LoadShaders(pRenderer);

	EnvironmentMapFileNames set;
	set.environmentMapFileName = "EpicQuadPanorama.HDR";
	set.irradianceMapFileName = "EpicQuadPanorama.HDR";
	m_EnvMap.Initialize(pRenderer, set);

	const SPathInfo* texPath = g_Engine->Get<PathManager>()->FindPath(TEXTURE_PATH);
	const std::string rootDirectory = texPath->aPath;
	fs::path defaultRoot = rootDirectory;
	defaultRoot /= "EpicQuadPanorama.HDR";

	//Default.. Move TO Editor
	m_SkyboxTexture = (TextureAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(defaultRoot.string()),TextureAsset::StaticClass());
	
	m_EnvironmentTex = (TextureAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(defaultRoot.string()), TextureAsset::StaticClass());
}

void SkyComponent::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);

	if(m_EnvironmentTex)
	{
		m_EnvMap.environmentMap = m_EnvironmentTex->GetTextureID();
		m_EnvMap.irradianceMap = m_EnvironmentTex->GetTextureID();
	}
	
}

void SkyComponent::Render(class Renderer* pRenderer)
{
	if (m_SkyboxTexture)
	{
		FShaderID skyShader = EShaders::SKYBOX;
		EGeometry geo = EGeometry::CUBE;
		if (m_SkyboxTexture->GetTextureID() == INDEX_NONE){
			return;		
		}
		if (m_SkyboxTexture->GetResource())
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			m_SkyboxTexture->GetResource()->GetDesc(&desc);
			if (desc.ViewDimension != D3D11_SRV_DIMENSION_TEXTURECUBE)
			{
				skyShader = EShaders::SKYBOX_EQUIRECTANGULAR;
				//geo = EGeometry::SPHERE;
			}
				
		}
		

		CameraComponent* currentCamera = nullptr;
		if (m_OwningScene)
			currentCamera = m_OwningScene->GetCameraManager()->GetCurrentCamera();
		else
			currentCamera = g_Engine->Get<CameraManager>()->GetCurrentCamera();

		Matrix matWorld = Matrix::StaticScaling(9999999, 9999999, 9999999);
		Matrix matProj = matWorld * currentCamera->GetViewMatrix() * currentCamera->GetProjMatrix();

		

		FIAPair IABuffers = pRenderer->GetBuiltInMesh(geo).GetIAPair();


		pRenderer->SetViewport(pRenderer->GetAASetting().ResolutionX, pRenderer->GetAASetting().ResolutionY);
		
		pRenderer->SetShader(skyShader);
		pRenderer->SetDepthStencilState(EDefaultDepthStencilState::DEPTH_TEST_ONLY);
		pRenderer->SetRasterizerState(EDefaultRasterizerState::CULL_NONE);
		pRenderer->SetConstant4x4f("worldViewProj", matProj);
		pRenderer->SetTexture("texSkybox", m_SkyboxTexture->GetTextureID());

		pRenderer->SetSamplerState("samWrap", EDefaultSamplerState::LINEAR_FILTER_SAMPLER_WRAP_UVW);
		pRenderer->SetVertexBuffer(IABuffers.first);
		pRenderer->SetIndexBuffer(IABuffers.second);
		pRenderer->BindDepthTarget(pRenderer->GetDefaultDepthTarget());		
		pRenderer->Apply();
		pRenderer->DrawIndexed();
	}
}

FRenderTargetID EnvironmentMap::sBRDFIntegrationLUTRT = INDEX_NONE;

FShaderID EnvironmentMap::sBRDFIntegrationLUTShader = INDEX_NONE;

FTextureID EnvironmentMap::sBRDFIntegrationLUTTexture = INDEX_NONE;

FTextureID EnvironmentMap::CreateBRDFIntegralLUTTexture(class Renderer* pRenderer)
{
	constexpr int32 TEXTURE_DIMENSION = 2048;
	
	// create the render target
	SRenderTargetDesc rtDesc = {};
	rtDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtDesc.TextureDesc.Width = TEXTURE_DIMENSION;
	rtDesc.TextureDesc.Height = TEXTURE_DIMENSION;
	rtDesc.TextureDesc.mipCount = 1;
	rtDesc.TextureDesc.arraySize = 1;
	rtDesc.TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtDesc.TextureDesc.Usage = ETextureUsage::RENDER_TARGET_RW;
	rtDesc.TextureDesc.bGenerateMips = false;
	rtDesc.TextureDesc.cpuAccessMode = ECPUAccess::NONE;
	sBRDFIntegrationLUTRT = pRenderer->AddRenderTarget(rtDesc);

	// render the lookup table
	const FIAPair IABuffers = pRenderer->GetBuiltInMesh(EGeometry::FULLSCREENQUAD).GetIAPair();
	
	pRenderer->BindRenderTarget(sBRDFIntegrationLUTRT);
	pRenderer->BeginRender(SClearCommand::Color(SVector4(0, 0, 0, 1)));
	pRenderer->UnbindDepthTarget();
	pRenderer->SetShader(sBRDFIntegrationLUTShader);
	pRenderer->SetViewport(TEXTURE_DIMENSION, TEXTURE_DIMENSION);
	pRenderer->SetVertexBuffer(IABuffers.first);
	pRenderer->SetIndexBuffer(IABuffers.second);
	pRenderer->Apply();
	pRenderer->DrawIndexed();
	pRenderer->GetDeviceContext()->Flush();
	return pRenderer->GetRenderTargetTexture(sBRDFIntegrationLUTRT);
}

FShaderID EnvironmentMap::sPrefilterShader;

FShaderID EnvironmentMap::sRenderIntoCubemapShader;

FTextureID EnvironmentMap::InitializePrefilteredEnvironmentMap(FTextureID specularMap, FTextureID irradienceMap)
{
	return INDEX_NONE;
}

void EnvironmentMap::Initialize(Renderer* pRenderer)
{

}

void EnvironmentMap::Initialize(class Renderer* pRenderer, const EnvironmentMapFileNames& files)
{
	PathManager* pathMgr = g_Engine->Get<PathManager>();

	const SPathInfo* texPath = pathMgr->FindPath(TEXTURE_PATH);

	//const std::string extensionEnvMap = files.environmentMapFileName;
	//const std::string extensionIrrMap = files.irradianceMapFileName;

	const std::string rootDirectory = texPath->aPath;

	// input textures for pre-filtered environment map calculation
	const std::string irradianceTextureFilePath = rootDirectory + files.irradianceMapFileName;
	const std::string skyboxTextureFilePath = rootDirectory + files.environmentMapFileName;

	// cached mipped-cubemap version of the textures (pick one of them at random -> 0th element 0th mip for example)

	// get the latest date of the environment map and irradiance map input textures
	// compare them with the corresponding cached textures
	//


	//constexpr bool USE_CACHE = false;
	//const bool bCacheEnabled = Engine::GetSettings().bCacheEnvironmentMapsOnDisk && USE_CACHE;
	//const bool bCacheExists = bCacheEnabled && DirectoryUtil::FileExists(cachedPrefilteredEnvMap);
	//const bool bUseCache = bCacheExists && DirectoryUtil::IsFileNewer(cachedPrefilteredEnvMap, irradianceTextureFilePath);
	irradianceMap = pRenderer->CreateHDRTexture(files.irradianceMapFileName, rootDirectory);
	environmentMap = pRenderer->CreateHDRTexture(files.environmentMapFileName, rootDirectory);
	
	
	//InitializePrefilteredEnvironmentMap(pRenderer->GetTextureObject(environmentMap), pRenderer->GetTextureObject(irradianceMap), rootDirectory);
	
	if (envMapSampler == INDEX_NONE)
	{
		D3D11_SAMPLER_DESC envMapSamplerDesc = {};
		envMapSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		envMapSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		envMapSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		envMapSamplerDesc.MipLODBias = 0;
		envMapSamplerDesc.MinLOD = 0;
		envMapSamplerDesc.MaxLOD = PREFILTER_MIP_LEVEL_COUNT - 1;
		envMapSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		envMapSamplerDesc.MaxAnisotropy = 1;

		envMapSampler = pRenderer->CreateSamplerState(envMapSamplerDesc, "EnvSmp");
	}

	if(sBRDFIntegrationLUTTexture == INDEX_NONE)
	{

		
		sBRDFIntegrationLUTTexture = CreateBRDFIntegralLUTTexture(pRenderer);

	}


}

void EnvironmentMap::LoadShaders(Renderer* pRenderer)
{
	const char* pSkyboxVS = "Skybox_vs.hlsl";

	const SShaderDesc BRDFIntegratorShaderDesc = { "BRDFIntegrator",
		SShaderStageDesc{"FullscreenQuad_vs.hlsl", {}},
		SShaderStageDesc{"IntegrateBRDF_IBL_ps.hlsl", {}},
	};
	const SShaderDesc brdfIntegrationShaderDesc = { "PreFilterConvolution",
		SShaderStageDesc{pSkyboxVS, {}},
		SShaderStageDesc{"PreFilterConvolution_ps.hlsl", {}},
	};
	const SShaderDesc cubemapShaderDesc = { "RenderIntoCubemap",
		SShaderStageDesc{pSkyboxVS, {}},
		SShaderStageDesc{"RenderIntoCubemap_ps.hlsl", {}},
	};

	
	sPrefilterShader = pRenderer->CreateShader(brdfIntegrationShaderDesc);
	sBRDFIntegrationLUTShader = pRenderer->CreateShader(BRDFIntegratorShaderDesc);
	sRenderIntoCubemapShader = pRenderer->CreateShader(cubemapShaderDesc);
}
