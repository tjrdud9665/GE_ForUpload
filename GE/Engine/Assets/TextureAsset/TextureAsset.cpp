#include "EnginePCH.h"
#include "TextureAsset.h"
#include "../../FileSystem/FileSystem.h"
#include "../../Renderer/Renderer.h"

TextureAsset::TextureAsset()
	:m_TextureID(INDEX_NONE)
{

}

TextureAsset::TextureAsset(const TextureAsset& Other)
	:Super(Other)
	,m_TextureID(Other.m_TextureID)
{

}

TextureAsset::~TextureAsset()
{

}

bool TextureAsset::PostLoad()
{
	bool bResult = Super::PostLoad();
	if (m_TextureID != INDEX_NONE)
	{
		return true;
	}

	fs::path rawFilePath = FileSystem::FindRawFile(GetInfo().GetSPath());
	std::string textureName = rawFilePath.filename().string();

	fs::path OnlyPath = rawFilePath;
	OnlyPath = OnlyPath.remove_filename();

	if (!rawFilePath.empty())
	{
		if(rawFilePath.extension() == ".HDR" || rawFilePath.extension() == ".hdr")
			m_TextureID = g_Engine->Get<Renderer>()->CreateHDRTexture(textureName, OnlyPath.string());
		else
		{
			m_TextureID = g_Engine->Get<Renderer>()->CreateTextureFromFile(textureName, OnlyPath.string());			
		}

	}

	return bResult;
}

ID3D11ShaderResourceView* TextureAsset::GetResource() const
{
	if (m_TextureID == INDEX_NONE)
		return nullptr;

	return	g_Engine->Get<Renderer>()->GetTextureSRV(m_TextureID);	
}
