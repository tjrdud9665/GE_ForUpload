#pragma once

//Renderer 를 통해서 변수에 접근할것
class D3DTexture
{
	friend class Renderer;
	friend struct SRenderTarget;
	friend struct SDepthTarget;
	friend struct SSetTextureCommand;
	friend class EditorManager;
	friend class TargetViewer;

public:
	struct CubemapUtility
	{
		// cube face order: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476906(v=vs.85).aspx
		//------------------------------------------------------------------------------------------------------
		// 0: RIGHT		1: LEFT
		// 2: UP		3: DOWN
		// 4: FRONT		5: BACK
		//------------------------------------------------------------------------------------------------------
		enum ECubeMapLookDirections
		{
			CUBEMAP_LOOK_RIGHT = 0,
			CUBEMAP_LOOK_LEFT,
			CUBEMAP_LOOK_UP,
			CUBEMAP_LOOK_DOWN,
			CUBEMAP_LOOK_FRONT,
			CUBEMAP_LOOK_BACK,

			NUM_CUBEMAP_LOOK_DIRECTIONS
		};
		static Matrix CalculateViewMatrix(ECubeMapLookDirections cubeFace, const SVector3& position = SVector3(0.0f));
		inline static Matrix CalculateViewMatrix(int32 face, const SVector3& position = SVector3(0.0f)) { return CalculateViewMatrix(static_cast<ECubeMapLookDirections>(face), position); }
	};


private:
	D3DTexture();
	~D3DTexture();

	bool InitializeTexture2D(const D3D11_TEXTURE2D_DESC& descriptor, class Renderer* pRenderer, bool initializeSRV);
	void Release();

public:
	//From BackBuffer
	uint32		GetWidth() const
	{
		return m_Width;
	}

	uint32		GetHeight() const
	{
		return m_Height;
	}

	ID3D11ShaderResourceView* GetSRV() const
	{
		return m_SRV;
	}

	const std::string& GetName() const
	{
		return m_Name;
	}

private:
	ID3D11ShaderResourceView*						m_SRV;
	ID3D11UnorderedAccessView*						m_UAV;
	std::vector<ID3D11ShaderResourceView*>			m_SRVArray;
	std::vector<ID3D11UnorderedAccessView*>			m_UAVArray;
	union
	{
		ID3D11Texture3D* m_Tex3D;
		ID3D11Texture2D* m_Tex2D;
	};

	uint32						m_Width;
	uint32						m_Height;
	uint32						m_Depth;			//For 3DTexture


	std::string					m_Name;
	FTextureID					m_ID;

};



class D3DSampler
{
	friend class Renderer;
	friend struct SSetSamplerCommand;

private:
	D3DSampler()
		:m_ID(INDEX_NONE)
		,m_SamplerState(nullptr)
		,m_Name("")
	{

	}
	~D3DSampler()
	{
		SAFE_RELEASE(m_SamplerState);

	}

private:
	ID3D11SamplerState*		m_SamplerState;
	std::string				m_Name;
	FSamplerID				m_ID;
};