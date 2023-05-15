#pragma once
#include "PropertyWidget.h"

class StringPropertyWidget :public PropertyWidget
{
	CLASS(StringPropertyWidget,PropertyWidget)

public:
	StringPropertyWidget();
	virtual ~StringPropertyWidget();

	virtual void PropertyGUI() override;


	virtual int32 GetArraySize() const override;

	virtual void ArrayElementClear() override;

	virtual void AddArrayElement() override;

	virtual void ArrayElementGUI(int32 idx) override;

	virtual void InsertArrayElement(int32 idx) override;

	virtual void DuplicateArrayElement(int32 idx) override;

	virtual void DeleteArrayElement(int32 idx) override;

protected:
	TPropertyHandle<std::string>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<std::string>*>(m_Prop);
	}

	TPropertyHandle<std::vector<std::string>>* GetArrayHandle() const
	{
		return dynamic_cast<TPropertyHandle<std::vector<std::string>>*>(m_Prop);
	}
};

