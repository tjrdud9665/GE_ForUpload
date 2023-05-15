#pragma once

#include "PropertyHandle.h"
#include "Reflection/ReflectionTypeTraits.h"

template<typename VarType>
class TPropertyHandle :public PropertyHandle
{
	using Access = function<VarType* (XObject*)>;

	Access m_Access;

public:
	TPropertyHandle() : PropertyHandle({}, nullptr) {}
	TPropertyHandle(XObject* instance,  const Property* prop, const Access& access)
		: PropertyHandle(instance, prop), m_Access{ access }
	{

	}
	virtual ~TPropertyHandle()
	{
		
	}

	VarType* GetValuePtr() const
	{
		if (m_ObjectInstance && m_Prop)
		{
			return m_Access(GetInstance());
		}
		return nullptr;
	}
	virtual void* GetRawValuePtr() const override { return GetValuePtr(); }

	bool GetValue(VarType& value) const
	{
		VarType* const valuePtr = GetValuePtr();
		if (valuePtr)
			value = *valuePtr;

		return valuePtr;
	}

	bool SetValue(const VarType& value) const
	{
		VarType* const valuePtr = GetValuePtr();
		if (valuePtr)
			*valuePtr = value;

		return valuePtr;
	}

	virtual XClass* GetClassDefinedWidgetClass()
	{
		//타입에 따라서 WidgetClass를 내놓아야한다.
	
		return m_Prop->GetDetailWidget<VarType>();	
	}

	virtual bool IsArrayType_() const override
	{
		return IsArrayType<VarType>();
	}

	virtual bool IsSharedType_() const override
	{
		if (!m_Prop->GetTypeName().find("TSharedPtr"))
			return true;

		if(IsArrayType_())
		{
			if (!m_Prop->GetTypeName().find("vector<TSharedPtr"))
				return true;
		}

		return IsSharedType<VarType>();
	}
};

