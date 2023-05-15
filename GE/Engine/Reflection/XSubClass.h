#pragma once

class XClass;

class XSubclassOf
{
	friend class XClass;
	friend class Globals;	
public:	
	XSubclassOf()
	{
		Class = nullptr;
		TemplatedClass = nullptr;
		
	}
	XSubclassOf(class XClass * From)
	{
		Class = From;
		TemplatedClass = From;
	}

	XSubclassOf(class XClass* From,const std::string& CDOName)
	{
		Class = From;
		TemplatedClass = From;
		CustomCDOName = CDOName;
	}


	XSubclassOf(const XSubclassOf& Other)
		:Class(Other.Class)
		,CustomCDOName(Other.CustomCDOName)
		, TemplatedClass(Other.TemplatedClass)
	{

	}

	~XSubclassOf()
	{

	}
	void operator=(class XClass* From)
	{
		Class = From;
	}

	void operator=(class XObject* FromCustomCDO);

	inline operator bool() const
	{
		return Class != nullptr;
	}
	inline bool operator!() const
	{
		return Class == nullptr;
	}

public:
	 XClass*	Class;
	 XClass*	TemplatedClass;

	 std::string		CustomCDOName;

};