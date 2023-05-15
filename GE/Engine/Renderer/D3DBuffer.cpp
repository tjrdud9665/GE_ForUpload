#include "EnginePCH.h"
#include "D3DBuffer.h"

bool SD3DBuffer::Initialize(const std::string& InName,ID3D11Device* device /*= nullptr*/, const void* pData /*= nullptr*/)
{
	Name = InName;
	
	// GPU BUFFER
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.Usage = static_cast<D3D11_USAGE>(Desc.Usage);
	bufDesc.BindFlags = static_cast<D3D11_BIND_FLAG>(Desc.BufferType);
	bufDesc.ByteWidth = Desc.Stride * Desc.ElementCount;	

	bufDesc.CPUAccessFlags = Desc.CPUAccess;
	
	bufDesc.MiscFlags = Desc.MiscFlag;

	bufDesc.StructureByteStride = Desc.Stride;

	D3D11_SUBRESOURCE_DATA* pBufData = nullptr;
	D3D11_SUBRESOURCE_DATA bufData = {};
	if (pData)
	{
		bufData.pSysMem = pData;
		bufData.SysMemPitch = 0;		// irrelevant for non-texture sub-resources
		bufData.SysMemSlicePitch = 0;	// irrelevant for non-texture sub-resources
		pBufData = &bufData;

		CPUData = malloc(bufDesc.ByteWidth);
		::memcpy(CPUData, pData, bufDesc.ByteWidth);
	}

	HRESULT hr = device->CreateBuffer(&bufDesc, pBufData, &GPUData);
	if (FAILED(hr))
	{
		return false;
	}

#if defined(_DEBUG)	
	if (GPUData)
	{
		GPUData->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(Name.length()), Name.c_str());
	}
#endif

	//Create ShderResouceView
	if(bufDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC	SRVDesc = {};

		SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		SRVDesc.BufferEx.FirstElement = 0;
		SRVDesc.BufferEx.Flags = 0;
		SRVDesc.BufferEx.NumElements = Desc.ElementCount;

		if (FAILED(device->CreateShaderResourceView(GPUData, &SRVDesc, &Srv)))
		{
			::OutputDebugStringA("Error creating SRV!");
			return false;
		}
	}
	if(bufDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC	UAVDesc = {};
		UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
		UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		UAVDesc.Buffer.FirstElement = 0;
		UAVDesc.Buffer.Flags = 0;
		UAVDesc.Buffer.NumElements = Desc.ElementCount;

		if (FAILED(device->CreateUnorderedAccessView(GPUData, &UAVDesc, &Uav)))
		{
			::OutputDebugStringA("Error creating UnorderedAccessView!");
			return false;
		}
	}


	bInitialized = true;
	return true;
}

bool SD3DBuffer::Resize(ID3D11Device* device, size_t ElemCount)
{
	CleanUp();
	Desc.ElementCount = (uint32)ElemCount;
	return Initialize(Name, device, nullptr);	
}

void SD3DBuffer::CleanUp()
{
	SAFE_RELEASE(GPUData);
	SAFE_DELETE(CPUData);

	SAFE_RELEASE(Srv);
	SAFE_RELEASE(Uav);

	bInitialized = false;
}

void SD3DBuffer::Update(ID3D11DeviceContext* Context, const void* pData)
{	
	if (bInitialized)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};

		uint32 Subresource = 0;
		uint32 MapFlags = 0;
		const uint32 Size = Desc.Stride * Desc.ElementCount;

		Context->Map(GPUData, Subresource, D3D11_MAP_WRITE_DISCARD, MapFlags, &mappedResource);
		::memcpy(mappedResource.pData, pData, Size);
		Context->Unmap(GPUData, Subresource);
	}
	else
	{
		//Not initialized..
	}	

}





void SD3DBuffer::ReadBuffer(ID3D11DeviceContext* Context, void*& pOutData)
{
	
	if (bInitialized && Desc.CPUAccess == CPU_R)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};	

		Context->Map(GPUData, 0, D3D11_MAP_READ, 0, &mappedResource);
		::memcpy(pOutData, mappedResource.pData, (size_t)(Desc.ElementCount *Desc.Stride));
		//::memcpy(mappedResource.pData, pData, Size);
		Context->Unmap(GPUData, 0);
	} 
}

SD3DBuffer::SD3DBuffer(const SBufferDesc& desc)
	: Desc(desc)
	, CPUData(nullptr)
	, GPUData(nullptr)
	, bInitialized(false)
	, Srv(nullptr)
	, Uav(nullptr)
{

}
