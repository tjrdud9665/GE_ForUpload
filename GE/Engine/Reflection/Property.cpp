#include "EnginePCH.h"
#include "Property.h"

Property::FDetialWidgetClasses Property::m_DetailWidgets;

XClass* Property::GetDetailWidget(type_index TypeIdx) 
{


	FDetialWidgetClasses::const_iterator iter = m_DetailWidgets.find(TypeIdx);

	if (iter != m_DetailWidgets.cend())
		return (*iter).second;

	//"class TEnum<>"
	std::string typeName = std::string(TypeIdx.name());
	std::string enumName = "class TEnum";
	std::string classOfName = "class TSubclassOf";
	

	if (typeName.find(enumName) != string::npos)
	{
		iter = m_DetailWidgets.find(std::type_index(typeid(XEnum)));
		if (iter != m_DetailWidgets.cend())
			return (*iter).second;
	}
	else if(typeName.find(classOfName) != std::string::npos)
	{
		iter = m_DetailWidgets.find(std::type_index(typeid(XSubclassOf)));
		if (iter != m_DetailWidgets.cend())
			return (*iter).second;
	}




	return nullptr;

}

void Property::RegisterDetailWidget(type_index TypeIdx, XClass* DetailWidgetClass)
{
	FDetialWidgetClasses::iterator iter = m_DetailWidgets.find(TypeIdx);

	if (iter != m_DetailWidgets.end())
		return;

	//TypeIdx,DetailSubject
	m_DetailWidgets.insert(std::make_pair(TypeIdx, DetailWidgetClass));
}
