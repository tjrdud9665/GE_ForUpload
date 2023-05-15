#pragma once
#include "PropertyWidget.h"
class AttributePropertyWidget :public PropertyWidget
{
	CLASS(AttributePropertyWidget, PropertyWidget)

public:
	AttributePropertyWidget();
	virtual ~AttributePropertyWidget();

	virtual void PropertyGUI() override;


	virtual int32 GetArraySize() const override;

	virtual void ArrayElementClear() override;

	virtual void AddArrayElement() override;

	virtual void ArrayElementGUI(int32 idx) override;

	virtual void InsertArrayElement(int32 idx) override;

	virtual void DuplicateArrayElement(int32 idx) override;

	virtual void DeleteArrayElement(int32 idx) override;

protected:

	TPropertyHandle<std::vector<SGameplayAttribute>>* GetArrayHandle() const
	{
		return dynamic_cast<TPropertyHandle<std::vector<SGameplayAttribute>>*>(m_Prop);
	}

	TPropertyHandle<SGameplayAttribute>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<SGameplayAttribute>*>(m_Prop);
	}


};

