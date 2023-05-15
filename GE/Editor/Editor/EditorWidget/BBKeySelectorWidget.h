#pragma once
#include "PropertyWidget.h"

class BBKeySelectorWidget :public PropertyWidget
{
	CLASS(BBKeySelectorWidget, PropertyWidget)

protected:
	BBKeySelectorWidget();
	virtual ~BBKeySelectorWidget();

	virtual void PropertyGUI() override;

	//virtual int32 GetArraySize() const override;

	//virtual void ArrayElementClear() override;

	//virtual void AddArrayElement() override;

	//virtual void ArrayElementGUI(int32 idx) override;

	//virtual void InsertArrayElement(int32 idx) override;

	//virtual void DuplicateArrayElement(int32 idx) override;

	//virtual void DeleteArrayElement(int32 idx) override;

protected:
	TPropertyHandle<SBlackboardKeySelector>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<SBlackboardKeySelector>*>(m_Prop);
	}

	//TPropertyHandle<std::vector<SBlackboardKeySelector>>* GetArrayHandle() const
	//{
	//	return dynamic_cast<TPropertyHandle<std::vector<SBlackboardKeySelector>>*>(m_Prop);
	//}
};

