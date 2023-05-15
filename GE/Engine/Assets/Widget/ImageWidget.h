#pragma once
#include "Widget.h"

class ImageWidget : public Widget
{
	CLASS(ImageWidget,Widget)

protected:
	ImageWidget();
	ImageWidget(const ImageWidget& Other);
	virtual ~ImageWidget();


	virtual void Render(class Renderer* pRenderer) override;



private:
	//UV 데이터를 가진 Texture 가 필요하다면 SpriteAsset을 제작할것.
	PROP(TSharedPtr<TextureAsset>, ImageTexture, DetailsEdit)
	TSharedPtr<TextureAsset> ImageTexture;
};

