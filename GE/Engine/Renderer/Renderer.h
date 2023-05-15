#pragma once

#include "../XObject/XObject.h"
#include "D3DTexture.h"
#include "Shader.h"
#include "D3DBuffer.h"
#include "RenderCommands.h"

#define SPOT_SHADOW_DIMENSION 2048
#define POINT_SHADOW_DIMENSION 2048
#define DIR_SHADOW_DIMENSION 4096


class Renderer :public XObject
{
	friend class Engine;
	friend class Globals;



private:
	Renderer();
	Renderer(const Renderer& r) = delete;
	virtual ~Renderer();

public:
	ID3D11Device* GetDeivce() const
	{
		return m_device;
	}
	ID3D11DeviceContext* GetDeviceContext() const
	{
		return m_deviceContext;
	}

	
private:
	/* CoreInterface - Using In Engine*/ 
	bool Initialize(class D3DManager* d3dMgr , bool bAntiAliasing);
	void ShutDown();





public:
	/*Getters*/

	//Consider Make Private..
	ID3D11ShaderResourceView*  GetTextureSRV(FTextureID ID) const;

	D3DTexture*			GetTextureObject(FTextureID ID) const;
	const FTextureID	GetTexture(const std::string name) const;
	D3DSampler*			GetSampler(FSamplerID ID) const;
	FTextureID			GetDepthTargetTexture(FDepthTargetID DT) const;
	
	
	float					AspectRatio()	const;
	//uint32					WindowHeight()	const;
	//uint32					WindowWidth()	const;
	uint32					FrameRenderTargetHeight() const;
	uint32					FrameRenderTargetWidth() const;
	SVector2					FrameRenderTargetDimensionsAsFloat2() const;
	//SVector2					GetWindowDimensionsAsFloat2() const;

	const D3D11_VIEWPORT& GetCurrentViewport() const;



	const SAntiAliasing& GetAASetting() const { return m_AASetting; }

	inline FRenderTargetID	GetBackBufferRenderTarget() const { return m_BackBufferRenderTarget; }
	inline FDepthTargetID	GetBoundDepthTarget() const { return m_PipelineState.DepthTargetID; }
	inline FTextureID		GetDefaultRenderTargetTexture() const { return m_RenderTargets[m_BackBufferRenderTarget].Texture->m_ID; }
	inline FTextureID		GetRenderTargetTexture(FRenderTargetID RT) const { return m_RenderTargets[RT].Texture->m_ID; }	
	inline FDepthTargetID	GetDefaultDepthTarget()const { return m_DefaultDepthTargetID; }
	inline FTextureID		GetDefaultDepthTargetTexture()const { return m_DefaultDepthBufferTexture; }

	inline  FRenderTargetID GetActiveRenderTaregetID(int32 Idx = 0) const { return m_PipelineState.RenderTargets[Idx]; }
	
	inline FBufferID		GetActiveVertexBufferID() const { return m_PipelineState.VertexBufferID; }
	const SPipelineState&	GetPipelineState() const;	
	void					SetPipelineState(const SPipelineState& InPipeLineState);
	//const SBufferDesc		GetBufferDesc(EBufferType bufferType, FBufferID bufferID) const;

	FBufferID				GetVertexBufferFromName(const std::string& InName) const;
	FBufferID				GetIndexBufferFromName(const std::string& InName) const;

	const Shader* GetShader(FShaderID shader_id) const;	
	
	inline const FShaderID	GetActiveShader() const 
	{
		return m_PipelineState.ShaderID; 
	}
	inline const SD3DBuffer& GetVertexBuffer(FBufferID id) 
	{
		return m_VertexBuffers[id]; 
	}
	inline const SD3DBuffer& GetIndexBuffer(FBufferID id)
	{
		return m_IndexBuffers[id];
	}
	inline const SD3DBuffer& GetRWStructuredBuffer(FBufferID id)
	{
		return m_UABuffers[id];
	}

	SShaderDesc				GetShaderDesc(FShaderID shaderID) const;
	DXGI_FORMAT				GetTextureImageFormat(FTextureID id) const;


public:
	/* Resource Initialization */
	/* Shader */
	FShaderID CreateShader(const SShaderDesc& InShaderDesc);


	FTextureID				CreateTextureFromMemory(void* pResource, size_t InSize, const std::string& InTexName, const std::string& DesiredPath, bool bGenerateMips = false);
	bool					SaveTextureToFile(FTextureID toSaveTexture, const std::string& InTexFullPath, bool bOverride = false);

	/* Texture InTexFileName 은 "someImage.png" InPath는 FileName와 확장자를 제외한 풀페스 */
	FTextureID				CreateTextureFromFile(const std::string& InTexFileName , const std::string& InPath, bool bGenerateMips = false);
	FTextureID				CreateTexture2D(const STextureDesc& texDesc);

	FTextureID				CreateTexture2D(D3D11_TEXTURE2D_DESC& textureDesc, bool initializeSRV);	// used by AddRenderTarget() | todo: remove this?
	FTextureID				CreateHDRTexture(const std::string& texFileName, const std::string& InPath);
	//FTextureID				CreateCubemapFromFaceTextures(const std::vector<std::string>& textureFiles, bool bGenerateMips, unsigned mipLevels = 1);

	
	// --- SAMPLER
	//
	FSamplerID				CreateSamplerState(D3D11_SAMPLER_DESC& samplerDesc ,const std::string& smpName);	// TODO: samplerDesc

	// --- BUFFER
	FBufferID				CreateBuffer(const SBufferDesc& bufferDesc, const void* pData = nullptr, const char* pBufferName = nullptr);

	//// --- PIPELINE STATES
	//FRasterizerStateID			AddRasterizerState(ERasterizerCullMode cullMode, ERasterizerFillMode fillMode, bool bEnableDepthClip, bool bEnableScissors);
	//FDepthStencilStateID		AddDepthStencilState(bool bEnableDepth, bool bEnableStencil);	// TODO: depthStencilStateDesc
	FDepthStencilStateID		AddDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& dsDesc);   //
	//FBlendStateID				AddBlendState( );


	// --- RENDER / DEPTH TARGETS
	//Create Texture and RenderTarget
	FRenderTargetID				AddRenderTarget(const SRenderTargetDesc& renderTargetDesc);

	//Create RenderTarget From TextureObject(Already Exist)
	FRenderTargetID				AddRenderTarget(D3DTexture* textureObj, D3D11_RENDER_TARGET_VIEW_DESC& RTVDesc);

	std::vector<FDepthTargetID>	AddDepthTarget(const SDepthTargetDesc& depthTargetDesc);

	//bool						RecycleDepthTarget(FDepthTargetID depthTargetID, const SDepthTargetDesc& newDepthTargetDesc);




public:
	//----------------------------------------------------------------------------------------------------------------
	// PIPELINE STATE MANAGEMENT
	//----------------------------------------------------------------------------------------------------------------
	//RS Viewport
	void					SetViewport(const uint32 Width, const uint32 Height);
	void					SetViewport(const D3D11_VIEWPORT& viewport);
	//Shader
	void					SetShader(FShaderID shaderID, bool bUnbindRenderTargets = false, bool bUnbindTextures = true);
	//IA
	void					SetVertexBuffer(FBufferID bufferID);
	void					SetIndexBuffer(FBufferID bufferID);	

	void					SetTexture(const char* texName, FTextureID tex);
	void					SetRWTexture(const char* texName, FTextureID tex);
	void					SetTextureArray(const char* texName, FTextureID texArray);
	void					SetTextureFromArraySlice(const char* texName, FTextureID texArray, uint32 slice);	

	void					SetTextureArray(const char* texName, const std::array<FTextureID, TEXTURE_ARRAY_SIZE>& TextureIDs, uint32 numTextures);


	void					SetSamplerState(const char* samplerName, FSamplerID sampler);
	void					SetRasterizerState(FRasterizerStateID rsStateID);
	void					SetBlendState(FBlendStateID blendStateID);
	void					SetDepthStencilState(FDepthStencilStateID depthStencilStateID);
	void					SetScissorsRect(int32 left, int32 right, int32 top, int32 bottom);
	FDepthStencilStateID	GetDepthStencilState() const;


	template <typename... Args>
	inline void				BindRenderTargets(Args const&... renderTargetIDs) { m_PipelineState.RenderTargets = { renderTargetIDs... }; }
	void					BindRenderTarget(FRenderTargetID rtvID);
	void					BindDepthTarget(FDepthTargetID dsvID);

	void					UnbindRenderTargets();
	void					UnbindDepthTarget();

	void					SetConstant4x4f(const char* cName, const XMMATRIX& matrix);
	void					SetConstant4x4f(const char* cName, const Matrix& matrix);
	void					SetConstant4f(const char* cName, const SVector4& float4);
	void					SetConstant3f(const char* cName, const SVector3& float3);
	void					SetConstant2f(const char* cName, const SVector2& float2);	
	void					SetConstant1f(const char* cName, const float& data);
	void					SetConstant1i(const char* cName, const int32& data);
	//Custom Struct
	void					SetConstantStruct(const char* cName, const void* data);

	//Create ComputeShader's Buffer
	FBufferID				CreateRWStructured(const char* cName,FShaderID shaderID, const void* data, uint32 ElemCount,ECPUAccess cpuAccess = ECPUAccess::NONE);

	//Only Send GPU to CPU  
	void					SetRWStructured(FBufferID bufferID, uint32 ElemCount);
	
	void					SetStructured(const char* cName, FBufferID bufferID );

	void					SetStructured(const char* cName, const void* data,size_t Size);	

	void					BeginRender(const SClearCommand& clearCmd);	// clears the bound render targets

	//Present
	void					EndFrame();
	void					ResetPipelineState();

	void					UpdateVertexBuffer(FBufferID buffer, const void* pData);
	void					UpdateIndexBuffer(FBufferID buffer, const void* pData);
	void					UpdateBuffer(FBufferID buffer, const void* pData);

	template<typename VtxData>
	void					UpdateVertexBuffer_Append(FBufferID buffer,const void* pData,const uint32 count, uint32 AppendPos)
	{
		assert(buffer >= 0 && buffer < m_VertexBuffers.size());
		m_VertexBuffers[buffer].UpdateNoOverwrite<VtxData>(m_deviceContext, pData, count, AppendPos);
	}

	void					ReadBuffer(FBufferID buffer ,void*& pOutData);


	void					Apply();

	void					ClearSRV();

	void					ForceClearSrv();




	//----------------------------------------------------------------------------------------------------------------
	// DRAW FUNCTIONS
	//----------------------------------------------------------------------------------------------------------------
	void					DrawIndexed(EPrimitiveTopology topology = EPrimitiveTopology::TRIANGLE_LIST);
	void					DrawIndexedInstanced(int32 instanceCount, EPrimitiveTopology topology = EPrimitiveTopology::TRIANGLE_LIST);
	void					Draw(int32 vertCount, EPrimitiveTopology topology = EPrimitiveTopology::POINT_LIST);

	void					DrawVtxOffset(uint32 vertCount, uint32 Offset ,EPrimitiveTopology topology = EPrimitiveTopology::TRIANGLE_LIST);

	void					DrawInstanced(int32 vertCount, int32 instanceCount, EPrimitiveTopology topology = EPrimitiveTopology::POINT_LIST);

	void					Dispatch(int32 x, int32 y, int32 z);

	//void					DrawQuadOnScreen(const DrawQuadOnScreenCommand& cmd); // BottomLeft<x,y> = (0,0)
	
	//void					DrawLine(const SVector3& pos1, const SVector3& pos2, const SLinearColor& color);


public:
	class Mesh GetBuiltInMesh(EGeometry builtInMesh) const;





private:
	void					SetConstant(const char* cName, const void* data);
	void					SetTexture_(const char* texName, FTextureID tex, uint32 slice = 0 /* only for texture arrays */);

	void					SetStructured_(const char* cName, const void* data, size_t Size);





	//Fast Accessing
private:
	class D3DManager*		 m_cachedMgr;	
	ID3D11Device*			 m_device;
	ID3D11DeviceContext*	 m_deviceContext;


	//PipeLineState
private:
	SPipelineState	m_PipelineState;
	SPipelineState	m_PrevPipelineState;

	FRenderTargetID					m_BackBufferRenderTarget;			//BackBufferRenderTarget
	FTextureID						m_DefaultDepthBufferTexture;
	FDepthTargetID					m_DefaultDepthTargetID;
	
	
	//Repo DX's Rendering Related
	//Can find ID each Index
private:
	std::vector<Shader*>		m_Shaders;
	std::vector<D3DTexture*>	m_Textures;
	std::vector<D3DSampler*>	m_Samplers;

	//Mesh, UAV
	std::vector<SD3DBuffer>		m_VertexBuffers;
	std::vector<SD3DBuffer>		m_IndexBuffers;

	//For RWStructuredBuffer
	std::vector<SD3DBuffer>		m_UABuffers;

	//RT,DT
	std::vector<SRenderTarget>		m_RenderTargets;
	std::vector<SDepthTarget>		m_DepthTargets;


	std::queue<SSetTextureCommand>	m_SetTextureCmds;
	std::queue<SSetSamplerCommand>	m_SetSamplerCmds;
	std::queue<SSetRWBufferCommand>	m_SetRWCmds;

	//RenderStates..
	std::vector<ID3D11RasterizerState*>		m_RasterizerStates;
	std::vector<ID3D11DepthStencilState*>	m_DepthStencilStates;
	std::vector<ID3D11BlendState*>			m_BlendStates;


	

	std::vector<Mesh>						m_BuiltInMeshs;


	

private:
	/*RenderingSettings*/
	SAntiAliasing	m_AASetting;

};

