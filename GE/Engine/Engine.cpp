#include "EnginePCH.h"
#include "Engine.h"
#include "Application.h"

#include "Path/PathManager.h"

#include "EngineWindow.h"
#include "Renderer/Renderer.h"

#include "Renderer/GeometryGenerator.h"
#include "Timer/TimerManager.h"

#include "AssetManager.h"
#include "SoundManager.h"
#include "SceneManager.h"
#include "RenderManager.h"
#include "D3D/D3DManager.h"

#include "Assets/Mesh/Mesh.h"
#include "Assets/Mesh/MeshAsset.h"
#include "Assets/Material/Material.h"
#include "Physics/PhysicsSystem.h"
#include <CollisionManager.h>
#include <Navigation/NavigationSystem.h>
#include <Input/Input.h>
#include <UI/UIManager.h>
#include "GameplaySystem/GameInstance.h"

float g_DeltaSeconds;

Engine::Engine()
	:m_EngineWindow(nullptr)
	, m_D3DManager(nullptr)
	, m_PathManger(nullptr)
	, m_FirstStep(true)	
{
	m_App = nullptr;
}

Engine::~Engine()
{
	m_PhySys->ClearEvents();

	//Force Delete Order 
	Set<Renderer>(nullptr);
	Set<D3DManager>(nullptr);	
	Set<EngineWindow>(nullptr);

	//m_D3DManager->AddRef();
	//Set<D3DManager>(nullptr);

	//Set<PhysicsSystem>(nullptr);
	Set<AssetManager>(nullptr);

	Set<SceneManager>(nullptr);		
	
	Set<PhysicsSystem>(nullptr);


	EngineObjectMap::iterator iter = m_EngineObjects.begin();
	EngineObjectMap::iterator End = m_EngineObjects.end();

	for (; iter != End; iter++)
	{
		SAFE_RELEASE((*iter).second);
	}


	m_EngineObjects.clear();
	//m_D3DManager->ReportLiveObjects();
	//SAFE_RELEASE(m_D3DManager);

	


}


XObject* Engine::GetObject(type_index TypeIdx) const
{
	EngineObjectMap::const_iterator iter = m_EngineObjects.find(TypeIdx);

	if (iter != m_EngineObjects.cend())
		return iter->second;

	return nullptr;
}

XObject* Engine::SetObject(type_index TypeIdx, XObject* New)
{
	XObject* Old = GetObject(TypeIdx);
	if (Old != New)
	{
		if (New)
		{
			New->AddRef();
		}

		m_EngineObjects[TypeIdx] = New;
		if (Old)
		{
			Old->Release();
		}

		return Old;
	}

	return nullptr;
}

void Engine::AddRenderCall(type_index TypeIdx, std::function<void(void)>&& PreFunc, std::function<void(void)>&& Func)
{	
	m_EditionalRender.insert(std::make_pair(TypeIdx, SEditionalRender{ std::move(PreFunc) ,std::move(Func)}));
}

void Engine::Initialize(const wstring& InTitle, const int32 WndWidth, const int32 WndHeight, const int32 dxWidth, const int32 dxHeight, bool FullScreen /*= false*/)
{
	m_Loop = true;
	m_EngineWindow = Globals::CreateEngineObject<EngineWindow>();
	Set<EngineWindow>(m_EngineWindow);
	m_EngineWindow->Initialize(InTitle.c_str(), WndWidth, WndHeight);
	



	m_PathManger = Globals::CreateEngineObject<PathManager>();
	m_PathManger->Initialize();
	Set<PathManager>(m_PathManger);

	m_TimerManager = Globals::CreateEngineObject<TimerManager>();
	m_TimerManager->Initialize();
	Set<TimerManager>(m_TimerManager);

	
	m_D3DManager = Globals::CreateEngineObject<D3DManager>();
	m_D3DManager->Initialize(dxWidth, dxHeight,true, m_EngineWindow->Handle(), FullScreen, DXGI_FORMAT_B8G8R8A8_UNORM);
	Set<D3DManager>(m_D3DManager);

	m_Renderer = Globals::CreateEngineObject<Renderer>();
	Set<Renderer>(m_Renderer);
	m_Renderer->Initialize(m_D3DManager, true);
	

	LoadShaders();

	m_Input = Globals::CreateEngineObject<Input>();
	Set<Input>(m_Input);
	m_Input->Initialize();

	m_CollisionManager = Globals::CreateEngineObject<CollisionManager>();	
	Set<CollisionManager>(m_CollisionManager);
	m_CollisionManager->Initialize();

	//Create Render pass 
	m_RenderManager = Globals::CreateEngineObject<RenderManager>();
	m_RenderManager->Initialize();
	Set<RenderManager>(m_RenderManager);

	m_PhySys = Globals::CreateEngineObject<PhysicsSystem>();
	m_PhySys->Initialize(m_RenderManager, m_CollisionManager);
	Set<PhysicsSystem>(m_PhySys);

	m_NavSystem = Globals::CreateEngineObject<NavigationSystem>();
	m_NavSystem->Initialize(m_PhySys);
	Set<NavigationSystem>(m_NavSystem);

	m_SoundManager = Globals::CreateEngineObject<SoundManager>();
	Set<SoundManager>(m_SoundManager);
	m_SoundManager->Initialize();

	m_UIManager = Globals::CreateEngineObject<UIManager>();
	m_UIManager->Initialize(m_Renderer);
	Set<UIManager>(m_UIManager);

	m_AssetManager = Globals::CreateEngineObject<AssetManager>();
	Set<AssetManager>(m_AssetManager);
	m_AssetManager->Initialize();	


	m_SceneManager = Globals::CreateEngineObject<SceneManager>();	
	m_SceneManager->Initialize();
	Set<SceneManager>(m_SceneManager);

	m_GameInst = Globals::CreateEngineObject<GameInstance>();
	Set<GameInstance>(m_GameInst);
	m_GameInst->Initialize();



	///Check If Game Scene Is Begin?
	m_Input->SetSceneManager(m_SceneManager);	
	
	//Load Rendering Objects
	LoadBuiltinAssets();
	
	D3D11_VIEWPORT vp;
	vp.Height = (float)dxHeight;
	vp.Width = (float)dxWidth;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	SetEditViewport(vp);
	
}

void Engine::Update()
{
	
	m_TimerManager->UpdateDeltaSeconds(OUT g_DeltaSeconds);
	
	//NoShader Apply
	//m_Renderer->ResetPipelineState();
	m_Renderer->BindRenderTarget(0);	
	m_Renderer->Apply();
	m_Renderer->BeginRender(SClearCommand::Color(SVector4::White));
	

	//some System Updates.

	EngineObjectMap::iterator iter = m_EngineObjects.begin();
	EngineObjectMap::iterator iterEnd = m_EngineObjects.end();
	for (; iter != iterEnd; iter++)
	{
		if ((*iter).second)
		{
			(*iter).second->PreUpdate(g_DeltaSeconds);
		}
	}


	iter = m_EngineObjects.begin();
	iterEnd = m_EngineObjects.end();
	for(;iter != iterEnd;iter++)
	{	
		if ((*iter).second)
			(*iter).second->Update(g_DeltaSeconds);
	}

	iter = m_EngineObjects.begin();
	iterEnd = m_EngineObjects.end();

	for (; iter != iterEnd; iter++)
	{
		if ((*iter).second)
			(*iter).second->PostUpdate(g_DeltaSeconds);
	}

	//RenderManager는 DeferredPass를 가지고 그린다 내부에서 Geo 및 Light 처리및 Post 까지다.
	PreRender();	
	Render();

	m_Renderer->EndFrame();
}

void Engine::ShutDown()
{	
	m_Loop = false;
}

bool Engine::Run(class Application* app)
{
	if (!m_EngineWindow)
	{
		return false;
	}

	m_App = app;
	MSG Msg;

	while (m_Loop)
	{
		if (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		} else
		{
			Update();
			if (m_FirstStep)
			{
				app->PostFirstUpdate(g_DeltaSeconds);
				m_FirstStep = false;
			}
			app->Update(g_DeltaSeconds);
		}

	}


	return true;
}


bool Engine::OnMessageHandle(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_App)
		return m_App->OnMessageHandle(hwnd, message, wParam, lParam);

	return false;
}

class Application* Engine::GetApplication() const
{
	return m_App;
}

void Engine::PreRender()
{
	m_RenderManager->PreRender();

	ExternalEngineObjectRenderMap::const_iterator iter = m_EditionalRender.begin();
	ExternalEngineObjectRenderMap::const_iterator iter_end = m_EditionalRender.end();

	for (; iter != iter_end; iter++)
	{
		if ((*iter).second.PreRenderFunc)
			(*iter).second.PreRenderFunc();
	}
}

void Engine::Render()
{
	m_RenderManager->Render();
	

	ExternalEngineObjectRenderMap::const_iterator iter = m_EditionalRender.begin();
	ExternalEngineObjectRenderMap::const_iterator iter_end = m_EditionalRender.end();

	for(;iter != iter_end;iter++)
	{
		if ((*iter).second.RenderFunc)
			(*iter).second.RenderFunc();
	}
}

bool Engine::LoadShaders()
{	
	
	constexpr uint32 VS_PS = SHADER_STAGE_VS | SHADER_STAGE_PS;
	const std::vector<SShaderDesc> shaderDescs =
	{
			SShaderDesc{ "ComputeAnimation",{SShaderStageDesc{ "AnimationUpdate_cs.hlsl", {} }}},
			SShaderDesc{ "PhongLighting<forward>",{SShaderStageDesc{ "Forward_Phong_vs.hlsl", {} },SShaderStageDesc{ "Forward_Phong_ps.hlsl", {} }}},
			SShaderDesc{ "UnlitTextureColor" , SShaderDesc::CreateStageDescsFromShaderName("UnlitTextureColor", VS_PS) },
			SShaderDesc{ "TextureCoordinates", {SShaderStageDesc{"MVPTransformationWithUVs_vs.hlsl", {} },SShaderStageDesc{"TextureCoordinates_ps.hlsl", {} }}},
			SShaderDesc{ "Normal"            , SShaderDesc::CreateStageDescsFromShaderName("Normal"  , VS_PS) },
			SShaderDesc{ "Tangent"           , SShaderDesc::CreateStageDescsFromShaderName("Tangent" , VS_PS) },
			SShaderDesc{ "Binormal"          , SShaderDesc::CreateStageDescsFromShaderName("Binormal", VS_PS) },
			SShaderDesc{ "Line"              , SShaderDesc::CreateStageDescsFromShaderName("Line"    , VS_PS | SHADER_STAGE_GS) },
			SShaderDesc{ "Solid"              , SShaderDesc::CreateStageDescsFromShaderName("Solid"    , VS_PS | SHADER_STAGE_GS) },
			SShaderDesc{ "TNB"               , SShaderDesc::CreateStageDescsFromShaderName("TNB"     , VS_PS) },
			SShaderDesc{ "Debug"             , SShaderDesc::CreateStageDescsFromShaderName("Debug"   , VS_PS) },
			SShaderDesc{ "Skybox"            , SShaderDesc::CreateStageDescsFromShaderName("Skybox"  , VS_PS) },
			SShaderDesc{ "SkyboxEquirectangular", {SShaderStageDesc{"Skybox_vs.hlsl"               , {} },SShaderStageDesc{"SkyboxEquirectangular_ps.hlsl", {} }}},
			SShaderDesc{ "DepthShader"           , SShaderDesc::CreateStageDescsFromShaderName("DepthShader", SHADER_STAGE_VS)},
			SShaderDesc{ "DepthShader_Instanced" , SShaderStageDesc{"DepthShader_vs.hlsl", {SShaderMacro{ "INSTANCED"     , "1" } }}},

			SShaderDesc{"Text", SShaderDesc::CreateStageDescsFromShaderName("Text", VS_PS)},
			SShaderDesc{"ParticleUpdate", {SShaderStageDesc{"ParticleUpdate_cs.hlsl",{} }}},
			SShaderDesc{"ParticleRender", SShaderDesc::CreateStageDescsFromShaderName("ParticleRender", VS_PS | SHADER_STAGE_GS)}
	};
	

	// todo: do not depend on array index, use a lookup, remove s_shaders[]
	size_t shaderCnt = shaderDescs.size();
	for (size_t i = 0; i < shaderCnt; ++i)
	{
		m_BuiltinShaders.push_back(m_Renderer->CreateShader(shaderDescs[i]));
	}


	//Prepare RenderPass
	//RenderPass::InitializeCommonSaders(mpRenderer);
	
	

	return true;
}

void Engine::LoadBuiltinAssets()
{
	const SPathInfo* matPathInfo = m_PathManger->FindPath(MATERIAL_PATH);
	fs::path builtinMaterialPath = matPathInfo->aPath;
	builtinMaterialPath /= "DefaultMaterial";
	Material* defaultMaterial = (Material*)m_AssetManager->LoadOrCreate(AssetInfo(builtinMaterialPath.string()), Material::StaticClass());
	defaultMaterial->Save();
	//Load BuiltinMesh Asset
	const SPathInfo* meshPathInfo = m_PathManger->FindPath(MESH_PATH);
	fs::path quadPath = meshPathInfo->aPath;
	quadPath /= "BuiltInQuad";



	MeshAsset* builtinQuad = (MeshAsset*)m_AssetManager->LoadOrCreate(AssetInfo(quadPath.string()), MeshAsset::StaticClass());
	if(builtinQuad->GetMeshCount() == 0)
	{
		Mesh Quadmesh = m_Renderer->GetBuiltInMesh(EGeometry::QUAD);			
		builtinQuad->AddMesh(Quadmesh);
		builtinQuad->AddMaterial(0, defaultMaterial);
		builtinQuad->Save();
	}



	fs::path cubePath = meshPathInfo->aPath;
	cubePath /= "BuiltInCube";


	MeshAsset* builtinCube = (MeshAsset*)m_AssetManager->LoadOrCreate(AssetInfo(cubePath.string()), MeshAsset::StaticClass());
	if (builtinCube->GetMeshCount() == 0)
	{
		Mesh Cubemesh = m_Renderer->GetBuiltInMesh(EGeometry::CUBE);
		builtinCube->AddMesh(Cubemesh);
		builtinCube->AddMaterial(0, defaultMaterial);
		builtinCube->Save();
	}

	fs::path spherePath = meshPathInfo->aPath;
	spherePath /= "BuiltInSphere";

	MeshAsset* builtinSphere = (MeshAsset*)m_AssetManager->LoadOrCreate(AssetInfo(spherePath.string()), MeshAsset::StaticClass());
	if (builtinSphere->GetMeshCount() == 0)
	{
		Mesh Spheremesh = m_Renderer->GetBuiltInMesh(EGeometry::SPHERE);		
		builtinSphere->AddMesh(Spheremesh);
		builtinSphere->AddMaterial(0, defaultMaterial);
		builtinSphere->Save();
	}

	

}

bool Engine::m_EditMode;
