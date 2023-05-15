#pragma once

#include "Property.h"

class PropertyHandle
{
	friend class PropertyWidget;
protected:	
	XObject* m_ObjectInstance;
	const Property* const m_Prop;

public:
	PropertyHandle( XObject* ObjInst, const Property* Prop)
		: m_ObjectInstance(ObjInst),m_Prop(Prop)
	{
	}
	virtual ~PropertyHandle(){}

public:
	std::string GetName() const
	{
		if (m_Prop)
			return m_Prop->GetName();
		return "Invalid";
	}
	const std::string& GetGroupName() const
	{
		if (m_Prop)
		{
			std::string groupName = m_Prop->GetGroupName();
			return groupName;
		}
		return "Invalid";
	}

	std::string GetTypeName() const
	{
		if (m_Prop)
			return m_Prop->GetTypeName();
		return "Invalid";
	}
	
	XClass* GetContainerType() const
	{
		if (m_Prop)
			return m_Prop->GetContainerType();

		return nullptr;
	}
	virtual bool IsArrayType_() const { return false; }
	virtual bool IsSharedType_() const { return false; }

	bool HasTag(ReflectionTags tag) const
	{
		return m_Prop ? m_Prop->HasTag(tag) : false;
	}
	XObject* GetInstance() const
	{
		return m_ObjectInstance;
	}


	virtual void* GetRawValuePtr() const = 0;

	virtual XClass* GetClassDefinedWidgetClass()
	{
		return nullptr;
	}
};

