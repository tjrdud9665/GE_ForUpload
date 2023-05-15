#include "EnginePCH.h"
#include "D3DManager.h"
#include <dxgi1_5.h>

#define ENABLE_D3D_LIVE_OBJECT_REPORTING 1
#define ENABLE_D3D_RESOURCE_DEBUG_NAMES 1


D3DManager::D3DManager()
{
	m_swapChain = nullptr;
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_wndHeight = m_wndWidth = 0;
	m_FullScreen = false;
}

D3DManager::~D3DManager()
{
	Shutdown();
}

bool D3DManager::Initialize(int width, int height, const bool VSYNC, HWND hwnd, const bool FULL_SCREEN, DXGI_FORMAT FrameBufferFormat)
{
	m_hwnd = hwnd;

	HRESULT result;
	IDXGIFactory* pFactory;
	IDXGIAdapter* pAdapter;
	IDXGIOutput* pAdapterOutput;
	unsigned numModes;

	// Store the vsync setting.
	m_vsync_enabled = VSYNC;
	m_FullScreen = FULL_SCREEN;

	// Get System Information
	//----------------------------------------------------------------------------------

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
	if (FAILED(result))
	{
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = pFactory->EnumAdapters(0, &pAdapter);
	if (FAILED(result))
	{
		return false;
	}

	/*
	bool tearing = false;
	IDXGIFactory5* factory5 = reinterpret_cast<IDXGIFactory5*>(pFactory);
	factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &tearing, sizeof(tearing));
	*/	

	// Enumerate the primary adapter output (monitor).
	result = pAdapter->EnumOutputs(0, &pAdapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	DXGI_MODE_DESC* displayModeList;
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures.
	result = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	uint32 numerator = 0;
	uint32 denominator = 0;
	for (uint32 i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)width)
		{
			if (displayModeList[i].Height == (unsigned int)height)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	if (numerator == 0 && denominator == 0)
	{
		char info[127];
		numerator = displayModeList[numModes / 2].RefreshRate.Numerator;
		denominator = displayModeList[numModes / 2].RefreshRate.Denominator;
		sprintf_s(info, "Specified resolution (%ux%u) not found: Using (%ux%u) instead\n",
				  width, height,
				  displayModeList[numModes / 2].Width, displayModeList[numModes / 2].Height);
		width = displayModeList[numModes / 2].Width;
		height = displayModeList[numModes / 2].Height;
		OutputDebugStringA(info);

		// also resize window
		SetWindowPos(hwnd, 0, 10, 10, width, height, SWP_NOZORDER);
	}

	// Get the adapter (video card) description.
	DXGI_ADAPTER_DESC adapterDesc;
	result = pAdapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	m_VRAM = (int32)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	size_t stringLength;
	int error;
	error = wcstombs_s(&stringLength, m_GPUDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// Release memory
	SAFE_DELETE_ARRAY(displayModeList);	
	SAFE_RELEASE(pAdapterOutput);
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pFactory);

	if (!InitSwapChain(hwnd, FULL_SCREEN, width, height, numerator, denominator, DXGI_FORMAT_R8G8B8A8_UNORM))
	{
		return false;
	}

	m_wndWidth = width;
	m_wndHeight = height;


#ifdef _DEBUG


	// Direct3D SDK Debug Layer
	//------------------------------------------------------------------------------------------
	// src1: https://blogs.msdn.microsoft.com/chuckw/2012/11/30/direct3d-sdk-debug-layer-tricks/
	// src2: http://seanmiddleditch.com/direct3d-11-debug-api-tricks/
	//------------------------------------------------------------------------------------------
	if (SUCCEEDED(m_device->QueryInterface(__uuidof(ID3D11Debug), (void**)&m_debug)))
	{
		ID3D11InfoQueue* d3dInfoQueue = nullptr;
		if (SUCCEEDED(m_debug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue)))
		{
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
			D3D11_MESSAGE_ID hide[] =
			{
				D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET,
				// Add more message IDs here as needed
			};

			D3D11_INFO_QUEUE_FILTER filter;
			memset(&filter, 0, sizeof(filter));
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
			d3dInfoQueue->Release();
		}
	}


	if (FAILED(m_deviceContext->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), (void**)&m_annotation)))
	{
		
		return false;
	}
#endif

	return true;
}

void D3DManager::Shutdown()
{
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}
	
	SAFE_RELEASE(m_deviceContext);
	SAFE_RELEASE(m_device);
	SAFE_RELEASE(m_swapChain);
#if _DEBUG
	SAFE_RELEASE(m_annotation);	
	SAFE_RELEASE(m_debug);
#endif
	
}

void D3DManager::EndFrame()
{
	UINT flag = 0;
	
	if (!m_FullScreen)
		flag = DXGI_PRESENT_ALLOW_TEARING;
	
	if (m_vsync_enabled)		m_swapChain->Present(0, flag);
	else						m_swapChain->Present(0, flag);
}

void D3DManager::GetVideoCardInfo(char* cardName, int32& OutMemory)
{
	::strcpy_s(cardName, 128, m_GPUDescription);
	OutMemory = m_VRAM;	

}

float D3DManager::GetAspectRatio() const
{
	return static_cast<float>(m_wndWidth) / static_cast<float>(m_wndHeight);
}

uint32 D3DManager::WindowWidth() const
{
	return m_wndWidth;
}

uint32 D3DManager::WindowHeight() const
{
	return m_wndHeight;
}

void D3DManager::ReportLiveObjects() const
{
#if ENABLE_D3D_LIVE_OBJECT_REPORTING
#ifdef _DEBUG
	HRESULT hr = m_device->QueryInterface(__uuidof(ID3D11Debug), (void**)&m_debug);
	hr = m_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif
#endif // ENABLE_D3D_LIVE_OBJECT_REPORTING
}

void D3DManager::SetDebugName(ID3D11DeviceChild* child, const std::string& name)
{

#if ENABLE_D3D_RESOURCE_DEBUG_NAMES
	if (child != nullptr && !name.empty())
		child->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.size()), name.c_str());
#endif // ENABLE_D3D_RESOURCE_DEBUG_NAMES

}
//https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/for-best-performance--use-dxgi-flip-model

bool D3DManager::InitSwapChain(HWND hwnd, bool fullscreen, int32 scrWidth, int32 scrHeight, uint32 numerator, uint32 denominator, DXGI_FORMAT FrameBufferFormat)
{
	HRESULT result;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	D3D_FEATURE_LEVEL featureLevel;

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 3;


		
	swapChainDesc.BufferDesc.Width = scrWidth;
	swapChainDesc.BufferDesc.Height = scrHeight;
	swapChainDesc.BufferDesc.Format = FrameBufferFormat;	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb173064(v=vs.85).aspx
	if (m_vsync_enabled)
	{	// Set the refresh rate of the back buffer.
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	} 
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	swapChainDesc.OutputWindow = hwnd;	// Set the handle for the window to render to.
	swapChainDesc.Windowed = !fullscreen;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;


	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//https://devblogs.microsoft.com/directx/dxgi-flip-model/
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	//DXGI_SWAP_EFFECT_FLIP_DISCARD
	

	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	featureLevel = D3D_FEATURE_LEVEL_11_0;

	

#if defined( _DEBUG )
	UINT flags = D3D11_CREATE_DEVICE_DEBUG;

#else
	UINT flags = 0;
#endif


	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(NULL,
										   D3D_DRIVER_TYPE_HARDWARE,
										   NULL,
										   flags,
										   &featureLevel,
										   1,
										   D3D11_SDK_VERSION,
										   &swapChainDesc,
										   &m_swapChain,
										   &m_device, NULL, &m_deviceContext);

	if(fullscreen)
	{
		m_swapChain->SetFullscreenState(true, nullptr);
	}


	if (FAILED(result))
	{		
		return false;
	}
	return true;
}

void D3DManager::ReseizeBuffer(uint32 width, uint32 height)
{
	if(m_swapChain)
	{
		m_swapChain->ResizeBuffers(3, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	}
	
}
