#pragma once

#include "XObject/XObject.h"
#include "GameplaySystem.h"


struct SGameplayAttribute
{
	SGameplayAttribute()
		:Attribute(nullptr)
	{

	}
	//Owner Maybe AttributeSet's Child
	SGameplayAttribute(Property* NewProperty)
		:Attribute(NewProperty)
	{

	}
	void SetProperty(Property* NewProperty)
	{
		Attribute = NewProperty;
	}


	bool IsValid() const
	{
		return Attribute != nullptr;
	}
	//Src �� ������ �������� �𸣴� SGameplayAttributeData�� ptr �� �����մϴ�.
	SGameplayAttributeData* GetGameplayAttributeData(class AttributeSet* Src) const;

	//Dest�� �ش� ��Ʈ������ �ִٸ� ����ģ��.
	void SetCurrentValue(float& NewValue, class AttributeSet* Dest) const;
	float GetCurrentValue(AttributeSet* Src) const;


	XClass* GetAttributeSetClass() const
	{
		if (Attribute)
			return Attribute->GetContainerType();

		return nullptr;
	}

	//TPropertyHandle<SGameplayAttributeData>* CachedHandle;
	Property* Attribute;
};

struct SGameplayEffectModifiedAttribute
{
	SGameplayAttribute Attribute; //��ɷ�ġ��

	float TotalMagnitude;		//������ ��
	SGameplayEffectModifiedAttribute(SGameplayAttribute _att, float _val)
		:TotalMagnitude(_val)
		, Attribute(_att)
	{


	}

};



class AttributeSet :public XObject
{
	CLASS(AttributeSet, XObject)
	friend class AbilityComponent;

protected:
	AttributeSet();
	AttributeSet(const AttributeSet& Other);
	virtual ~AttributeSet();

public:	

	virtual void PostGameplayEffectExecute(class GameplayEffect* InDef, class AbilityComponent* Src, class AbilityComponent* Target) {}

	virtual void PreAttributeChange(const SGameplayAttribute& Attribute, float& NewValue) {}

};

