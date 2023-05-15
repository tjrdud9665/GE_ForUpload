#include "PCH.h"
#include "SceneOutliner.h"
#include "SceneManager.h"
#include "ObjectManager.h"
#include "Assets/Scene/Scene.h"
#include "GameFramework/GameObject/GameObject.h"
#include "imgui_internal.h"
#include "../SceneEditor.h"
#include "SceneDetails.h"

SceneOutliner::SceneOutliner()
{

}

SceneOutliner::SceneOutliner(const SceneOutliner& Other)
	:Super(Other)
{
		
}

SceneOutliner::~SceneOutliner()
{
	m_SelectedList.clear();
}

void SceneOutliner::Build()
{
	m_cachedSceneManager = g_Engine->Get<SceneManager>();

}

void SceneOutliner::Update(float DeltaSeconds)
{
	if (m_Open)
	{
		BeginWindow();				
		ImVec2 ListSize = ImGui::GetContentRegionAvail();
		if (ListSize.x <= 32.0f)
			ListSize.x = 150.0f;
		if (ListSize.y <= 32.0f)
			ListSize.y = 150.0f;
		if (ImGui::BeginListBox("##ObjectList", ListSize))
		{
			BuildObjectTree();
			ImGui::EndListBox();
		}
		EndWindow();

		std::list<TSharedPtr< class GameObject>>::iterator iter = m_SelectedList.begin();
		std::list<TSharedPtr< class GameObject>>::iterator iter_end = m_SelectedList.end();
		for(;iter != iter_end;)
		{
			if(!(*iter)->IsValidate())
			{
				 iter = m_SelectedList.erase(iter);
				 iter_end = m_SelectedList.end();
			}
			else
			{
				iter++;
			}
		}

	}
}

void SceneOutliner::SetPickedComponentObject(class GameObject* InObject)
{
	//m_Clicked = InObject;
	m_SelectedList.clear();
	m_SelectedList.push_back(InObject);
}

void SceneOutliner::BuildObjectTree()
{
	Scene* currentScene = m_cachedSceneManager->GetCurrentScene();

	ObjectManager* ObjMgr = g_Engine->Get<ObjectManager>();

	m_Hovered = nullptr;
	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

	if (ObjMgr)
	{

		if (ImGui::TreeNodeEx(currentScene->GetName().c_str(), flag))
		{
			//BuildObjectSubTree();
			const FObjectList& objList = ObjMgr->GetObjectList();

			FObjectList::const_iterator iter = objList.begin();
			FObjectList::const_iterator iter_end = objList.end();

			for (; iter != iter_end; iter++)
			{
				BuildObjectSubTree((*iter));

			}

			ImGui::TreePop();
		}


		//ImGui::GetMergedKeyModFlags()
		HandleClick();
	}



}

void SceneOutliner::BuildObjectSubTree(class GameObject* ParentGameObject)
{
	if(ParentGameObject)
	{
		ImGuiTreeNodeFlags treeFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
		
		//게임 오브젝트가 자식 게임오브젝트를 가질 수 있도록 해야한다면 -> 

		if (!ParentGameObject->HasChildGameObject())
			treeFlag |= ImGuiTreeNodeFlags_Leaf;

		if(HasSelectedList(ParentGameObject))
			treeFlag |= ImGuiTreeNodeFlags_Selected;


		if (ImGui::TreeNodeEx(ParentGameObject->GetName().c_str(), treeFlag))
		{			
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
			{
				m_Hovered = ParentGameObject;
			}
			//GetChildss ParentGameObject->GetChild BuildObjectSubTree()	
			ImGui::TreePop();
		}


	}

}

void SceneOutliner::HandleClick()
{
	const bool is_window_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
	const bool left_click = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
	const bool right_click = ImGui::IsMouseClicked(ImGuiMouseButton_Right);


	if (is_window_hovered)
	{
		// 
// Left click on item - Don't select yet
		if (left_click && m_Hovered)
		{
			m_Clicked = m_Hovered;
			if (ImGui::GetMergedModFlags() != ImGuiKeyModFlags_Ctrl)
			{
				ClearSlectedList();
			}

			AddSelectedList(m_Clicked);
		}

		// Right click on item - Select and show context menu
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			if (m_Hovered)
			{
				//Open Component ContextPopUp
				ImGui::OpenPopup("ObjectContext");
				if(m_SelectedList.size())
					m_ShowContextPop = true;

			}
		}


		// Clicking on empty space - Clear selection
		if ((left_click || right_click) && !m_Hovered)
		{
			ClearSlectedList();

		}
	}
	
	if (m_ShowContextPop && m_SelectedList.size() >0)
	{
		if (ImGui::BeginPopup("ObjectContext"))
		{
			if (ImGui::Selectable("Delete"))
			{
				m_ShowContextPop = false;
				std::list<TSharedPtr< class GameObject>>::iterator iter = m_SelectedList.begin();
				std::list<TSharedPtr< class GameObject>>::iterator iter_end = m_SelectedList.end();

				for (iter; iter != iter_end; iter++)
				{
					(*iter)->Kill();
				}
				m_SelectedList.clear();
				SceneDetails* details = GetOwner<SceneEditor>()->GetSceneDetails();
				if (details)
				{
					details->SetDetailTarget(nullptr);
				}
				ImGui::CloseCurrentPopup();
			} else if (ImGui::Selectable("Copy"))
			{
				m_ShowContextPop = false;
				
				std::list<TSharedPtr< class GameObject>>::iterator iter = m_SelectedList.begin();
				std::list<TSharedPtr< class GameObject>>::iterator iter_end = m_SelectedList.end();
				for (iter; iter != iter_end; iter++)
				{					
					g_Engine->Get<ObjectManager>()->SpawnCreatedGameObject((*iter)->Clone());
				}

				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}


}

void SceneOutliner::AddSelectedList(class GameObject* InObject)
{
	if (!HasSelectedList(InObject))
	{
		m_SelectedList.push_back(InObject);

		SceneDetails* details = GetOwner<SceneEditor>()->GetSceneDetails();
		if(details)
		{
			details->SetDetailTarget(InObject);
		}
	}
}

void SceneOutliner::ClearSlectedList()
{
	m_SelectedList.clear();

	SceneDetails* details = GetOwner<SceneEditor>()->GetSceneDetails();
	if (details)
	{
		details->ClearDetailTarget();
	}
}

bool SceneOutliner::HasSelectedList(class GameObject* InObject) const
{
	std::list<TSharedPtr< class GameObject>>::const_iterator iter = m_SelectedList.begin();
	std::list<TSharedPtr< class GameObject>>::const_iterator iter_end = m_SelectedList.end();
	for(;iter != iter_end;iter++)
	{
		if ((*iter) == InObject)
			return true;
	}

	return false;
}
