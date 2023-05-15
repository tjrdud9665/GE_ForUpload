#include "EnginePCH.h"
#include "UserWidget.h"
#include "WidgetTree.h"
#include <FileSystem/FileSystem.h>

UserWidget::UserWidget()
{
	
}

UserWidget::UserWidget(const UserWidget& Other)
	:Super(Other)
{
	if (Other.IsConstructed())
	{
		m_WidgetTree = Other.m_WidgetTree->Clone();		
		//문제 1. Panel에서 여러개의 자식을가지고있으면 자식에서 부모를 만들기떄문에 문제가된다.
		// 
		//Has Parent->
		//if(Other.Slot)
		//{
		//	int32 idx = 0;
		//	PanelWidget* otherParent = Other.m_WidgetTree->FindWidgetParent(&Other, OUT idx);			
		//}
	


	}
}

UserWidget::~UserWidget()
{
	if (!m_OwningScene)
	{
		std::string sPath = GetInfo().GetSPath();
		if (!sPath.empty())
		{
			std::string CDOName = FileSystem::GetRelativePath(FileSystem::FromString(sPath)) + GetName();
			GetClass()->DeleteCDO(CDOName);
		}
	}
}

void UserWidget::BeginPlay()
{
	Super::BeginPlay();
	if (m_WidgetTree)
	{
		m_WidgetTree->BeginPlay();
	}
}

void UserWidget::SetOwningScene(class Scene* InScene)
{
	Super::SetOwningScene(InScene);
	if (m_WidgetTree)
	{
		m_WidgetTree->SetOwningScene(InScene);
	}
}

void UserWidget::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
	m_WidgetTree = Globals::CreateX<WidgetTree>(this, "WidgetTree");
	
}

bool UserWidget::PostLoad()
{
	bool bResult = Super::PostLoad();

	std::string CDOName = FileSystem::GetRelativePath(FileSystem::FromString(GetInfo().GetSPath())) + GetName();

	GetClass()->RegisterCDO(CDOName, this, WIDGET_PATH);

	return bResult;
}

void UserWidget::PreUpdate(float DeltaSeconds)
{
	Super::PreUpdate(DeltaSeconds);
	if (m_WidgetTree)
	{
		m_WidgetTree->PreUpdate(DeltaSeconds);
	}

}

void UserWidget::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);
	if (m_WidgetTree)
	{
		m_WidgetTree->Update(DeltaSeconds);
	}
}

void UserWidget::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);

	if (m_WidgetTree)
	{
		m_WidgetTree->PostUpdate(DeltaSeconds);
	}
	Widget* root = m_WidgetTree->GetRootWidget();
	if (root)
	{
		if (Slot)
		{
			DesiredSize = root->DesiredSize;
		}
	}
}

Widget* UserWidget::CollisionMouse(class Input* pInput)
{	
	if (m_WidgetTree)		
	{	

		Widget* ret = m_WidgetTree->CollisionMouse(pInput);
	

		return ret;
	}

	return nullptr;
}

void UserWidget::SetClipSpace(const SScissorsRect& InRect)
{
	if (m_WidgetTree)
	{
		if(m_WidgetTree->RootWidget)
			m_WidgetTree->RootWidget->SetClipSpace(InRect);
	}
}

void UserWidget::Render(class Renderer* pRenderer)
{
	if(m_WidgetTree)
	{		
		Widget* root = m_WidgetTree->GetRootWidget();
		if(root && Visible)
		{	
			if (!Slot)
			{
				DesiredSize = root->DesiredSize = pRenderer->FrameRenderTargetDimensionsAsFloat2();
				pRenderer->SetShader(EShaders::UNLIT, true);
				pRenderer->SetBlendState(EDefaultBlendState::ALPHA_BLEND);
				pRenderer->SetRasterizerState(EDefaultRasterizerState::CULL_NONE_Rect);
				SVector2 scrSize = pRenderer->FrameRenderTargetDimensionsAsFloat2();
				pRenderer->SetScissorsRect(0, (int32)scrSize.x, 0, (int32)scrSize.y);
				root->SetRenderedSize(DesiredSize);
			}
			else
			{				
				//DesiredSize = Slot->GetSlotSize();
				//DesiredSize = GetRenderedSize();
			}				

			
			root->Render(pRenderer);
		

		}
	}
}


bool UserWidget::Serialize(Archive& ar)
{
	bool bResult = Super::Serialize(ar);

	if(ar.IsLoading())
	{		
		uint32 widgetSize = 0;
		ar.BeginObject("childWidgetClass");
		ar.SerializeArraySize(widgetSize);		
		std::vector<XClass*> widgetClasses;
		widgetClasses.resize(widgetSize);
		std::vector<Widget*> widgetInst;

		std::vector<XClass*>::reverse_iterator iter = widgetClasses.rbegin();
		std::vector<XClass*>::reverse_iterator iter_end = widgetClasses.rend();
		for (uint32 i = 0; iter != iter_end; iter++, i++)
		{
			ar.BeginObject(i);
			bool createdEd = false;
			ar.Serialize("_createEd", createdEd);
			ar.Serialize("val", (*iter));
			if(createdEd)
				widgetInst.push_back(m_WidgetTree->ConstructWidget<Widget>((*iter),"",true));
			else
			{
				std::string _name;
				ar.Serialize("_name", _name);				
				Widget* w = m_WidgetTree->FindWidgetElse(_name);
				w->m_IsCreatedEditor = false;
				widgetInst.push_back(w);

			}
			ar.EndObject();
		}
	
		ar.EndObject();

		ar.BeginObject("childWidgetInst");
		ar.SerializeArraySize(widgetSize);
		std::vector<Widget*>::iterator wIter =  widgetInst.begin();
		std::vector<Widget*>::iterator wIter_end = widgetInst.end();

		for (uint32 i = 0; wIter != wIter_end; wIter++, i++)
		{
			ar.BeginObject(i);			
			(*wIter)->Serialize(ar);		
			ar.EndObject();
		}
		ar.EndObject();

		 wIter = widgetInst.begin();
		 wIter_end = widgetInst.end();
		 for (; wIter != wIter_end; wIter++)
		 {
			 if ((*wIter)->GetParent() == nullptr)
			 {
				 m_WidgetTree->SetRootWidget((*wIter));
				 break;
			 }
		 }
		 //RootWidget지정		
	}
	else
	{
		//All of Widget Class 0 is root
		std::vector<Widget*> allWidgets;
		uint32 widgetSize = (uint32)m_WidgetTree->GetAllWidgets(OUT allWidgets);
		// root 부터가아닌 자식부터 루트까지..

		ar.BeginObject("childWidgetClass");		
		ar.SerializeArraySize(widgetSize);
		std::vector<Widget*>::reverse_iterator iter = allWidgets.rbegin();
		std::vector<Widget*>::reverse_iterator iter_end = allWidgets.rend();
		for(uint32 i = 0;iter != iter_end;iter++,i++)
		{
			ar.BeginObject(i);
			XClass* widgetClass = (*iter)->GetClass();
			ar.Serialize("_createEd", (*iter)->m_IsCreatedEditor);		
			ar.Serialize("val", widgetClass);			
			if (!(*iter)->m_IsCreatedEditor)
			{
				std::string _name = (*iter)->GetName();
				ar.Serialize("_name", _name);
			}
			//로드시에 생성할지말지 여부->
			//c++코드로 만들어진경우 생성x


			ar.EndObject();
		}	
		ar.EndObject();

		ar.BeginObject("childWidgetInst");
		ar.SerializeArraySize(widgetSize);
		iter = allWidgets.rbegin();
		iter_end = allWidgets.rend();
		for (uint32 i = 0; iter != iter_end; iter++, i++)
		{
			ar.BeginObject(i);
			(*iter)->Serialize(ar);
			ar.EndObject();
		}		
		ar.EndObject();
		
	}

	return bResult;
}

bool UserWidget::IsVisible() const
{
	return Visible;
}

void UserWidget::SetVisible(bool visible)
{
	Visible = visible;
}

class WidgetTree* UserWidget::GetWidgetTree() const
{
	return m_WidgetTree;
}
