#include "EnginePCH.h"
#include "D3DTexture.h"
#include "Renderer.h"

D3DTexture::D3DTexture()
	:m_SRV(nullptr)
	,m_UAV(nullptr)
	,m_Tex2D(nullptr)
	,m_Width(0)
	,m_Height(0)
	,m_Depth(0)
	,m_Name("")
	,m_ID(INDEX_NONE)
{

}

D3DTexture::~D3DTexture()
{

}

template<class T>
void ReleaseResources(std::vector<T*>& resources, T*& pResourceView)
{
	if (resources.size() > 0)
	{
		for (size_t i = 0; i < resources.size(); ++i)
		{
			SAFE_RELEASE(resources[i]);	
		}
	}
	else
	{
		SAFE_RELEASE(pResourceView);
	}
}


bool D3DTexture::InitializeTexture2D(const D3D11_TEXTURE2D_DESC& descriptor, class Renderer* pRenderer, bool initializeSRV)
{
	if(!pRenderer)
	{
		return false;
	}

	HRESULT hr = pRenderer->GetDeivce()->CreateTexture2D(&descriptor, nullptr, &m_Tex2D);
#if defined(_DEBUG)
	if (!m_Name.empty())
	{
		m_Tex2D->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(m_Name.length()), m_Name.c_str());		
	}
#endif

	if (!SUCCEEDED(hr))
	{
		//Log::Error("Texture::InitializeTexture2D(): Cannot create texture2D");
		return false;
	}
	m_Width = descriptor.Width;
	m_Height = descriptor.Height;

	if (initializeSRV)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = descriptor.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;	// array maybe? check descriptor.
		srvDesc.Texture2D.MipLevels = descriptor.MipLevels;
		pRenderer->GetDeivce()->CreateShaderResourceView(m_Tex2D, &srvDesc, &m_SRV);

#if defined(_DEBUG)
		if (!m_Name.empty())
		{
			m_SRV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(m_Name.length()), m_Name.c_str());
		}
#endif
	}
	return true;
}

void D3DTexture::Release()
{
	ReleaseResources(m_SRVArray, m_SRV);
	ReleaseResources(m_UAVArray, m_UAV);
	SAFE_RELEASE(m_Tex2D);	
	m_Width = m_Height= m_Depth= 0;
	m_Name= "";
	m_ID = INDEX_NONE;	
}

Matrix D3DTexture::CubemapUtility::CalculateViewMatrix(ECubeMapLookDirections cubeFace, const SVector3& position /*= SVector3(0.0f)*/)
{
	// cube face order : https://msdn.microsoft.com/en-us/library/windows/desktop/ff476906(v=vs.85).aspx
	// //------------------------------------------------------------------------------------------------------
	// // 0: RIGHT		1: LEFT
	// // 2: UP		3: DOWN
	// // 4: FRONT		5: BACK
	//------------------------------------------------------------------------------------------------------
	
	switch (cubeFace)
	{
	case D3DTexture::CubemapUtility::CUBEMAP_LOOK_RIGHT:
		return XMMatrixLookAtLH(position.Convert(), (position + SVector3::RightVector).Convert(), SVector3::UpVector.Convert());
	case D3DTexture::CubemapUtility::CUBEMAP_LOOK_LEFT:	
		return XMMatrixLookAtLH(position.Convert(), (position + SVector3::LeftVector).Convert(), SVector3::UpVector.Convert());
	case D3DTexture::CubemapUtility::CUBEMAP_LOOK_UP:
		return XMMatrixLookAtLH(position.Convert(), (position + SVector3::UpVector).Convert(), SVector3::BackwardVector.Convert());
	case D3DTexture::CubemapUtility::CUBEMAP_LOOK_DOWN:
		return XMMatrixLookAtLH(position.Convert(), (position + SVector3::DownVector).Convert(), SVector3::ForwardVector.Convert());
	case D3DTexture::CubemapUtility::CUBEMAP_LOOK_FRONT:
		return XMMatrixLookAtLH(position.Convert(), (position + SVector3::ForwardVector).Convert(), SVector3::UpVector.Convert());
	case D3DTexture::CubemapUtility::CUBEMAP_LOOK_BACK:
		return XMMatrixLookAtLH(position.Convert(), (position + SVector3::BackwardVector).Convert(), SVector3::UpVector.Convert());
	default: 
		return Matrix::StaticIdentity();
	}

	return Matrix::StaticIdentity();

}
