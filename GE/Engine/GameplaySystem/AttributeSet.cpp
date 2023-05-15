#include "EnginePCH.h"
#include "AttributeSet.h"

SGameplayAttributeData* SGameplayAttribute::GetGameplayAttributeData(class AttributeSet* Src) const
{
	SGameplayAttributeData* RetvaluePtr = nullptr;
	if (Attribute && Src)
	{
		PropertyHandle* handle = Attribute->CreateHandle(Src);
		TPropertyHandle<SGameplayAttributeData>* propHandle = dynamic_cast<TPropertyHandle<SGameplayAttributeData>*>(handle);
		if (propHandle)
		{
			RetvaluePtr = propHandle->GetValuePtr();
		}

		SAFE_DELETE(handle);
	}

	return RetvaluePtr;
}

void SGameplayAttribute::SetCurrentValue(float& NewValue, class AttributeSet* Dest) const
{
	if (Attribute && Dest)
	{
		PropertyHandle* handle = Attribute->CreateHandle(Dest);
		TPropertyHandle<SGameplayAttributeData>* propHandle = dynamic_cast<TPropertyHandle<SGameplayAttributeData>*>(handle);

		if (propHandle)
		{
			SGameplayAttributeData* DataPtr = propHandle->GetValuePtr();
			float OldValue = 0.f;
			OldValue = DataPtr->GetCurrentValue();
			//Dest->PreAttributeChange(*this, NewValue);
			DataPtr->SetCurrentValue(NewValue);
		}

		SAFE_DELETE(handle);
	}

}

float SGameplayAttribute::GetCurrentValue(AttributeSet* Src) const
{
	float ret = 0.0f;

	if (Attribute && Src)
	{
		PropertyHandle* handle = Attribute->CreateHandle(Src);

		TPropertyHandle<SGameplayAttributeData>* propHandle = dynamic_cast<TPropertyHandle<SGameplayAttributeData>*>(handle);
		if (propHandle)
		{
			SGameplayAttributeData* DataPtr = propHandle->GetValuePtr();
			ret = DataPtr->GetCurrentValue();
		}

		SAFE_DELETE(propHandle);


	}

	return ret;
}

AttributeSet::AttributeSet()
{

}

AttributeSet::AttributeSet(const AttributeSet& Other)
	:Super(Other)
{

}

AttributeSet::~AttributeSet()
{

}
