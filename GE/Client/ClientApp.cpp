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

	//Ŭ���̾�Ʈ������ ������
	// �н��� �� �¿� ShaderRenderTarget�� 
	// BackBuffer�� ����Ÿ���� �ɰ� �� ��½�Ų��.
	m_ClientManager = Globals::CreateEngineObject<ClientManager>();
	g_Engine->Set<ClientManager>(m_ClientManager);
	m_ClientManager->Initialize();

}

void ClientApp::Update(float DeltaSeconds)
{

}
