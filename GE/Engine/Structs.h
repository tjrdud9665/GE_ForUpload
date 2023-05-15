#pragma once

//D3D11_TEXTURE2D_DESC 을 만들기위함
/*
*     UINT Width;
	UINT Height;
	UINT MipLevels;
	UINT ArraySize;
	DXGI_FORMAT Format;
	DXGI_SAMPLE_DESC SampleDesc;
	D3D11_USAGE Usage;
	UINT BindFlags;
	UINT CPUAccessFlags;
	UINT MiscFlags;
	*/


// GPU - constant buffer struct for materials
struct SSurfaceMaterial
{
	SVector3	Diffuse;
	float		Alpha;

	SVector3	Specular;
	float		Roughness;

	float		Metalness;
	float		Shininess;
	SVector2	Tiling;


	// bit 0: hasDiffuseMap
	// bit 1: hasNormalMap
	// bit 2: hasSpecularMap
	// bit 3: hasAlphaMask
	int32 TextureConfig;
	int32 pad0, pad1, pad2;

	SVector3 Emissive;
	float EmissiveIntensity;

	//float	BRDF_OR_PHONG;
};

struct STextureInfo
{
	SVector3 diffuse;
	float alpha;

	float isDiffuseMap;
	SVector3 pad;
};

struct SVertex_Static
{
	SVector3 Position;
	SVector3 Normal;
	SVector3 Tangent;
	SVector2 UV;
};

struct SVertex
{
	SVector3 Position;
	SVector3 Normal;
	SVector3 Tangent;
	SVector2 UV;

	SVector4	BlendWeight;
	SVector4	BlendIndex;
};

struct SVertex_PUV
{
	SVector3 Position;
	SVector2 UV;
};


struct SAntiAliasing
{
	//SSAA(SuperSampling_Antialiasing)
	//MSAA(Multi-Sampling_Antialiasing)
	//FXAA()/...


	float UpscaleFactor = 1.0f;
	uint32 ResolutionX = 0;
	uint32 ResolutionY = 0;
};

struct STextureDesc
{
	int32 Width;
	int32 Height;
	DXGI_FORMAT Format;
	ETextureUsage Usage;
	std::string	 FileName;	//FromFile

	int32 mipCount;
	int32 arraySize;
	bool bIsCubeMap;
	bool bGenerateMips;
	ECPUAccess cpuAccessMode;

	void* pData;
	int32 dataPitch;		// used for 2D/3D Textures - how many bytes per row of pixels
	int32 dataSlicePitch;	// used for 3D Textures

	STextureDesc() :
		Width(1),
		Height(1),
		Format(DXGI_FORMAT_R32G32B32A32_FLOAT),
		Usage(RESOURCE),
		FileName(""),
		pData(nullptr),
		dataSlicePitch(0),
		mipCount(1),
		arraySize(1),
		bIsCubeMap(false),
		bGenerateMips(false),
		cpuAccessMode(ECPUAccess::NONE)
	{
	}

	D3D11_TEXTURE2D_DESC dxDesc;
};

struct SBufferDesc
{
	EBufferType  BufferType = EBufferType::BUFFER_TYPE_UNKNOWN;
	EBufferUsage Usage = EBufferUsage::GPU_READ_WRITE;
	ECPUAccess	CPUAccess = ECPUAccess::NONE;
	uint32		ElementCount = 0;
	uint32		Stride = 0;


	UINT32		MiscFlag = 0;
};


struct SRenderTargetDesc
{
	STextureDesc TextureDesc;
	DXGI_FORMAT Format;
};

struct SRenderTarget
{
	ID3D11Resource* GetTextureResource() const
	{ 
		if(Texture)
			return Texture->m_Tex2D;

		return nullptr;
	}

	D3DTexture*				Texture = nullptr;
	ID3D11RenderTargetView* RTV = nullptr;
};

struct SDepthTargetDesc
{
	STextureDesc		TextureDesc;
	DXGI_FORMAT			Format;
};

struct SDepthTarget
{
	ID3D11Resource* GetTextureResource() const
	{
		if (Texture)
			return Texture->m_Tex2D;

		return nullptr;
	}
	FTextureID GetTextureID() const 
	{ 
		if (Texture)
			return Texture->m_ID;

		return INDEX_NONE;
	}

	D3DTexture*				Texture = nullptr;
	ID3D11DepthStencilView* DSV		 = nullptr;
};


struct SPipelineState
{
	//Shader
	FShaderID				ShaderID = INDEX_NONE;

	//IA
	FBufferID				VertexBufferID = INDEX_NONE;
	FBufferID				IndexBufferID = INDEX_NONE;
	EPrimitiveTopology		Topology;

	//RS
	D3D11_VIEWPORT			ViewPort;
	FRasterizerStateID		RasterizerStateID;

	
	FDepthStencilStateID	DepthStencilStateID;
	FBlendStateID			BlendStateID;	
	FDepthTargetID			DepthTargetID;

	//Can MultipleRenderTarget -> Make Array
	FRenderTargetIDs		RenderTargets;
};


struct SBoneInfo
{
	std::string			Name;
	//int32				Depth;
	int32				ParentIdx;
	Matrix				matOffset;
	Matrix				matBone;

	SBoneInfo()
	{

	}
};



//For Shader's CBuffer

struct SObjectMatrices
{
	Matrix World;
	Matrix Normal;
	Matrix WorldViewProj;
};

struct SObjectShadowMatrices
{
	Matrix World;
	Matrix Wvp;
};



struct SAnimFrameTrans
{
	SVector4 Translate;
	SVector4 Scale;
	SVector4 Rot;

	SAnimFrameTrans()
	{
		Translate = SVector4::ZeroVector;
		Scale = SVector4(1,1,1,1);
		Rot = SVector4(0,0,0,1);
	}

	Matrix toMatrix() const
	{

		Matrix matTrs = Matrix::StaticTranslation(Translate);
		Matrix matRot = Matrix::StaticRotationQuaternion(Rot);
		Matrix matScl = Matrix::StaticScaling(Scale);

		return matScl * matRot * matScl;	

	}
};
struct SAnimationIfno
{
	int32 AnimBoneCount;
	int32 AnimCurrentFrame;
	int32 AnimNextFrame;
	float AnimRatio;

	int32 AnimFrameCount;
	int32 AnimRowIndex;
	int32 AnimChangeAnimation;
	float AnimChangeRatio;


	int32 AnimChangeFrameCount;
	SVector3 padding;
};

struct SInstanceInfo
{
	SObjectMatrices objMatrics;

	//Per IA Pair 
	SSurfaceMaterial objSurface;

	int32			useSkinning;
	
	SVector3		rootOffset;
};

struct SDepthInstanceInfo
{
	SObjectShadowMatrices objMatrics;
	//Per IA Pair 
	int32			useSkinning;

	SVector3		rootOffset;
};

struct SPointLightBuffer
{
	SVector3 LightPosition;
	float FarPlane;
};

//DirectionalLight
struct SDirectionalLight
{ // 40 bytes
	SVector3 lightDirection;
	float brightness;
	SVector3 color;
	float depthBias;

	int32 shadowing;
	int32 enabled;
	SVector2 pad;
};

//PointLight
struct SPointLight
{ // 48 bytes
	SVector3	position;
	float		range;
	SVector3	color;
	float		brightness;
	SVector3	attenuation;
	float		depthBias;
};

struct SSpotLight
{	
	//-----------------------
	SVector3 position;
	float  halfAngle;
	//-----------------------
	SVector3 color;
	float  brightness;
	//-----------------------
	SVector3 spotDir;
	float depthBias;
	//-----------------------
	float innerConeAngle;
	float dummy;
	float dummy1;
	float dummy2;
};



//1Scene 1 Directional Light  //Send GPU
struct SSceneLighting
{
	// non-shadow caster counts
	int32 numPointLights;
	int32 numSpots;
	// shadow caster counts
	int32 numPointCasters;
	int32 numSpotCasters;
	//----------------------------------------------
	SDirectionalLight directional;
	Matrix shadowViewDirectional;

	SSceneLighting()
	{
		numPointLights = 0;
		numSpots = 0;
		numPointCasters = 0;
		numSpotCasters = 0;
	}

	void Clear()
	{
		numPointLights = 0;
		numSpots = 0;
		numPointCasters = 0;
		numSpotCasters = 0;
	}
};

/** Information about a single contact point during physics collision. */
struct ContactPoint
{
	SVector3 PositionA; /**< Contact point in world space. */
	SVector3 PositionB; /**< Contact point in world space. */
	SVector3 Normal; /**< Normal pointing from the second shape to the first shape. */
	float Impulse; /** Impulse applied to the objects to keep them from penetrating. Divide by simulation step to get the force. */
	float Distance; /**< Determines how far are the objects. Negative value denotes penetration. */

};


struct SCollisionResult
{
	class PrimitiveComponent*	Src;
	class PrimitiveComponent*	Dest;
	std::vector<ContactPoint>	ConstactPoint;


	SCollisionResult() :
		Src(nullptr),
		Dest(nullptr)		
	{}
	SCollisionResult(class PrimitiveComponent* _Src, class PrimitiveComponent* _Dest) :
		Src(_Src)
		, Dest(_Dest)		
	{}
};

struct SHitResult
{
	class PrimitiveComponent* HitComponent =nullptr;
	
	SVector3 point;

	float fraction = 0.0f;
	 
	SVector3 normal;
	bool Hit = false;

};

struct SlineInfo
{
	SVector3 p1;
	SVector3 p2;
	SVector3 color;
};

struct SSolidInfo
{
	SVector3 p1;
	SVector3 p2;
	SVector3 p3;
	SVector3 color;
};


struct SCollisionChannel
{
	std::string			Name;
	ECollision_Channel	Channel;
	ECollision_Interaction	Interaction;

	SCollisionChannel()
		:Channel(ECollision_Channel::Default)
		, Interaction(ECollision_Interaction::Overlap)
	{

	}
};

struct SCollisionProfile
{
	std::string							Name;
	SCollisionChannel* Channel;
	bool								Enable;
	std::vector<ECollision_Interaction>	vecCollisionInteraction;
	std::string							Descrition;

	bool								Defaults;

	SCollisionProfile()
		:Enable(true)
		, Channel(nullptr)
		, Defaults(false)
	{}
};

struct SMeshCollisionData
{
	std::vector<SVector3> vertexData;
	//float* vertexData;
	std::vector<uint32> indexData;

	class btTriangleIndexVertexArray* CollisonVertexArray = nullptr;

	~SMeshCollisionData()
	{
		
	}
};


struct SActionKeyIfno
{
	std::string ActionName;
	bool	Ctrl;
	bool	Shift;
	bool	Alt;

	SActionKeyIfno(const std::string& actionName)
		:ActionName(actionName)
		, Ctrl(false)
		, Shift(false)
		, Alt(false)
	{

	}
	SActionKeyIfno()
		:ActionName("")
		, Ctrl(false)
		, Shift(false)
		, Alt(false)
	{

	}
};



struct SKeyAxisValue
{
	uchar8			Key;
	float					scale = 0.0f;
	bool					Called = false;


	bool	Ctrl;
	bool	Shift;
	bool	Alt;
};

struct SWidgetTransform
{
	SVector2 Translation;

	SVector2 Scale;

	float Angle;


	SWidgetTransform()
		: Translation(SVector2(0.0f))
		, Scale(SVector2(1.0f))
		, Angle(0)
	{}

	Matrix ToMatrixWithScale()
	{
		Matrix tr = Matrix::StaticTranslation(Translation);
		Matrix rot = Matrix::StaticRotationZ(Angle);
		Matrix scale = Matrix::StaticScaling(Scale);
		return scale * rot * tr;
	}

};


struct SScissorsRect
{
	int32 left;
	int32 right;

	int32 bottom;
	int32 top;


	SScissorsRect(int32 _L, int32 _T,int32 _R,int32 _B)
		:left(_L),right(_R),bottom(_B),top(_T)
	{		
	}
	SScissorsRect()
		:left(0)
		, top(0)
		, right(1920)
		, bottom(1080)
	{

	}

	bool operator==(const SScissorsRect& Other)
	{
		return left == Other.left && right == Other.right && top == Other.top && bottom == Other.bottom;
	}

	bool operator!=(const SScissorsRect& Other)
	{
		return !((*this) == Other);
	}
};

struct SParticleInfo
{
	SVector3 WorldPos;
	float Speed;

	SVector3 Dir;
	float LifeTime;

	float LifeTimeMax;
	int32 Alive;

	int32 SpriteIdx;
	float __pad;
	
	//int SpriteIndex; //현재인덱스인데 -> LifeTime에 의한 Ratio로 쓸건지?
};

struct SParticleInfoShared
{
	uint32		SpawnEnable;
	SVector3	ScaleMin;

	int32			SpriteCount;
	SVector3		ScaleMax;

	SVector4	ColorMin;
	SVector4	ColorMax;

	int32 SpriteRow;
	int32 SpriteCol;

	SVector2 __Pad;

	SVector4	 WorldOffset;
};


struct SParticleUpdateInfo
{
	uint32 SpawnEnable; // 파티클 생성여부
	SVector3 StartMin; // 파티클이 생성될 영역의 Min

	SVector3 StartMax; // 파티클이 생성될 영역의 Max    
	uint32 SpawnCountMax; // 생성될 파티클의 최대

	SVector3 ScaleMin; // 생성될 파티클 크기의 Min
	float LifeTimeMin; // 생성될 파티클이 살아있을 최소시간

	SVector3 ScaleMax; // 생성될 파티클 크기의 Max
	float LifeTimeMax; // 생성될 파티클이 살아있을 최대시간


	SVector4 ColorMin; // 생성될 파티클의 색상 Min    
	SVector4 ColorMax; // 생성될 파티클의 색상 Max    

	float SpeedMin; // 파티클의 최소 이동속도
	SVector3 MoveDirMin;

	float SpeedMax; // 파티클의 최대 이동속도
	SVector3 MoveDirMax;


	//ParticleSpriteInfo g_SpriteInfo[10]; //


	int32 SpriteCount;
	int32 KillAllParticle;
	int32 SpriteCol;  //Count
	int32 SpriteRow;
};
