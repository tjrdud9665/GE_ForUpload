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
	//UV �����͸� ���� Texture �� �ʿ��ϴٸ� SpriteAsset�� �����Ұ�.
	PROP(TSharedPtr<TextureAsset>, ImageTexture, DetailsEdit)
	TSharedPtr<TextureAsset> ImageTexture;
};

