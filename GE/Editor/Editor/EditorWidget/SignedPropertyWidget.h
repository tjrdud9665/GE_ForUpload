#pragma once
#include "PropertyWidget.h"

class SignedPropertyWidget :public PropertyWidget
{
	CLASS(SignedPropertyWidget,PropertyWidget)

public:
	SignedPropertyWidget();
	virtual ~SignedPropertyWidget();

	virtual void PropertyGUI() override;



	virtual int32 GetArraySize() const override;

	virtual void ArrayElementClear() override;

	virtual void AddArrayElement() override;

	virtual void ArrayElementGUI(int32 idx) override;

	virtual void InsertArrayElement(int32 idx) override;

	virtual void DuplicateArrayElement(int32 idx) override;

	virtual void DeleteArrayElement(int32 idx) override;

protected:
	TPropertyHandle<int32>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<int32>*>(m_Prop);
	}

	TPropertyHandle<std::vector<int32>>* GetArrayHandle() const
	{
		return dynamic_cast<TPropertyHandle<std::vector<int32>>*>(m_Prop);
	}
};

