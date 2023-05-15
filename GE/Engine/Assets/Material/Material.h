#pragma once

#include "../AssetData.h"
#include "../TextureAsset/TextureAsset.h"




class Material : public AssetData
{
	CLASS(Material, AssetData)
	friend class MaterialManager;

protected:
	Material();
	Material(const Material& Other);
	virtual ~Material();

	virtual bool PostLoad()			override;

public:
	void SetMaterialConstants(Renderer* renderer, EShaders shader) const;
	bool IsTransparent() const;
	int32 GetTextureConfig() const;
	inline bool HasTexture() const { return GetTextureConfig() != 0; }

	virtual SSurfaceMaterial GetCBufferData() const;
	virtual void Clear();

	static SSurfaceMaterial GetDefaultMaterialCBufferData();

	//Setter
	void SetDiffuseColor(const SLinearColor& InDiffuse)
	{
		m_Diffuse = InDiffuse;
	}
	//Opacity
	void SetOpacity(float InOpacity)
	{
		m_Opacity = InOpacity;
	}

	void SetSpecularColor(const SLinearColor& InSpecular)
	{
		m_Specular = InSpecular;
	}

	void SetShininess(float InShininess)
	{
		m_Shininess = InShininess;
	}

	void SetRoughness(float InRoughness)
	{
		m_Roughness = InRoughness;
	}

	void SetMetalness(float InMetalness)
	{
		m_Metalness = InMetalness;
	}

	void SetTiling(const SVector2& InTiling)
	{
		m_Tiling = InTiling;
	}

	void SetUV_Bias(const SVector2& InUv_bias)
	{
		m_Uv_bias = InUv_bias;
	}

	void SetEmissiveColor(const SLinearColor& InEmissiveColor)
	{
		m_EmissiveColor = InEmissiveColor;
	}

	void SetEmissiveIntensity(float InEmissiveIntensity)
	{
		m_EmissiveIntensity = InEmissiveIntensity;
	}

	//Texture Params//////////////////////////////////////////////////////////////////////////
	void SetDiffuseMap(class TextureAsset* InDiffuseMap)
	{
		m_DiffuseMap = InDiffuseMap;
	}	

	void SetNormalMap(class TextureAsset* InNormalMap)
	{
		m_NormalMap = InNormalMap;
	}

	void SetSpecularMap(class TextureAsset* InSpecularMap)
	{
		m_SpecularMap = InSpecularMap;
	}

	void SetAlphaMap(class TextureAsset* InAlphMask)
	{
		m_AlphMask = InAlphMask;
	}

	void SetHeightMap(class TextureAsset* InHeightMap)
	{
		m_HeightMap = InHeightMap;
	}

	void SetRoughnessMap(class TextureAsset* InRoughnessMap)
	{
		m_RoughnessMap = InRoughnessMap;
	}

	void SetMetallicMap(class TextureAsset* InMetallicMap)
	{
		m_MetallicMap = InMetallicMap;
	}

	void SetEmissiveMap(class TextureAsset* InEmissiveMap)
	{
		m_EmissiveMap = InEmissiveMap;
	}

	class TextureAsset* GetDiffuseTextureAsset() const { return m_DiffuseMap; }
	class TextureAsset* GetNormalAsset() const { return m_NormalMap; }
	class TextureAsset* GetSpecularTextureAsset() const { return m_SpecularMap; }
	class TextureAsset* GetAlphaTextureAsset() const { return m_AlphMask; }
	class TextureAsset* GetHeightTextureAsset() const { return m_HeightMap; }

	class TextureAsset* GetRoughnessTextureAsset() const { return m_RoughnessMap; }
	class TextureAsset* GetMetallicTextureAsset() const { return m_MetallicMap; }

	class TextureAsset* GetEmissiveTextureAsset() const { return m_EmissiveMap; }


private:
	PROP(TSharedPtr<TextureAsset>, m_DiffuseMap, DetailsEdit, "MaterialTexture")
	TSharedPtr<TextureAsset> m_DiffuseMap;

	PROP(TSharedPtr<TextureAsset>, m_NormalMap, DetailsEdit, "MaterialTexture")
	TSharedPtr<TextureAsset> m_NormalMap;

	PROP(TSharedPtr<TextureAsset>, m_SpecularMap, DetailsEdit, "MaterialTexture")
	TSharedPtr<TextureAsset> m_SpecularMap;

	PROP(TSharedPtr<TextureAsset>, m_AlphMask, DetailsEdit, "MaterialTexture")
	TSharedPtr<TextureAsset> m_AlphMask;

	PROP(TSharedPtr<TextureAsset>, m_HeightMap, DetailsEdit, "MaterialTexture")
	TSharedPtr<TextureAsset> m_HeightMap;


	PROP(TSharedPtr<class TextureAsset>, m_RoughnessMap, DetailsEdit, "MaterialTexture")
	TSharedPtr<class TextureAsset> m_RoughnessMap;

	PROP(TSharedPtr<class TextureAsset>, m_MetallicMap, DetailsEdit, "MaterialTexture")
	TSharedPtr<class TextureAsset> m_MetallicMap;

	PROP(TSharedPtr<class TextureAsset>, m_EmissiveMap, DetailsEdit, "MaterialTexture")
	TSharedPtr<class TextureAsset> m_EmissiveMap;

	PROP(SLinearColor, m_Diffuse, DetailsEdit, "MaterialParams")
	SLinearColor		m_Diffuse;

	PROP(float, m_Opacity, DetailsEdit, "MaterialParams")
	float			m_Opacity;

	PROP(SLinearColor, m_Specular, DetailsEdit, "MaterialParams")
	SLinearColor		m_Specular;

	PROP(float, m_Shininess, DetailsEdit, "MaterialParams")
	float			m_Shininess;		//Phong

	PROP(float, m_Metalness, DetailsEdit, "MaterialParams")
	float			m_Metalness;

	PROP(float, m_Roughness, DetailsEdit, "MaterialParams")
	float			m_Roughness;

	PROP(SVector2, m_Tiling, DetailsEdit, "MaterialParams")
	SVector2		m_Tiling;	   

	PROP(SVector2, m_Uv_bias, DetailsEdit, "MaterialParams")
	SVector2		m_Uv_bias;	
		
	PROP(SLinearColor, m_EmissiveColor, DetailsEdit, "MaterialParams")
	SLinearColor		m_EmissiveColor;

	PROP(float, m_EmissiveIntensity, DetailsEdit, "MaterialParams")
	float			m_EmissiveIntensity;
};

