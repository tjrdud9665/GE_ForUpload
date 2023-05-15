#pragma once

#include "ReflectionTag.h"
#include "XSubClass.h"

class XClass;
class XObject;

class Property
{	
private:
	using FDetialWidgetClasses = std::unordered_map<std::type_index, XClass*>;
	//for EditDetailWidget
	static FDetialWidgetClasses m_DetailWidgets;

public:
	static XClass* GetDetailWidget(type_index TypeIdx) ;
	static void RegisterDetailWidget(type_index TypeIdx, XClass* DetailWidgetClass);

	template<typename T>
	static XClass* GetDetailWidget() 
	{
		if(std::is_base_of<XEnum, T>::value)
		{
			return GetDetailWidget(type_index(typeid(XEnum)));
		}
		else if(std::is_base_of<XSubclassOf,T>::value)
		{
			return GetDetailWidget(type_index(typeid(XSubclassOf)));
		}
		else if constexpr (HasItemType<T>::value)
		{
			XClass* ret = GetDetailWidget(type_index(typeid(typename T::value_type)));
			if (ret)
				return ret;						
			
		}

		return GetDetailWidget(type_index(typeid(T)));		
	}

	template<typename T>
	static void RegisterDetailWidget(XClass* WidgetClass)
	{
		RegisterDetailWidget(type_index(typeid(T)), WidgetClass);
	}


private:
	XClass*			m_TypePtr;
	std::string		m_TypeName;
	std::string		m_Name;
	std::string		m_GroupName;

	ReflectionTags	m_Tags;
	

	Property() = delete;
	Property(Property&&) = delete;
	Property(const Property&) = delete;

protected:
	Property(XClass* _typePtr, const std::string& _typeName, const std::string& _name, ReflectionTags _tags, const std::string& _groupName)
		: m_TypeName(_typeName), m_TypePtr(_typePtr), m_Name(_name), m_Tags(_tags), m_GroupName(_groupName)
	{
		
	}

public:
	virtual ~Property() {}

	const std::string& GetName() const { return m_Name; }
	const std::string& GetGroupName() const { return m_GroupName; }
		

	bool HasTag(ReflectionTags tag)      const { return HasAnyTags(tag); }
	bool HasAllTags(ReflectionTags inTags) const { return (m_Tags & inTags) == inTags; }
	bool HasAnyTags(ReflectionTags inTags) const { return (m_Tags & inTags) > 0; }

	void RemoveTag(ReflectionTags tag) 
	{
		m_Tags = ReflectionTags((*m_Tags) & ~(*tag));
		
	}

	void AddTag(ReflectionTags tag)
	{
		m_Tags = ReflectionTags((*m_Tags) | (*tag));		
	}


	XClass* GetContainerType() const { return m_TypePtr; }
	const std::string& GetTypeName() const { return m_TypeName; }

	virtual class PropertyHandle* CreateHandle(XObject* Instance) const = 0;
	
};
