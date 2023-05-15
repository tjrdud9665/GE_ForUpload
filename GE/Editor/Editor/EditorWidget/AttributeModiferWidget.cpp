#include "PCH.h"
#include "AttributeModiferWidget.h"
#include <GameplaySystem/AttributeSet.h>
#include "imgui.h"
#include <Assets/GameplayEffect/GameplayEffect.h>

AttributeModiferWidget::AttributeModiferWidget()
{

}

AttributeModiferWidget::~AttributeModiferWidget()
{

}

void AttributeModiferWidget::PropertyGUI()
{

}

int32 AttributeModiferWidget::GetArraySize() const
{
	std::vector<AttributeModifer*>* val = GetArrayHandle()->GetValuePtr();

	if (val)
	{
		return (int32)val->size();
	}

	return INDEX_NONE;
}

void AttributeModiferWidget::ArrayElementClear()
{
	std::vector<AttributeModifer*>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->clear();
}

void AttributeModiferWidget::AddArrayElement()
{
	std::vector<AttributeModifer*>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->push_back(Globals::CreateX<AttributeModifer>());
}

void AttributeModiferWidget::ArrayElementGUI(int32 idx)
{
	std::stringstream ss;
	ss << "##" << GetName() << "_" << idx;

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::Text("Attribute");
	ImGui::TableSetColumnIndex(1);
	//////////////////////////////////////////////////////////////////////////
	//Attribute///

	std::vector<AttributeModifer*>* propTarget = GetArrayHandle()->GetValuePtr();

	AttributeModifer* targetValue = (*propTarget)[idx];

	const Property* Att = targetValue->GetClass()->FindProperty("Attribute");

	TPropertyHandle<SGameplayAttribute>* AttHandle = (TPropertyHandle<SGameplayAttribute>*)(Att->CreateHandle(targetValue));

	if (AttHandle)
	{
		SGameplayAttribute* val = AttHandle->GetValuePtr();
		bool valid = val->IsValid();

		std::string preViewString;
		if (!valid)
		{
			preViewString = "NONE";
		} else
		{
			preViewString = val->Attribute->GetContainerType()->GetName() + "." + val->Attribute->GetName();
		}

		const FChildContainer& childs = AttributeSet::StaticClass()->GetOwnChildren();

		FChildContainer::const_iterator iter = childs.begin();
		FChildContainer::const_iterator iterEnd = childs.end();

		for (; iter != iterEnd; iter++)
		{
			std::string AttributeSetName = (*iter)->GetName();

			std::string  attLabel = ss.str();
			attLabel += "Attlabel";
			if (ImGui::BeginCombo(attLabel.c_str(), preViewString.c_str()))
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
						val->SetProperty((prop_iter)->second);
					}
				}

				ImGui::EndCombo();
			}
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("##None"))
		{
			val->Attribute = nullptr;
		}
	}

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::Text("ModOp");
	ImGui::TableSetColumnIndex(1);
	//////////////////////////////////////////////////////////////////////////
	//ModOp///
	const Property* ModOp = targetValue->GetClass()->FindProperty("ModiferOp");

	TPropertyHandle<XEnum>* ModOpHandle = (TPropertyHandle<XEnum>*)(ModOp->CreateHandle(targetValue));

	if (ModOpHandle)
	{
		XEnum* val = ModOpHandle->GetValuePtr();
		int32 enumVal = val->GetUnderlying();
		std::string  modOpLabel = ss.str();
		modOpLabel += "modOp";
		if (ImGui::Combo(modOpLabel.c_str(), &enumVal, val->Names(), (int32)val->Count()))
		{
			val->Set(enumVal);
		}
	}

	//ModCalc
	const Property* ModCalc = targetValue->GetClass()->FindProperty("ModCalcType");
	TPropertyHandle<XEnum>* ModCalcHandle = (TPropertyHandle<XEnum>*)(ModCalc->CreateHandle(targetValue));
	int32 calcType = 0;

	if (ModCalcHandle)
	{
		XEnum* val = ModCalcHandle->GetValuePtr();
		int32 enumVal = val->GetUnderlying();
		std::string  modCalcLabel = ss.str();
		modCalcLabel += "ModCalc";
		if (ImGui::Combo(modCalcLabel.c_str(), &enumVal, val->Names(), (int32)val->Count()))
		{
			val->Set(enumVal);
		}
		calcType = enumVal;
	}
	SAFE_DELETE(ModCalcHandle);

	//ModiferMagnitude 를 보여줄지
	if (calcType == (int32)EGameplayModCalcType::StaticFloat)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("ModiferMagnitude");
		ImGui::TableSetColumnIndex(1);
		const Property* ModiferMagnitude = targetValue->GetClass()->FindProperty("ModiferMagnitude");
		TPropertyHandle<float>* ModiferMagnitudeHandle = (TPropertyHandle<float>*)(ModiferMagnitude->CreateHandle(targetValue));
		if (ModiferMagnitudeHandle)
		{
			std::string magLabel = ss.str();
			magLabel += "modMag";
			ImGui::InputFloat(magLabel.c_str(), ModiferMagnitudeHandle->GetValuePtr());
		}
		SAFE_DELETE(ModiferMagnitudeHandle);

	} else if (calcType == (int32)EGameplayModCalcType::AttributeBased)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("BackingAttribute");
		ImGui::TableSetColumnIndex(1);

		const Property* BackingAtt = targetValue->GetClass()->FindProperty("BackingAttribute");
		TPropertyHandle<SGameplayAttribute>* backAttHandle = (TPropertyHandle<SGameplayAttribute>*)(BackingAtt->CreateHandle(targetValue));

		if (backAttHandle)
		{
			SGameplayAttribute* val = backAttHandle->GetValuePtr();
			bool valid = val->IsValid();

			std::string preViewString;
			if (!valid)
			{
				preViewString = "NONE";
			} else
			{
				preViewString =
					val->Attribute->GetContainerType()->GetName() + "." + val->Attribute->GetName();
			}

			const FChildContainer& childs = AttributeSet::StaticClass()->GetOwnChildren();

			FChildContainer::const_iterator iter = childs.begin();
			FChildContainer::const_iterator iterEnd = childs.end();

			for (; iter != iterEnd; iter++)
			{
				std::string AttributeSetName = (*iter)->GetName();

				std::string  attLabel = ss.str();
				attLabel += "BackAttlabel";
				if (ImGui::BeginCombo(attLabel.c_str(), preViewString.c_str()))
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
							val->SetProperty((prop_iter)->second);
						}
					}

					ImGui::EndCombo();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("##None"))
			{
				val->Attribute = nullptr;
			}
		}
		SAFE_DELETE(backAttHandle);

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("AttribuetSource");
		ImGui::TableSetColumnIndex(1);
		const Property* AttributeSource = targetValue->GetClass()->FindProperty("AttribuetSource");
		TPropertyHandle<XEnum>* AttributeSourceHandle = (TPropertyHandle<XEnum>*)(AttributeSource->CreateHandle(targetValue));

		if (AttributeSourceHandle)
		{
			XEnum* val = AttributeSourceHandle->GetValuePtr();
			int32 enumVal = val->GetUnderlying();
			std::string  attributeSrcLabel = ss.str();
			attributeSrcLabel += "attSrc";
			if (ImGui::Combo(attributeSrcLabel.c_str(), &enumVal, val->Names(), (int32)val->Count()))
			{
				val->Set(enumVal);
			}
		}

		SAFE_DELETE(AttributeSourceHandle);




		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Coefficient");
		ImGui::TableSetColumnIndex(1);
		const Property* Coefficient = targetValue->GetClass()->FindProperty("Coefficient");
		TPropertyHandle<float>* CoefficientHandle = (TPropertyHandle<float>*)(Coefficient->CreateHandle(targetValue));
		if (CoefficientHandle)
		{
			std::string magLabel = ss.str();
			magLabel += "coefficient";
			ImGui::InputFloat(magLabel.c_str(), CoefficientHandle->GetValuePtr());
		}
		SAFE_DELETE(CoefficientHandle);

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("PreMultiplyAdditiveValue");
		ImGui::TableSetColumnIndex(1);
		const Property* PreMultiplyAdditiveValue = targetValue->GetClass()->FindProperty("PreMultiplyAdditiveValue");
		TPropertyHandle<float>* PreMultiplyAdditiveValueHandle = (TPropertyHandle<float>*)(PreMultiplyAdditiveValue->CreateHandle(targetValue));
		if (PreMultiplyAdditiveValueHandle)
		{
			std::string magLabel = ss.str();
			magLabel += "PreMultiplyAdditiveValue";
			ImGui::InputFloat(magLabel.c_str(), PreMultiplyAdditiveValueHandle->GetValuePtr());
		}
		SAFE_DELETE(PreMultiplyAdditiveValueHandle);



		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("PostMultiplyAdditiveValue");
		ImGui::TableSetColumnIndex(1);
		const Property* PostMultiplyAdditiveValue = targetValue->GetClass()->FindProperty("PostMultiplyAdditiveValue");
		TPropertyHandle<float>* PostMultiplyAdditiveValueHandle = (TPropertyHandle<float>*)(PostMultiplyAdditiveValue->CreateHandle(targetValue));
		if (PostMultiplyAdditiveValue)
		{
			std::string magLabel = ss.str();
			magLabel += "PostMultiplyAdditiveValue";
			ImGui::InputFloat(magLabel.c_str(), PostMultiplyAdditiveValueHandle->GetValuePtr());
		}
		SAFE_DELETE(PostMultiplyAdditiveValueHandle);

	}



	//BackingAttribute Coefficient PreMultiplyAdditiveValue PostMultiplyAdditiveValue 를 보여줄지를 

	//ModCalc 의 타입이 결정한다.

	SAFE_DELETE(ModOpHandle);
	SAFE_DELETE(AttHandle);
}

void AttributeModiferWidget::InsertArrayElement(int32 idx)
{
	std::vector<AttributeModifer*>* val = GetArrayHandle()->GetValuePtr();	
	if (val)
		val->insert((val->begin() + idx), Globals::CreateX<AttributeModifer>());
}

void AttributeModiferWidget::DuplicateArrayElement(int32 idx)
{
	std::vector<AttributeModifer*>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->insert((val->begin() + idx), val->at(idx)->Clone());
}

void AttributeModiferWidget::DeleteArrayElement(int32 idx)
{
	std::vector<AttributeModifer*>* val = GetArrayHandle()->GetValuePtr();
	if (val)
		val->erase((val->begin() + idx));
}
