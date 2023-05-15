#include "EnginePCH.h"
#include "Material.h"
#include "../../Renderer/Renderer.h"


Material::Material()
	: m_Diffuse(SVector3(1.0f))
	, m_Opacity(1.0f)
	, m_Specular(SVector3(1.0f))
	, m_Tiling(SVector2(1, 1))
	, m_EmissiveColor(SVector3(1.0f))
	, m_EmissiveIntensity(1.0f)
	, m_DiffuseMap(nullptr)
	, m_NormalMap(nullptr)
	, m_HeightMap(nullptr)
	, m_SpecularMap(nullptr)
	, m_AlphMask(nullptr)
	, m_RoughnessMap(nullptr)
	, m_MetallicMap(nullptr)
	, m_EmissiveMap(nullptr)
	, m_Shininess(0.0f)
	, m_Roughness(0.0f)
	, m_Metalness(0.0f)
{

}

Material::Material(const Material& Other)
	: m_Diffuse(Other.m_Diffuse)
	, m_Opacity(Other.m_Opacity)
	, m_Specular(Other.m_Specular)
	, m_Tiling(Other.m_Tiling)
	, m_EmissiveColor(Other.m_EmissiveColor)
	, m_EmissiveIntensity(Other.m_EmissiveIntensity)
	, m_DiffuseMap(Other.m_DiffuseMap)
	, m_NormalMap(Other.m_NormalMap)
	, m_HeightMap(Other.m_HeightMap)
	, m_SpecularMap(Other.m_SpecularMap)
	, m_AlphMask(Other.m_AlphMask)
	, m_RoughnessMap(Other.m_RoughnessMap)
	, m_MetallicMap(Other.m_MetallicMap)
	, m_EmissiveMap(Other.m_EmissiveMap)	
	, m_Shininess(Other.m_Shininess)
	, m_Roughness(Other.m_Roughness)
	, m_Metalness(Other.m_Metalness)
{

}

Material::~Material()
{

}

bool Material::PostLoad()
{
	bool bResult = Super::PostLoad();


	return bResult;
}

void Material::SetMaterialConstants(Renderer* renderer, EShaders shader) const
{
	switch (shader)
	{
	case EShaders::NORMAL:
		renderer->SetConstant2f("uvScale", m_Tiling);
		renderer->SetConstant1f("isNormalMap", m_NormalMap == nullptr ? 0.0f : 1.0f);
		if (m_NormalMap != nullptr) 
			renderer->SetTexture("texNormalMap", m_NormalMap->GetTextureID());
		if (m_AlphMask != nullptr)	
			renderer->SetTexture("texAlphaMask", m_AlphMask->GetTextureID());
		break;
	case EShaders::UNLIT:
		renderer->SetConstant3f("diffuse", m_Diffuse.ToVector3());
		renderer->SetConstant1f("isDiffuseMap", m_DiffuseMap == nullptr ? 0.0f : 1.0f);
		if (m_DiffuseMap)
			renderer->SetTexture("texDiffuseMap", m_DiffuseMap->GetTextureID());
		break;
	default:
		if (m_DiffuseMap)	
			renderer->SetTexture("texDiffuseMap", m_DiffuseMap->GetTextureID());
		if (m_NormalMap)		
			renderer->SetTexture("texNormalMap", m_NormalMap->GetTextureID());
		if (m_SpecularMap)	
			renderer->SetTexture("texSpecularMap", m_SpecularMap->GetTextureID());
		if (m_AlphMask)			
			renderer->SetTexture("texAlphaMask", m_AlphMask->GetTextureID());
		if (m_MetallicMap)	
			renderer->SetTexture("texMetallicMap", m_MetallicMap->GetTextureID());
		if (m_RoughnessMap)
			renderer->SetTexture("texRoughnessMap", m_RoughnessMap->GetTextureID());
		if (m_EmissiveMap)
			renderer->SetTexture("texEmissiveMap", m_EmissiveMap->GetTextureID());
		break;
	}


	switch (shader)
	{
	case EShaders::NORMAL:
		renderer->SetConstant2f("uvScale", m_Tiling);
		renderer->SetConstant1i("textureConfig", GetTextureConfig());
		break;
	case EShaders::UNLIT:
		break;
	default:
		const SSurfaceMaterial mat = GetCBufferData();
		renderer->SetConstantStruct("surfaceMaterial", &mat);	
		break;
	}
}

bool Material::IsTransparent() const
{
	return m_Opacity != 1.0f;
}

int32 Material::GetTextureConfig() const
{
	int32 textureConfig = 0;
	textureConfig |= m_DiffuseMap		== nullptr ? 0 : (1 << 0);
	textureConfig |= m_NormalMap		== nullptr ? 0 : (1 << 1);
	textureConfig |= m_SpecularMap		== nullptr ? 0 : (1 << 2);
	textureConfig |= m_AlphMask			== nullptr ? 0 : (1 << 3);
	textureConfig |= m_RoughnessMap		== nullptr ? 0 : (1 << 4);
	textureConfig |= m_MetallicMap		== nullptr ? 0 : (1 << 5);
	textureConfig |= m_HeightMap		== nullptr ? 0 : (1 << 6);
	textureConfig |= m_EmissiveMap		== nullptr ? 0 : (1 << 7);
	return textureConfig;

}

SSurfaceMaterial Material::GetCBufferData() const
{
	SSurfaceMaterial cbMaterial;

	cbMaterial.Diffuse = m_Diffuse.ToVector3();
	cbMaterial.Alpha= m_Opacity;

	cbMaterial.Specular= m_Specular.ToVector3();
	cbMaterial.Roughness = m_Roughness;				//Use Only BRDF
	cbMaterial.Metalness = m_Metalness;
	cbMaterial.Shininess = m_Shininess;				//Use Only Phong
	cbMaterial.Tiling = m_Tiling;
	cbMaterial.TextureConfig = GetTextureConfig();
	cbMaterial.Emissive = m_EmissiveColor.ToVector3();
	cbMaterial.EmissiveIntensity = m_EmissiveIntensity;

	return cbMaterial;
}

void Material::Clear()
{
	m_Diffuse = SVector3(1.0f);
	m_Opacity = 1.0f;
	m_Specular = SVector3(1.0f);
	m_Shininess = 0.0f;
	m_Metalness = 0.0f;
	m_Roughness = 0.0f;
	m_EmissiveColor = SVector3(1.0f);
	m_EmissiveIntensity = 0.0f;
	m_DiffuseMap = nullptr;
	m_NormalMap = nullptr;
	m_AlphMask = nullptr;
	m_RoughnessMap = nullptr;
	m_SpecularMap = nullptr;
	m_MetallicMap = nullptr;
	m_EmissiveMap = nullptr;
	m_EmissiveIntensity = 1.0f;
}

SSurfaceMaterial Material::GetDefaultMaterialCBufferData()
{
	Material m; 
	m.Clear();
	return m.GetCBufferData();
}
