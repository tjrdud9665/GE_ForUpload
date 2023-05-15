#pragma once

//Use Vertex Buffer 
// User index Buffer
struct SD3DBuffer
{
	bool Initialize(const std::string& InName,ID3D11Device* device = nullptr, const void* pData = nullptr);

	bool Resize(ID3D11Device* device, size_t ElemCount);


	void CleanUp();
	void Update(ID3D11DeviceContext* Context, const void* pData);	

	template<typename DataType>
	void UpdateNoOverwrite(ID3D11DeviceContext* Context, const void* pData,const uint32 count, uint32 Postion)
	{
		if (bInitialized)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource = {};

			uint32 Subresource = 0;
			uint32 MapFlags = 0;
			/*const uint32 Size = Desc.Stride * Desc.ElementCount;*/

			Context->Map(GPUData, Subresource, D3D11_MAP_WRITE_NO_OVERWRITE, MapFlags, &mappedResource);
			//::memcpy(mappedResource.pData, pData, Size);
			memcpy(&((DataType*)mappedResource.pData)[Postion], pData, count * sizeof(DataType));
			Context->Unmap(GPUData, Subresource);
		} else
		{
			//Not initialized..
		}
	}

	void ReadBuffer(ID3D11DeviceContext* Context, void*& pOutData);
	

	SD3DBuffer(const SBufferDesc& desc);

	void*			CPUData;
	ID3D11Buffer*	GPUData;
	bool			bInitialized;
	SBufferDesc		Desc;

	ID3D11ShaderResourceView* Srv;	//Maybe For Output Only
	ID3D11UnorderedAccessView* Uav;


	//Debug
	std::string		Name;
};

