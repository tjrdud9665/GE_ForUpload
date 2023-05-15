#pragma once
#include "PropertyWidget.h"
class AttributeModiferWidget :public PropertyWidget
{
	CLASS(AttributeModiferWidget,PropertyWidget)

protected:
	AttributeModiferWidget();
	virtual ~AttributeModiferWidget();
	

	virtual void PropertyGUI() override;


	virtual int32 GetArraySize() const override;

	virtual void ArrayElementClear() override;

	virtual void AddArrayElement() override;

	virtual void ArrayElementGUI(int32 idx) override;

	virtual void InsertArrayElement(int32 idx) override;

	virtual void DuplicateArrayElement(int32 idx) override;

	virtual void DeleteArrayElement(int32 idx) override;

protected:

	TPropertyHandle<std::vector<AttributeModifer*>>* GetArrayHandle() const
	{
		return dynamic_cast<TPropertyHandle<std::vector<AttributeModifer*>>*>(m_Prop);
	}

	TPropertyHandle<AttributeModifer*>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<AttributeModifer*>*>(m_Prop);
	}


};

