#include "EnginePCH.h"
#include "CanvasPanelSlot.h"

CanvasPanelSlot::CanvasPanelSlot()
	:SizeToContent(false)
{

}

CanvasPanelSlot::CanvasPanelSlot(const CanvasPanelSlot& Other)
	:Super(Other)
	,LayoutData(Other.LayoutData)
	, SizeToContent(Other.SizeToContent)
{

}

CanvasPanelSlot::~CanvasPanelSlot()
{

}




void CanvasPanelSlot::SetupGeometry(class Renderer* pRenderer)
{
	//	  1	+-----+ 2	0, 1, 2
	//		|	  |		2, 3, 0
	//		|	  |		
	//	  0 +-----+ 3	
	//     size		

	//LayoutData.Offsets
	SVector2 scrSize = pRenderer->FrameRenderTargetDimensionsAsFloat2();

	//0~~1920
	//-1~~1	
	SVector2 parentPos = SVector2(0, 0);
	if(Parent)
	{
		parentPos = Parent->GetLeftTop();
	}


	//0~~ 1920
	//leftTop
	SVector2 postion = SVector2(LayoutData.Offsets.Left + parentPos.x, -LayoutData.Offsets.Top - parentPos.y);
	SVector2 desiredSize = SVector2(LayoutData.Offsets.Right - LayoutData.Offsets.Left, LayoutData.Offsets.Bottom - LayoutData.Offsets.Top );
	

	if(SizeToContent)
	{
		desiredSize = Content->GetDesiredSize();		
	}
	

	Content->SetRenderedSize(desiredSize);
	//Center
	//postion.x += desiredSize.x * 0.5f;
	//postion.y -= desiredSize.y * 0.5f;

	float x2 = postion.x + desiredSize.x;
	float y2 = -postion.y + desiredSize.y;

	SVector2 upLeft(postion.x, postion.y);
	SVector2 upRight(x2, postion.y);
	SVector2 downLeft(postion.x, -y2);
	SVector2 downRight(x2, -y2);
	
	//1		2
	//	
	//0		3
	m_Vertices[0].Position = SVector3(downLeft, 0.0f);
	m_Vertices[1].Position = SVector3(upLeft, 0.0f);
	m_Vertices[2].Position = SVector3(upRight, 0.0f);

	////0
	m_Vertices[3].Position = m_Vertices[0].Position;
	////2
	m_Vertices[4].Position = m_Vertices[2].Position;
	////3
	m_Vertices[5].Position = SVector3(downRight, 0.0f);

	
	for(int32 i =0;i<6;i++)
	{
		//0~~ 2
		m_Vertices[i].Position /= scrSize*0.5f;
		//-1~~1
		m_Vertices[i].Position.x -= 1.0f;
		m_Vertices[i].Position.y += 1.0f;

		m_Vertices[i].Position.z = 0;
	}
	
}


