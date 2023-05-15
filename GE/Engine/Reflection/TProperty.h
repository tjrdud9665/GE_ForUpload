#pragma once

#include "ReflectionTag.h"
#include "Property.h"
#include "TPropertyHandle.h"



template <typename VarType>
class TProperty : public Property
{
	using Access = std::function<VarType* (XObject*)>;

private:
	Access m_Access;


public:

	TProperty(XClass* typePtr, const std::string& typeName, const std::string& name, Access&& _access, ReflectionTags tags, const std::string& GroupName)
		: Property(typePtr, typeName, name, tags,GroupName), m_Access(_access)
	{

	}

	virtual PropertyHandle* CreateHandle( XObject* Instance) const override
	{
		return (new TPropertyHandle<VarType>(Instance, this, m_Access));
	}

	
	

};