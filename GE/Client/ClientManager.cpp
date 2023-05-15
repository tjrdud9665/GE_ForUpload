#include "PCH.h"
#include "ClientManager.h"
#include <Renderer/Renderer.h>
#include <SceneManager.h>
#include <Path/PathManager.h>
#include <Assets/Scene/Scene.h>
#include <AssetManager.h>
#include <RenderManager.h>


ClientManager::ClientManager()
{
	m_FXAAShader = INDEX_NONE;	
}

ClientManager::~ClientManager()
{

}

void ClientManager::Initialize()
{
	m_cachedRenderer = g_Engine->Get<Renderer>();
	m_cachedSceneManager = g_Engine->Get<SceneManager>();
	m_cachedRenderManager = g_Engine->Get<RenderManager>();

	const SShaderDesc FxAAShaderDesc =
	{ "FXAAPass",
		{
		SShaderStageDesc{ "FullScreenQuad_vs.hlsl", {} },
		SShaderStageDesc{ "FxAA_ps.hlsl", {} }
		}
	};
	
	m_FXAAShader = m_cachedRenderer->CreateShader(FxAAShaderDesc);
	//클라이언트 고유의 랜더링
	g_Engine->AddRenderCall<ClientManager>(this, std::bind(&ClientManager::PreRender, this), std::bind(&ClientManager::Render, this));


	const SPathInfo* pathInfo = g_Engine->Get<PathManager>()->FindPath(SCENE_PATH);
	std::string scenePath = pathInfo->aPath + "Start.meta";

	Scene* scene = (Scene*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(scenePath), Scene::StaticClass());
	m_cachedSceneManager->SetAutoPlay(true);
	m_cachedSceneManager->CreateNextScene(scene, true);
}


void ClientManager::Render()
{
	m_cachedRenderer->SetShader(m_FXAAShader,true,true);

	//FXAA
	//BindBackBuffer..
	m_cachedRenderer->BindRenderTarget(0);
	
	FTextureID finalTex = m_cachedRenderManager->GetFinalTargetTextureID();
		//m_cachedRenderer->GetRenderTargetTexture(m_cachedRenderManager->GetDeferredPass().ShaderRenderTarget);

	
	m_cachedRenderer->SetViewport(m_cachedRenderer->GetAASetting().ResolutionX, m_cachedRenderer->GetAASetting().ResolutionY);

	m_cachedRenderer->SetRasterizerState(EDefaultRasterizerState::CULL_BACK);	
	m_cachedRenderer->SetDepthStencilState(EDefaultDepthStencilState::DEPTH_STENCIL_DISABLED);


	m_cachedRenderer->SetVertexBuffer((int32)EGeometry::FULLSCREENQUAD);
	m_cachedRenderer->SetIndexBuffer((int32)EGeometry::FULLSCREENQUAD);
	
	
	m_cachedRenderer->SetConstant2f("rcpFrame", SVector2(1.0f / m_cachedRenderer->GetAASetting().ResolutionX, 1.0f / m_cachedRenderer->GetAASetting().ResolutionY));
	m_cachedRenderer->SetSamplerState("sAnisoSampler", EDefaultSamplerState::LINEAR_FILTER_SAMPLER_WRAP_UVW);
	m_cachedRenderer->SetTexture("tex", finalTex);
	//rcpFrame

	m_cachedRenderer->Apply();
	m_cachedRenderer->DrawIndexed();
	
	//Bind Tex..

	

}

void ClientManager::PreRender()
{

}
