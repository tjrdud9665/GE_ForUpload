#include "EnginePCH.h"
#include "HorizontalBoxSlot.h"
#include "HorizontalBox.h"

HorizontalBoxSlot::HorizontalBoxSlot()
	:Left(0)
	, Top(0)
	, Right(0)
	, Bottom(0)
	, HorizontalAlignment(EHorizontalAlignment::HAlign_Fill)
	, VerticalAlignment(EVerticalAlignment::VAlign_Fill)
{
	
}

HorizontalBoxSlot::HorizontalBoxSlot(const HorizontalBoxSlot& Other)
	:Super(Other)
	, HorizontalAlignment(Other.HorizontalAlignment)
	, VerticalAlignment(Other.VerticalAlignment)
	, Left(Other.Left)
	, Top(Other.Top)
	, Right(Other.Right)
	, Bottom(Other.Bottom)
{

}

HorizontalBoxSlot::~HorizontalBoxSlot()
{

}

void HorizontalBoxSlot::SetupGeometry(class Renderer* pRenderer)
{
	SVector2 parentSize = pRenderer->FrameRenderTargetDimensionsAsFloat2();
	SVector2 scrSize = parentSize;
	//1		2
	//
	//0

	//		4
	//
	//3		5

	SVector2 parentPos = SVector2(0, 0);
	SVector2 parentRenderedSize = SVector2(0, 0);
	if (Parent)
	{
		parentPos = Parent->GetLeftTop();
		parentRenderedSize = Parent->GetRenderedSize();
		PanelSlot* panelSlot = Parent->GetPanelSlot();
		if (panelSlot)
		{

			parentSize = Parent->GetPanelSlot()->GetParent()->GetRenderedSize();

			if (panelSlot->GetContent())
				parentRenderedSize = panelSlot->GetContent()->GetRenderedSize();
		}



	}

	SVector2 contentDesiredSize = Content->GetDesiredSize();

	//gridSlot이 가지는 최대 크기 
	SVector2 slotMaxSize = Parent->GetRenderedSize();
	int32 idx = Parent->GetChildIndex(Content);

	//Parent->GetMaxDesiredSize();
	
	int32 colCnt = std::max(1, Parent->GetChildrenCount());//ZeroDiv
	
	slotMaxSize.x = SMathUtils::Clamp(Parent->GetMaxDesiredSize().x, 0.0f, parentRenderedSize.x / colCnt);
	slotMaxSize.y = SMathUtils::Clamp(Parent->GetMaxDesiredSize().y, 0.0f, parentRenderedSize.y);

	//각 Slot 의 Center


	//Left Top
	SVector2 desiredSize =
		SVector2(SMathUtils::Clamp(contentDesiredSize.x, 0.0f, slotMaxSize.x)
				 , SMathUtils::Clamp(contentDesiredSize.y, 0.0f, slotMaxSize.y));



	if (HorizontalAlignment.Get() == EHorizontalAlignment::HAlign_Fill)
	{
		desiredSize.x = slotMaxSize.x;
	}
	if (VerticalAlignment.Get() == EVerticalAlignment::VAlign_Fill)
	{
		desiredSize.y = slotMaxSize.y;
	}
	Content->SetRenderedSize(desiredSize);
	desiredSize *= 2.0f;

	

	SVector2 lt = SVector2((idx)*slotMaxSize.x * 2.0f - scrSize.x + parentPos.x,
						   (scrSize.y + parentPos.y));



	SVector2 center = SVector2(lt.x + slotMaxSize.x, lt.y - slotMaxSize.y);
	SVector2 rightBot = SVector2(lt.x + slotMaxSize.x * 2.0f, +lt.y - slotMaxSize.y * 2.0f);

	float x2 = lt.x + desiredSize.x;
	float y2 = -lt.y + desiredSize.y;

	SVector2 upLeft(lt.x, lt.y);
	SVector2 upRight(x2, lt.y);
	SVector2 downLeft(lt.x, -y2);
	SVector2 downRight(x2, -y2);

	if (HorizontalAlignment.Get() == EHorizontalAlignment::HAlign_Center)
	{
		upLeft.x = center.x - desiredSize.x * 0.5f;
		upRight.x = center.x + desiredSize.x * 0.5f;
		downLeft.x = center.x - desiredSize.x * 0.5f;
		downRight.x = center.x + desiredSize.x * 0.5f;
	} else if (HorizontalAlignment.Get() == EHorizontalAlignment::HAlign_Right)
	{
		upLeft.x = rightBot.x - desiredSize.x;
		upRight.x = rightBot.x;
		downLeft.x = rightBot.x - desiredSize.x;
		downRight.x = rightBot.x;
	}

	if (VerticalAlignment.Get() == EVerticalAlignment::VAlign_Center)
	{
		upLeft.y = center.y + desiredSize.y * 0.5f;
		upRight.y = center.y + desiredSize.y * 0.5f;
		downLeft.y = center.y - desiredSize.y * 0.5f;
		downRight.y = center.y - desiredSize.y * 0.5f;
	} else if (VerticalAlignment.Get() == EVerticalAlignment::VAlign_Bottom)
	{
		upLeft.y = rightBot.y + desiredSize.y;
		upRight.y = rightBot.y + desiredSize.y;
		downLeft.y = rightBot.y;
		downRight.y = rightBot.y;
	}


	{
		m_Vertices[0].Position = downLeft + SVector2(Left * 2.0f, Bottom * 2.0f);
		m_Vertices[1].Position = upLeft + SVector2(Left * 2.0f, -Top * 2.0f);
		m_Vertices[2].Position = upRight + SVector2(-Right * 2.0f, -Top * 2.0f);
		////0
		m_Vertices[3].Position = m_Vertices[0].Position;
		////2
		m_Vertices[4].Position = m_Vertices[2].Position;
		////3
		m_Vertices[5].Position = downRight + SVector2(-Right * 2.0f, Bottom * 2.0f);
	}


	for (int32 i = 0; i < 6; i++)
	{
		m_Vertices[i].Position /= scrSize;
		m_Vertices[i].Position.z = 0;
	}
}

