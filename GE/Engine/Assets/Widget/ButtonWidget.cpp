#include "EnginePCH.h"
#include "ButtonWidget.h"
#include <UI/UIManager.h>


//SingleChild..

ButtonWidget::ButtonWidget()
	:PressedColor(SLinearColor(1, 1, 1, 1))
	, NormalColor(SLinearColor(1, 1, 1, 1))
	, HoveredColor(SLinearColor(1, 1, 1, 1))
{

	const float& size = 1.0f;

	m_ButtonVertices.resize(6);

	// vertices - CW
	m_ButtonVertices[0].Position = SVector3(-size, -size, 0.0f);
	m_ButtonVertices[0].UV = SVector2(0.0f, 1.0f);

	m_ButtonVertices[1].Position = SVector3(-size, +size, 0.0f);
	m_ButtonVertices[1].UV = SVector2(0.0f, 0.0f);

	m_ButtonVertices[2].Position = SVector3(+size, +size, 0.0f);
	m_ButtonVertices[2].UV = SVector2(1.0f, 0.0f);

	//0
	m_ButtonVertices[3].Position = m_ButtonVertices[0].Position;
	m_ButtonVertices[3].UV = m_ButtonVertices[0].UV;
	//2
	m_ButtonVertices[4].Position = m_ButtonVertices[2].Position;
	m_ButtonVertices[4].UV = m_ButtonVertices[2].UV;
	//3
	m_ButtonVertices[5].Position = SVector3(+size, -size, 0.0f);
	m_ButtonVertices[5].UV = SVector2(1.0f, 1.0f);
}

ButtonWidget::ButtonWidget(const ButtonWidget& Other)
	:Super(Other)
	, NormalImage(Other.NormalImage)
	, HoveredImage(Other.HoveredImage)
	, PressedImage(Other.PressedImage)
	, PressedColor(Other.PressedColor)
	, NormalColor(Other.NormalColor)
	, HoveredColor(Other.HoveredColor)
{
	size_t vertexCnt = Other.m_ButtonVertices.size();
	m_ButtonVertices.resize(vertexCnt);
	for (size_t i = 0; i < vertexCnt; i++)
	{
		m_ButtonVertices[i] = Other.m_ButtonVertices[i];

	}


}

ButtonWidget::~ButtonWidget()
{

}

void ButtonWidget::Render(class Renderer* pRenderer)
{
	//DrawButtonImage..
	FTextureID btnImage = INDEX_NONE;
	SVector4	tintColor = SVector4(1.0);
	if(m_Pressed && PressedImage)
	{
		btnImage = PressedImage->GetTextureID();
		tintColor = PressedColor.ToVector4();
	}
	else if (m_MouseHovered && HoveredImage)
	{
		btnImage = HoveredImage->GetTextureID();
		tintColor = HoveredColor.ToVector4();
	}
	else if(NormalImage)
	{
		btnImage = NormalImage->GetTextureID();
		tintColor = NormalColor.ToVector4();
	}
	SetupButtonGeometry(pRenderer);
	if (btnImage != INDEX_NONE)
	{
		SVector4 contentColor = SVector4(TintColor.ToVector3(), RenderOpacity);
		
		m_CachedManager->PushTexturedTri(btnImage, tintColor,m_ButtonVertices.data(),6);
	}


	Super::Render(pRenderer);

	
}

void ButtonWidget::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);

	size_t slotSize = Slots.size();
	SVector2 maxSize = SVector2(1.0f);
	if(slotSize)
	{
		maxSize = Slots[0]->GetContent()->GetDesiredSize();
	}

	DesiredSize = maxSize;
}

void ButtonWidget::SetupButtonGeometry(class Renderer* pRenderer)
{
	SVector2 parentSize = pRenderer->FrameRenderTargetDimensionsAsFloat2();
	SVector2 scrSize = parentSize;
	//1		2
	//
	//0

	//		4
	//
	//3		5

	SVector2 parentPos = GetLeftTop();
	SVector2 parentRenderedSize = GetRenderedSize();
	

	PanelSlot* panelSlot = GetPanelSlot();
	if (panelSlot)
	{
		parentSize = panelSlot->GetParent()->GetRenderedSize();

		if (panelSlot->GetContent())
			parentRenderedSize = panelSlot->GetContent()->GetRenderedSize();
	}



	

	SVector2 contentDesiredSize = GetDesiredSize();
	//Left Top
	SVector2 desiredSize =
		SVector2(SMathUtils::Clamp(contentDesiredSize.x, 0.0f, parentRenderedSize.x)
				 , SMathUtils::Clamp(contentDesiredSize.y, 0.0f, parentRenderedSize.y));

	desiredSize = parentRenderedSize;


	desiredSize *= 2.0f;


	SVector2 lt = SVector2(-scrSize.x + parentPos.x, scrSize.y + parentPos.y);

	SVector2 center = SVector2(lt.x + parentRenderedSize.x, -lt.y + parentRenderedSize.y);
	SVector2 rightBot = SVector2(lt.x + parentRenderedSize.x * 2.0f, +lt.y - parentRenderedSize.y * 2.0f);

	float x2 = lt.x + desiredSize.x;
	float y2 = -lt.y + desiredSize.y;

	SVector2 upLeft(lt.x, lt.y);
	SVector2 upRight(x2, lt.y);
	SVector2 downLeft(lt.x, -y2);
	SVector2 downRight(x2, -y2);

	{
		m_ButtonVertices[0].Position = downLeft;
		m_ButtonVertices[1].Position = upLeft;
		m_ButtonVertices[2].Position = upRight;
		////0
		m_ButtonVertices[3].Position = m_ButtonVertices[0].Position;
		////2
		m_ButtonVertices[4].Position = m_ButtonVertices[2].Position;
		////3
		m_ButtonVertices[5].Position = downRight;
	}



	for (int32 i = 0; i < 6; i++)
	{
		m_ButtonVertices[i].Position /= scrSize;
		m_ButtonVertices[i].Position.z = 0;
	}
}

void ButtonWidget::OnMouseEntered(const SVector2& InMousePos)
{
	if(m_OnButtonHovered)
	{
		m_OnButtonHovered();
	}
}

void ButtonWidget::OnMouseLeaved()
{
	if (m_OnButtonUnHovered)
	{
		m_OnButtonUnHovered();
	}
}

void ButtonWidget::OnMouseButtonDown(const SVector2& InMousePos, char MouseBtn)
{
	if(m_OnButtonPressed)
	{
		m_OnButtonPressed();
	}
}

void ButtonWidget::OnMouseButtonUp(const SVector2& InMousePos, char MouseBtn)
{
	if (m_OnButtonReleased)
	{
		m_OnButtonReleased();
	}

	if(m_MouseHovered)
	{
		if(m_OnButtonClicked)
			m_OnButtonClicked();
	}
}
