#pragma once

#include "SceneComponent.h"
#include "../../Assets/TextureAsset/TextureAsset.h"



struct EnvironmentMapFileNames
{
	//std::string skyboxFileName;
	std::string irradianceMapFileName;
	std::string environmentMapFileName;
	//std::string settingsFileName;
};

constexpr size_t PREFILTER_MIP_LEVEL_COUNT = 8;
using MipRenderTargets = std::array<FRenderTargetID, PREFILTER_MIP_LEVEL_COUNT>;

struct sIBLSettings
{
	float gamma;
	float exposure;
	size_t environmentMapSize;
};

class EnvironmentMap
{
public:

	// learn opengl:	https://learnopengl.com/#!PBR/IBL/Specular-IBL	
	static FRenderTargetID		sBRDFIntegrationLUTRT;
	static FShaderID			sBRDFIntegrationLUTShader;
	static FTextureID			sBRDFIntegrationLUTTexture;
	static FTextureID CreateBRDFIntegralLUTTexture(class Renderer* pRenderer);

	// renders pre-filtered environment map texture into mip levels 
	// with the convolution being based on the roughness
	static FShaderID	sPrefilterShader;
	static FShaderID	sRenderIntoCubemapShader;

	static void Initialize(class Renderer* pRenderer);
	static void LoadShaders(class Renderer* pRenderer);

	//--------------------------------------------------------
	// MEMBER INTERFACE
	//--------------------------------------------------------
	EnvironmentMap()
		:irradianceMap(INDEX_NONE)
		, environmentMap(INDEX_NONE)
		, mippedEnvironmentCubemap(INDEX_NONE)
		, prefilteredEnvironmentMap(INDEX_NONE)
		, envMapSampler(INDEX_NONE)
	{

	}
	FTextureID InitializePrefilteredEnvironmentMap(FTextureID specularMap, FTextureID irradienceMap);
	void Initialize(class Renderer* pRenderer, const EnvironmentMapFileNames& files);

	//--------------------------------------------------------
	// DATA
	//--------------------------------------------------------
	FTextureID irradianceMap;
	FTextureID environmentMap;

	FTextureID mippedEnvironmentCubemap;
	FTextureID prefilteredEnvironmentMap;

	FSamplerID envMapSampler;
	sIBLSettings settings;
};

class SkyComponent :public SceneComponent
{
	CLASS(SkyComponent, SceneComponent)

	friend class RenderManager;

protected:
	SkyComponent();
	SkyComponent(const SkyComponent& Other);
	virtual ~SkyComponent();

public:
	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void PostUpdate(float DeltaSeconds) override;

	virtual void Render(class Renderer* pRenderer);


private:
	PROP(TSharedPtr<class TextureAsset> , m_SkyboxTexture,DetailsEdit,"SkyTexture")
	TSharedPtr<class TextureAsset> 				m_SkyboxTexture;

	PROP(TSharedPtr<class TextureAsset>, m_EnvironmentTex, DetailsEdit, "SkyTexture")
	TSharedPtr<class TextureAsset> 				m_EnvironmentTex;

	EnvironmentMap			m_EnvMap;
};

