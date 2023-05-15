#include "PCH.h"
#include "ObjectListWidget.h"
#include "imgui.h"
#include <GameFramework/GameObject/GameObject.h>

ObjectListWidget::ObjectListWidget()
{

}

ObjectListWidget::~ObjectListWidget()
{

}

void ObjectListWidget::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
	m_TargetClass = GameObject::StaticClass();
}

void ObjectListWidget::Update(float DeltaSeconds)
{
	m_Hovered = nullptr;
	std::string name = "##" + GetName();
	if (ImGui::BeginListBox(name.c_str(), ImVec2(-FLT_MIN, ImGui::GetContentRegionAvail().y - 64)))
	{		
		BuildGameObjectHierachy(m_TargetClass);
		ImGui::EndListBox();
	}
}

TSubclassOf<class GameObject> ObjectListWidget::GetClickedClass() const
{
	return m_Clicked;
}

void ObjectListWidget::BuildGameObjectHierachy(TSubclassOf<class GameObject> InParent)
{
	std::string name = InParent->GetName();
	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

	//Selected?
	if ((m_Clicked == InParent))
		flag |= ImGuiTreeNodeFlags_Selected;

	//Leaf ?
	if (!InParent->HasChild() && !InParent->HasCustomCDO())
		flag |= ImGuiTreeNodeFlags_Leaf;

	

	if (ImGui::TreeNodeEx(name.c_str(), flag))
	{
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
		{
			m_Hovered = InParent;
		}

		if (!(flag & ImGuiTreeNodeFlags_Leaf))
		{
			
			FChildContainer childs = InParent->GetOwnChildren();

			size_t childSize = childs.size();	

			for (size_t i = 0; i < childSize; i++)
			{
				BuildGameObjectHierachy(childs[i]);				
			}		
		}


		std::vector<XObject*> customCDO;

		size_t customSize = InParent->GetOwnCustomCDO(OUT customCDO);

		flag |= ImGuiTreeNodeFlags_Leaf;

	

		for (size_t j = 0; j < customSize; j++)
		{
			std::string relPathWithName;
			customCDO[j]->GetRelativePathWithName(OUT relPathWithName);
			if ((m_Clicked.CustomCDOName == relPathWithName))
			{
				flag |= ImGuiTreeNodeFlags_Selected;
			}
			else
				flag &= ~ImGuiTreeNodeFlags_Selected;

			if (ImGui::TreeNodeEx(customCDO[j]->GetName().c_str(), flag))
			{
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
				{
					m_Hovered = customCDO[j]->GetClass();
					customCDO[j]->GetRelativePathWithName(OUT m_Hovered.CustomCDOName);					
				}
				ImGui::TreePop();
			}
		}

		HandleClick();
		ImGui::TreePop();
	}
}

void ObjectListWidget::HandleClick()
{
	const bool is_window_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
	const bool left_click = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
	const bool right_click = ImGui::IsMouseClicked(ImGuiMouseButton_Right);

	if (!is_window_hovered)
		return;


	if (left_click && m_Hovered)
	{
		m_Clicked = m_Hovered;
	
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
	}
}
