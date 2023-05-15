#include "PCH.h"
#include "AttributePropertyWidget.h"
#include <GameplaySystem/AttributeSet.h>
#include "imgui.h"

AttributePropertyWidget::AttributePropertyWidget()
{

}

AttributePropertyWidget::~AttributePropertyWidget()
{

}

void AttributePropertyWidget::PropertyGUI()
{
	std::string name = "##" + GetName();

	SGameplayAttribute* val = GetHandle()->GetValuePtr();

	if (val)
	{
		const FChildContainer& childs = AttributeSet::StaticClass()->GetOwnChildren();
		FChildContainer::const_iterator iter = childs.begin();
		FChildContainer::const_iterator iterEnd = childs.end();

		for (; iter != iterEnd; iter++)
		{
			std::string AttributeSetName = (*iter)->GetName();

			std::string preViewString = "NONE";

			preViewString = (*val).Attribute->GetContainerType()->GetName() + "." + (*val).Attribute->GetName();


			if (ImGui::BeginCombo("##Attributes", preViewString.c_str()))
			{
				FPropertyMap props;
				(*iter)->GetOwnProperties(OUT props);

				FPropertyMap::const_iterator prop_iter = props.begin();
				FPropertyMap::const_iterator prop_iterEnd = props.end();

				for (; prop_iter != prop_iterEnd; prop_iter++)
				{
					std::string attributeName = AttributeSetName + "." + (prop_iter)->first;
					if (ImGui::Selectable(attributeName.c_str()))
					{
						(*val).SetProperty((prop_iter)->second);
					}
				}

				ImGui::EndCombo();
			}
		}

		ImGui::SameLine();
		if (ImGui::SmallButton("##None"))
		{
			//attributePtr->Attribute = nullptr;
		}

	}
}

int32 AttributePropertyWidget::GetArraySize() const
{
	std::vector<SGameplayAttribute>* val = GetArrayHandle()->GetValuePtr();

	if (val)
	{
		return (int32)val->size();
	}

	return INDEX_NONE;
}

void AttributePropertyWidget::ArrayElementClear()
{
	std::vector<SGameplayAttribute>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->clear();
}

void AttributePropertyWidget::AddArrayElement()
{
	std::vector<SGameplayAttribute>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->push_back(SGameplayAttribute());
}

void AttributePropertyWidget::ArrayElementGUI(int32 idx)
{
	std::vector<SGameplayAttribute>* val = GetArrayHandle()->GetValuePtr();
	if (val)
	{
		std::stringstream ss;
		ss << "##" << GetName() << "_" << idx;
		const FChildContainer& childs = AttributeSet::StaticClass()->GetOwnChildren();
		FChildContainer::const_iterator iter = childs.begin();
		FChildContainer::const_iterator iterEnd = childs.end();

		for (; iter != iterEnd; iter++)
		{
			std::string AttributeSetName = (*iter)->GetName();

			std::string preViewString = "NONE";
	
			preViewString = (*val)[idx].Attribute->GetContainerType()->GetName() + "." + (*val)[idx].Attribute->GetName();	


			if (ImGui::BeginCombo("##Attributes", preViewString.c_str()))
			{
				FPropertyMap props;
				(*iter)->GetOwnProperties(OUT props);

				FPropertyMap::const_iterator prop_iter = props.begin();
				FPropertyMap::const_iterator prop_iterEnd = props.end();

				for (; prop_iter != prop_iterEnd; prop_iter++)
				{
					std::string attributeName = AttributeSetName + "." + (prop_iter)->first;
					if (ImGui::Selectable(attributeName.c_str()))
					{
						(*val)[idx].SetProperty((prop_iter)->second);
					}
				}

				ImGui::EndCombo();
			}
		}

		ImGui::SameLine();
		if (ImGui::SmallButton("##None"))
		{
			//attributePtr->Attribute = nullptr;
		}
		
	}
}

void AttributePropertyWidget::InsertArrayElement(int32 idx)
{
	std::vector<SGameplayAttribute>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->insert((val->begin() + idx), SGameplayAttribute());
}

void AttributePropertyWidget::DuplicateArrayElement(int32 idx)
{
	std::vector<SGameplayAttribute>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->insert((val->begin() + idx), val->at(idx));
}

void AttributePropertyWidget::DeleteArrayElement(int32 idx)
{
	std::vector<SGameplayAttribute>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->erase((val->begin() + idx));
}
