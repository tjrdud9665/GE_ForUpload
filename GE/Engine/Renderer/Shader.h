#pragma once

#include "../XObject/XObject.h"

using FCPUConstantID = int32;
using GPU_ConstantBufferSlotIndex = int32;
using FConstantBufferMapping = std::pair<GPU_ConstantBufferSlotIndex, FCPUConstantID>;
using FileTimeStamp = std::filesystem::file_time_type;

struct SCPUConstant
{
	using CPUConstantRefIDPair = std::tuple<SCPUConstant&, FCPUConstantID>;

	friend class Renderer;
	friend class Shader;

	SCPUConstant()
		: Name(), Size(0), Data(nullptr)
	{

	}

	std::string Name;
	size_t		Size;
	void*		Data;

private:
	inline bool operator==(const SCPUConstant& c) const
	{
		return Name == c.Name && Data == c.Data && Size == c.Size;
	}
	inline bool operator!=(const SCPUConstant& c) const
	{
		return Name != c.Name || Data != c.Data || Size == c.Size;
	}
};


struct SCPUStructed
{	

	friend class Renderer;
	friend class Shader;

	SCPUStructed()
		: Name(), Size(0), Data(nullptr)
	{
	}	
	//어디에 놓을것?
	//SRV 
	//UAV
	//Count

	std::string Name;
	size_t		Size;						
	void* Data;

private:
	inline bool operator==(const SCPUStructed& c) const
	{
		return Name == c.Name && Data == c.Data && Size == c.Size;
	}
	inline bool operator!=(const SCPUStructed& c) const
	{
		return Name != c.Name || Data != c.Data || Size == c.Size;
	}
};

struct SConstantBufferBinding
{
	EShaderStage	ShaderStage;
	uint32			BufferSlot;//Reg
	ID3D11Buffer*	Data;
	bool			Dirty;
};

struct SStructedBufferBinding
{	 
	EShaderStage	ShaderStage;
	uint32			BufferSlot;			//Reg
	ID3D11Buffer*	Data;				//BufferID
	bool			Dirty;
	size_t			Resize;				//DesiredResizing -> if Not zero Want Resize

	//TODO : Texture 로 옮기고 TextureID로 처리
	ID3D11ShaderResourceView*	SRV;
	ID3D11UnorderedAccessView*	UAV;		

	

	SStructedBufferBinding()
	{
		SRV = nullptr;
		UAV = nullptr;
		Resize = 0;
	}
};


struct STextureBinding
{
	EShaderStage	ShaderStage;
	uint32			TextureSlot;
};

struct SSamplerBinding
{
	EShaderStage ShaderStage;
	uint32	     SamplerSlot;
	//std::string  Name;	
};
struct SRWBinding
{
	EShaderStage	ShaderStage;
	uint32			RWSlot;
};

/*
*DXGI_FORMAT_R32G32_FLOAT,			//Ex.UV
*DXGI_FORMAT_R32G32B32_FLOAT,		//EX Position
*DXGI_FORMAT_R32G32B32A32_FLOAT		//Ex Color~
*/
struct SInputLayout
{
	std::string		SemanticName;
	DXGI_FORMAT		Format;
};

struct SShaderMacro
{
	std::string Name;
	std::string Value;
};

struct SShaderStageDesc
{
	std::string FileName;
	std::vector<SShaderMacro> Macros;
};

struct SShaderDesc
{
	using ShaderStageArr = std::array<SShaderStageDesc, EShaderStageFlags::SHADER_STAGE_COUNT>;
	static ShaderStageArr CreateStageDescsFromShaderName(const char* shaderName, uint32 flagStages);

	std::string ShaderName;
	std::array<SShaderStageDesc, EShaderStage::COUNT> Stages;
};

//struct ShaderLoadDesc
//{
//	ShaderLoadDesc() = default;
//	ShaderLoadDesc(const std::string& path, const std::string& cachePath_) : fullPath(path), cachePath(cachePath_)
//	{
//		lastWriteTime = std::experimental::filesystem::last_write_time(fullPath);
//		cacheLastWriteTime = std::experimental::filesystem::last_write_time(cachePath);
//	}
//	std::string fullPath;
//	std::string cachePath;
//	FileTimeStamp lastWriteTime;
//	FileTimeStamp cacheLastWriteTime;
//};


static bool MakeShaderFilePath(WCHAR (&OutFullPath)[MAX_PATH] ,const std::string& InFileName);


class Shader :public XObject
{
	friend class Renderer;
	using FShaderTextureLookup = std::unordered_map<std::string, FTextureID>;
	using FShaderSamplerLookup = std::unordered_map<std::string, FSamplerID>;

public:
	union ShaderBlobs
	{
		struct
		{
			ID3D10Blob* vs;
			ID3D10Blob* gs;
			ID3D10Blob* ds;
			ID3D10Blob* hs;
			ID3D10Blob* ps;
			ID3D10Blob* cs;
		};
		ID3D10Blob* of[EShaderStage::COUNT] = { nullptr };
	};

	union ShaderReflections
	{
		struct
		{
			ID3D11ShaderReflection* vsRefl;
			ID3D11ShaderReflection* gsRefl;
			ID3D11ShaderReflection* dsRefl;
			ID3D11ShaderReflection* hsRefl;
			ID3D11ShaderReflection* psRefl;
			ID3D11ShaderReflection* csRefl;
		};
		ID3D11ShaderReflection* of[EShaderStage::COUNT] = { nullptr };
	};

	struct SShaderStages
	{
		ID3D11VertexShader*			VertexShader = nullptr;
		ID3D11PixelShader*			PixelShader = nullptr;
		ID3D11GeometryShader*		GeometryShader = nullptr;
		ID3D11HullShader*			HullShader = nullptr;
		ID3D11DomainShader*			DomainShader = nullptr;
		ID3D11ComputeShader*		ComputeShader = nullptr;

		~SShaderStages()
		{
			SAFE_RELEASE(VertexShader);
			SAFE_RELEASE(PixelShader);
			SAFE_RELEASE(GeometryShader);
			SAFE_RELEASE(HullShader);
			SAFE_RELEASE(DomainShader);
			SAFE_RELEASE(ComputeShader);
		}
	};

	struct SConstantBufferLayout
	{	
		// information used to create GPU/CPU constant buffers
		D3D11_SHADER_BUFFER_DESC					Desc;
		std::vector<D3D11_SHADER_VARIABLE_DESC>		Variables;
		std::vector<D3D11_SHADER_TYPE_DESC>			Types;
		uint32										BuffSize;
		EShaderStage								Stage;
		uint32										BufSlot;
		D3D_SHADER_INPUT_TYPE						InputType;		//RW or NotRW
	};




public:
	Shader(const SShaderDesc& desc);
	Shader(const std::string& shaderFileName);
	virtual ~Shader();

	bool Reload(ID3D11Device* device);
	void ClearConstantBuffers();
	void UpdateConstants(ID3D11DeviceContext* context);

	void UpdateStructed(ID3D11Device* device,ID3D11DeviceContext* context);

	void ClearResources(ID3D11DeviceContext* context);


	FShaderID    GetID()   const 
	{
		return m_ID; 
	}

	const std::vector<SConstantBufferLayout>&	GetConstantBufferLayouts() const;
	const std::vector<SConstantBufferBinding>&	GetConstantBuffers() const;
	const std::vector<SCPUConstant>&			GetCPUConstantBuffers() const;

	const std::vector<SConstantBufferLayout>&	GetStructedBufferLayouts() const;
	const std::vector<SStructedBufferBinding>&	GetStructedBuffers() const;
	const std::vector<SCPUStructed>&			GetCPUStructedBuffers() const;

	const STextureBinding& GetTextureBinding(const std::string& textureName) const;
	const SSamplerBinding& GetSamplerBinding(const std::string& samplerName) const;
	bool HasTextureBinding(const std::string& textureName) const;
	bool HasSamplerBinding(const std::string& samplerName) const;

	//bool HasSourceFileBeenUpdated() const;


private:
	//----------------------------------------------------------------------------------------------------------------
	// STATIC PRIVATE INTERFACE
	//----------------------------------------------------------------------------------------------------------------
	// Compiles shader from source file with the given shader macros
	//
	static bool CompileFromSource(
		const std::string& pathToFile
		, const EShaderStage& type
		, ID3D10Blob*& ref_pBob
		, std::string& outErrMsg
		, const std::vector<SShaderMacro>& InMacros);

	//// Reads in cached binary from %APPDATA%/Engine/ShaderCache folder into ID3D10Blob 
	////
	//static ID3D10Blob* CompileFromCachedBinary(const std::string& cachedBinaryFilePath);

	//// Writes out compiled ID3D10Blob into %APPDATA%/Engine/ShaderCache folder
	////
	//static void			CacheShaderBinary(const std::string& shaderCacheFileName, ID3D10Blob* pCompiledBinary);

	// example filePath: "rootPath/filename_vs.hlsl"
	//                                      ^^----- shaderTypeString
	static EShaderStage	GetShaderTypeFromSourceFilePathA(const std::string& shaderFilePath);
	static EShaderStage	GetShaderTypeFromSourceFilePathW(const std::wstring& shaderFilePath);

	//----------------------------------------------------------------------------------------------------------------
	// UTILITY FUNCTIONS
	//----------------------------------------------------------------------------------------------------------------
	void ReflectConstantBufferLayouts(ID3D11ShaderReflection* sRefl, EShaderStage type);
	bool CompileShaders(ID3D11Device* device, const SShaderDesc& desc);
	void SetReflections(const ShaderBlobs& blobs);
	void CreateShaderStage(ID3D11Device* pDevice, EShaderStage stage, void* pBuffer, const size_t szShaderBinary);
	void ReleaseResources();
	


private:
	FShaderID			m_ID;	
	SShaderStages		m_Stages;

	ShaderReflections		m_Reflections;
	ID3D11InputLayout*		m_InputLayout;

	std::vector<SConstantBufferBinding>		m_ConstantBuffers;				//GPU side
	std::vector<SCPUConstant>				m_CPUConstantBuffers;			//CPU side
	std::vector<SConstantBufferLayout>		m_CBLayouts;				
	std::vector<FConstantBufferMapping>		m_Constants;

	std::vector<SStructedBufferBinding>			m_StructedBuffers;				//GPU side
	std::vector<SCPUStructed>					m_CPUStructedBuffers;			//CPU side
	std::vector<SConstantBufferLayout>		m_SBLayouts;


	std::vector<STextureBinding>			m_TextureBindings;
	std::vector<SSamplerBinding>			m_SamplerBindings;

	//CurrentValue
	FShaderTextureLookup m_ShaderTextureLookup;
	FShaderSamplerLookup m_ShaderSamplerLookup;
	

	SShaderDesc m_Descriptor;	// used for shader reloading
	//SShaderDirectoryLookup mDirectories;
	
};

