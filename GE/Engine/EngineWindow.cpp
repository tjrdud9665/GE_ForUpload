#include "EnginePCH.h"
#include "EngineWindow.h"
#include "D3D/D3DManager.h"
#include <Input/Input.h>


static LPCWSTR NAME_SLWNDCLS = L"SLWndCls";


EngineWindow::EngineWindow()
{
	
}

EngineWindow::~EngineWindow()
{

}

LRESULT CALLBACK EngineWindow::SetupMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		EngineWindow* const pWnd = static_cast<EngineWindow*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&EngineWindow::AssignMessageHandler));
		pWnd->m_handle = hWnd;
		
		return pWnd->MessageHandler(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT	CALLBACK EngineWindow::AssignMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	EngineWindow* const pWnd = reinterpret_cast<EngineWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	return pWnd->MessageHandler(hWnd, msg, wParam, lParam);
}

LRESULT EngineWindow::MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(g_Engine)
	{
		if(g_Engine->OnMessageHandle(hwnd, message,wParam,lParam))
		{
			return true;
		}
		
	}

	

	switch (message)
	{
	case WM_SIZE:
	{
		UINT width = LOWORD(lParam);
		UINT height = HIWORD(lParam);
		D3DManager* d3d = g_Engine->Get<D3DManager>();
		if (d3d)
			d3d->ReseizeBuffer(width, height);

	}
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000))
		{

		}
		break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
		break;
	case WM_SETFOCUS:

		return 0;
	case WM_KILLFOCUS:
		break;
	case WM_CHAR:
		break;
	case WM_MOUSEWHEEL:
	{
		Input* input = g_Engine->Get<Input>();
		if(input)
		{
			input->SetMouseWheelDelta(GET_WHEEL_DELTA_WPARAM(wParam));
		}
		
	}
	break;
	case WM_CLOSE:
	case WM_DESTROY:
		g_Engine->ShutDown();		
		PostQuitMessage(0);

		break;
	}



	return DefWindowProc(hwnd, message, wParam, lParam);
}

void EngineWindow::Initialize(const TCHAR* Title, uint32 Width, uint32 Height)
{
	m_title = Title;
	m_size = SIZE{ (int32)Width, (int32)Height };

	Initialize();
}

void EngineWindow::Initialize()
{
	RegisterWindowClass();

	//win10 이후에 GetSystemMetrics(SM_CXSCREEN) 값이 꼬일수 있음.
	::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	DWORD dwStyle = WS_POPUP | WS_EX_TOPMOST;
		//WS_CAPTION  | WS_MINIMIZEBOX | WS_SYSMENU;

	if (m_bFullScreen)
	{
		dwStyle = WS_POPUP | WS_EX_TOPMOST;
		m_size = SIZE{ ::GetSystemMetrics(SM_CXSCREEN) , ::GetSystemMetrics(SM_CYSCREEN) };
	}



	int centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - m_size.cx / 2;
	int centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - m_size.cy / 2;


	RECT rt;
	rt.left = centerScreenX;
	rt.top = centerScreenY;
	rt.right = rt.left + m_size.cx;
	rt.bottom = rt.top + m_size.cy;


	if (AdjustWindowRect(&rt, dwStyle, FALSE) == 0)
	{
		return;
	}


	m_handle = CreateWindowEx(0,
							  NAME_SLWNDCLS, m_title.c_str(),
							  dwStyle,
							  rt.left,
							  rt.top,
							  rt.right - rt.left,
							  rt.bottom - rt.top,
							  nullptr, nullptr, ::GetModuleHandle(NULL), this //<<-CREATESTRUCT구조의lpCreateParams로 전달
	);

	if (!m_handle)
	{
		return;
	}

	ShowWindow(m_handle, SW_SHOW);
	UpdateWindow(m_handle);
}

void EngineWindow::SetTitle(const wstring& title)
{
	SetWindowText(m_handle, title.c_str());
}

void EngineWindow::RegisterWindowClass()
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(0, 0, 0)));
	wcex.hIcon = nullptr;
	wcex.hIconSm = nullptr;
	wcex.lpszClassName = NAME_SLWNDCLS;
	wcex.lpszMenuName = nullptr;
	wcex.hInstance = ::GetModuleHandle(NULL);
	wcex.lpfnWndProc = SetupMessageHandler;
	RegisterClassEx(&wcex);
}


