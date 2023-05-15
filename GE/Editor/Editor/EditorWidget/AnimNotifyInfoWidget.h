#pragma once
#include "PropertyWidget.h"



class AnimNotifyInfoWidget :public PropertyWidget
{
	CLASS(AnimNotifyInfoWidget, PropertyWidget)

public:
	AnimNotifyInfoWidget();
	virtual ~AnimNotifyInfoWidget();

	virtual void PropertyGUI() override;

	virtual void Initialize(PropertyHandle* InProperty, bool IsArray) override;

	virtual int32 GetArraySize() const override;

	virtual void ArrayElementClear() override;

	virtual void AddArrayElement() override;

	virtual void ArrayElementGUI(int32 idx) override;

	virtual void InsertArrayElement(int32 idx) override;

	virtual void DuplicateArrayElement(int32 idx) override;

	virtual void DeleteArrayElement(int32 idx) override;

	virtual void AdditionalArrayElementGUI(int32 idx, float DeltaSeconds) override;

protected:
	TPropertyHandle<AnimNotifyInfo*>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<AnimNotifyInfo*>*>(m_Prop);
	}

	TPropertyHandle<std::vector<AnimNotifyInfo*>>* GetArrayHandle() const
	{
		return dynamic_cast<TPropertyHandle<std::vector<AnimNotifyInfo*>>*>(m_Prop);
	}
	
	std::unordered_map<AnimNotifyInfo*,class PropertySet*>		m_PropertySets;
};

