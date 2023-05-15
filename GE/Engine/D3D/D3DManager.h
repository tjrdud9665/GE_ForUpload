#pragma once
#include "../XObject/XObject.h"
#include <d3d11_1.h>
#include <d3d11sdklayers.h>

/*
* https://megayuchi.com/2018/04/10/surface-book%ec%97%90%ec%84%9c-60%ed%94%84%eb%a0%88%ec%9e%84-%eb%9d%bd-%ed%95%b4%ea%b2%b0%eb%b0%a9%eb%b2%95/

*/


class D3DManager :public XObject
{
	friend class Engine;
	friend class Renderer;
	friend class Globals;
	friend class EngineWindow;
	friend class Shader;

private:
	D3DManager();
	virtual ~D3DManager();

	bool Initialize(int width, int height, const bool VSYNC, HWND hwnd, const bool FULL_SCREEN, DXGI_FORMAT FrameBufferFormat);
	void Shutdown();

	void EndFrame();

	void GetVideoCardInfo(char* cardName , int32& OutMemory);

public:
	float    GetAspectRatio() const;
	uint32	 WindowWidth() const;
	uint32	 WindowHeight() const;

	inline HWND	 WindowHandle() const { return m_hwnd; }

	void ReportLiveObjects() const;

	
	static void SetDebugName(ID3D11DeviceChild* child, const std::string& name);

private:
	bool InitSwapChain(HWND hwnd, bool fullscreen, int32 scrWidth, int32 scrHeight, uint32 numerator, uint32 denominator, DXGI_FORMAT FrameBufferFormat);

	void ReseizeBuffer(uint32 width, uint32 height);	

private:
	bool						m_vsync_enabled;
	int							m_VRAM;
	char						m_GPUDescription[128];
	HWND						m_hwnd;

	bool						m_FullScreen;

	IDXGISwapChain* m_swapChain;

	ID3D11Device*			m_device;		
	ID3D11DeviceContext* m_deviceContext;

	uint32 m_wndWidth;
	uint32 m_wndHeight;


#if _DEBUG
	ID3D11Debug* m_debug;
	ID3DUserDefinedAnnotation* m_annotation;
#endif


};

