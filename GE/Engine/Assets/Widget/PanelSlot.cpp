#include "EnginePCH.h"
#include "PanelSlot.h"
#include <UI/UIManager.h>

PanelSlot::PanelSlot()
	:m_GeometryDirty(true)
{
	//	  1	+-----+ 2	0, 1, 2
	//		|	  |		0, 2 ,3
	//		|	  |		
	//	  0 +-----+ 3	
	//     size	

	const float& size = 1.0f;
	
	m_Vertices.resize(6);

	// vertices - CW
	m_Vertices[0].Position	= SVector3(-size, -size, 0.0f);
	m_Vertices[0].UV		= SVector2(0.0f, 1.0f);

	m_Vertices[1].Position = SVector3(-size, +size, 0.0f);
	m_Vertices[1].UV = SVector2(0.0f, 0.0f);

	m_Vertices[2].Position = SVector3(+size, +size, 0.0f);
	m_Vertices[2].UV = SVector2(1.0f, 0.0f);

	//0
	m_Vertices[3].Position = m_Vertices[0].Position;
	m_Vertices[3].UV = m_Vertices[0].UV;
	//2
	m_Vertices[4].Position = m_Vertices[2].Position;
	m_Vertices[4].UV = m_Vertices[2].UV;
	//3
	m_Vertices[5].Position = SVector3(+size, -size, 0.0f);
	m_Vertices[5].UV = SVector2(1.0f, 1.0f);
}

PanelSlot::PanelSlot(const PanelSlot& Other)
	:Super(Other)
{
	m_cachedManager = g_Engine->Get<UIManager>();

	//Copy Geometry Info
	m_GeometryDirty = true;
	m_Vertices.resize(Other.m_Vertices.size());



	size_t vertexCnt = m_Vertices.size();

	for (size_t i = 0; i < vertexCnt; i++)
	{
		m_Vertices[i] = Other.m_Vertices[i];

	}

	
	

	Parent = nullptr;
	if (Other.Content)
	{
		Content = Other.Content->Clone();
		Content->Slot = this;
	}

}

PanelSlot::~PanelSlot()
{
	//Content->GetOwner<WidgetTree>();
	//Created의 목록에서 제거
	//SAFE_DELETE(Content);
}




void PanelSlot::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
	m_cachedManager = g_Engine->Get<UIManager>();

	

}




void PanelSlot::Render(class Renderer* pRenderer)
{
	if (Content)
	{
		SetupGeometry(pRenderer);
		Content->Render(pRenderer);
	}
}

void PanelSlot::SetupGeometry(class Renderer* pRenderer)
{	
}

const std::vector<SVertex_PUV>& PanelSlot::GetGeometryVertices() const
{
	return m_Vertices;
}

void PanelSlot::SetClipSpace(const SScissorsRect& InClipSpace)
{
	m_ClipSpace = InClipSpace;

	if(Content)
	{
		Content->SetClipSpace(InClipSpace);
	}
}

