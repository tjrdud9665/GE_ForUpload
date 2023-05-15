#pragma once
#include "../AssetData.h"

class TextureAsset :public AssetData
{
	CLASS(TextureAsset,AssetData)

protected:
	TextureAsset();
	TextureAsset(const TextureAsset& Other);
	virtual ~TextureAsset();
	
	virtual bool PostLoad() override;

	//Use Only Editor..
public:
	ID3D11ShaderResourceView* GetResource() const;

	FTextureID GetTextureID() const { return m_TextureID; }


private:
	FTextureID		m_TextureID;




};

