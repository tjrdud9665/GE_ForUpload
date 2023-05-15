#include "PCH.h"
#include "PropertySet.h"
#include "imgui.h"
#include "PropertyWidget.h"

PropertySet::PropertySet()
{

}

PropertySet::~PropertySet()
{
	m_PropertiesByGroup.clear();
}

void PropertySet::SetPropertyTarget(class XObject* InPropertyTarget)
{
	m_PropertyTarget = InPropertyTarget;
	ForceRebuild();
}

void PropertySet::UpdateChildProperties(float DeltaSeconds)
{
	FPropertyWidgetByGroup::iterator iter = m_PropertiesByGroup.begin();
	FPropertyWidgetByGroup::iterator iter_end = m_PropertiesByGroup.end();

	for(;iter != iter_end;iter++)
	{
		ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed;
		if(ImGui::TreeNodeEx(&(*iter).second, flag, (*iter).first.c_str()))
		{
			FProperties::iterator _iter = (*iter).second.begin();
			FProperties::iterator _iter_end = (*iter).second.end();
			
			for(;_iter !=_iter_end;_iter++)
			{
				(*_iter)->OnUpdate(DeltaSeconds);
			}
			ImGui::TreePop();
		}
		
	}
	
}

void PropertySet::Build()
{
	m_PropertiesByGroup.clear();

	if (!m_PropertyTarget)
		return;

	XClass* ObjClass = m_PropertyTarget->GetClass();

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
				PropertyHandle* handle = (*iter).second->CreateHandle(m_PropertyTarget);
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

void PropertySet::Update(float DeltaSeconds)
{
	if (!m_PropertyTarget || !m_PropertyTarget->IsValidate())
	{
		m_PropertyTarget = nullptr;
		return;
	}

	std::string OwnLayer = m_PropertyTarget->GetClass()->GetName();

	ImGui::BeginGroup();

	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed;

	if (ImGui::TreeNodeEx(this,flag, OwnLayer.c_str()))
	{
		UpdateChildProperties(DeltaSeconds);
		ImGui::TreePop();		
	}

	ImGui::EndGroup();
}
