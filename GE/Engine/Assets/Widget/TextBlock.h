#pragma once
#include "Widget.h"
#include "../Font/FontAsset.h"

class TextBlock :public Widget
{
	CLASS(TextBlock, Widget)		

protected:
	TextBlock();
	TextBlock(const TextBlock& Other);
	virtual ~TextBlock();

	virtual void Render(class Renderer* pRenderer) override;	
	

	virtual void Construct(const std::string& InName = "") override;

	void CreateFontGeometry(class Renderer* pRenderer);

	void CreateTextBlockTarget(class Renderer* pRenderer);

public:
	void SetText(const std::string& InText);

private:
	//UV 데이터를 가진 Texture 가 필요하다면 SpriteAsset을 제작할것.
	PROP(TSharedPtr<FontAsset>, Font, DetailsEdit)
		TSharedPtr<FontAsset> Font;

	PROP(SLinearColor, FontColor, DetailsEdit)
	SLinearColor	FontColor;

	PROP(std::string, Text, DetailsEdit)
	std::string Text;
	
	PROP(int32 , FontSize, DetailsEdit)
	int32 FontSize;

	PROP(float, FontScale, DetailsEdit)
	float FontScale;

	PROP(TEnum<ETextAlignment>, TextAlignemt, DetailsEdit)
	TEnum<ETextAlignment>	TextAlignemt;

	//Font VertexBuffer pos2,uv2 not Indexed
	FBufferID QuadTexBuffer;	

	FRenderTargetID	TextTarget;

	float RenderedScale;
	std::string RenderedText;
	SLinearColor RenderedColor;
	SVector2	RenderPos;
	int32		RenderedTextSize;
	int32		 RenderedAlignment;

	static std::vector<SVertex_PUV>		m_RendertedTextVertex;
};

