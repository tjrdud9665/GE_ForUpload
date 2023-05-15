#pragma once

//#include "TClass.h"
#include "XSubClass.h"

template<typename T>
class TClass;

template<class ClassType>
class TSubclassOf : public XSubclassOf
{	
private:
	template <class ClassTypeA>
	friend class TSubclassOf;

public:
	using value_type = ClassType;	

	//TClass<ClassType>* Class;

	TSubclassOf()		
	{
		Class = ClassType::StaticClass();
		TemplatedClass = ClassType::StaticClass();
	}

	TSubclassOf(XClass* From)
	{
		TemplatedClass = ClassType::StaticClass();
		Class = From;
	}

	TSubclassOf(TClass<ClassType>* From) 		
	{
		TemplatedClass = ClassType::StaticClass();
		Class = (XClass*)(From);
	}

	TSubclassOf(const TSubclassOf& Other)		
	{
		(*this) = Other;
	}

	template<typename ClassTypeA>
	TSubclassOf(TSubclassOf<ClassTypeA>& Other)
		:XSubclassOf(Other)		
	{

	}


	template<typename ClassTypeA, typename = std::enable_if_t<std::is_base_of<ClassType, ClassTypeA>::value>>
	TSubclassOf(TClass<ClassTypeA>* From)
	{
		Class = (TClass<ClassType>*)(From);
	}

	TSubclassOf& operator=(TClass<ClassType>* From)
	{
		Class = (XClass*)From;
		return *this;
	}
	
	TSubclassOf& operator=(XClass* From)
	{
		Class = (XClass*)From;
		return *this;
	}

	TSubclassOf& operator=(nullptr_t null)
	{
		Class = nullptr;
		return *this;
	}
	TSubclassOf& operator=(XObject* From)
	{
		XSubclassOf::operator =(From);
		return *this;
	}
	explicit operator XClass* () 
	{ 
		return Class;
	}

	XClass* operator->()
	{
		return Class;
	}
	
	bool operator ==(const TSubclassOf<ClassType>& other)
	{
		return (other.Class == Class) && other.CustomCDOName == CustomCDOName;
	}

	
	template<class ClassTypeA , typename = std::enable_if_t<std::is_base_of<ClassType, ClassTypeA>::value>>
	TSubclassOf& operator=(TClass<ClassTypeA>* From)
	{
		Class = (TClass<ClassType>*)From;
		return *this;
	}

	template<class ClassTypeA, typename = std::enable_if_t<std::is_base_of<ClassType, ClassTypeA>::value>>
	TSubclassOf& operator=(TSubclassOf<ClassTypeA> From)
	{		
		(*this) = From;
		return *this;
	}


	TClass<ClassType>* GetClass()
	{
		return (TClass<ClassType>*)Class;
	}

};
