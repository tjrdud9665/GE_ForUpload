#include "EnginePCH.h"
#include "Application.h"

Application::Application(const wstring& InTitle, int32 WndWidth, int32 WndHeight, int32 dxWidth, int32 dxHeight, bool FullScreen)
	:m_InitInfo(InTitle, WndWidth, WndHeight, dxWidth, dxHeight, FullScreen)
{

}

Application::~Application()
{

}



void Application::Initialize()
{
	//Bind Some Keys..


}

void Application::PostFirstUpdate(float DeltaSeconds)
{

}

void Application::_SetEngine(class Engine* InEngine)
{
	m_Engine = InEngine;
}

SEngineInitialInfo Application::GetInitInfo() const
{
	return m_InitInfo;
}

bool Application::OnMessageHandle(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return false;
}
