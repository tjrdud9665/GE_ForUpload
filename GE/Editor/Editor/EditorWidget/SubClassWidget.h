#pragma once
#include "PropertyWidget.h"
class SubClassWidget :public PropertyWidget
{
	CLASS(SubClassWidget, PropertyWidget)

public:
	SubClassWidget();
	virtual ~SubClassWidget();

	virtual void PropertyGUI() override;
	
	virtual void Initialize(PropertyHandle* InProperty, bool IsArray) override;

	virtual int32 GetArraySize() const override;

	virtual void ArrayElementClear() override;

	virtual void AddArrayElement() override;

	virtual void ArrayElementGUI(int32 idx) override;

	virtual void InsertArrayElement(int32 idx) override;

	virtual void DuplicateArrayElement(int32 idx) override;

	virtual void DeleteArrayElement(int32 idx) override;


protected:
	TPropertyHandle<XSubclassOf>* GetHandle() const
	{
		return (TPropertyHandle<XSubclassOf>*)(m_Prop);
	}

	TPropertyHandle<std::vector<XSubclassOf>>* GetArrayHandle() const
	{
		return (TPropertyHandle<std::vector<XSubclassOf>>*)(m_Prop);
	}

protected:
	XClass* m_ArrayDefaultClass;
};

