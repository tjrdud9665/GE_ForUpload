#pragma once
#include "PropertyWidget.h"

class BoolPropertyWidget :public PropertyWidget
{
	CLASS(BoolPropertyWidget,PropertyWidget)

protected:
	BoolPropertyWidget();
	virtual ~BoolPropertyWidget();


	virtual void PropertyGUI() override;

	virtual void PostEditProperties() override;

	virtual size_t GetValueSize() const override
	{
		return sizeof(bool);
	}

	virtual void BeginProperty() override;
	virtual void EndProperty() override;

	virtual int32 GetArraySize() const override;

	virtual void ArrayElementClear() override;

	virtual void AddArrayElement() override;

	virtual void ArrayElementGUI(int32 idx) override;

	virtual void InsertArrayElement(int32 idx) override;

	virtual void DuplicateArrayElement(int32 idx) override;

	virtual void DeleteArrayElement(int32 idx) override;


	TPropertyHandle<bool>* GetHandle() const
	{	
		return dynamic_cast<TPropertyHandle<bool>*>(m_Prop);
	}

	TPropertyHandle<std::vector<bool>>* GetArrayHandle() const
	{
		return dynamic_cast<TPropertyHandle<std::vector<bool>>*>(m_Prop);
	}

private:
	bool		m_Prev;	



};

