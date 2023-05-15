#pragma once
#include "PropertyWidget.h"

class FloatPropertyWidget :public PropertyWidget
{
	CLASS(FloatPropertyWidget, PropertyWidget)

public:
	FloatPropertyWidget();
	virtual ~FloatPropertyWidget();

	virtual void PropertyGUI() override;



	virtual int32 GetArraySize() const override;

	virtual void ArrayElementClear() override;

	virtual void AddArrayElement() override;

	virtual void ArrayElementGUI(int32 idx) override;

	virtual void InsertArrayElement(int32 idx) override;

	virtual void DuplicateArrayElement(int32 idx) override;

	virtual void DeleteArrayElement(int32 idx) override;

protected:
	TPropertyHandle<float>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<float>*>(m_Prop);
	}

	TPropertyHandle<std::vector<float>>* GetArrayHandle() const
	{
		return dynamic_cast<TPropertyHandle<std::vector<float>>*>(m_Prop);
	}
};

