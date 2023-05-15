#include "PCH.h"
#include "SceneComponentWidget.h"
#include "imgui.h"
#include "GameFramework/Components/SceneComponent.h"

SceneComponentWidget::SceneComponentWidget()
{

}

SceneComponentWidget::~SceneComponentWidget()
{

}

void SceneComponentWidget::Build()
{
	m_PropertiesByGroup.clear();

	TPropertyHandle<TSharedPtr<class SceneComponent>>* propHandle = GetHandle();
	TSharedPtr<SceneComponent> comp = nullptr;
	propHandle->GetValue(comp);

	if (!comp)
		return;

	XClass* ObjClass = comp->GetClass();

	if (ObjClass)
	{
		FPropertyMap Properties;
		ObjClass->GetAllProperties(Properties);

		FPropertyMap::iterator iter = Properties.begin();
		FPropertyMap::iterator End = Properties.end();
		for (; iter != End; iter++)
		{
			if ((*iter).second->HasTag(DetailsEdit) || (*iter).second->HasTag(DetailsView))
			{
				PropertyHandle* handle = (*iter).second->CreateHandle(comp);
				EditorWidget* widget = Add(PropertyWidget::NewPropertyWidget(this, handle));
				if (widget)
				{
					FPropertyWidgetByGroup::iterator find_iter = m_PropertiesByGroup.find((*iter).second->GetGroupName());
					if (find_iter == m_PropertiesByGroup.end())
					{
						FProperties  newProperties;
						newProperties.push_back(widget);
						std::string groupName = (*iter).second->GetGroupName();
						m_PropertiesByGroup.insert(std::make_pair(groupName, newProperties));
					} else
					{
						(*find_iter).second.push_back(widget);
					}
				}

			}
		}

	}
}

void SceneComponentWidget::Update(float DeltaSeconds)
{
	TPropertyHandle<TSharedPtr<class SceneComponent>>*  propHandle = GetHandle();
	TSharedPtr<SceneComponent> comp = nullptr;
	propHandle->GetValue(comp);

	if (!comp || !comp->IsValidate())
		return;

	
	
	std::stringstream ss;
	ss << comp->GetName() << "(" << comp->GetClass()->GetName() << ")";

	ImGui::BeginGroup();

	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed;

	if (ImGui::TreeNodeEx(ss.str().c_str(), flag))
	{
		UpdateChildProperties(DeltaSeconds);
		ImGui::TreePop();
	}

	ImGui::EndGroup();
}

void SceneComponentWidget::UpdateChildProperties(float DeltaSeconds)
{
	FPropertyWidgetByGroup::iterator iter = m_PropertiesByGroup.begin();
	FPropertyWidgetByGroup::iterator iter_end = m_PropertiesByGroup.end();

	for (; iter != iter_end; iter++)
	{
		ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed;
		if (ImGui::TreeNodeEx((*iter).first.c_str(), flag))
		{
			FProperties::iterator _iter = (*iter).second.begin();
			FProperties::iterator _iter_end = (*iter).second.end();

			for (; _iter != _iter_end; _iter++)
			{
				(*_iter)->OnUpdate(DeltaSeconds);
			}
			ImGui::TreePop();
		}

	}
}
