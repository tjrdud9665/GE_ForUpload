#include "PCH.h"
#include "ComponentDetails.h"

#include "GameFramework/GameObject/GameObject.h"
#include "GameFramework/Components/SceneComponent.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "SceneDetailWidget.h"



ComponentDetails::ComponentDetails()
{
	m_Hovered = nullptr;
	m_Clicked = nullptr;
}

ComponentDetails::ComponentDetails(const ComponentDetails& Other)
	:Super(Other)
{
	assert(true);
	m_Hovered = nullptr;
	m_Clicked = nullptr;
}

ComponentDetails::~ComponentDetails()
{
	
}

void ComponentDetails::Update(float DeltaSeconds)
{
	if (m_EditTarget)
	{
		if (!m_EditTarget->IsValidate())
		{
			m_EditTarget = nullptr;
			return;
		}
		m_Hovered = nullptr;

		const std::string name = "##" + GetName();
		ImVec2 listSize =  ImGui::GetContentRegionAvail();
		if (ImGui::BeginListBox(name.c_str(), ImVec2(listSize.x, listSize.y * 0.35f)))
		{			
			bool selected = m_EditTarget == m_Clicked;
			if(ImGui::Selectable(m_EditTarget->GetName().c_str(), selected))
			{				
				SceneDetailWidget* details = GetOwner<SceneDetailWidget>();
				if (details)
				{
					details->SetDetailTarget(m_EditTarget);
				}
				m_Clicked = m_EditTarget;
			}			
			
			ImGui::Separator();			
			BuildComponentHierarchy(m_EditTarget->GetRootComponent());			
			ImGui::Separator();
			const FObjectComponentList& compList = m_EditTarget->GetObjectComponents();
			size_t compSize = compList.size();
			for(size_t i =0;i<compSize;i++)
			{
				BuildComponentNonHierarchy(compList[i]);
			}


			ImGui::EndListBox();
		}
	}
	

}

void ComponentDetails::BuildComponentHierarchy(SceneComponent* ParentComp)
{
	if (!ParentComp)
		return;
	std::string name = "##" + ParentComp->GetName();
	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

	//Selected?
	if (m_Clicked == ParentComp)
		flag |= ImGuiTreeNodeFlags_Selected;

	//Leaf ?
	if (!ParentComp->HasChild())
		flag |= ImGuiTreeNodeFlags_Leaf;
	
	//std::stringstream ss;

	const std::string& compName = ParentComp->GetName();
	const std::string& className = ParentComp->GetClass()->GetName();

	std::string Format = compName;
	Format += "(";
	Format += className;
	Format += ")";

	//ss << compName.c_str() << "(" << className.c_str() << ")";

	if(ImGui::TreeNodeEx(Format.c_str(),flag ))
	{
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
		{
			m_Hovered = ParentComp;
		}

		if(!(flag & ImGuiTreeNodeFlags_Leaf))
		{
			std::vector<class SceneComponent*> childs;
			size_t childSize = ParentComp->GetAttachChildren(OUT childs);

			for(size_t i =0;i<childSize;i++)
			{
				BuildComponentHierarchy(childs[i]);
			}

			//const FChildComponents& childs = ParentComp->GetAttachChildren();

			//FChildComponents::const_iterator iter = childs.begin();
			//FChildComponents::const_iterator iter_end = childs.end();
			//for(;iter != iter_end;iter++)
			//{
			//	BuildComponentHierarchy((*iter));
			//}
		
		}
		HandleClick();
		ImGui::TreePop();
	}

}

void ComponentDetails::BuildComponentNonHierarchy(class ObjectComponent* ObjectComp)
{
	const std::string& compName = ObjectComp->GetName();
	const std::string& className = ObjectComp->GetClass()->GetName();

	std::string Format = compName;
	Format += "(";
	Format += className;
	Format += ")";

	bool IsSel = m_Clicked == ObjectComp;

	if (ImGui::Selectable(Format.c_str(), IsSel))
	{
		m_Hovered = ObjectComp;
		SceneDetailWidget* details = GetOwner<SceneDetailWidget>();

		if (m_Hovered)
		{
			m_Clicked = m_Hovered;

			if (details)
			{
				details->SetDetailTarget(m_Clicked);
			}
		}
	
	}
	
}

void ComponentDetails::HandleClick()
{
	const bool is_window_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
	const bool left_click = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
	const bool right_click = ImGui::IsMouseClicked(ImGuiMouseButton_Right);

	if (!is_window_hovered)
		return;		

	SceneDetailWidget* details = GetOwner<SceneDetailWidget>();

	if (left_click && m_Hovered)
	{
		m_Clicked = m_Hovered;		
		
		if (details)
		{
			details->SetDetailTarget(m_Clicked);
		}
	}


	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		if (m_Hovered)
		{
		

		}
	}

	// Clicking on empty space - Clear selection
	if ((left_click || right_click) && !m_Hovered)
	{	
		if (details)
		{			
			m_Clicked = details->GetComponentDetail()->GetCurrentEditTarget();
			details->SetDetailTarget(m_Clicked);
				
		}
	}
}

void ComponentDetails::SetEditTarget(class GameObject* EditTarget)
{
	m_EditTarget = EditTarget;
	m_Hovered = nullptr;
	m_Clicked = m_EditTarget;
	

}

void ComponentDetails::SetEditTarget_GameObjectOnly(class GameObject* EditTarget,  XObject* clickedComp)
{
	m_EditTarget = EditTarget;
	m_Hovered = clickedComp;
	m_Clicked = clickedComp;
}

XObject* ComponentDetails::GetCurrentEditTarget() const
{
	return m_EditTarget;
}
