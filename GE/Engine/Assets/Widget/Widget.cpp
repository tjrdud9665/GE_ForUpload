#include "EnginePCH.h"
#include "Widget.h"
#include "Renderer/Renderer.h"
#include <UI/UIManager.h>
#include <Input/Input.h>


Widget::Widget()	
	:m_OwningScene(nullptr)
	, RenderOpacity(1.0f)
	, DesiredSize(SVector2(1.0f))
	, TintColor(SLinearColor(1,1,1,1))
	, m_PreHovered(false)
	, m_MouseHovered(false)
	,m_Pressed(false)
	, Visible(true)
	, m_IsCreatedEditor(true)
{

	
}

Widget::Widget(const Widget& Other)
	:Super(Other)
	, RenderOpacity(Other.RenderOpacity)
	, DesiredSize(SVector2(1.0f))
	, TintColor(Other.TintColor)
	, m_PreHovered(false)
	, m_MouseHovered(false)
	, m_Pressed(false)
	, Visible(Other.Visible)
	, m_IsCreatedEditor(Other.m_IsCreatedEditor)
{
	m_CachedRenderer = g_Engine->Get<Renderer>();
	m_CachedInput = g_Engine->Get<Input>();
	m_CachedManager = g_Engine->Get<UIManager>();
}

Widget::~Widget()
{

}

void Widget::BeginPlay()
{

}

void Widget::SetOwningScene(class Scene* InScene)
{
	m_OwningScene = InScene;
}

void Widget::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	m_CachedRenderer = g_Engine->Get<Renderer>();
	m_CachedManager = g_Engine->Get<UIManager>();
	m_CachedInput = g_Engine->Get<Input>();

	/*DesiredSize = m_CachedRenderer->FrameRenderTargetDimensionsAsFloat2();*/


}



void Widget::PreUpdate(float DeltaSeconds)
{
	
}

void Widget::Update(float DeltaSeconds)
{
	
}

void Widget::PostUpdate(float DeltaSeconds)
{	
	if(Visible)
	{
		if (m_PreHovered != m_MouseHovered)
		{
			if (m_MouseHovered)
			{
				MouseEntered(m_HoveredPos);
			} else
			{
				MouseLeaved();
			}
		}
		if (m_MouseHovered && m_CachedInput->GetMouseState(VK_LBUTTON))
		{
			MouseButtonDown(m_CachedInput->GetMousePosition(), VK_LBUTTON);
		}

		if (m_Pressed && !m_CachedInput->GetMouseState(VK_LBUTTON))
		{
			MouseButtonUp(m_HoveredPos, VK_LBUTTON);
		}
	}
	
}

void Widget::SetRenderTransform(SWidgetTransform InTransform)
{

}

const SVector2& Widget::GetDesiredSize() const
{
	return DesiredSize;
}

const SVector2& Widget::GetRenderedSize() const
{
	return RenderedSize;
}

class PanelWidget* Widget::GetParent() const
{
	if(Slot)
	{
		return Slot->Parent;
	}

	return nullptr;
}

class PanelWidget* Widget::GetRoot() const
{
	PanelWidget* CurrentParent = GetParent();
	while(CurrentParent->GetParent())
	{
		CurrentParent = CurrentParent->GetParent();
	}

	return CurrentParent;
	
}

void Widget::RemoveFromParent()
{
	PanelWidget* CurrentParent = GetParent();
	if (CurrentParent)
	{
		CurrentParent->RemoveChild(this);
	}
}

void Widget::Render(class Renderer* pRenderer)
{
	if (!Slot)
	{
		//Root
		
	}
}

class PanelSlot* Widget::GetPanelSlot() const
{
	return Slot;
}

void Widget::SetDesiredSize(const SVector2& InDesiredSize)
{
	DesiredSize = InDesiredSize;
}

SVector2 Widget::GetLeftTop()
{
	if(Slot)
	{
		//-1~~1
		SVector2 leftTop = Slot->m_Vertices[1].Position;
		//0~2
		leftTop += SVector2(1.0f, -1.0f);
		return leftTop * m_CachedRenderer->FrameRenderTargetDimensionsAsFloat2();
		
	}
	return SVector2(0, 0);
}

SVector2 Widget::GetRightBottom()
{

	if (Slot)
	{
		//-1~~1
		SVector2 rightBot= Slot->m_Vertices[5].Position;
		//0~2
		rightBot += SVector2(1.0f, -1.0f);
		return rightBot * m_CachedRenderer->FrameRenderTargetDimensionsAsFloat2();

	}
	return SVector2(0, 0);
}

void Widget::SetClipSpace(const SScissorsRect& InRect)
{

}

void Widget::SetRenderedSize(const SVector2& InRenderedSize)
{
	RenderedSize = InRenderedSize;
}

Widget* Widget::CollisionMouse(class Input* pInput)
{
	//RenderedSize
	SVector2 scrSize = m_CachedRenderer->FrameRenderTargetDimensionsAsFloat2();

	scrSize.y *= -1.0f;
	//1		2
	//
	//0
	//		4
	// 
	//3		5
	//-1~~1
	
	if (m_OwningScene)
	{		
		//UserWidget의 자식으로 있는 UserWidget인 경우 Slot 이없다.
		if (Slot)
		{
			D3D11_VIEWPORT vp = g_Engine->GetEditViewport();
	
			SVector2 res = m_CachedRenderer->FrameRenderTargetDimensionsAsFloat2();

			SVector2 LeftTop = Slot->m_Vertices[1].Position;
			SVector2 RightBot = Slot->m_Vertices[5].Position;
			//0~~2
			LeftTop += SVector2(1.0f , -1.0f);
			RightBot += SVector2(1.0f, -1.0f);
			SVector2 vpSize = SVector2(vp.Width, vp.Height);
			//0~~1
			LeftTop *= (0.5f * vpSize);
			RightBot *= (0.5f * vpSize);
			LeftTop.y *= -1.0f;
			RightBot.y *= -1.0f;
			
			SVector2 mousePos = pInput->GetMousePosition();
			
			SVector2 editorAdjust = (mousePos);
			editorAdjust += -SVector2(vp.TopLeftX, -vp.TopLeftY);
		

	

			if(LeftTop.x < editorAdjust.x  &&
			   RightBot.x > editorAdjust.x &&
			   LeftTop.y < editorAdjust.y &&
			   RightBot.y > editorAdjust.y)
			{		
				if (Slot->Content)
				{
					SetHovered(true);
					//Slot->Content->m_MouseHovered = true;
					//Slot->Content->m_HoveredPos = mousePos;
				}			

				return Slot->Content;
			}	
			else
			{
				SetHovered(false);
			}
		}		
		
		
		
	}

	return nullptr;

}

void Widget::MouseEntered(const SVector2& InMousePos)
{
	m_MouseHovered = true;
	OnMouseEntered(InMousePos);
	if (m_OnMouseEntered)
		m_OnMouseEntered();
}

void Widget::MouseLeaved()
{
	m_MouseHovered = false;
	OnMouseLeaved();

	if (m_OnMouseLeaved)
		m_OnMouseLeaved();
}

void Widget::MouseButtonDown(const SVector2& InMousePos, char MouseBtn)
{
	m_Pressed = true;
	OnMouseButtonDown(InMousePos, MouseBtn);
}

void Widget::MouseButtonUp(const SVector2& InMousePos, char MouseBtn)
{
	m_Pressed = false;
	OnMouseButtonUp(InMousePos, MouseBtn);
}

void Widget::SetHovered(bool InHovered)
{
	m_PreHovered = m_MouseHovered;
	m_MouseHovered = InHovered;
}
