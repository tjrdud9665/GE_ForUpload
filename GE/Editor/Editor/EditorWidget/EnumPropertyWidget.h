#pragma once
#include "PropertyWidget.h"
class EnumPropertyWidget :public PropertyWidget
{
	CLASS(EnumPropertyWidget, PropertyWidget)

public:
	EnumPropertyWidget();
	virtual ~EnumPropertyWidget();

	virtual void PropertyGUI() override;

	virtual int32 GetArraySize() const override;

	virtual void ArrayElementClear() override;

	virtual void AddArrayElement() override;

	virtual void ArrayElementGUI(int32 idx) override;

	virtual void InsertArrayElement(int32 idx) override;

	virtual void DuplicateArrayElement(int32 idx) override;

	virtual void DeleteArrayElement(int32 idx) override;

protected:
	int32				m_EnumValue;


	std::unordered_map<int32, int32>	m_EnumValues;


protected:
	TPropertyHandle<XEnum>* GetHandle() const
	{
		return (TPropertyHandle<XEnum>*)(m_Prop);
	}
	TPropertyHandle<std::vector<XEnum>>* GetArrayHandle() const
	{
		return dynamic_cast<TPropertyHandle<std::vector<XEnum>>*>(m_Prop);
	}
};

