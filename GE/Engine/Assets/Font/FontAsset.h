#pragma once
#include "../AssetData.h"
#include "ft2build.h"
#include FT_FREETYPE_H

struct SFontGlyph
{
	FTextureID tex;	// ID handle of the glyph texture
	SVector2 size;		// Size of glyph
	SVector2 bearing;	// Offset from baseline to left/top of glyph
	int32 advance;	// Offset to advance to next glyph
};


class FontAsset :public AssetData
{
	CLASS(FontAsset, AssetData)
	using FFontGlyphMap = std::unordered_map<wchar_t, SFontGlyph>;
	using FFontSizeMap = std::unordered_map<int32, FFontGlyphMap>;

protected:
	FontAsset();
	FontAsset(const FontAsset& Other);
	virtual ~FontAsset();

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual bool PostLoad() override;

	void RegisterTextCache(const wchar_t& c, int32 FontSize);


public:
	
	//void RenderText(const std::string& InText,class Renderer* pRenderer, const SVector2& InPostion,const SVector3& InColor,const FBufferID& fontBuffer );

	SVector2 RenderText(const std::wstring& InText, class Renderer* pRenderer, const SVector2& InRes,const SVector2& InPostion , const SVector3& InColor, float InScale ,ETextAlignment alignment, int32 FontSize, const FBufferID& fontBuffer);

private:
	float GetFontWidth(const std::wstring& InText, float InScale ,int32 FontSize) ;

private:
	class Renderer* m_cachedRenderer;

	FT_Face m_face;

	static FT_Library	m_ftLib;
	
	std::unordered_map<char, SFontGlyph>		m_FontGlyph;

	FFontSizeMap								 m_wFontGlyphPerSize;

	//std::unordered_map<wchar_t, SFontGlyph>		m_wFontGlyph;

	int32				DefulatFontSize;

	bool		m_Loaded;

};

