#pragma once
#include "GameFramework/Components/SkyComponent.h"

using FDepthTargetIDArray = std::vector<FDepthTargetID>;

//Rendering 은 수명주기에 관여x
using FRenderingList = std::vector<class SceneComponent*>;
using FInstancingRenderList = std::unordered_map<class MeshAsset*, FRenderingList>;
using FLightingList = std::vector<class LightComponent*>;

using FParticleList = std::vector<class ParticleComponent*>;


struct SSceneView
{
	Matrix		matView;
	Matrix		matViewProj;
	Matrix		matViewInverse;
	Matrix		matProj;
	Matrix		matProjInverse;
	Matrix		DirectionalLightProjection;

	SVector3	CameraPosition;
	//bool			bIsPBRLightingUsed;
	//bool			bIsDeferredRendering;


	FRenderingList OpaqueList;
	FRenderingList AlphaList;

	FParticleList	ParticleList;

	//Instancing Per Mesh Asset	
	FInstancingRenderList InstanceList;

	FLightingList	LightingList;
	int32			PointLightCount;
	int32			SpotLightCount;	

	int32			PointLightCasterCount;

	class SkyComponent* SceneSky;
	//bool			bIsIBLEnabled;
	EnvironmentMap	environmentMap;

	float			AmbientFactor;

	float			SSAOIntensity;
	float			Radius;
	FTextureID		AOTex;

	SSceneView()
	{
		//OpaqueList.clear();
		//AlphaList.clear();
		PointLightCount = 0;
		SpotLightCount= 0;
		AmbientFactor = 1.0f;
		SceneSky = nullptr;

		SSAOIntensity = 3.40f;
		Radius = 17.5f;
		AOTex = INDEX_NONE;
	}

};

//using FLightRenderListLookup = std::unordered_map<const LightComponent*, FRenderingList>;
//using FLightInstancedRenderListLookup = std::unordered_map<const LightComponent*, RenderListLookup>;

struct SFrameLightingData
{
	std::vector<SPointLight> PointLights;
	std::vector<SPointLight> PointLightCasters;
	std::vector<SSpotLight> SpotLights;
	SDirectionalLight	directionalLight;
	//Type 별로 나눠서 사이즈할당할것인지?
	//Lighting Info 를 하나의 구조체로 통합할것인지?
	SSceneLighting sceneLightings;
};

struct SShadowView
{
	FLightingList	Spots;
	FLightingList	Points;

	Matrix			viewMatrix;
	class LightComponent* Directional;

	//DirectionalLight 
	FRenderingList Casters;
	
	FInstancingRenderList	Casters_Inst;
	//Instancing Render List Up

	//FRenderListLookup RenderListsPerMeshType;

	// culled render lists per shadowing light
	//FLightRenderListLookup shadowMapRenderListLookUp;
	//LightInstancedRenderListLookup shadowMapInstancedRenderListLookUp;

	// mesh render list (to replace other render lists which are in object-level)
	//PointLightMeshDrawListLookup shadowCubeMapMeshDrawListLookup;

	void Clear()
	{
		Casters_Inst.clear();
		Spots.clear();
		Points.clear();
		Casters.clear();
		Directional = nullptr;
	}
	
};


struct SGBuffer
{
	bool				 Initialized;
	FRenderTargetID		RTDiffuseRoughness;
	FRenderTargetID		RTSpecularMetallic;
	FRenderTargetID		RTNormals;
	FRenderTargetID		RTEmissive;

	SGBuffer()
		:Initialized(false)
		, RTDiffuseRoughness(INDEX_NONE)
		, RTSpecularMetallic(INDEX_NONE)
		, RTNormals(INDEX_NONE)
		, RTEmissive(INDEX_NONE)
	{

	}
};



struct SDeferredRenderingPasses
{
	SDeferredRenderingPasses()
		:ShaderRenderTarget(INDEX_NONE)
		, UseDepthPrepass(false)
	{
		/*GeometryStencilState(INDEX_NONE)
		,GeometryStencilStatePreZ(INDEX_NONE)
		,GeometryShader(INDEX_NONE)
		,GeometryInstancedShader(INDEX_NONE)
		,AmbientShader(INDEX_NONE)
		,AmbientIBLShader(INDEX_NONE)
		, AmbientRenderTarget(INDEX_NONE)
		, blurSmp(INDEX_NONE)
		*/
	}

	void Initialize(class Renderer* pRenderer, bool bAAResolve,const DXGI_FORMAT& targetFormet = DXGI_FORMAT_R32G32B32A32_FLOAT);
	void InitializeGBuffer(class Renderer* pRenderer);

	void ClearGBuffer(class Renderer* pRenderer);
	//void RenderGBuffer(class Renderer* pRenderer, const Scene* pScene, const SceneView& sceneView) const;

	/* 이곳에서 모든 등록된 그릴 물체를 취합하여 GBuffer에 씁니다.*/
	void RenderGBuffer(class Renderer* pRenderer,const SSceneView& sceneView) const;
	
	void RenderLightingPass(class Renderer* pRenderer,class RenderManager* pRenderManager,const SSceneView& sceneView , const SFrameLightingData& data) const;

	void RenderToneMapper(class Renderer* pRenderer,FRenderTargetID FinalTarget);

	void RenderParticles(class Renderer* pRenderer, class RenderManager* pRenderManager, const SSceneView& sceneView,FRenderTargetID FinalTarget) const;


	SGBuffer			GBuffer;
	FRenderTargetID		ShaderRenderTarget;

	FRenderTargetID		AmbientRenderTarget;

	static FDepthStencilStateID	GeometryStencilState;
	static FDepthStencilStateID	GeometryStencilStatePreZ;

	static FShaderID				GeometryShader;
	static FShaderID				GeometryInstancedShader;
	static FShaderID				AmbientShader;
	static FShaderID				AmbientIBLShader;
	static FShaderID				BRDFLightingShader;	
	static FShaderID		_toneMappingShaderHDR;	
	static FSamplerID blurSmp;	



	float exposure = 1.2f;

	static bool		ShaderInitilized;
	

	
	
	//https://scahp.tistory.com/63
	bool					UseDepthPrepass;

};