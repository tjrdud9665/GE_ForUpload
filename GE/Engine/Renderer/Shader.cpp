#include "EnginePCH.h"
#include "Shader.h"
#include "../Path/PathManager.h"
#include <D3D/D3DManager.h>

#define DEFAULT_STRUCTED_COUNT 32



static void(__cdecl ID3D11DeviceContext::* SetShaderConstants[EShaderStage::COUNT])
(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) =
{
	&ID3D11DeviceContext::VSSetConstantBuffers,
	&ID3D11DeviceContext::GSSetConstantBuffers,
	&ID3D11DeviceContext::DSSetConstantBuffers,
	&ID3D11DeviceContext::HSSetConstantBuffers,
	&ID3D11DeviceContext::PSSetConstantBuffers,
	&ID3D11DeviceContext::CSSetConstantBuffers,
}; 

static const std::unordered_map<EShaderStage, const char*> SHADER_COMPILER_VERSION_LOOKUP =
{
	{ EShaderStage::VS, "vs_5_0" },
	{ EShaderStage::GS, "gs_5_0" },
	{ EShaderStage::DS, "ds_5_0" },
	{ EShaderStage::HS, "hs_5_0" },
	{ EShaderStage::PS, "ps_5_0" },
	{ EShaderStage::CS, "cs_5_0" },
};

static std::unordered_map <std::string, EShaderStage > s_ShaderTypeStrLookup =
{
	{"vs", EShaderStage::VS},
	{"gs", EShaderStage::GS},
	{"ds", EShaderStage::DS},
	{"hs", EShaderStage::HS},
	{"cs", EShaderStage::CS},
	{"ps", EShaderStage::PS}
};



static const std::unordered_map<EShaderStage, const char*> SHADER_ENTRY_POINT_LOOKUP =
{
	{ EShaderStage::VS, "VSMain" },
	{ EShaderStage::GS, "GSMain" },
	{ EShaderStage::DS, "DSMain" },
	{ EShaderStage::HS, "HSMain" },
	{ EShaderStage::PS, "PSMain" },
	{ EShaderStage::CS, "CSMain" },
};

ID3DInclude* const SHADER_INCLUDE_HANDLER = D3D_COMPILE_STANDARD_FILE_INCLUDE;

#if defined( _DEBUG ) 
const UINT SHADER_COMPILE_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
const UINT SHADER_COMPILE_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS;
#endif

std::string GetCompileError(ID3D10Blob*& errorMessage, const std::string& shdPath)
{
	if (errorMessage)
	{
		char* compileErrors = (char*)errorMessage->GetBufferPointer();
		size_t bufferSize = errorMessage->GetBufferSize();

		std::stringstream ss;
		for (unsigned int i = 0; i < bufferSize; ++i)
		{
			ss << compileErrors[i];
		}
		errorMessage->Release();
		return ss.str();
	} else
	{
		//Log::Error(shdPath);
		return ("Error: " + shdPath);
	}
}

bool MakeShaderFilePath(WCHAR (&OutFullPath)[MAX_PATH],  const std::string& InFileName)
{
	//WCHAR	FullPath[MAX_PATH] = {};
	const SPathInfo* Info = g_Engine->Get<PathManager>()->FindPath(SHADER_PATH);

	if (Info)
		::lstrcpyW(OutFullPath, Info->wPath.c_str());
	else
		return false;

	
	::lstrcatW(OutFullPath, StringToWString(InFileName).c_str());

	return true;
}


SShaderDesc::ShaderStageArr SShaderDesc::CreateStageDescsFromShaderName(const char* pShaderName, uint32 flagStages)
{
	const std::string shaderName = pShaderName;
	std::array<SShaderStageDesc, EShaderStageFlags::SHADER_STAGE_COUNT> descs;
	int32 idx = 0;
	if (flagStages & SHADER_STAGE_VS)
	{
		descs[idx++] = SShaderStageDesc{ shaderName + "_vs.hlsl", {} };
	}
	if (flagStages & SHADER_STAGE_GS)
	{
		descs[idx++] = SShaderStageDesc{ shaderName + "_gs.hlsl", {} };
	}
	if (flagStages & SHADER_STAGE_DS)
	{
		descs[idx++] = SShaderStageDesc{ shaderName + "_ds.hlsl", {} };
	}
	if (flagStages & SHADER_STAGE_HS)
	{
		descs[idx++] = SShaderStageDesc{ shaderName + "_hs.hlsl", {} };
	}
	if (flagStages & SHADER_STAGE_PS)
	{
		descs[idx++] = SShaderStageDesc{ shaderName + "_ps.hlsl", {} };
	}
	if (flagStages & SHADER_STAGE_CS)
	{
		descs[idx++] = SShaderStageDesc{ shaderName + "_cs.hlsl", {} };
	}
	return descs;
}

Shader::Shader(const SShaderDesc& desc)
	:  m_Descriptor(desc)
	, m_ID(INDEX_NONE)
{
	SetName(desc.ShaderName);
}

Shader::Shader(const std::string& shaderFileName)
	:m_ID(INDEX_NONE)
{
	SetName(shaderFileName);
}

Shader::~Shader()
{
	ReleaseResources();
}

bool Shader::Reload(ID3D11Device* device)
{
	Shader copy(m_Descriptor);
	copy.m_ID= m_ID;
	ReleaseResources();
	m_ID= copy.m_ID;
	return CompileShaders(device, copy.m_Descriptor);

}

void Shader::ClearConstantBuffers()
{
	size_t cbSize = m_ConstantBuffers.size();

	for(size_t i =0;i<cbSize;i++)
	{
		m_ConstantBuffers[i].Dirty = true;
	}
}

void Shader::UpdateConstants(ID3D11DeviceContext* context)
{
	size_t cbSize = m_ConstantBuffers.size();

	for (size_t i = 0; i < cbSize; i++)
	{
		SConstantBufferBinding& CB = m_ConstantBuffers[i];
		if(CB.Dirty)
		{
			ID3D11Buffer* data = CB.Data;
			D3D11_MAPPED_SUBRESOURCE mappedResouce;
			
			
			context->Map(data, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResouce);
			BYTE* bufferPos = static_cast<BYTE*>(mappedResouce.pData);
			size_t CPUSideBufferSize = m_CPUConstantBuffers.size();

			for (const FConstantBufferMapping& indexIDPair : m_Constants)
			{			
				const int32 slotIndex = indexIDPair.first;
				if(CB.BufferSlot == slotIndex)
				{
					const FCPUConstantID c_id = indexIDPair.second;
					assert(c_id < m_CPUConstantBuffers.size());
					SCPUConstant& c = m_CPUConstantBuffers[c_id];
					::memcpy(bufferPos, c.Data, c.Size);
					bufferPos += c.Size;
				}

			}
			//for (size_t c = 0; c < CPUSideBufferSize; c++)
			//{
			//	::memcpy(bufferPos, m_CPUConstantBuffers[c].Data, m_CPUConstantBuffers[c].Size);
			//	bufferPos += m_CPUConstantBuffers[c].Size;
			//}
			context->Unmap(data, 0);			
			(context->*SetShaderConstants[CB.ShaderStage])(CB.BufferSlot, 1, &data);			
			CB.Dirty = false;
		}
	}
	
}

void Shader::UpdateStructed(ID3D11Device* device ,ID3D11DeviceContext* context)
{
	//RW STructed
	//m_SBLayouts
	//BufferID 에서 UAV SRV 꺼내서 binds..
	
	

	//Structed
	size_t sbSize = m_StructedBuffers.size();

	for (size_t i = 0; i < sbSize; i++)
	{
		SStructedBufferBinding& SB = m_StructedBuffers[i];
		if (SB.Dirty)
		{			
			if (SB.Resize)
			{
				ClearResources(context);			
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				SB.SRV->GetDesc(&srvDesc);
				D3D11_BUFFER_DESC bufferDesc;
				SB.Data->GetDesc(&bufferDesc);

				srvDesc.BufferEx.NumElements = (UINT)(SB.Resize / bufferDesc.StructureByteStride);
						
				bufferDesc.ByteWidth = (UINT)SB.Resize;

				SAFE_RELEASE(SB.SRV);	
				SAFE_RELEASE(SB.Data);										
				
				
				device->CreateBuffer(&bufferDesc, nullptr, &SB.Data);
				//Resize
				//SRV 
				//UAV 
			
				if (FAILED(device->CreateShaderResourceView(SB.Data, &srvDesc, &SB.SRV)))
				{
					OutputDebugStringA("Error Resize SRV!");
					return;
				}
				if (SB.UAV)
				{
					D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
					SB.UAV->GetDesc(&uavDesc);
					uavDesc.Buffer.NumElements = (uint32)SB.Resize / bufferDesc.StructureByteStride;
					SAFE_RELEASE(SB.UAV);
					if (FAILED(device->CreateUnorderedAccessView(SB.Data, &uavDesc, &SB.UAV)))
					{
						OutputDebugStringA("Error Resize UAV!");
						return;
					}
				}				
				SB.Resize = 0;	
				context->Flush();
			}

						
			ID3D11Buffer* data = SB.Data;
			D3D11_BUFFER_DESC bufferDesc;
			data->GetDesc(&bufferDesc);

			if(bufferDesc.Usage & D3D11_USAGE_DYNAMIC)
			{
				D3D11_MAPPED_SUBRESOURCE mappedResouce;
				context->Map(data, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResouce);
				BYTE* bufferPos = static_cast<BYTE*>(mappedResouce.pData);
				size_t CPUSideBufferSize = m_CPUConstantBuffers.size();
				::memcpy(bufferPos, m_CPUStructedBuffers[i].Data, m_CPUStructedBuffers[i].Size);
				context->Unmap(data, 0);
			}
			//!Dynamic  && Has UAV
			
			SB.Dirty = false;					
		}

		if(!SB.UAV)
		{			
			(context->*SetShaderResources[SB.ShaderStage])(SB.BufferSlot, 1, &SB.SRV);
		}
		else
		{
			uint32 Count = -1;
			context->CSSetUnorderedAccessViews(SB.BufferSlot, 1, &SB.UAV, &Count);
		}

	}
}

void Shader::ClearResources(ID3D11DeviceContext* context)
{
	
	size_t SBufferSize = m_StructedBuffers.size();
	for (size_t i = 0; i < SBufferSize; i++)
	{		
		ID3D11ShaderResourceView* nullSRV = nullptr;
		(context->*SetShaderResources[m_StructedBuffers[i].ShaderStage])(m_StructedBuffers[i].BufferSlot, 1, &nullSRV);
	}

}

const std::vector<Shader::SConstantBufferLayout>& Shader::GetConstantBufferLayouts() const
{
	return m_CBLayouts;
}

const std::vector<SConstantBufferBinding>& Shader::GetConstantBuffers() const
{
	return m_ConstantBuffers;
}

const std::vector<SCPUConstant>& Shader::GetCPUConstantBuffers() const
{
	return m_CPUConstantBuffers;
}

const std::vector<Shader::SConstantBufferLayout>& Shader::GetStructedBufferLayouts() const
{
	return m_SBLayouts;
}

const std::vector<SStructedBufferBinding>& Shader::GetStructedBuffers() const
{
	return m_StructedBuffers;
}

const std::vector<SCPUStructed>& Shader::GetCPUStructedBuffers() const
{
	return m_CPUStructedBuffers;
}

const STextureBinding& Shader::GetTextureBinding(const std::string& textureName) const
{
	FShaderTextureLookup::const_iterator find = m_ShaderTextureLookup.find(textureName);

	if (find != m_ShaderTextureLookup.end())
	{
		return 		m_TextureBindings[(*find).second];
	}
	return m_TextureBindings[0];
}

const SSamplerBinding& Shader::GetSamplerBinding(const std::string& samplerName) const
{
	FShaderSamplerLookup::const_iterator find = m_ShaderSamplerLookup.find(samplerName);

	if(find != m_ShaderSamplerLookup.end())
	{
		return 		m_SamplerBindings[(*find).second];
	}

	return m_SamplerBindings[0];
}

bool Shader::HasTextureBinding(const std::string& textureName) const
{
	return m_ShaderTextureLookup.find(textureName) != m_ShaderTextureLookup.end();
}

bool Shader::HasSamplerBinding(const std::string& samplerName) const
{
	return m_ShaderSamplerLookup.find(samplerName) != m_ShaderSamplerLookup.end();
}

bool Shader::CompileFromSource(const std::string& pathToFile, const EShaderStage& type, ID3D10Blob*& ref_pBob, std::string& outErrMsg, const std::vector<SShaderMacro>& InMacros)
{
	
	
	//const WCHAR* PathStr = ;
	ID3D10Blob* errorMessage = nullptr;

	int32 i = 0;
	std::vector<D3D10_SHADER_MACRO> d3dMacros(InMacros.size() + 1);

	size_t macroSize = InMacros.size();
	for(i =0;i<macroSize;i++)
	{
		d3dMacros[i] = D3D10_SHADER_MACRO({ InMacros[i].Name.c_str(), InMacros[i].Value.c_str() });
	}	
	d3dMacros[i] = { NULL, NULL };

	if (FAILED(D3DCompileFromFile(
		StringToWString(pathToFile).c_str(),
		d3dMacros.data(),
		SHADER_INCLUDE_HANDLER,
		SHADER_ENTRY_POINT_LOOKUP.at(type),
		SHADER_COMPILER_VERSION_LOOKUP.at(type),
		SHADER_COMPILE_FLAGS,
		0,
		&ref_pBob,
		&errorMessage)))
	{

		outErrMsg = GetCompileError(errorMessage, pathToFile);
		return false;
	}
	return true;
}


EShaderStage Shader::GetShaderTypeFromSourceFilePathA(const std::string& shaderFilePath)
{
	std::filesystem::path shaderPath = shaderFilePath;


	const std::string sourceFileName = shaderPath.replace_extension("").string();
	const std::string shaderTypeStr = { *(sourceFileName.rbegin() + 1), *sourceFileName.rbegin() };
	return s_ShaderTypeStrLookup.at(shaderTypeStr);

}

EShaderStage Shader::GetShaderTypeFromSourceFilePathW(const std::wstring& shaderFilePath)
{
	std::filesystem::path shaderPath = shaderFilePath;


	const std::wstring sourceFileName = shaderPath.replace_extension("").wstring();
	const std::wstring shaderTypeStr = { *(sourceFileName.rbegin() + 1), *sourceFileName.rbegin() };

	
	return s_ShaderTypeStrLookup.at(WStringToString(shaderTypeStr));
}

/*
* https://learn.microsoft.com/en-us/windows/win32/api/d3d11shader/nn-d3d11shader-id3d11shaderreflectionconstantbuffer
*/

void Shader::ReflectConstantBufferLayouts(ID3D11ShaderReflection* sRefl, EShaderStage type)
{
	D3D11_SHADER_DESC desc;
	sRefl->GetDesc(&desc);
	
	//Test Code 
	
	for(uint32 s =0;s< desc.BoundResources;s++)
	{		
		D3D11_SHADER_INPUT_BIND_DESC InputDesc;
		sRefl->GetResourceBindingDesc(s, &InputDesc);		
		
		
		//Create SRV Only
		if(InputDesc.Type == D3D11_SIT_STRUCTURED)
		{
			int32 c = 0;
		}
		//Create UAV SRV
		else if (InputDesc.Type == D3D_SIT_UAV_RWSTRUCTURED)
		{
			
		}
		
	}
	
	

	//Register
	uint32	BufferSlot = 0;
	for (uint32 i = 0; i < desc.ConstantBuffers; ++i)
	{
		SConstantBufferLayout bufferLayout;
		bufferLayout.BuffSize = 0;
		ID3D11ShaderReflectionConstantBuffer* pCBuffer = sRefl->GetConstantBufferByIndex(i);
		pCBuffer->GetDesc(&bufferLayout.Desc);

		D3D11_SHADER_INPUT_BIND_DESC InputDesc;
		sRefl->GetResourceBindingDescByName(bufferLayout.Desc.Name, &InputDesc);


		if(bufferLayout.Desc.Type == D3D_CT_RESOURCE_BIND_INFO)
		{
			for (uint32 j = 0; j < bufferLayout.Desc.Variables; ++j)
			{
				ID3D11ShaderReflectionVariable* pVariable = pCBuffer->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC varDesc;
				pVariable->GetDesc(&varDesc);
				bufferLayout.Variables.push_back(varDesc);

				ID3D11ShaderReflectionType* pType = pVariable->GetType();
				D3D11_SHADER_TYPE_DESC typeDesc;
				pType->GetDesc(&typeDesc);
				bufferLayout.Types.push_back(typeDesc);				
				// accumulate buffer size
				bufferLayout.BuffSize += varDesc.Size;
			}
			bufferLayout.Stage = type;
			//bufferLayout.BufSlot = BufferSlot;
			//++BufferSlot;
			D3D11_SHADER_INPUT_BIND_DESC inputDesc;
			sRefl->GetResourceBindingDescByName(bufferLayout.Desc.Name, &inputDesc);			
			bufferLayout.BufSlot = inputDesc.BindPoint;
			bufferLayout.InputType = inputDesc.Type;
			m_SBLayouts.push_back(bufferLayout);
			
			continue;
		}
		
		
		for (uint32 j = 0; j < bufferLayout.Desc.Variables; ++j)
		{			
			ID3D11ShaderReflectionVariable* pVariable = pCBuffer->GetVariableByIndex(j);
			D3D11_SHADER_VARIABLE_DESC varDesc;
			pVariable->GetDesc(&varDesc);
			bufferLayout.Variables.push_back(varDesc);

			ID3D11ShaderReflectionType* pType = pVariable->GetType();
			D3D11_SHADER_TYPE_DESC typeDesc;
			pType->GetDesc(&typeDesc);
			bufferLayout.Types.push_back(typeDesc);

			// accumulate buffer size
			bufferLayout.BuffSize += varDesc.Size;
		}
		bufferLayout.Stage = type;
		//bufferLayout.BufSlot = BufferSlot;
		//++BufferSlot;

		D3D11_SHADER_INPUT_BIND_DESC inputDesc;
		sRefl->GetResourceBindingDescByName(bufferLayout.Desc.Name, &inputDesc);
		bufferLayout.BufSlot = inputDesc.BindPoint;

		m_CBLayouts.push_back(bufferLayout);
	}

}

bool Shader::CompileShaders(ID3D11Device* device, const SShaderDesc& desc)
{
	constexpr const char* SHADER_BINARY_EXTENSION = ".bin";
	m_Descriptor = desc;
	HRESULT result;
	ShaderBlobs blobs;
	bool bPrinted = false;

	// COMPILE SHADER STAGES
	//----------------------------------------------------------------------------
	

	for (uint32 i = 0; i< EShaderStage::COUNT;i++)
	{
		SShaderStageDesc stageDesc = m_Descriptor.Stages[i];
		
		if (stageDesc.FileName.empty())
			continue;

		//Make Shader FullPath From Descriptor
		WCHAR	FullPath[MAX_PATH] = {};
		MakeShaderFilePath(OUT FullPath, stageDesc.FileName);
		
		const std::wstring sourceFilePathW = FullPath;
		const std::string sourceFilePath = WStringToString(sourceFilePathW);

		const EShaderStage stage = GetShaderTypeFromSourceFilePathW(sourceFilePathW);


		std::string errMsg;
		ID3D10Blob* pBlob;
		if (CompileFromSource(sourceFilePath, stage, pBlob, errMsg, stageDesc.Macros))
		{
			blobs.of[stage] = pBlob;
			
		}
		else
		{
			::OutputDebugStringA(errMsg.c_str());
			return false;
		}



		CreateShaderStage(device, stage, blobs.of[stage]->GetBufferPointer(), blobs.of[stage]->GetBufferSize());
		SetReflections(blobs);

		
		//mDirectories[stage] = ShaderLoadDesc(sourceFilePath, cacheFilePath);
	}

	// INPUT LAYOUT (VS)
	//---------------------------------------------------------------------------
	// src: https://stackoverflow.com/questions/42388979/directx-11-vertex-shader-reflection
	// setup the layout of the data that goes into the shader
	//
	if(m_Reflections.vsRefl)
	{
		D3D11_SHADER_DESC shaderDesc = {};
		m_Reflections.vsRefl->GetDesc(&shaderDesc);
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout(shaderDesc.InputParameters);

		D3D_PRIMITIVE primitiveDesc = shaderDesc.InputPrimitive;

		for (uint32 i = 0; i < shaderDesc.InputParameters; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
			m_Reflections.vsRefl->GetInputParameterDesc(i, &paramDesc);

			// fill out input element desc
			D3D11_INPUT_ELEMENT_DESC elementDesc;
			elementDesc.SemanticName = paramDesc.SemanticName;
			elementDesc.SemanticIndex = paramDesc.SemanticIndex;
			elementDesc.InputSlot = 0;
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			// determine DXGI format
			if (paramDesc.Mask == 1)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)  
					elementDesc.Format = DXGI_FORMAT_R32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) 
					elementDesc.Format = DXGI_FORMAT_R32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			} 
			else if (paramDesc.Mask <= 3)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) 
					elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  
					elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) 
					elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			} 
			else if (paramDesc.Mask <= 7)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) 
					elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) 
					elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			} 
			else if (paramDesc.Mask <= 15)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)  
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			inputLayout[i] = elementDesc; //save element desc
		}

		// Try to create Input Layout
		const D3D11_INPUT_ELEMENT_DESC* pData = inputLayout.data();
		if (pData)
		{
			result = device->CreateInputLayout(
				pData,
				shaderDesc.InputParameters,
				blobs.vs->GetBufferPointer(),
				blobs.vs->GetBufferSize(),
				&m_InputLayout);

			if (FAILED(result))
			{
				OutputDebugStringA("Error creating input layout");
				return false;
			}
		}


	}

	// CONSTANT BUFFERS 
	//---------------------------------------------------------------------------
	// Obtain cbuffer layout information
	for (EShaderStage type = EShaderStage::VS; type < EShaderStage::COUNT; type = (EShaderStage)(type + 1))
	{
		if (m_Reflections.of[type])
		{
			ReflectConstantBufferLayouts(m_Reflections.of[type], type);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Create CPU & GPU constant buffers
	// CPU StructuredBuffer(SB) 	
	size_t sbLayoutSize = m_SBLayouts.size();
	for (size_t i = 0; i < sbLayoutSize; i++)
	{			
		//Don't Create RW Buffer IN Shader!
		if(m_SBLayouts[i].InputType == D3D_SIT_UAV_RWSTRUCTURED)
			continue;
			
		D3D11_SHADER_BUFFER_DESC varDesc = m_SBLayouts[i].Desc;
		SCPUStructed s;
		s.Name = varDesc.Name;
		s.Size = varDesc.Size * DEFAULT_STRUCTED_COUNT;
		s.Data = new char[s.Size];
		//s.Count
		::memset(s.Data, 0, s.Size);
		//m_constants.push_back(std::make_pair(constantBufferSlot, c_id));
		m_CPUStructedBuffers.push_back(s);
	}



	
	
	// GPU CBuffers
	
	D3D11_BUFFER_DESC sBufferDesc = {};
	sBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	
	
	for (size_t i = 0; i < sbLayoutSize; i++)
	{
		SStructedBufferBinding sBuffer;
		sBufferDesc.StructureByteStride = m_SBLayouts[i].Desc.Size;
		sBufferDesc.ByteWidth = m_SBLayouts[i].Desc.Size * DEFAULT_STRUCTED_COUNT;			 //* DEFAULT_STRUCTED_COUNT?

		if(m_SBLayouts[i].InputType == D3D_SIT_STRUCTURED)
		{
			sBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			sBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			sBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		}
		//Not Dynamic
		else if(m_SBLayouts[i].InputType == D3D_SIT_UAV_RWSTRUCTURED)
		{
			//RWStructered 의 소유권을 Shader 에서 박탈 
			continue;

			sBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			sBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		}


		if (FAILED(device->CreateBuffer(&sBufferDesc, NULL, &sBuffer.Data)))
		{
			OutputDebugStringA("Error creating Structed buffer");
			return false;
		}		
		//sBuffer.Data->SetPri		

		sBuffer.Dirty = true;
		sBuffer.ShaderStage = m_SBLayouts[i].Stage;
		sBuffer.BufferSlot = m_SBLayouts[i].BufSlot;

		if (sBufferDesc.Usage != D3D11_USAGE_DYNAMIC)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC	UAVDesc = {};
			UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
			UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			UAVDesc.Buffer.FirstElement = 0;
			UAVDesc.Buffer.Flags = 0;
			UAVDesc.Buffer.NumElements = DEFAULT_STRUCTED_COUNT;

			if (FAILED(device->CreateUnorderedAccessView(sBuffer.Data, &UAVDesc, &sBuffer.UAV)))
			{
				OutputDebugStringA("Error creating UnorderedAccessView!");
				return false;
			}
		}


		D3D11_SHADER_RESOURCE_VIEW_DESC	SRVDesc = {};

		SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		SRVDesc.BufferEx.FirstElement = 0;
		SRVDesc.BufferEx.Flags = 0;
		SRVDesc.BufferEx.NumElements = DEFAULT_STRUCTED_COUNT;

		if (FAILED(device->CreateShaderResourceView(sBuffer.Data, &SRVDesc, &sBuffer.SRV)))
		{
			OutputDebugStringA("Error creating SRV!");
			return false;
		}

	
		//Create Srv>?
		m_StructedBuffers.push_back(sBuffer);
		
	}

	//////////////////////////////////////////////////////////////////////////




	// Create CPU & GPU constant buffers
	// CPU CBuffers 	
	//int constantBufferSlot = 0;
	size_t cbLayoutSize = m_CBLayouts.size();
	for(size_t i = 0;i< cbLayoutSize;i++)
	{
		std::vector<FCPUConstantID> cpuBuffers;
		size_t valSize = m_CBLayouts[i].Variables.size();
		int32 bufferSlot = m_CBLayouts[i].BufSlot;
		for(size_t j = 0; j<valSize;j++)
		{
			D3D11_SHADER_VARIABLE_DESC varDesc = m_CBLayouts[i].Variables[j];
			SCPUConstant c;
			FCPUConstantID c_id = static_cast<FCPUConstantID>(m_CPUConstantBuffers.size());
			c.Name = varDesc.Name;
			c.Size = varDesc.Size;
			c.Data = new char[c.Size];
			::memset(c.Data, 0, c.Size);						
			m_Constants.push_back(std::make_pair(bufferSlot, c_id));
			m_CPUConstantBuffers.push_back(c);
		}
		//++constantBufferSlot;
		
	}


	// GPU CBuffers
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBufferDesc.MiscFlags = 0;
	cBufferDesc.StructureByteStride = 0;
	for(size_t i = 0;i< cbLayoutSize;i++)
	{
		SConstantBufferBinding cBuffer;		
		cBufferDesc.ByteWidth = m_CBLayouts[i].Desc.Size;
		if (FAILED(device->CreateBuffer(&cBufferDesc, NULL, &cBuffer.Data)))
		{
			OutputDebugStringA("Error creating constant buffer");
			return false;
		}
		cBuffer.Dirty = true;		
		cBuffer.ShaderStage = m_CBLayouts[i].Stage;
		cBuffer.BufferSlot = m_CBLayouts[i].BufSlot;
		m_ConstantBuffers.push_back(cBuffer);	
		
	}


	// TEXTURES & SAMPLERS
	//---------------------------------------------------------------------------
	for (uint32 shaderStage = 0; shaderStage < EShaderStage::COUNT; ++shaderStage)
	{
		uint32 texSlot = 0;	
		uint32 smpSlot = 0;
		uint32 uavSlot = 0;
		ID3D11ShaderReflection*& sRefl = m_Reflections.of[shaderStage];
		if (sRefl)
		{
			D3D11_SHADER_DESC desc = {};
			sRefl->GetDesc(&desc);

			for (uint32 i = 0; i < desc.BoundResources; ++i)
			{
				D3D11_SHADER_INPUT_BIND_DESC shdInpDesc;
				sRefl->GetResourceBindingDesc(i, &shdInpDesc);

				switch (shdInpDesc.Type)
				{
				case D3D_SIT_SAMPLER:
				{
					SSamplerBinding smp;
					smp.ShaderStage = static_cast<EShaderStage>(shaderStage);					
					smp.SamplerSlot= shdInpDesc.BindPoint;
					m_SamplerBindings.push_back(smp);										
					m_ShaderSamplerLookup[shdInpDesc.Name] = static_cast<int32>(m_SamplerBindings.size() - 1);
				} 
				break;
				case D3D_SIT_TEXTURE:
				{
					STextureBinding tex;
					tex.ShaderStage = static_cast<EShaderStage>(shaderStage);
					tex.TextureSlot= shdInpDesc.BindPoint;
					m_TextureBindings.push_back(tex);
					m_ShaderTextureLookup[shdInpDesc.Name] = static_cast<int32>(m_TextureBindings.size() - 1);
				} 
				break;
				case D3D11_SIT_UAV_RWSTRUCTURED:
				{
					int32 t = 0;
				}
				break;

				case D3D_SIT_UAV_RWTYPED:
				{
					STextureBinding tex;
					tex.ShaderStage = static_cast<EShaderStage>(shaderStage);
					tex.TextureSlot= uavSlot++;
					m_TextureBindings.push_back(tex);
					m_ShaderTextureLookup[shdInpDesc.Name] = static_cast<int32>(m_TextureBindings.size() - 1);
				} break;

				case D3D_SIT_CBUFFER: break;


				default:
					//Log::Warning("Unhandled shader input bind type in shader reflection");
					break;

				} // switch shader input type
			} // bound resource
		} // sRefl
	} // shaderStage

	// release blobs
	for (uint32 type = EShaderStage::VS; type < EShaderStage::COUNT; ++type)
	{
		if (blobs.of[type])
			blobs.of[type]->Release();
	}

	return true;
}

void Shader::SetReflections(const ShaderBlobs& blobs)
{
	for (uint32 type = EShaderStage::VS; type < EShaderStage::COUNT; ++type)
	{
		if (blobs.of[type])
		{
			
			void** ppBuffer = reinterpret_cast<void**>(&m_Reflections.of[type]);
			if (FAILED(D3DReflect(blobs.of[type]->GetBufferPointer(), blobs.of[type]->GetBufferSize(), IID_ID3D11ShaderReflection, ppBuffer)))
			{
				OutputDebugStringA("Shader::SetReflections::D3DReflect : ERROR!");
				assert(false);
			}
		}
	}
}

void Shader::CreateShaderStage(ID3D11Device* pDevice, EShaderStage stage, void* pBuffer, const size_t szShaderBinary)
{
	HRESULT hr = {};
	const char* msg = "";
	switch (stage)
	{
	case EShaderStage::VS:
		if (FAILED(pDevice->CreateVertexShader(pBuffer, szShaderBinary, NULL, &m_Stages.VertexShader)))
		{
			msg = "Error creating vertex shader program";
		}
		break;
	case EShaderStage::PS:
		if (FAILED(pDevice->CreatePixelShader(pBuffer, szShaderBinary, NULL, &m_Stages.PixelShader)))
		{
			msg = "Error creating pixel shader program";
		}
		break;
	case EShaderStage::GS:
		if (FAILED(pDevice->CreateGeometryShader(pBuffer, szShaderBinary, NULL, &m_Stages.GeometryShader)))
		{
			msg = "Error creating pixel geometry program";
		}
		break;
	case EShaderStage::CS:
		if (FAILED(pDevice->CreateComputeShader(pBuffer, szShaderBinary, NULL, &m_Stages.ComputeShader)))
		{
			msg = "Error creating compute shader program";
		}
		break;
	}

	if (FAILED(hr))
	{
		OutputDebugStringA(msg);
		assert(false);
	}
}

void Shader::ReleaseResources()
{

	size_t CBufferSize = m_ConstantBuffers.size();
	for(size_t i =0;i<CBufferSize;i++)
	{
		SAFE_RELEASE(m_ConstantBuffers[i].Data);
	}
	m_ConstantBuffers.clear();
	//Release BufferObjects.
	// 
	//Release CPUSide

	size_t CPUCBufferSize = m_CPUConstantBuffers.size();
	for(size_t i =0 ;i<CPUCBufferSize;i++)
	{
		//Delete CPU Side CBuffer 
		SAFE_DELETE_ARRAY(m_CPUConstantBuffers[i].Data);
	}
	m_CPUConstantBuffers.clear();


	size_t SBufferSize = m_StructedBuffers.size();
	for (size_t i = 0; i < SBufferSize; i++)
	{
		SAFE_RELEASE(m_StructedBuffers[i].SRV);
		SAFE_RELEASE(m_StructedBuffers[i].UAV);
		SAFE_RELEASE(m_StructedBuffers[i].Data);	
	}
	m_StructedBuffers.clear();
	//Release BufferObjects.


	//Release CPUSide
	size_t CPUSBufferSize = m_CPUStructedBuffers.size();
	for (size_t i = 0; i < CPUSBufferSize; i++)
	{
		//Delete CPU Side CBuffer 
		SAFE_DELETE_ARRAY(m_CPUStructedBuffers[i].Data);
	}
	m_CPUStructedBuffers.clear();


	SAFE_RELEASE(m_InputLayout);
	SAFE_RELEASE(m_Stages.VertexShader);//Vs
	SAFE_RELEASE(m_Stages.PixelShader);//ps
	SAFE_RELEASE(m_Stages.ComputeShader);//cs
	SAFE_RELEASE(m_Stages.GeometryShader);//ps
	SAFE_RELEASE(m_Stages.DomainShader);//ds


	for (uint32 type = EShaderStage::VS; type < EShaderStage::COUNT; ++type)
	{
		SAFE_RELEASE(m_Reflections.of[type]);
	}

	m_CBLayouts.clear();	
	m_TextureBindings.clear();
	m_SamplerBindings.clear();
	m_ShaderTextureLookup.clear();
	m_ShaderSamplerLookup.clear();
	m_SBLayouts.clear();

	
}

