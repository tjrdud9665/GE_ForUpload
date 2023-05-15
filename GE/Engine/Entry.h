#pragma once
#include "Engine.h"
#include "Application.h"
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG

#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )

void list_remaining_d3d_objects()
{

	HMODULE dxgidebugdll = GetModuleHandleW(L"dxgidebug.dll");
	decltype(&DXGIGetDebugInterface) GetDebugInterface = reinterpret_cast<decltype(&DXGIGetDebugInterface)>(GetProcAddress(dxgidebugdll, "DXGIGetDebugInterface"));

	IDXGIDebug* debug;

	GetDebugInterface(IID_PPV_ARGS(&debug));

	OutputDebugStringW(L"Starting Live Direct3D Object Dump:\r\n");
	debug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
	OutputDebugStringW(L"Completed Live Direct3D Object Dump.\r\n");

	debug->Release();
}
#endif

extern Application* EntryApplication();

Engine* g_Engine = nullptr;


INT CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(2069016);

	Application* entryApp = nullptr;
	//Run Engine and Applications
	g_Engine = Globals::CreateEngineObject<Engine>();
	entryApp = EntryApplication();
	
	SEngineInitialInfo InitInfo = entryApp->GetInitInfo();
	entryApp->_SetEngine(g_Engine);
	entryApp->PreInitialize();
	g_Engine->Initialize(InitInfo.WndTitle, InitInfo.WndWidth, InitInfo.WndHeight, InitInfo.dxWidth, InitInfo.dxHeight, InitInfo.FullScreen);		
	entryApp->Initialize();
	
	g_Engine->Run(entryApp);


	SAFE_DELETE(entryApp);
	SAFE_RELEASE(g_Engine);
	



#ifdef _DEBUG
	list_remaining_d3d_objects();
#endif

	return true;
}