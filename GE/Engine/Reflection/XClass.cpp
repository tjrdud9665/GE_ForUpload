#include "EnginePCH.h"
#include "XClass.h"


XClass::~XClass()
{
	FPropertyMap::iterator iter = m_Properties.begin();
	FPropertyMap::iterator End = m_Properties.end();

	for(;iter != End;iter++)
	{
		SAFE_DELETE((*iter).second);
	}
}

void XClass::GetAllChildren(std::vector<XClass*>& outChildren) const
{
	outChildren.insert(outChildren.end(), m_Children.begin(), m_Children.end());

	FChildContainer::const_iterator iter = m_Children.begin();
	FChildContainer::const_iterator End = m_Children.end();

	for(;iter !=End; iter++)
	{
		(*iter)->GetAllChildren(outChildren);
	}

}

void XClass::GetInstanceableChilren(std::vector<XClass*>& outInstaceableChildren)
{
	FChildContainer::iterator iter = m_Children.begin();
	FChildContainer::iterator End = m_Children.end();


	for(;iter != End;iter++)
	{
		if(!(*iter)->HasTag(Abstract))
		{
			outInstaceableChildren.push_back((*iter));
		}
		
	}
	



	iter = m_Children.begin();
	for (; iter != End; iter++)
	{
		(*iter)->GetInstanceableChilren(outInstaceableChildren);
	}

}

const FChildContainer& XClass::GetOwnChildren() const 
{
	return m_Children;
}

XClass* XClass::FindChild(const std::string& InName) const
{
	if (InName.empty())
		return nullptr;


	FChildContainer::const_iterator iter = m_Children.begin();
	FChildContainer::const_iterator End = m_Children.end();

	for(;iter != End; iter++)
	{
		if ((*iter)->GetName() == InName)
		{
			return *iter;
		} 
		else if (XClass* found = (*iter)->FindChild(InName))
		{
			return found;
		}

	}

	//CustomCDO에 해당하는처리...

	return nullptr;
}



bool XClass::IsChildOf(const XClass* other) const
{
	if (!other || !m_Parent)
		return false;

	const XClass* current = m_Parent;
	while (current)
	{
		if (other == current)
			return true;

		current = current->m_Parent;
	}
	return false;
}

const Property* XClass::FindProperty(const std::string& propertyName) const
{
	FPropertyMap::const_iterator it = m_Properties.find(propertyName);
	if (it != m_Properties.cend())
		return (*it).second;

	return nullptr;
}


void XClass::GetOwnProperties(FPropertyMap& outProperties) const
{
	FPropertyMap::const_iterator iter = m_Properties.begin();
	FPropertyMap::const_iterator End = m_Properties.end();

	for(;iter != End; iter++)
	{
		outProperties.insert(make_pair((*iter).first, (*iter).second));
	}

}

void XClass::GetAllProperties(FPropertyMap& outProperties) const
{	
	if (m_Parent)
	{
		m_Parent->GetAllProperties(outProperties);
	}
	GetOwnProperties(outProperties);
}

