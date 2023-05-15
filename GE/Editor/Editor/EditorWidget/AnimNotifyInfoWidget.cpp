#include "PCH.h"
#include "AnimNotifyInfoWidget.h"
#include <Assets/Animation/AnimationSequence.h>
#include "imgui.h"
#include <Assets/Mesh/Skeleton.h>
#include "PropertySet.h"



AnimNotifyInfoWidget::AnimNotifyInfoWidget()
{

}

AnimNotifyInfoWidget::~AnimNotifyInfoWidget()
{
	m_PropertySets.clear();
}


void AnimNotifyInfoWidget::PropertyGUI()
{
	//TODO  Single Notify..??
	// 특별하게 사용하는 경우가 없을것 같음.
	ImGui::Text("Override AnimNotifyInfoWidget::PropertyGUI");
	
}

void AnimNotifyInfoWidget::Initialize(PropertyHandle* InProperty, bool IsArray)
{
	Super::Initialize(InProperty, IsArray);

	std::vector<AnimNotifyInfo*>* val = GetArrayHandle()->GetValuePtr();
	if (val)
	{
		size_t valSize = val->size();

		for(size_t i = 0 ;i<valSize;i++)
		{
			if((*val)[i])
			{	
				std::unordered_map<AnimNotifyInfo*, class PropertySet*>::iterator find_iter = m_PropertySets.find((*val)[i]);

				if(find_iter == m_PropertySets.end())
				{
					PropertySet* propSet = EditorWidget::AddNew<PropertySet>("Notify");
					m_PropertySets[(*val)[i]] = propSet;
					propSet->SetPropertyTarget((*val)[i]->GetNotifyInst());
				}				
			}
		}
		


	}
}

int32 AnimNotifyInfoWidget::GetArraySize() const
{
	std::vector<AnimNotifyInfo*>* val = GetArrayHandle()->GetValuePtr();

	if (val)
	{
		return (int32)val->size();
	}

	return INDEX_NONE;
}

void AnimNotifyInfoWidget::ArrayElementClear()
{
	std::vector<AnimNotifyInfo*>* val = GetArrayHandle()->GetValuePtr();
	if (val)
	{
		size_t notifySize = val->size();
		for(size_t i =0;i<notifySize;i++)
		{
			SAFE_DELETE((*val)[i]);
		}
		val->clear();
	}

	RemoveAllChildren();
	
}

void AnimNotifyInfoWidget::AddArrayElement()
{
	std::vector<AnimNotifyInfo*>* val = GetArrayHandle()->GetValuePtr();
	if (val)
	{		
		AnimNotifyInfo* newInfo = Globals::CreateX<AnimNotifyInfo>(m_Prop->GetInstance(), "AnimNotifyInfo");
		val->push_back(newInfo);

		PropertySet* propSet = EditorWidget::AddNew<PropertySet>("Notify");		
		propSet->SetPropertyTarget(newInfo->GetNotifyInst());
		m_PropertySets[newInfo] = propSet;
	}

}

void AnimNotifyInfoWidget::ArrayElementGUI(int32 idx)
{
	std::vector<AnimNotifyInfo*>* val = GetArrayHandle()->GetValuePtr();
	if (val)
	{
		std::stringstream ss;
		ss << "##" << GetName() << "_" << idx;
		std::string name = ss.str();

		FChildContainer childClass;
		if ((*val)[idx])
		{

			(*val)[idx]->GetNotifyClass().TemplatedClass->GetAllChildren(OUT childClass);

			int32 SelectedClassIdx = INDEX_NONE;
			std::vector<std::string> ClassNames;

			for (size_t i = 0; i < childClass.size(); i++)
			{
				ClassNames.push_back(childClass[i]->GetName());

				if ((*val)[idx]->GetNotifyClass().GetClass()->GetName() == childClass[i]->GetName())
					SelectedClassIdx = (int32)i;

			}
			

			if (ImGui::BeginCombo(name.c_str(), SelectedClassIdx == INDEX_NONE ? "NONE" : ClassNames[SelectedClassIdx].c_str()))
			{
				for (size_t i = 0; i < ClassNames.size(); i++)
				{
					bool is_selected = (SelectedClassIdx == i);
					if (ImGui::Selectable(ClassNames[i].c_str(), is_selected))
					{
						SelectedClassIdx = (int32)i;
						(*val)[idx]->SetNotifyClass(childClass[i]);
						m_PropertySets[(*val)[idx]]->SetPropertyTarget((*val)[idx]->GetNotifyInst());

					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
		}
	}
	
}

void AnimNotifyInfoWidget::InsertArrayElement(int32 idx)
{
	std::vector<AnimNotifyInfo*>* val = GetArrayHandle()->GetValuePtr();
	if (val)
	{
		AnimNotifyInfo* newInfo = Globals::CreateX<AnimNotifyInfo>(m_Prop->GetInstance(), "AnimNotifyInfo");
		val->insert((val->begin() + idx), newInfo);

		PropertySet* propSet = EditorWidget::AddNew<PropertySet>("Notify");
		propSet->SetPropertyTarget(newInfo->GetNotifyInst());
		m_PropertySets[newInfo] = propSet;		
	}
}

void AnimNotifyInfoWidget::DuplicateArrayElement(int32 idx)
{
	std::vector<AnimNotifyInfo*>* val = GetArrayHandle()->GetValuePtr();
	if (val)
	{
		AnimNotifyInfo* newInfo = val->at(idx)->Clone();
		val->insert((val->begin() + idx), newInfo);		
		
		PropertySet* propSet = EditorWidget::AddNew<PropertySet>("Notify");
		propSet->SetPropertyTarget(newInfo->GetNotifyInst());
		m_PropertySets[newInfo] = propSet;
	}
}

void AnimNotifyInfoWidget::DeleteArrayElement(int32 idx)
{
	std::vector<AnimNotifyInfo*>* val = GetArrayHandle()->GetValuePtr();
	if (val)
	{		
		PropertySet* removal = m_PropertySets[(*val)[idx]];
		RemoveWidget(removal);
		m_PropertySets.erase((*val)[idx]);
		SAFE_DELETE((*val)[idx]);
		val->erase((val->begin() + idx));

	}

	
}

void AnimNotifyInfoWidget::AdditionalArrayElementGUI(int32 idx, float DeltaSeconds)
{
	std::vector<AnimNotifyInfo*>* val = GetArrayHandle()->GetValuePtr();
	if (m_PropertySets[(*val)[idx]])
	{
		m_PropertySets[(*val)[idx]]->OnUpdate(g_DeltaSeconds);
	}
}
