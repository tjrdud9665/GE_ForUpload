#include "PCH.h"
#include "ClientApp.h"
#include "ClientManager.h"

#include "Includes.h"

ClientApp::ClientApp(const wstring& InTitle, int32 WndWidth, int32 WndHeight, int32 dxWidth, int32 dxHeight, bool FullScreen)
	:Application(InTitle, WndWidth, WndHeight, dxWidth, dxHeight, FullScreen)
{

}

ClientApp::~ClientApp()
{

}

void ClientApp::PreInitialize()
{
	m_Engine->SetEditMode(false);
}

void ClientApp::Initialize()
{
	Application::Initialize();

	//클라이언트에서의 랜더링
	// 패스를 다 태운 ShaderRenderTarget을 
	// BackBuffer의 랜더타겟을 걸고 을 출력시킨다.
	m_ClientManager = Globals::CreateEngineObject<ClientManager>();
	g_Engine->Set<ClientManager>(m_ClientManager);
	m_ClientManager->Initialize();

}

void ClientApp::Update(float DeltaSeconds)
{

}
