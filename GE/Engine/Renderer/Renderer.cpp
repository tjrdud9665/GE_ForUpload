#include "EnginePCH.h"
#include "Renderer.h"
#include "../D3D/D3DManager.h"
#include "../FileSystem/FileSystem.h"
#include <wincodec.h>
#include <Path/PathManager.h>
#include "GeometryGenerator.h"
#include <Assets/Mesh/Mesh.h>

Renderer::Renderer()
	:m_BackBufferRenderTarget(INDEX_NONE)
	, m_DefaultDepthBufferTexture(INDEX_NONE)
	, m_cachedMgr(nullptr)
	, m_device(nullptr)
	, m_deviceContext(nullptr)
	, m_RasterizerStates(std::vector<ID3D11RasterizerState*>(EDefaultRasterizerState::RASTERIZER_STATE_COUNT))
	, m_DepthStencilStates(std::vector<ID3D11DepthStencilState*>(EDefaultDepthStencilState::DEPTH_STENCIL_STATE_COUNT))
	, m_BlendStates(std::vector<ID3D11BlendState*>(EDefaultBlendState::BLEND_STATE_COUNT))
	//, m_Samplers(std::vector<D3DSampler*>(EDefaultSamplerState::DEFAULT_SAMPLER_COUNT))
{

}

Renderer::~Renderer()
{
	//D3DMgr 생성
	//Renderer 생성
	// 
	// Renderer 소멸
	// D3DMgr 소멸
	ShutDown();
}

bool Renderer::Initialize(class D3DManager* d3dMgr, bool bAntiAliasing)
{
	m_cachedMgr = d3dMgr;
	if (!m_cachedMgr)
		return false;

	m_device = m_cachedMgr->m_device;
	m_deviceContext = m_cachedMgr->m_deviceContext;


	// BackBuffer
	//SRV from BackBuffer
	SRenderTarget	backBufferRT;
	backBufferRT.Texture = new D3DTexture();


	ID3D11Texture2D* backBufferPtr = nullptr;
	HRESULT hr = m_cachedMgr->m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(hr))
	{
		//Log::Error("Cannot get back buffer pointer in DefaultRenderTarget initialization");
		return false;
	}
	
	D3D11_TEXTURE2D_DESC texDescBackBuffer;
	backBufferPtr->GetDesc(&texDescBackBuffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;	
	srvDesc.Format = texDescBackBuffer.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	m_device->CreateShaderResourceView(backBufferPtr, &srvDesc, &(backBufferRT.Texture->m_SRV));
	backBufferRT.Texture->m_Tex2D= backBufferPtr;
	backBufferRT.Texture->m_Width = texDescBackBuffer.Width;
	backBufferRT.Texture->m_Height = texDescBackBuffer.Height;
	backBufferRT.Texture->m_Name = "BackBufferRT";
	hr = m_device->CreateRenderTargetView(backBufferPtr, nullptr, &(backBufferRT.RTV));
	
	if (FAILED(hr))
	{
		//Log::Error("Cannot create default render target view.");
		return false;
	}

#if _DEBUG
	m_cachedMgr->SetDebugName(backBufferRT.RTV, "BackBufferRTV");
	m_cachedMgr->SetDebugName(backBufferRT.Texture->m_SRV, "BackBufferSRV");	
	m_cachedMgr->SetDebugName(backBufferPtr, "BackBuffer");						//Texture2D
#endif

	//Repo
	backBufferRT.Texture->m_ID = static_cast<int32>(m_Textures.size());
	m_Textures.push_back(backBufferRT.Texture);
	m_RenderTargets.push_back(backBufferRT);
	m_BackBufferRenderTarget = static_cast<int32>(m_RenderTargets.size() - 1);
	


	const float& UpscaleFactor = m_AASetting.UpscaleFactor;

	if(bAntiAliasing)
	{
		SRenderTargetDesc rtDesc;

		rtDesc.TextureDesc.Width = static_cast<int32>(texDescBackBuffer.Width * UpscaleFactor);
		rtDesc.TextureDesc.Height = static_cast<int32>(texDescBackBuffer.Height * UpscaleFactor);
		rtDesc.TextureDesc.mipCount = 1;
		rtDesc.TextureDesc.arraySize = 1;
		rtDesc.TextureDesc.Format = texDescBackBuffer.Format;
		rtDesc.TextureDesc.Usage = ETextureUsage::RENDER_TARGET_RW;
		rtDesc.Format = texDescBackBuffer.Format;

		const FRenderTargetID RT_ID = AddRenderTarget(rtDesc);
		//Resolutions
		m_AASetting.ResolutionX = rtDesc.TextureDesc.Width;
		m_AASetting.ResolutionY = rtDesc.TextureDesc.Height; 
	}
	else
	{
		m_AASetting.ResolutionX = m_cachedMgr->WindowHeight();
		m_AASetting.ResolutionY = m_cachedMgr->WindowWidth();			
	}


	// DEFAULT DEPTH TARGET
	//--------------------------------------------------------------------
	{
		STextureDesc depthTexDesc;	// Set up the description of the depth buffer.
		depthTexDesc.Width = m_cachedMgr->WindowWidth();
		depthTexDesc.Height = m_cachedMgr->WindowHeight();
		depthTexDesc.arraySize = 1;
		depthTexDesc.mipCount = 1;
		depthTexDesc.Format= DXGI_FORMAT_R32_TYPELESS;
		depthTexDesc.Usage = ETextureUsage(DEPTH_TARGET | RESOURCE); //SRV |DSV

		SDepthTargetDesc depthDesc;
		depthDesc.Format= DXGI_FORMAT_D32_FLOAT;
		depthDesc.TextureDesc = depthTexDesc;
		m_DefaultDepthTargetID = AddDepthTarget(depthDesc)[0];
		m_DefaultDepthBufferTexture = GetDepthTargetTexture(m_DefaultDepthTargetID);
	}

	// DEFAULT RASTERIZER STATES
	//--------------------------------------------------------------------
	{
		HRESULT hr;
		const std::string err("Unable to create Rasterizer State: Cull ");

		// MSDN: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476198(v=vs.85).aspx
		D3D11_RASTERIZER_DESC rsDesc = {};

		rsDesc.FillMode = D3D11_FILL_SOLID; 
		rsDesc.FrontCounterClockwise = false;
		rsDesc.DepthBias = 0;
		rsDesc.ScissorEnable = false;
		rsDesc.DepthBiasClamp = 0;
		rsDesc.SlopeScaledDepthBias = 0.0f;
		rsDesc.DepthClipEnable = true;
		rsDesc.AntialiasedLineEnable = true;
		rsDesc.MultisampleEnable = false;

		rsDesc.CullMode = D3D11_CULL_BACK;
		hr = m_device->CreateRasterizerState(&rsDesc, &m_RasterizerStates[(int32)EDefaultRasterizerState::CULL_BACK]);
		if (FAILED(hr))
		{
			//Log::Error(err + "Back\n");
		}

		rsDesc.CullMode = D3D11_CULL_FRONT;
		hr = m_device->CreateRasterizerState(&rsDesc, &m_RasterizerStates[(int32)EDefaultRasterizerState::CULL_FRONT]);
		if (FAILED(hr))
		{
			//Log::Error(err + "Front\n");
		}

		rsDesc.CullMode = D3D11_CULL_NONE;
		hr = m_device->CreateRasterizerState(&rsDesc, &m_RasterizerStates[(int32)EDefaultRasterizerState::CULL_NONE]);
		if (FAILED(hr))
		{
			//Log::Error(err + "None\n");
		}
		rsDesc.ScissorEnable = true;
		rsDesc.CullMode = D3D11_CULL_NONE;
		hr = m_device->CreateRasterizerState(&rsDesc, &m_RasterizerStates[(int32)EDefaultRasterizerState::CULL_NONE_Rect]);
		if (FAILED(hr))
		{
			//Log::Error(err + "Wireframe\n");

		}
		
		rsDesc.FillMode = D3D11_FILL_WIREFRAME;
		hr = m_device->CreateRasterizerState(&rsDesc, &m_RasterizerStates[(int32)EDefaultRasterizerState::WIREFRAME]);
		if (FAILED(hr))
		{
			//Log::Error(err + "Wireframe\n");
		}

	
	}

	// DEFAULT BLEND STATES
	//--------------------------------------------------------------------
	{
		D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
		rtBlendDesc.BlendEnable = true;
		rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
		rtBlendDesc.SrcBlend = D3D11_BLEND_ONE;
		rtBlendDesc.DestBlend = D3D11_BLEND_ONE;
		rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_MIN;
		rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		rtBlendDesc.DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
		rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		D3D11_BLEND_DESC desc = {};
		desc.RenderTarget[0] = rtBlendDesc;

		m_device->CreateBlendState(&desc, &(m_BlendStates[EDefaultBlendState::ADDITIVE_COLOR]));

		rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
		rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		desc.RenderTarget[0] = rtBlendDesc;
		m_device->CreateBlendState(&desc, &(m_BlendStates[EDefaultBlendState::ALPHA_BLEND]));

		rtBlendDesc.BlendEnable = false;
		desc.RenderTarget[0] = rtBlendDesc;
		m_device->CreateBlendState(&desc, &(m_BlendStates[EDefaultBlendState::DISABLED]));
	}


	// DEFAULT SAMPLER STATES
	//--------------------------------------------------------------------
	//EDefaultSamplerState 옵션의 순서에 맞게 ID를 맵핑
	{
		D3D11_SAMPLER_DESC samplerDesc = {};

		/* POINT_SAMPLER */
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		CreateSamplerState(samplerDesc, "DEFAULT_POINT_SAMPLER");
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		CreateSamplerState(samplerDesc, "DEFAULT_POINT_SAMPLER_WRAP");


		/* LINEAR_FILTER_SAMPLER_WRAP_UVW */
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		CreateSamplerState(samplerDesc, "DEFAULT_LINEAR_FILTER_SAMPLER_WRAP_UVW_SAMPLER");		


		/* LINEAR_FILTER_SAMPLER */
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		CreateSamplerState(samplerDesc, "DEFAULT_LINEAR_FILTER_SAMPLER");
		//m_device->CreateSamplerState(&samplerDesc, &(m_Samplers[EDefaultSamplerState::LINEAR_FILTER_SAMPLER]->m_SamplerState));


		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MaxAnisotropy = 1;
		CreateSamplerState(samplerDesc, "DEFAULT_ANISOTROPIC_1_CLAMPED_SAMPLER");		
		samplerDesc.MaxAnisotropy = 2;
		CreateSamplerState(samplerDesc, "DEFAULT_ANISOTROPIC_2_CLAMPED_SAMPLER");		
		samplerDesc.MaxAnisotropy = 4;
		CreateSamplerState(samplerDesc, "DEFAULT_ANISOTROPIC_4_CLAMPED_SAMPLER");		
		samplerDesc.MaxAnisotropy = 16;
		CreateSamplerState(samplerDesc, "DEFAULT_ANISOTROPIC_16_CLAMPED_SAMPLER");		
		
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = 1;
		CreateSamplerState(samplerDesc, "DEFAULT_ANISOTROPIC_1_WRAPPED_SAMPLER");		
		samplerDesc.MaxAnisotropy = 2;
		CreateSamplerState(samplerDesc, "DEFAULT_ANISOTROPIC_2_WRAPPED_SAMPLER");		
		samplerDesc.MaxAnisotropy = 4;
		CreateSamplerState(samplerDesc, "DEFAULT_ANISOTROPIC_4_WRAPPED_SAMPLER");		
		samplerDesc.MaxAnisotropy = 16;
		CreateSamplerState(samplerDesc, "DEFAULT_ANISOTROPIC_16_WRAPPED_SAMPLER");		
	}


	// DEFAULT DEPTHSTENCIL SATATES
	//--------------------------------------------------------------------
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil states.
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilStates[EDefaultDepthStencilState::DEPTH_STENCIL_WRITE]);
	if(FAILED(hr))
	{
		return false;
	}	

	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.StencilEnable = false;
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilStates[EDefaultDepthStencilState::DEPTH_STENCIL_DISABLED]);
	if (FAILED(hr))
	{
		return false;
	}

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.StencilEnable = false;
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilStates[EDefaultDepthStencilState::DEPTH_WRITE]);
	if (FAILED(hr))
	{
		return false;
	}

	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.StencilEnable = true;
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilStates[EDefaultDepthStencilState::STENCIL_WRITE]);
	if (FAILED(hr))
	{
		return false;
	}

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.StencilEnable = false;
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilStates[EDefaultDepthStencilState::DEPTH_TEST_ONLY]);
	if (FAILED(hr))
	{
		return false;
	}
	
	/*
	* enum EGeometry
{
	//TRIANGLE = 0,
	QUAD = 0,
	FULLSCREENQUAD,
	CUBE,
	CYLINDER,
	SPHERE,
	GRID,
	CONE,
	LIGHT_CUE_CONE,
	//BONE,

	MESH_TYPE_COUNT
};
	*/
	
	m_BuiltInMeshs.push_back(GeometryGenerator::Quad(0.5f));
	m_BuiltInMeshs.push_back(GeometryGenerator::FullScreenQuad());
	m_BuiltInMeshs.push_back(GeometryGenerator::Cube());
	m_BuiltInMeshs.push_back(GeometryGenerator::Cylinder(3.1415f,1.0f,1.0f,70,20));
	m_BuiltInMeshs.push_back(GeometryGenerator::Sphere(2, 12, 60));


	return true;
}


void Renderer::ShutDown()
{	
	constexpr size_t BUFFER_TYPE_COUNT = 3;

	std::vector<SD3DBuffer>* buffers[BUFFER_TYPE_COUNT] = { &m_VertexBuffers, &m_IndexBuffers, &m_UABuffers };

	//cleanup EachBuffers
	for (int32 i = 0; i < BUFFER_TYPE_COUNT; ++i)
	{
		std::vector<SD3DBuffer>& refBuffer = *buffers[i];
		size_t bufSize = refBuffer.size();
		for(size_t j= 0;j< bufSize;j++)
		{
			refBuffer[j].CleanUp();
		}		
		refBuffer.clear();
	}

	// Unload shaders
	size_t shaderSize = m_Shaders.size();
	for(size_t i = 0 ;i<shaderSize;i++)
	{		
		SAFE_DELETE(m_Shaders[i]);
	}
	m_Shaders.clear();


	size_t texSize = m_Textures.size();
	for (size_t i = 0; i < texSize; i++)
	{
		m_Textures[i]->Release();
		SAFE_DELETE(m_Textures[i]);

	}
	m_Textures.clear();


	size_t smpSize = m_Samplers.size();
	for (size_t i = 0; i < smpSize; i++)
	{
		SAFE_DELETE(m_Samplers[i]);
	}
	m_Samplers.clear();

	size_t rtSize = m_RenderTargets.size();
	for(size_t i = 0;i<rtSize;i++)
	{
		SAFE_RELEASE(m_RenderTargets[i].RTV);
		m_RenderTargets[i].Texture = nullptr;		//Release SRV Tex..

	}
	m_RenderTargets.clear();

	size_t rsSize = m_RasterizerStates.size();
	for (size_t i = 0; i < rsSize; i++)
	{
		SAFE_RELEASE(m_RasterizerStates[i]);		
		
	}
	m_RasterizerStates.clear();

	size_t dsSize = m_DepthStencilStates.size();
	for(size_t i =0;i<dsSize;i++)
	{
		m_DepthStencilStates[i]->Release();
		m_DepthStencilStates[i] = nullptr;
	}
	m_DepthStencilStates.clear();


	size_t bsSize = m_BlendStates.size();
	for(size_t i =0;i<bsSize;i++)
	{
		m_BlendStates[i]->Release();
		m_BlendStates[i] = nullptr;
	}
	m_BlendStates.clear();


	size_t dtSize = m_DepthTargets.size();
	for (size_t i = 0; i < dtSize; i++)
	{
		SAFE_RELEASE(m_DepthTargets[i].DSV);		
	}
	m_DepthTargets.clear();	
	
}

ID3D11ShaderResourceView* Renderer::GetTextureSRV(FTextureID ID) const
{
	if (ID != INDEX_NONE)
		return m_Textures[ID]->m_SRV;

	return nullptr;
}

D3DTexture* Renderer::GetTextureObject(FTextureID ID) const
{
	assert(ID >= 0 && static_cast<size_t>(ID) < m_Textures.size());
	return m_Textures[ID];
}

const FTextureID Renderer::GetTexture(const std::string name) const
{
	std::vector<D3DTexture*>::const_iterator found = std::find_if(m_Textures.begin(), m_Textures.end(), [&name](D3DTexture* tex) { return tex->m_Name == name; });
	if (found != m_Textures.end())
	{
		return (*found)->m_ID;
	}
	//Log::Error("Texture not found: " + name);
	return INDEX_NONE;
}

D3DSampler* Renderer::GetSampler(FSamplerID ID) const
{
	assert(ID >= 0 && static_cast<size_t>(ID) < m_Samplers.size());
	return m_Samplers[ID];
}

FTextureID Renderer::GetDepthTargetTexture(FDepthTargetID DT) const
{
	assert(DT >= 0 && static_cast<size_t>(DT) < m_DepthTargets.size());
	return m_DepthTargets[DT].Texture->m_ID;
}

float Renderer::AspectRatio() const
{
	return m_cachedMgr->GetAspectRatio();
}

uint32 Renderer::FrameRenderTargetHeight() const
{
	return m_AASetting.ResolutionY;
}

uint32 Renderer::FrameRenderTargetWidth() const
{
	return m_AASetting.ResolutionX;
}

SVector2 Renderer::FrameRenderTargetDimensionsAsFloat2() const
{
	return SVector2((float)FrameRenderTargetWidth(), (float)FrameRenderTargetHeight());
}

const D3D11_VIEWPORT& Renderer::GetCurrentViewport() const
{
	return m_PipelineState.ViewPort;
}

const SPipelineState& Renderer::GetPipelineState() const
{
	return m_PipelineState;
}

void Renderer::SetPipelineState(const SPipelineState& InPipeLineState)
{	
	m_PipelineState = InPipeLineState;
}

FBufferID Renderer::GetVertexBufferFromName(const std::string& InName) const
{
	size_t vertexBufferSize = m_VertexBuffers.size();
	for(size_t i =0;i<vertexBufferSize;i++)
	{
		if (m_VertexBuffers[i].Name == InName)
			return (FBufferID)i;
	}

	return INDEX_NONE;
}

FBufferID Renderer::GetIndexBufferFromName(const std::string& InName) const
{
	size_t indexBufferSize = m_IndexBuffers.size();
	for (size_t i = 0; i < indexBufferSize; i++)
	{
		if (m_IndexBuffers[i].Name == InName)
			return (FBufferID)i;
	}

	return INDEX_NONE;
}

const Shader* Renderer::GetShader(FShaderID shader_id) const
{
	assert(shader_id >= 0 && (FShaderID)m_Shaders.size() > shader_id);
	return m_Shaders[shader_id];
}

FShaderID Renderer::CreateShader(const SShaderDesc& InShaderDesc)
{
	Shader* shader = new Shader(InShaderDesc.ShaderName);
	shader->CompileShaders(m_device, InShaderDesc);

	m_Shaders.push_back(shader);
	shader->m_ID = (static_cast<int32>(m_Shaders.size()) - 1);
	return shader->GetID();
}

FTextureID Renderer::CreateTextureFromMemory(void* pResource, size_t InSize, const std::string& InTexName, const std::string& DesiredPath, bool bGenerateMips )
{
	//Want Hint Format..
	if (InTexName.empty())
	{
		//Log::Warning("Warning: CreateTextureFromFile() - empty texture file name passed as parameter");
		return INDEX_NONE;
	}
	const std::string FullPath = DesiredPath + InTexName;
	D3DTexture* tex = nullptr;
	FTextureID retID = INDEX_NONE;

	std::wstring wPath = StringToWString(FullPath);

	const SPathInfo* resRootinfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
	fs::path fullPath = FullPath;
	std::string nameKey = fullPath.lexically_relative(resRootinfo->aPath).string();
	
	std::vector<D3DTexture*>::iterator found = std::find_if(m_Textures.begin(), m_Textures.end(), [&nameKey](D3DTexture*& tex) { return tex->m_Name == nameKey; });
	if (found != m_Textures.end())
	{
		return (*found)->m_ID;
	}



	
	DirectX::ScratchImage* img = new DirectX::ScratchImage();

	fs::path fileName = wPath;
	bool bSuccessFromMemory = false;

	if (fileName.extension() == ".tga" || fileName.extension() == ".TGA")
	{
		if (SUCCEEDED(LoadFromTGAMemory(pResource, InSize, nullptr, *img)))
		{
			bSuccessFromMemory = true;
		}
	} 
	else if (fileName.extension() == ".dds" || fileName.extension() == ".DDS")
	{
		if (SUCCEEDED(LoadFromDDSMemory(pResource,InSize, DirectX::DDS_FLAGS_NONE, nullptr, *img)))
		{
			bSuccessFromMemory = true;
		}
	} else
	{
		if (SUCCEEDED(LoadFromWICMemory(pResource,InSize, WIC_FLAGS_NONE, nullptr, *img)))
		{
			bSuccessFromMemory = true;
		}
	}


	if (bSuccessFromMemory)
	{
		//Create SRV or SRVARray(Mip)
		const TexMetadata& meta = img->GetMetadata();

		ID3D11ShaderResourceView* SRV = nullptr;

		if (SUCCEEDED(CreateShaderResourceView(m_device, img->GetImages(), img->GetImageCount(), meta, &(SRV))))
		{
			if (bGenerateMips)
			{
				ID3D11Resource* resource = nullptr;
				SRV->GetResource(&resource);
				SAFE_RELEASE(SRV);

				// std::log2(1024) == 10
				size_t mipLevels = std::min(static_cast<size_t>(std::log2(meta.width)), static_cast<size_t>(std::log2(meta.height)));
				//meta.mipLevels = std::min(static_cast<size_t>(std::log2(meta.width)), static_cast<size_t>(std::log2(meta.height)));

				STextureDesc texDesc = {};
				texDesc.bGenerateMips = true;
				texDesc.bIsCubeMap = meta.depth != 1; // false?
				texDesc.arraySize = static_cast<int32>(meta.arraySize);
				texDesc.Format = meta.format;
				texDesc.Width = static_cast<int32>(meta.width);
				texDesc.Height = static_cast<int32>(meta.height);
				texDesc.mipCount = static_cast<int32>(mipLevels);
				texDesc.Usage = ETextureUsage::RENDER_TARGET_RW;
				texDesc.FileName = InTexName;

				tex = GetTextureObject(CreateTexture2D(texDesc));

				tex->m_Name = fileName.lexically_relative(resRootinfo->aPath).string();
				m_deviceContext->CopySubresourceRegion(tex->m_Tex2D, 0, 0, 0, 0, resource, 0, NULL);
				m_deviceContext->GenerateMips(tex->m_SRV);
				m_deviceContext->Flush();

				resource->Release();
			}
			//No Mips
			else
			{
				tex = new D3DTexture();
				tex->m_Name = fileName.lexically_relative(resRootinfo->aPath).string();

				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				SRV->GetDesc(&srvDesc);

				// read width & height
				ID3D11Resource* resource = nullptr;

				SRV->GetResource(&resource);
				tex->m_SRV = SRV;
				if (SUCCEEDED(resource->QueryInterface(&tex->m_Tex2D)))
				{
					D3D11_TEXTURE2D_DESC desc;
					tex->m_Tex2D->GetDesc(&desc);
					tex->m_Width = desc.Width;
					tex->m_Height = desc.Height;
				}
				resource->Release();
				tex->m_ID = static_cast<int32>(m_Textures.size());
				m_Textures.push_back(tex);
			}
			retID = m_Textures.back()->m_ID;
		}
	}


	SAFE_DELETE(img);

	return retID;
}

bool Renderer::SaveTextureToFile(FTextureID toSaveTexture, const std::string& InTexFullPath, bool bOverride /*= false*/)
{
	if (FileSystem::FileExists(InTexFullPath) && !bOverride)
	{			
		return true;
	}
	if (toSaveTexture == INDEX_NONE)
		return false;

	

	wstring wTexFullPath = StringToWString(InTexFullPath);	
	if(!FileSystem::FolderExists(wTexFullPath))
		fs::create_directory(wTexFullPath.c_str());

	fs::path pathFromName = m_Textures[toSaveTexture]->m_Name;	
	wTexFullPath += pathFromName.filename().wstring();

	fs::path fileName = wTexFullPath;
	
	DirectX::ScratchImage* img = new DirectX::ScratchImage();
	HRESULT hr = DirectX::CaptureTexture(m_device, m_deviceContext, m_Textures[toSaveTexture]->m_Tex2D,*img);
	if(SUCCEEDED(hr))
	{
		const Image iimg = *img->GetImages();
		if (fileName.extension() == ".tga" || fileName.extension() == ".TGA")
		{
			
			if (SUCCEEDED(DirectX::SaveToTGAFile(iimg,wTexFullPath.c_str(), &img->GetMetadata())))
			{

			}
		} 
		else if (fileName.extension() == ".dds" || fileName.extension() == ".DDS")
		{
			if (SUCCEEDED(DirectX::SaveToDDSFile(img->GetImages(), img->GetImageCount(), img->GetMetadata(), DirectX::DDS_FLAGS_NONE, wTexFullPath.c_str())))
			{

			}
		} 
		else
		{
			if (SUCCEEDED(DirectX::SaveToWICFile(img->GetImages(), img->GetImageCount(),DirectX::WIC_FLAGS_NONE,GUID_ContainerFormatPng,wTexFullPath.c_str())))
			{
				
			}
		}
	}
	

	
	SAFE_DELETE(img);

	return true;
}

FTextureID Renderer::CreateTextureFromFile(const std::string& InTexFileName, const std::string& InPath, bool bGenerateMips /*= false*/)
{
	if (InTexFileName.empty())
	{
		//Log::Warning("Warning: CreateTextureFromFile() - empty texture file name passed as parameter");
		return INDEX_NONE;
	}	

	const std::string FullPath = InPath + InTexFileName;
	D3DTexture* tex = nullptr;
	FTextureID retID = INDEX_NONE;

	std::wstring wPath = StringToWString(FullPath);

	const SPathInfo* resRootinfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);	
	fs::path fullPath = FullPath;
	std::string nameKey = fullPath.lexically_relative(resRootinfo->aPath).string();

	std::vector<D3DTexture*>::iterator found = std::find_if(m_Textures.begin(), m_Textures.end(), [&nameKey](D3DTexture*& tex) { return tex->m_Name == nameKey; });
	if (found != m_Textures.end())
	{
		return (*found)->m_ID;
	}


	DirectX::ScratchImage* img = new DirectX::ScratchImage();

	fs::path fileName = wPath;
	bool bSuccessFromFile = false;

	if(fileName.extension() == ".tga" || fileName.extension() == ".TGA")
	{
		if (SUCCEEDED(LoadFromTGAFile(wPath.c_str(), nullptr, *img)))
		{
			bSuccessFromFile = true;
		}
	}
	else if (fileName.extension() == ".dds" || fileName.extension() == ".DDS")
	{
		if (SUCCEEDED(LoadFromDDSFile(wPath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, *img)))
		{
			bSuccessFromFile = true;
		}
	}
	else
	{
		if (SUCCEEDED(LoadFromWICFile(wPath.c_str(), WIC_FLAGS_NONE, nullptr, *img)))
		{
			bSuccessFromFile = true;
		}
	}


	if(bSuccessFromFile)
	{
		//Create SRV or SRVARray(Mip)
		const TexMetadata& meta = img->GetMetadata();

		ID3D11ShaderResourceView* SRV = nullptr;

		if (SUCCEEDED(CreateShaderResourceView(m_device, img->GetImages(), img->GetImageCount(), meta, &(SRV))))
		{
			if (bGenerateMips)
			{
				ID3D11Resource* resource = nullptr;
				SRV->GetResource(&resource);
				SAFE_RELEASE(SRV);

				// std::log2(1024) == 10
				size_t mipLevels = std::min(static_cast<size_t>(std::log2(meta.width)), static_cast<size_t>(std::log2(meta.height)));
				//meta.mipLevels = std::min(static_cast<size_t>(std::log2(meta.width)), static_cast<size_t>(std::log2(meta.height)));

				STextureDesc texDesc = {};
				texDesc.bGenerateMips = true;
				texDesc.bIsCubeMap = meta.depth != 1; // false?
				texDesc.arraySize = static_cast<int32>(meta.arraySize);
				texDesc.Format = meta.format;
				texDesc.Width = static_cast<int32>(meta.width);
				texDesc.Height = static_cast<int32>(meta.height);
				texDesc.mipCount = static_cast<int32>(mipLevels);
				texDesc.Usage = ETextureUsage::RENDER_TARGET_RW;
				texDesc.FileName = InTexFileName;

				tex = GetTextureObject(CreateTexture2D(texDesc));

				tex->m_Name = fileName.lexically_relative(resRootinfo->aPath).string();
				m_deviceContext->CopySubresourceRegion(tex->m_Tex2D, 0, 0, 0, 0, resource, 0, NULL);
				m_deviceContext->GenerateMips(tex->m_SRV);
				m_deviceContext->Flush();

				resource->Release();
			}
			//No Mips
			else
			{
				tex = new D3DTexture();							
				tex->m_Name = fileName.lexically_relative(resRootinfo->aPath).string();
					//InTexFileName;

				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				SRV->GetDesc(&srvDesc);

				// read width & height
				ID3D11Resource* resource = nullptr;

				SRV->GetResource(&resource);
				tex->m_SRV = SRV;
				if (SUCCEEDED(resource->QueryInterface(&tex->m_Tex2D)))
				{
					D3D11_TEXTURE2D_DESC desc;
					tex->m_Tex2D->GetDesc(&desc);
					tex->m_Width = desc.Width;
					tex->m_Height = desc.Height;
				}
				resource->Release();
				tex->m_ID = static_cast<int32>(m_Textures.size());
				m_Textures.push_back(tex);
			}
			retID = m_Textures.back()->m_ID;
		}
	}


	SAFE_DELETE(img);

	return retID;
}

FTextureID Renderer::CreateTexture2D(const STextureDesc& texDesc)
{
	//D3DTexture tex;
	//tex._width = texDesc.width;
	//tex._height = texDesc.height;
	//tex._name = texDesc.texFileName;


	// check multi sampling quality level
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb173072(v=vs.85).aspx
	//UINT maxMultiSamplingQualityLevel = 0;
	//m_device->CheckMultisampleQualityLevels(, , &maxMultiSamplingQualityLevel);
	//---


//////////////////////////////////////////////////////////////////////////
//	Create id3dTexture2D
//////////////////////////////////////////////////////////////////////////
	D3DTexture* tex = new D3DTexture();	

	// Texture2D Resource
	UINT miscFlags = 0;
	miscFlags |= texDesc.bIsCubeMap ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;
	miscFlags |= texDesc.bGenerateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	UINT arrSize = texDesc.arraySize;
	const bool bIsTextureArray = texDesc.arraySize > 1;
	arrSize = texDesc.bIsCubeMap ? 6 * arrSize : arrSize;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Format = texDesc.Format;
	desc.Height = tex->m_Height = std::max(texDesc.Height, 1);
	desc.Width = tex->m_Width = std::max(texDesc.Width, 1);
	desc.ArraySize = arrSize;
	desc.MipLevels = texDesc.mipCount;
	desc.SampleDesc = { 1, 0 };
	desc.BindFlags = texDesc.Usage;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = static_cast<D3D11_CPU_ACCESS_FLAG>(texDesc.cpuAccessMode);
	desc.MiscFlags = miscFlags;

	D3D11_SUBRESOURCE_DATA dataDesc = {};
	D3D11_SUBRESOURCE_DATA* pDataDesc = nullptr;
	if (texDesc.pData)
	{
		dataDesc.pSysMem = texDesc.pData;
		dataDesc.SysMemPitch = texDesc.dataPitch;
		dataDesc.SysMemSlicePitch = texDesc.dataSlicePitch;
		pDataDesc = &dataDesc;
	}
	
	
	HRESULT hr = m_device->CreateTexture2D(&desc, pDataDesc, &(tex->m_Tex2D));

	if(FAILED(hr))
	{
		SAFE_DELETE(tex);
		//Failed To TextureCreation.
		return INDEX_NONE;
	}

//////////////////////////////////////////////////////////////////////////



#if defined(_DEBUG)
	if (!texDesc.FileName.empty())
	{
		m_cachedMgr->SetDebugName((tex->m_Tex2D), texDesc.FileName + "_Tex2D");
	}
#endif

	
	// Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	switch (texDesc.Format)
	{
		// caution: if initializing for depth texture, and the depth texture
		//			has stencil defined (d24s8), we have to check for 
		//			DXGI_FORMAT_R24_UNORM_X8_TYPELESS vs R32F
	case DXGI_FORMAT_R24G8_TYPELESS:
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		break;
	case DXGI_FORMAT_R32_TYPELESS:
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		//DXGI_FORMAT_D24_UNORM_S8_UINT:
		break;	

	}

	//CubMap
	if (texDesc.bIsCubeMap)
	{
		if (bIsTextureArray)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
			srvDesc.TextureCubeArray.NumCubes = arrSize / 6;
			srvDesc.TextureCubeArray.MipLevels = texDesc.mipCount;
			srvDesc.TextureCubeArray.MostDetailedMip = 0;
			srvDesc.TextureCubeArray.First2DArrayFace = 0;
			m_device->CreateShaderResourceView((tex->m_Tex2D), &srvDesc, &(tex->m_SRV));
		}
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MipLevels = texDesc.mipCount;
			srvDesc.TextureCube.MostDetailedMip = 0;
			m_device->CreateShaderResourceView((tex->m_Tex2D), &srvDesc, &(tex->m_SRV));
		}
#if _DEBUG
		if (!texDesc.FileName.empty())
		{
			m_cachedMgr->SetDebugName((tex->m_SRV), texDesc.FileName + "_SRV");
		}
#endif
	}
	//Array
	else
	{
		if (bIsTextureArray)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.MipLevels = texDesc.mipCount;
			srvDesc.Texture2DArray.MostDetailedMip = 0;

			tex->m_SRVArray.resize(desc.ArraySize, nullptr);
			tex->m_Depth = desc.ArraySize;			
			for (uint32 i = 0; i < desc.ArraySize; ++i)
			{
				srvDesc.Texture2DArray.FirstArraySlice = i;
				srvDesc.Texture2DArray.ArraySize = desc.ArraySize - i;

				m_device->CreateShaderResourceView(tex->m_Tex2D, &srvDesc, &tex->m_SRVArray[i]);

				if (i == 0)
					tex->m_SRV = tex->m_SRVArray[i];					
#if _DEBUG
				if (!texDesc.FileName.empty())
				{
					m_cachedMgr->SetDebugName(tex->m_SRVArray[i], texDesc.FileName + "_SRV[" + std::to_string(i) + "]");
				}
#endif
			}

			if (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
			{
				D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
				uavDesc.Format = texDesc.Format;
				uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
				uavDesc.Texture2D.MipSlice = 0;

				tex->m_UAVArray.resize(desc.ArraySize, nullptr);
				tex->m_Depth = desc.ArraySize;
				for (uint32 i = 0; i < desc.ArraySize; ++i)
				{
					uavDesc.Texture2DArray.FirstArraySlice = i;
					uavDesc.Texture2DArray.ArraySize = desc.ArraySize - i;
					m_device->CreateUnorderedAccessView(tex->m_Tex2D, &uavDesc, &tex->m_UAVArray[i]);
					if (i == 0)
						tex->m_UAV= tex->m_UAVArray[i];
				}
			}
		} 
		//SingleTexture
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = texDesc.mipCount;
			srvDesc.Texture2D.MostDetailedMip = 0;			
			m_device->CreateShaderResourceView(tex->m_Tex2D, &srvDesc, &(tex->m_SRV));

			if (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
			{
				D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
				uavDesc.Format = texDesc.Format;
				uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
				uavDesc.Texture2D.MipSlice = 0;

				m_device->CreateUnorderedAccessView(tex->m_Tex2D, &uavDesc, &tex->m_UAV);
			}
		}
	}

	FTextureID retID = INDEX_NONE;
	std::vector<D3DTexture*>::iterator itTex = std::find_if(m_Textures.begin(), m_Textures.end(), [](const D3DTexture* tex1) { return tex1->m_ID == -1; });
	if (itTex != m_Textures.end())
	{
		SAFE_DELETE(*itTex);
		(*itTex) = tex;
		(*itTex)->m_ID = static_cast<FTextureID>((int32)std::distance(m_Textures.begin(), itTex));
		retID = (*itTex)->m_ID;
	} 
	else
	{
		tex->m_ID = static_cast<int32>(m_Textures.size());
		m_Textures.push_back(tex);
		retID = m_Textures.back()->m_ID;
	}
	return retID;
}

FTextureID Renderer::CreateTexture2D(D3D11_TEXTURE2D_DESC& textureDesc, bool initializeSRV)
{
	D3DTexture* pTex = new D3DTexture();
	pTex->InitializeTexture2D(textureDesc, this, initializeSRV);
	m_Textures.push_back(pTex);
	m_Textures.back()->m_ID = static_cast<int32>(m_Textures.size() - 1);
	return m_Textures.back()->m_ID;
}

FTextureID Renderer::CreateHDRTexture(const std::string& texFileName, const std::string& InPath)
{
	if (texFileName.empty())
	{
		//Log::Warning("Warning: CreateTextureFromFile() - empty texture file name passed as parameter");
		return INDEX_NONE;
	}

	const std::string FullPath = InPath + texFileName;
	D3DTexture* tex = nullptr;
	FTextureID retID = INDEX_NONE;

	std::wstring wPath = StringToWString(FullPath);

	const SPathInfo* resRootinfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
	fs::path fullPath = FullPath;
	std::string nameKey = fullPath.lexically_relative(resRootinfo->aPath).string();

	std::vector<D3DTexture*>::iterator found = std::find_if(m_Textures.begin(), m_Textures.end(), [&nameKey](D3DTexture*& tex) { return tex->m_Name == nameKey; });
	if (found != m_Textures.end())
	{
		return (*found)->m_ID;
	}
	DirectX::ScratchImage* img = new DirectX::ScratchImage();
	if(SUCCEEDED(DirectX::LoadFromHDRFile(wPath.c_str(), nullptr, *img)))
	{
		const TexMetadata& meta = img->GetMetadata();

		STextureDesc texDesc = {};
		texDesc.Width = (int32)meta.width;
		texDesc.Height = (int32)meta.height;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.FileName = texFileName;
		texDesc.pData = img->GetPixels();
		texDesc.dataPitch = sizeof(SVector4) * (int32)meta.width;
		texDesc.mipCount = 1;
		texDesc.bGenerateMips = false;

		FTextureID newTex = CreateTexture2D(texDesc);
		
		SAFE_DELETE(img);
		if (newTex == INDEX_NONE)
		{
			//Error
		}
		return newTex;
	}

	return INDEX_NONE;

}

FSamplerID Renderer::CreateSamplerState(D3D11_SAMPLER_DESC& samplerDesc, const std::string& smpName)
{
	ID3D11SamplerState* pSamplerState;
	HRESULT hr = m_device->CreateSamplerState(&samplerDesc, &pSamplerState);
	if (FAILED(hr))
	{
		//Log::Error("Cannot create sampler state\n");
		return INDEX_NONE;
	}

	D3DSampler* out = new D3DSampler();
	out->m_ID = static_cast<FSamplerID>(m_Samplers.size());
	out->m_SamplerState = pSamplerState;
	out->m_Name = smpName;	// ?
	m_Samplers.push_back(out);
	return out->m_ID;
}

FBufferID Renderer::CreateBuffer(const SBufferDesc& bufferDesc, const void* pData /*= nullptr*/, const char* pBufferName /*= nullptr*/)
{
	//FBufferID sameNameID = INDEX_NONE;
	//if(bufferDesc.BufferType == EBufferType::VERTEX_BUFFER)
	//{
	//	sameNameID = GetVertexBufferFromName(pBufferName);
	//}	
	//else if (bufferDesc.BufferType == EBufferType::INDEX_BUFFER)
	//{
	//	sameNameID = GetIndexBufferFromName(pBufferName);
	//}	
	//	
	//if (sameNameID != INDEX_NONE)
	//	return sameNameID;

	SD3DBuffer buffer(bufferDesc);
	buffer.Initialize(pBufferName,m_device, pData);
#if _DEBUG
	if (pBufferName)
	{
		m_cachedMgr->SetDebugName(buffer.GPUData, pBufferName);
	}
#endif
	return static_cast<int32>([&]()
	{
		switch (bufferDesc.BufferType)
		{
		case EBufferType::VERTEX_BUFFER:
			m_VertexBuffers.push_back(buffer);
			return m_VertexBuffers.size() - 1;
		case EBufferType::INDEX_BUFFER:
			m_IndexBuffers.push_back(buffer);
			return m_IndexBuffers.size() - 1;
		case EBufferType::RWSTRUCTED_BUFFER:
		{
			m_UABuffers.push_back(buffer);
			return m_UABuffers.size() - 1;
		}
		default:
			//Log::Warning("Unknown Buffer Type");
			return std::numeric_limits<size_t>::max();
		}
	}());
}

FDepthStencilStateID Renderer::AddDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& dsDesc)
{
	
	HRESULT result;
	ID3D11DepthStencilState* newDSState = nullptr;
	result = m_device->CreateDepthStencilState(&dsDesc, &newDSState);
	if (FAILED(result))
	{		
		return false;
	}

	m_DepthStencilStates.push_back(newDSState);
	return static_cast<FDepthStencilStateID>(m_DepthStencilStates.size() - 1);
}

FRenderTargetID Renderer::AddRenderTarget(const SRenderTargetDesc& renderTargetDesc)
{
	SRenderTarget newRenderTarget;

	const FTextureID texID = CreateTexture2D(renderTargetDesc.TextureDesc);
	D3DTexture* textureObj = GetTextureObject(texID);
	newRenderTarget.Texture= textureObj;

	// create the render target view
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = renderTargetDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	HRESULT hr = m_device->CreateRenderTargetView(newRenderTarget.Texture->m_Tex2D, &rtvDesc, &(newRenderTarget.RTV));
	if (!SUCCEEDED(hr))
	{
		//Log::Error("Cannot create Render Target View");
		return INDEX_NONE;
	}
#if _DEBUG
	const std::string RTVName = (renderTargetDesc.TextureDesc.FileName.empty() ? "UnnamedRenderTarget" : renderTargetDesc.TextureDesc.FileName) + "_RTV";
	m_cachedMgr->SetDebugName(newRenderTarget.RTV, RTVName.c_str());
#endif

	// register & return
	m_RenderTargets.push_back(newRenderTarget);
	return static_cast<int32>(m_RenderTargets.size() - 1);	
}

FRenderTargetID Renderer::AddRenderTarget(D3DTexture* textureObj, D3D11_RENDER_TARGET_VIEW_DESC& RTVDesc)
{
	SRenderTarget newRenderTarget;
	newRenderTarget.Texture = textureObj;
	HRESULT hr = m_device->CreateRenderTargetView(newRenderTarget.Texture->m_Tex2D, &RTVDesc, &newRenderTarget.RTV);
	if (!SUCCEEDED(hr))
	{
		//Log::Error("Render Target View");
		return INDEX_NONE;
	}
#if _DEBUG
	const std::string RTVName = (textureObj->m_Name.empty()? "UnnamedRenderTarget": textureObj->m_Name)+ "_RTV";
	m_cachedMgr->SetDebugName(newRenderTarget.RTV, RTVName.c_str());
#endif

	m_RenderTargets.push_back(newRenderTarget);
	return static_cast<int>(m_RenderTargets.size() - 1);
}

std::vector<FDepthTargetID> Renderer::AddDepthTarget(const SDepthTargetDesc& depthTargetDesc)
{
	const bool bIsDepthTargetCubemap = depthTargetDesc.TextureDesc.bIsCubeMap;
	const int32 numTextures = depthTargetDesc.TextureDesc.arraySize * (bIsDepthTargetCubemap ? 6 : 1);
	const bool bIsDepthTargetArray = numTextures > 1;

	// allocate new depth target
	std::vector<FDepthTargetID> newDepthTargetIDs(numTextures, -1);
	std::vector<SDepthTarget> newDepthTargets(numTextures);

	for(int32 i = 0 ;i<numTextures;i++)
	{		
		newDepthTargets[i].DSV = nullptr;		
	}

	// create depth texture
	//D3D11_BIND_DEPTH_STENCIL
	const FTextureID texID = CreateTexture2D(depthTargetDesc.TextureDesc);
	D3DTexture* textureObj = GetTextureObject(texID);

	// create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = depthTargetDesc.Format;
	dsvDesc.ViewDimension = bIsDepthTargetArray ? D3D11_DSV_DIMENSION_TEXTURE2DARRAY : D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2DArray.MipSlice = 0;

	const int32 faceCount = (bIsDepthTargetCubemap ? 6 : 1);
	const int32 cubemapCount = bIsDepthTargetCubemap ? (numTextures / faceCount) : numTextures;
	for (int32 i = 0; i < cubemapCount; ++i)
	{
		for (int32 face = 0; face < faceCount; ++face)
		{
			const int32 depthTargetIndex = i * faceCount + face;
			SDepthTarget& newDepthTarget = newDepthTargets[depthTargetIndex];
			dsvDesc.Texture2DArray.ArraySize = numTextures - (face + i * faceCount);
			dsvDesc.Texture2DArray.FirstArraySlice = face + i * faceCount;

			HRESULT hr = m_device->CreateDepthStencilView(textureObj->m_Tex2D, &dsvDesc, &newDepthTarget.DSV);
			if (FAILED(hr))
			{
				//Log::Error("Depth Stencil Target View");
				continue;
			}

#if _DEBUG
			const std::string SRVName = (depthTargetDesc.TextureDesc.FileName.empty() ? "UnnamedDepthTarget": depthTargetDesc.TextureDesc.FileName) + "_DSV[" + std::to_string(face) + "]";
			m_cachedMgr->SetDebugName(newDepthTarget.DSV, SRVName.c_str());
#endif

			// register
			newDepthTarget.Texture= textureObj;
			m_DepthTargets.push_back(newDepthTarget);
			newDepthTargetIDs[depthTargetIndex] = static_cast<FDepthTargetID>(m_DepthTargets.size() - 1);
		}
	}

	return newDepthTargetIDs;

}

void Renderer::SetViewport(const uint32 Width, const uint32 Height)
{
	m_PipelineState.ViewPort.TopLeftX = 0;
	m_PipelineState.ViewPort.TopLeftY = 0;
	m_PipelineState.ViewPort.Width = static_cast<float>(Width);
	m_PipelineState.ViewPort.Height = static_cast<float>(Height);
	m_PipelineState.ViewPort.MinDepth = 0;
	m_PipelineState.ViewPort.MaxDepth = 1;
}

void Renderer::SetViewport(const D3D11_VIEWPORT& viewport)
{
	m_PipelineState.ViewPort = viewport;
}

void Renderer::SetShader(FShaderID shaderID, bool bUnbindRenderTargets /*= false*/, bool bUnbindTextures /*= true*/)
{
	assert(shaderID >= 0 && static_cast<size_t>(shaderID) < m_Shaders.size());
	if(m_PipelineState.ShaderID != INDEX_NONE)
	{
		//Already Seted Same Shader
		if(shaderID  != m_PipelineState.ShaderID)
		{
			Shader* shader = m_Shaders[shaderID];

			//Clear Bounded Textures..
			if(bUnbindTextures)
			{
				ClearSRV();
				size_t shaderTexBoundSize = shader->m_TextureBindings.size();

				for(size_t i =0;i<shaderTexBoundSize;i++)
				{
					STextureBinding& bindingTex = shader->m_TextureBindings[i];

					constexpr uint32 NumNullSRV = 12;
					ID3D11ShaderResourceView* nullSRV[NumNullSRV] = { nullptr };					
					switch (bindingTex.ShaderStage)
					{
					case EShaderStage::VS:
						m_deviceContext->VSSetShaderResources(bindingTex.TextureSlot, NumNullSRV, nullSRV);
						break;
					case EShaderStage::GS:
						m_deviceContext->GSSetShaderResources(bindingTex.TextureSlot, NumNullSRV, nullSRV);
						break;
					case EShaderStage::HS:
						m_deviceContext->HSSetShaderResources(bindingTex.TextureSlot, NumNullSRV, nullSRV);
						break;
					case EShaderStage::DS:
						m_deviceContext->DSSetShaderResources(bindingTex.TextureSlot, NumNullSRV, nullSRV);
						break;
					case EShaderStage::PS:
						m_deviceContext->PSSetShaderResources(bindingTex.TextureSlot, NumNullSRV, nullSRV);
						break;
					case EShaderStage::CS:
					{
						ID3D11UnorderedAccessView* nullUAV[NumNullSRV] = { nullptr };
						m_deviceContext->CSSetShaderResources(bindingTex.TextureSlot, NumNullSRV, nullSRV);
						m_deviceContext->CSSetUnorderedAccessViews(bindingTex.TextureSlot, 7, nullUAV, nullptr);
					}	break;
					default:
						break;
					}
				}//Unbind forLoop
			}//if bUnbindTex

			if (bUnbindRenderTargets)
			{
				ID3D11RenderTargetView* nullRTV[6] = { nullptr };
				ID3D11DepthStencilView* nullDSV = { nullptr };
				m_deviceContext->OMSetRenderTargets(6, nullRTV, nullDSV);
				UnbindRenderTargets();	// update the state to reflect the current OM
			}

			const float blendFactor[4] = { 1,1,1,1 };
			m_deviceContext->OMSetBlendState(nullptr, blendFactor, 0xffffffff);

		}//Check Same Shader
	}//Check ID


	if (shaderID != m_PipelineState.ShaderID)
	{
		m_PipelineState.ShaderID= shaderID;
		m_Shaders[shaderID]->ClearConstantBuffers();
	}
}

void Renderer::SetVertexBuffer(FBufferID bufferID)
{
	m_PipelineState.VertexBufferID = bufferID;	
}

void Renderer::SetIndexBuffer(FBufferID bufferID)
{
	m_PipelineState.IndexBufferID = bufferID;
}

void Renderer::SetTexture(const char* texName, FTextureID tex)
{
	SetTexture_(texName, tex, 0);
}

void Renderer::SetRWTexture(const char* texName, FTextureID tex)
{
	assert(tex >= 0);

	const Shader* shader = m_Shaders[m_PipelineState.ShaderID];
	const std::string textureName = std::string(texName);

	const bool bFound = shader->HasTextureBinding(textureName);

	if (bFound)
	{
		SSetTextureCommand cmd(tex, shader->GetTextureBinding(textureName), 0, true);
		m_SetTextureCmds.push(cmd);
	}

#ifdef _DEBUG
	if (!bFound)
	{
		//Log::Error("UnorderedAccessTexture not found: \"%s\" in Shader(Id=%d) \"%s\"", texName, mPipelineState.shader, shader->Name().c_str());
	}
#endif
}

void Renderer::SetTextureArray(const char* texName, FTextureID texArray)
{
	SetTexture(texName, texArray);
}

void Renderer::SetTextureArray(const char* texName, const std::array<FTextureID, TEXTURE_ARRAY_SIZE>& TextureIDs, uint32 numTextures)
{
	const Shader* shader = m_Shaders[m_PipelineState.ShaderID];
	if (shader->HasTextureBinding(texName))
	{
		SSetTextureCommand cmd(TextureIDs, numTextures, shader->GetTextureBinding(texName), 0);
		m_SetTextureCmds.push(cmd);
		
	}

}

void Renderer::SetSamplerState(const char* samplerName, FSamplerID samplerID)
{
	const Shader* shader = m_Shaders[m_PipelineState.ShaderID];
			
	bool bFound = shader->HasSamplerBinding(samplerName);
	

	if (bFound)
	{
		SSetSamplerCommand cmd(samplerID, shader->GetSamplerBinding(samplerName));
		m_SetSamplerCmds.push(cmd);
	}

}

void Renderer::SetRasterizerState(FRasterizerStateID rsStateID)
{
	assert(rsStateID > -1 && static_cast<size_t>(rsStateID) < m_RasterizerStates.size());
	m_PipelineState.RasterizerStateID= rsStateID;
}

void Renderer::SetBlendState(FBlendStateID blendStateID)
{
	assert(blendStateID > -1 && static_cast<size_t>(blendStateID) < m_BlendStates.size());
	m_PipelineState.BlendStateID = blendStateID;
}

void Renderer::SetDepthStencilState(FDepthStencilStateID depthStencilStateID)
{
	assert(depthStencilStateID > -1 && static_cast<size_t>(depthStencilStateID) < m_DepthStencilStates.size());
	m_PipelineState.DepthStencilStateID = depthStencilStateID;
}

void Renderer::SetScissorsRect(int32 left, int32 right, int32 top, int32 bottom)
{
	D3D11_RECT rects[1];
	rects[0].left = left;
	rects[0].right = right;
	rects[0].top = top;
	rects[0].bottom = bottom;

	// Make Command ??
	m_deviceContext->RSSetScissorRects(1, rects);	
	
}

FDepthStencilStateID Renderer::GetDepthStencilState() const
{
	return m_PipelineState.DepthStencilStateID;
}

void Renderer::BindRenderTarget(FRenderTargetID rtvID)
{
	assert(rtvID > -1 && static_cast<size_t>(rtvID) < m_RenderTargets.size());
	//for(RenderTargetID& hRT : m_state._boundRenderTargets) 
	m_PipelineState.RenderTargets = { rtvID };
}

void Renderer::BindDepthTarget(FDepthTargetID dsvID)
{
	assert(dsvID > -1 && static_cast<size_t>(dsvID) < m_DepthTargets.size());
	m_PipelineState.DepthTargetID = dsvID;
}

void Renderer::UnbindRenderTargets()
{
	m_PipelineState.RenderTargets = { INDEX_NONE, INDEX_NONE, INDEX_NONE, INDEX_NONE, INDEX_NONE, INDEX_NONE };
}

void Renderer::UnbindDepthTarget()
{
	m_PipelineState.DepthTargetID = INDEX_NONE;
}

void Renderer::SetConstant4x4f(const char* cName, const XMMATRIX& matrix)
{
	XMFLOAT4X4 m;	XMStoreFloat4x4(&m, matrix);
	float* data = &m.m[0][0];
	SetConstant(cName, data);
}

void Renderer::SetConstant4x4f(const char* cName, const Matrix& matrix)
{
	XMFLOAT4X4 m;	XMStoreFloat4x4(&m, matrix.m);
	float* data = &m.m[0][0];
	SetConstant(cName, data);
}

void Renderer::SetConstant4f(const char* cName, const SVector4& float4)
{
	SetConstant(cName, static_cast<const void*>(float4.Data()));
}

void Renderer::SetConstant3f(const char* cName, const SVector3& float3)
{
	SetConstant(cName, static_cast<const void*>(float3.Data()));
}

void Renderer::SetConstant2f(const char* cName, const SVector2& float2)
{
	SetConstant(cName, static_cast<const void*>(float2.Data()));
}

void Renderer::SetConstant1f(const char* cName, const float& data)
{
	SetConstant(cName, static_cast<const void*>(&data));
}

void Renderer::SetConstant1i(const char* cName, const int32& data)
{
	SetConstant(cName, static_cast<const void*>(&data));
}

void Renderer::SetConstantStruct(const char* cName, const void* data)
{
	SetConstant(cName, data);
}

FBufferID Renderer::CreateRWStructured(const char* cName, FShaderID shaderID, const void* data, uint32 ElemCount, ECPUAccess cpuAccess)
{
	Shader* shader = m_Shaders[shaderID];
	std::string name = cName;
	FBufferID ret = INDEX_NONE;
	size_t layoutSize = shader->m_SBLayouts.size();
	for(size_t i=0;i< layoutSize;i++)
	{
		D3D11_SHADER_BUFFER_DESC varDesc = shader->m_SBLayouts[i].Desc;		

		if (name == varDesc.Name)
		{
			if(shader->m_SBLayouts[i].InputType == D3D_SIT_UAV_RWSTRUCTURED)
			{				
				SBufferDesc desc;
				desc.BufferType = EBufferType::RWSTRUCTED_BUFFER;
				desc.ElementCount = ElemCount;
				desc.MiscFlag = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
				desc.Stride = shader->m_SBLayouts[i].BuffSize;
				desc.Usage = EBufferUsage::GPU_READ_WRITE;
				desc.CPUAccess = cpuAccess;
				ret = CreateBuffer(desc, data, name.c_str());
			}
		}
	}	

	return ret;
}

void Renderer::SetStructured(const char* cName, const void* data, size_t Size)
{
	SetStructured_(cName, data, Size);
}



void Renderer::SetRWStructured(FBufferID bufferID, uint32 ElemCount)
{	
	size_t sbLayoutSize = m_Shaders[m_PipelineState.ShaderID]->m_SBLayouts.size();
	for(size_t i=0;i<sbLayoutSize;i++)
	{
		auto& SB = m_Shaders[m_PipelineState.ShaderID]->m_SBLayouts[i];

		if(SB.InputType == D3D11_SIT_UAV_RWSTRUCTURED)
		{
			if(m_UABuffers[bufferID].Name == SB.Desc.Name)
			{
				if(m_UABuffers[bufferID].Desc.ElementCount < ElemCount)
				{
					//ReSizing Buffer..
					m_UABuffers[bufferID].Resize(m_device, ElemCount);					
				}


				SRWBinding rwBinding;
				rwBinding.RWSlot = SB.BufSlot;
				rwBinding.ShaderStage = SB.Stage;
				SSetRWBufferCommand cmd(bufferID, rwBinding);
				m_SetRWCmds.push(cmd);

				//m_UABuffers[bufferID].Desc.ElementCount 
			}
		}

	}
	
	
}
//이쉐이더에 있는 
void Renderer::SetStructured(const char* cName, FBufferID bufferID)
{
	size_t sbLayoutSize = m_Shaders[m_PipelineState.ShaderID]->m_SBLayouts.size();
	std::string valName = cName;

	for (size_t i = 0; i < sbLayoutSize; i++)
	{
		auto& SB = m_Shaders[m_PipelineState.ShaderID]->m_SBLayouts[i];

		//if (SB.InputType == D3D11_SIT_UAV_RWSTRUCTURED)
		{
			if (valName == SB.Desc.Name)
			{
				SRWBinding rwBinding;
				rwBinding.RWSlot = SB.BufSlot;
				rwBinding.ShaderStage = SB.Stage;
				SSetRWBufferCommand cmd(bufferID, rwBinding);
				m_SetRWCmds.push(cmd);
			}
		}

	}
}



void Renderer::SetTextureFromArraySlice(const char* texName, FTextureID texArray, uint32 slice)
{
	SetTexture_(texName, texArray, slice);
}

void Renderer::BeginRender(const SClearCommand& clearCmd)
{
	if(clearCmd.bDoClearColor)
	{
		size_t rtSize = m_PipelineState.RenderTargets.size();
		for(size_t i=0;i<rtSize;i++)
		{
			FRenderTargetID rtID = m_PipelineState.RenderTargets[i];
			if( rtID >=0 )
			{
				m_deviceContext->ClearRenderTargetView(m_RenderTargets[rtID].RTV, clearCmd.ClearColor.Data());
			}			
		}
	}

	const bool bClearDepthStencil = clearCmd.bDoClearDepth || clearCmd.bDoClearStencil;
	if (bClearDepthStencil)
	{
		const FDepthTargetID dsv = m_PipelineState.DepthTargetID;

		UINT clearFlag = 0;

		if (clearCmd.bDoClearDepth)
			clearFlag |= D3D11_CLEAR_DEPTH;

		if (clearCmd.bDoClearStencil)
			clearFlag |= D3D11_CLEAR_STENCIL;


		if (dsv >= 0)	
			m_deviceContext->ClearDepthStencilView(m_DepthTargets[dsv].DSV, clearFlag, clearCmd.ClearDepth, clearCmd.ClearStencil);
		
	}
}

void Renderer::EndFrame()
{
	m_cachedMgr->EndFrame();
}

void Renderer::ResetPipelineState()
{
	m_PipelineState.ShaderID = INDEX_NONE;
	
}

void Renderer::UpdateVertexBuffer(FBufferID buffer, const void* pData)
{
	assert(buffer >= 0 && buffer < m_VertexBuffers.size());
	m_VertexBuffers[buffer].Update(m_deviceContext, pData);
}

void Renderer::UpdateIndexBuffer(FBufferID buffer, const void* pData)
{
	assert(buffer >= 0 && buffer < m_IndexBuffers.size());
	m_IndexBuffers[buffer].Update(m_deviceContext, pData);
}

void Renderer::UpdateBuffer(FBufferID buffer, const void* pData)
{
	assert(buffer >= 0 && buffer < m_UABuffers.size());
	m_UABuffers[buffer].Update(m_deviceContext, pData);
}

void Renderer::ReadBuffer(FBufferID buffer, void*& pOutData)
{
	assert(buffer >= 0 && buffer < m_UABuffers.size());
	m_UABuffers[buffer].ReadBuffer(m_deviceContext, pOutData);
}

void Renderer::Apply()
{

	const bool bShaderChanged =					m_PipelineState.ShaderID			!= m_PrevPipelineState.ShaderID;
	const bool bVertexBufferChanged =			m_PipelineState.VertexBufferID		!= m_PrevPipelineState.VertexBufferID;
	const bool bIndexBufferChanged =			m_PipelineState.IndexBufferID		!= m_PrevPipelineState.IndexBufferID;
	const bool bRasterizerStateChanged =		m_PipelineState.RasterizerStateID	!= m_PrevPipelineState.RasterizerStateID;
	const bool bViewPortChanged =				m_PipelineState.ViewPort			!= m_PrevPipelineState.ViewPort;
	const bool bDepthStencilStateChanged =		m_PipelineState.DepthStencilStateID != m_PrevPipelineState.DepthStencilStateID;
	const bool bBlendStateChanged =				m_PipelineState.BlendStateID		!= m_PrevPipelineState.BlendStateID;
	const bool bDepthTargetChanged =			m_PipelineState.DepthTargetID		!= m_PrevPipelineState.DepthTargetID;
	const bool bRenderTargetChanged = [&]()
	{
		const FRenderTargetIDs& RTVs_curr = m_PipelineState.RenderTargets;
		const FRenderTargetIDs& RTVs_prev = m_PrevPipelineState.RenderTargets;
		if (RTVs_curr.size() != RTVs_prev.size())
			return true;		
		return !std::equal(std::begin(RTVs_curr), std::end(RTVs_curr), RTVs_prev.begin());
	}();

	Shader* shader = m_PipelineState.ShaderID>= 0 ? m_Shaders[m_PipelineState.ShaderID] : nullptr;
	if (!shader)
	{
		//Log::Error("Renderer::Apply() : Shader null...\n");		
		m_deviceContext->OMSetRenderTargets(1, &m_RenderTargets[0].RTV, nullptr);
		m_PrevPipelineState = m_PipelineState;		
		return;
	}


	// INPUT ASSEMBLY
	// ----------------------------------------
	const bool bVBufferValid = m_PipelineState.VertexBufferID != INDEX_NONE;
	const bool bIBufferValid = m_PipelineState.IndexBufferID != INDEX_NONE;	

	//const SD3DBuffer& VertexBuffer = m_VertexBuffers[m_PipelineState.VertexBufferID];
	//const SD3DBuffer& IndexBuffer = m_IndexBuffers[m_PipelineState.IndexBufferID];


	uint32 stride = 0;
	uint32 offset = 0;

	if (bVBufferValid && bVertexBufferChanged) 
	{ 
		const SD3DBuffer& VertexBuffer = m_VertexBuffers[m_PipelineState.VertexBufferID];
		stride = VertexBuffer.Desc.Stride;
		m_deviceContext->IASetVertexBuffers(0, 1, &(VertexBuffer.GPUData), &stride, &offset);
	}
	if (bIBufferValid && bIndexBufferChanged) 
	{ 
		const SD3DBuffer& IndexBuffer = m_IndexBuffers[m_PipelineState.IndexBufferID];
		m_deviceContext->IASetIndexBuffer(IndexBuffer.GPUData, DXGI_FORMAT_R32_UINT, 0);/////////////////////////////////////////////  INDEX 32
	}


	// SHADER STAGES
	// ----------------------------------------
	if (bShaderChanged)
	{
		ID3D11ClassInstance* const* pClassInstance = nullptr;
		m_deviceContext->IASetInputLayout(shader->m_InputLayout);
		m_deviceContext->VSSetShader(shader->m_Stages.VertexShader, pClassInstance, 0);
		m_deviceContext->PSSetShader(shader->m_Stages.PixelShader, pClassInstance, 0);
		m_deviceContext->GSSetShader(shader->m_Stages.GeometryShader, pClassInstance, 0);
		m_deviceContext->HSSetShader(shader->m_Stages.HullShader, pClassInstance, 0);
		m_deviceContext->DSSetShader(shader->m_Stages.DomainShader, pClassInstance, 0);
		m_deviceContext->CSSetShader(shader->m_Stages.ComputeShader, pClassInstance, 0);	
		
	}


	// CONSTANT BUFFERS & SHADER RESOURCES
	// ----------------------------------------
	shader->UpdateConstants(m_deviceContext);




	//StructedBuffer -> Resize 시에 Device 필요
	shader->UpdateStructed(m_device,m_deviceContext);
	//RW Structed 는 

	

	while (m_SetSamplerCmds.size() > 0)
	{
		SSetSamplerCommand& cmd = m_SetSamplerCmds.front();
		cmd.SetResource(this);
		m_SetSamplerCmds.pop();
	}

	while (m_SetTextureCmds.size() > 0)
	{
		SSetTextureCommand& cmd = m_SetTextureCmds.front();
		cmd.SetResource(this);
		m_SetTextureCmds.pop();
	}

	while (m_SetRWCmds.size() > 0)
	{
		SSetRWBufferCommand& cmd = m_SetRWCmds.front();
		cmd.SetResource(this);
		m_SetRWCmds.pop();
	}


	// RASTERIZER
	// ----------------------------------------
	if (bViewPortChanged) 
	{
		m_deviceContext->RSSetViewports(1, &m_PipelineState.ViewPort);
	}
	if (bRasterizerStateChanged) 
	{ 
		m_deviceContext->RSSetState(m_RasterizerStates[m_PipelineState.RasterizerStateID]); 
	}



	// OUTPUT MERGER
	// ----------------------------------------
	if (bBlendStateChanged) 
	{ 
		m_deviceContext->OMSetBlendState(m_BlendStates[m_PipelineState.BlendStateID], nullptr, 0xffffffff);
	}
	if (bDepthStencilStateChanged) 
	{
		m_deviceContext->OMSetDepthStencilState(m_DepthStencilStates[m_PipelineState.DepthStencilStateID], 0);
	}

	// get the bound render target addresses
	const int32 indexRTV = m_PipelineState.RenderTargets[0];

	static std::array<ID3D11RenderTargetView*, 7> RTVs;
	UINT numRTV = 0;
	//Make RTVs
	const bool bAllNullptr = [&]()
	{
		bool bAllNull = true;
		size_t rtSize = m_PipelineState.RenderTargets.size();
		for(size_t i =0;i<rtSize;i++)
		{
			FRenderTargetID rtID = m_PipelineState.RenderTargets[i];
			if(rtID>=0)
			{
				RTVs[numRTV++] = m_RenderTargets[rtID].RTV;
				bAllNull = false;
			}

		}	
		return bAllNull;
	}();


	ID3D11RenderTargetView** RTV = bAllNullptr ? nullptr : &RTVs[0];

	ID3D11DepthStencilView* DSV = m_PipelineState.DepthTargetID == INDEX_NONE ? nullptr : m_DepthTargets[m_PipelineState.DepthTargetID].DSV;

	//if(bRenderTargetChanged || bDepthStencilStateChanged) //#TODO: 
	// currently need bRenderTargetChanged for unbindRenderTargets + apply

	if (RTV || bRenderTargetChanged || (DSV && bDepthTargetChanged))
	{
		m_deviceContext->OMSetRenderTargets(numRTV, RTV, DSV);
	}

	m_PrevPipelineState = m_PipelineState;
}

void Renderer::ClearSRV()
{
	Shader* shader = m_Shaders[m_PipelineState.ShaderID];
	constexpr uint32 NumNullSRV = 12;
	ID3D11ShaderResourceView* nullSRV[NumNullSRV] = { nullptr };
	
		
	for (size_t i = 0; i < shader->m_SBLayouts.size(); i++)
	{
		m_deviceContext->CSSetShaderResources(shader->m_SBLayouts[i].BufSlot, NumNullSRV, nullSRV);
		m_deviceContext->VSSetShaderResources(shader->m_SBLayouts[i].BufSlot, NumNullSRV, nullSRV);
		m_deviceContext->GSSetShaderResources(shader->m_SBLayouts[i].BufSlot, NumNullSRV, nullSRV);
		m_deviceContext->HSSetShaderResources(shader->m_SBLayouts[i].BufSlot, NumNullSRV, nullSRV);
		m_deviceContext->DSSetShaderResources(shader->m_SBLayouts[i].BufSlot, NumNullSRV, nullSRV);
		m_deviceContext->PSSetShaderResources(shader->m_SBLayouts[i].BufSlot, NumNullSRV, nullSRV);
		
		ID3D11UnorderedAccessView* nullUAV[12] = { nullptr };
		m_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		m_deviceContext->CSSetUnorderedAccessViews(1, 1, nullUAV, nullptr);
		m_deviceContext->CSSetUnorderedAccessViews(2, 1, nullUAV, nullptr);
		m_deviceContext->CSSetUnorderedAccessViews(3, 1, nullUAV, nullptr);
		m_deviceContext->CSSetUnorderedAccessViews(4, 1, nullUAV, nullptr);
		m_deviceContext->CSSetUnorderedAccessViews(5, 1, nullUAV, nullptr);
		m_deviceContext->CSSetUnorderedAccessViews(6, 1, nullUAV, nullptr);
		m_deviceContext->CSSetUnorderedAccessViews(7, 1, nullUAV, nullptr);
		
		switch (shader->m_SBLayouts[i].Stage)
		{
		case EShaderStage::VS:
			m_deviceContext->VSSetShaderResources(shader->m_SBLayouts[i].BufSlot, NumNullSRV, nullSRV);
			break;
		case EShaderStage::GS:
			m_deviceContext->GSSetShaderResources(shader->m_SBLayouts[i].BufSlot, NumNullSRV, nullSRV);
			break;
		case EShaderStage::HS:
			m_deviceContext->HSSetShaderResources(shader->m_SBLayouts[i].BufSlot, NumNullSRV, nullSRV);
			break;
		case EShaderStage::DS:
			m_deviceContext->DSSetShaderResources(shader->m_SBLayouts[i].BufSlot, NumNullSRV, nullSRV);
			break;
		case EShaderStage::PS:
			m_deviceContext->PSSetShaderResources(shader->m_SBLayouts[i].BufSlot, NumNullSRV, nullSRV);
			break;
		case EShaderStage::CS:
		{
			ID3D11UnorderedAccessView* nullUAV[12] = { nullptr };
			m_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
			m_deviceContext->CSSetUnorderedAccessViews(1, 1, nullUAV, nullptr);
			m_deviceContext->CSSetUnorderedAccessViews(2, 1, nullUAV, nullptr);
			m_deviceContext->CSSetUnorderedAccessViews(3, 1, nullUAV, nullptr);
			m_deviceContext->CSSetUnorderedAccessViews(4, 1, nullUAV, nullptr);
			m_deviceContext->CSSetUnorderedAccessViews(5, 1, nullUAV, nullptr);
			m_deviceContext->CSSetUnorderedAccessViews(6, 1, nullUAV, nullptr);
			m_deviceContext->CSSetUnorderedAccessViews(7, 1, nullUAV, nullptr);
			m_deviceContext->CSSetShaderResources(shader->m_SBLayouts[i].BufSlot, NumNullSRV, nullSRV);
		}	break;
		default:
			break;
		}
	}
}

void Renderer::ForceClearSrv()
{

	for (size_t i = 0; i < 12; i++)
	{
		constexpr uint32 NumNullSRV = 12;
		ID3D11ShaderResourceView* nullSRV[NumNullSRV] = { nullptr };

		m_deviceContext->GSSetShaderResources((uint32)i, NumNullSRV, nullSRV);
		m_deviceContext->HSSetShaderResources((uint32)i, NumNullSRV, nullSRV);
		m_deviceContext->DSSetShaderResources((uint32)i, NumNullSRV, nullSRV);
		m_deviceContext->PSSetShaderResources((uint32)i, NumNullSRV, nullSRV);
		m_deviceContext->VSSetShaderResources((uint32)i, NumNullSRV, nullSRV);
	}


}

void Renderer::DrawIndexed(EPrimitiveTopology topology /*= EPrimitiveTopology::TRIANGLE_LIST*/)
{
	const SD3DBuffer& VertexBuffer = m_VertexBuffers[m_PipelineState.VertexBufferID];
	const SD3DBuffer& IndexBuffer = m_IndexBuffers[m_PipelineState.IndexBufferID];

	const uint32 numIndices = IndexBuffer.Desc.ElementCount;
	const uint32 numVertices = VertexBuffer.Desc.ElementCount;

	m_PipelineState.Topology = topology;
	if (m_PipelineState.Topology != m_PrevPipelineState.Topology)
	{
		m_deviceContext->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(topology));
	}

	m_deviceContext->DrawIndexed(numIndices, 0, 0);
}

void Renderer::DrawIndexedInstanced(int32 instanceCount, EPrimitiveTopology topology /*= EPrimitiveTopology::TRIANGLE_LIST*/)
{
	const SD3DBuffer& VertexBuffer = m_VertexBuffers[m_PipelineState.VertexBufferID];
	const SD3DBuffer& IndexBuffer = m_IndexBuffers[m_PipelineState.IndexBufferID];

	const uint32 numIndices = IndexBuffer.Desc.ElementCount;
	const uint32 numVertices = VertexBuffer.Desc.ElementCount;

	m_PipelineState.Topology = topology;
	if (m_PipelineState.Topology != m_PrevPipelineState.Topology)
	{
		m_deviceContext->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(topology));
	}

	m_deviceContext->DrawIndexedInstanced(numIndices, instanceCount, 0, 0, 0);
}
	
void Renderer::Draw(int32 vertCount, EPrimitiveTopology topology /*= EPrimitiveTopology::POINT_LIST*/)
{	
	m_PipelineState.Topology = topology;
	if (m_PipelineState.Topology != m_PrevPipelineState.Topology)
	{
		m_deviceContext->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(topology));
	}
	m_deviceContext->Draw(vertCount, 0);
}

void Renderer::DrawVtxOffset(uint32 vertCount, uint32 Offset, EPrimitiveTopology topology /*= EPrimitiveTopology::TRIANGLE_LIST*/)
{
	m_PipelineState.Topology = topology;
	if (m_PipelineState.Topology != m_PrevPipelineState.Topology)
	{
		m_deviceContext->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(topology));
	}
	m_deviceContext->Draw(vertCount, Offset);
}

void Renderer::DrawInstanced(int32 vertCount, int32 instanceCount, EPrimitiveTopology topology /*= EPrimitiveTopology::POINT_LIST*/)
{
	m_PipelineState.Topology = topology;
	if (m_PipelineState.Topology != m_PrevPipelineState.Topology)
	{
		m_deviceContext->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(topology));
	}
	m_deviceContext->DrawInstanced(vertCount, instanceCount,0,0);
}

void Renderer::Dispatch(int32 x, int32 y, int32 z)
{
	m_deviceContext->Dispatch(x, y, z);
	
}


//void Renderer::DrawLine(const SVector3& pos1, const SVector3& pos2, const SLinearColor& color)
//{	
//	SetConstant3f("p1", pos1);
//	SetConstant3f("p2", pos2);
//	SetConstant3f("color", color.ToVector3());
//	Apply();
//	Draw(1, EPrimitiveTopology::POINT_LIST);	
//}

class Mesh Renderer::GetBuiltInMesh(EGeometry builtInMesh) const
{
	return m_BuiltInMeshs[(int32)builtInMesh];
}

void Renderer::SetConstant(const char* cName, const void* data)
{
	Shader* shader = m_Shaders[m_PipelineState.ShaderID];	

	size_t cpuCBufferSize = shader->m_CPUConstantBuffers.size();
	
	const std::vector<SConstantBufferBinding>&			cbBindings		    = shader->GetConstantBuffers();	
	const std::vector<SCPUConstant>&					cbCPUSizeBuffer    = shader->GetCPUConstantBuffers();
	
	

	for (size_t i = 0; i < cpuCBufferSize; i++)
	{				
		if (shader->m_CPUConstantBuffers[i].Name == cName)
		{
			//어떤 버퍼에 바인딩 되었는가
			int32 t = shader->m_Constants[i].first;
			int32 t2 = shader->m_Constants[i].second;
			const size_t GPUcBufferSlot = t;

			
			const SCPUConstant& c = cbCPUSizeBuffer[i];


			size_t gpuside = shader->m_ConstantBuffers.size();
			for(size_t j = 0;j< gpuside;j++)
			{
				if(shader->m_ConstantBuffers[j].BufferSlot == GPUcBufferSlot)
				{
					if (::memcmp(c.Data, data, c.Size) != 0)	// copy data if its not the same
					{
						::memcpy(c.Data, data, c.Size);
						shader->m_ConstantBuffers[j].Dirty = true;						
						return;
					}
					
				}
			}
	

		}
	}

}

void Renderer::SetTexture_(const char* texName, FTextureID tex, uint32 slice /*= 0 /* only for texture arrays */)
{
	if (tex == INDEX_NONE)
		return;

	const Shader* shader = m_Shaders[m_PipelineState.ShaderID];
	

	const bool bFound = shader->HasTextureBinding(texName);

	if (bFound)
	{
		SSetTextureCommand cmd(tex, shader->GetTextureBinding(texName), slice);
		m_SetTextureCmds.push(cmd);
	}

#ifdef _DEBUG
	if (!bFound)
	{
		//Log::Error("Texture not found: \"%s\" in Shader(Id=%d) \"%s\"", texName, mPipelineState.shader, shader->Name().c_str());
	}
#endif

}

void Renderer::SetStructured_(const char* cName, const void* data, size_t Size)
{
	if (Size <= 0)
	{
		assert(false);
		return;
	}
	Shader* shader = m_Shaders[m_PipelineState.ShaderID];

	size_t cpuCBufferSize = shader->m_StructedBuffers.size();
	
	const std::vector<SStructedBufferBinding>& sbBindings = shader->GetStructedBuffers();
	
	std::vector<SCPUStructed>& cbCPUSizeBuffer = shader->m_CPUStructedBuffers;


	for (size_t i = 0; i < cpuCBufferSize; i++)
	{		
		if (shader->m_CPUStructedBuffers[i].Name == cName)
		{
			const size_t GPUcBufferSlot = shader->m_StructedBuffers[i].BufferSlot;		
			SCPUStructed& c = cbCPUSizeBuffer[i];
			bool resize = false;
			if (Size > c.Size)
			{
				resize  = true;
				shader->m_StructedBuffers[i].Resize = Size;	//Send NewSize
				c.Size = Size;
				SAFE_DELETE_ARRAY(c.Data);
				c.Data = new char[c.Size];
				::memset(c.Data, 0, c.Size);
			}
			else
			{
				resize = shader->m_StructedBuffers[i].Resize = 0;
			}

			if (::memcmp(c.Data, data, Size) != 0 || resize)	// copy data if its not the same
			{
				::memcpy(c.Data, data, Size);
				shader->m_StructedBuffers[i].Dirty = true;
			}
		

		}
	}
}
