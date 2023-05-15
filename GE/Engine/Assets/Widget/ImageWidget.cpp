#include "EnginePCH.h"
#include "ImageWidget.h"
#include <UI/UIManager.h>

ImageWidget::ImageWidget()
{

}

ImageWidget::ImageWidget(const ImageWidget& Other)
	:Super(Other)
	, ImageTexture(Other.ImageTexture)
{

}

ImageWidget::~ImageWidget()
{

}

void ImageWidget::Render(class Renderer* pRenderer)
{
	if(Slot)
	{
		if (ImageTexture)
		{
			m_CachedManager->PushSlot(Slot, ImageTexture->GetTextureID());

			D3DTexture* texObj = pRenderer->GetTextureObject(ImageTexture->GetTextureID());
			DesiredSize = SVector2(texObj->GetWidth(), texObj->GetHeight());
		}
		else
			m_CachedManager->PushSlot(Slot, INDEX_NONE);

	}

}



