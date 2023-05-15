#include "PCH.h"
#include "imgui.h"
#include "EditorApp.h"
#include "Engine.h"
#include "Editor/EditorManager.h"
#include "EngineWindow.h"
#include "ObjectManager.h"
#include "GameObject/TestObject.h"
#include "Editor/SceneEditor.h"
#include "Editor/ContentEditor.h"
#include "Editor/SettingEditor.h"
//Load GameObjects
#include "GameFramework/GameObject/DirectionalLight.h"
#include "GameFramework/GameObject/SkyBox.h"
#include "GameFramework/GameObject/PlayerCharacter.h"
#include "GameFramework/GameObject/PlayerController.h"
#include "GameFramework/GameObject/PlayerStart.h"

#include <Input/Input.h>
#include <GameFramework/Components/CameraComponent.h>
#include <CameraManager.h>
#include <Renderer/Renderer.h>




extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



EditorApp::EditorApp(const wstring& InTitle, int32 WndWidth, int32 WndHeight, int32 dxWidth, int32 dxHeight, bool FullScreen)
	:Application(InTitle, WndWidth, WndHeight, dxWidth, dxHeight, FullScreen)
{

	m_FrameCounter = 0;
	m_FPSTimerID = 0;
}

EditorApp::~EditorApp()
{
	g_Engine->Set<EditorManager>(nullptr);

}

void EditorApp::PreInitialize()
{	
	m_Engine->SetEditMode(true);
}

void EditorApp::Initialize()
{
	Application::Initialize();
	m_EditorManager = Globals::CreateEngineObject<EditorManager>();
	m_EditorManager->Initialize();
	g_Engine->Set<EditorManager>(m_EditorManager);	

	//g_Engine->Get<Input>()->CreateAxisKey("MoveRIght", 'D', 1.0f);
	//g_Engine->Get<Input>()->CreateAxisKey("MoveRIght", 'A', -1.0f);
	//g_Engine->Get<Input>()->CreateAxisKey("MoveForward", 'W', 1.0f);
	//g_Engine->Get<Input>()->CreateAxisKey("MoveForward", 'S', -1.0f);

	g_Engine->Get<Input>()->CreateActionKey("MLB", VK_LBUTTON);
	

	//Scene 에 게임오브젝트/컴포넌트를 추가.
	//Camera.TArgetarm 등등 추가..

	m_AppTimer.SetTimer(OUT m_FPSTimerID, 1.0f, true, std::bind(&EditorApp::FPSTick, this));

	m_EditorManager->CreateEditor<SettingEditor>();
	m_EditorManager->CreateEditor<SceneEditor>();
	m_EditorManager->CreateEditor<ContentEditor>();	



	//Test
	//g_Engine->Get<Input>()->AddActionBind("MLB", EInputEventType::IE_Pressed,std::bind(&EditorApp::MLBTest,this));

	

}

void EditorApp::Update(float DeltaSeconds)
{	
	m_FrameCounter++;

}

void EditorApp::PostFirstUpdate(float DeltaSeconds)
{
	ObjectManager* objMgr = g_Engine->Get<ObjectManager>();

	if(objMgr)
	{
		DirectionalLight* dir = objMgr->SpawnGameObject<DirectionalLight>("DirectionalLight");
		//TestObject* t1 = objMgr->SpawnGameObject<TestObject>("Test");
		//TestObject* t2 = objMgr->SpawnGameObject<TestObject>("Test2");
		//TestObject* t3 = objMgr->SpawnGameObject<TestObject>("Test3");
		//TestObject* t4 = objMgr->SpawnGameObject<TestObject>("Test4");
		//TestObject* t5 = objMgr->SpawnGameObject<TestObject>("Test5");
		//TestObject* t6 = objMgr->SpawnGameObject<TestObject>("Test6");
		//t1->SetObjectLocation(SVector3(3, 0, 5));
		//t2->SetObjectLocation(SVector3(2, 0, 5));
		//t2->SetObjectRotation(SVector3(0, 0, 1));

		objMgr->SpawnGameObject<SkyBox>("Sky");
	}
}

bool EditorApp::OnMessageHandle(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
	{
		return true;
	}
	return false;
}

bool EditorApp::IsConsumeIMGUIMouseInput() const
{
	return ImGui::GetIO().WantCaptureMouse;
}

bool EditorApp::IsConsumeIMGUIKeyInput() const
{
	return ImGui::GetIO().WantCaptureKeyboard;
}

void EditorApp::FPSTick()
{
	std::wstring fpsText;

	fpsText = L"FPS :" + std::to_wstring(m_FrameCounter);

	g_Engine->Get<EngineWindow>()->SetTitle(fpsText);
	m_FrameCounter = 0;
}

void EditorApp::MLBTest()
{
	CameraComponent* camComp = g_Engine->Get<CameraManager>()->GetCurrentCamera();	
	SVector3 origin;
	SVector3 direction;

	SVector2 mouse =  g_Engine->Get<Input>()->GetMousePosition();

	SHitResult hit = camComp->pickRay(g_Engine->Get<Renderer>()->GetCurrentViewport(), mouse.x,mouse.y,OUT origin, OUT direction);	

	if(hit.Hit)
	{
		int32 t = 0;
	}
	
	
}
