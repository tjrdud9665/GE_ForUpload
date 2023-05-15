#pragma once

#include "ReflectionTag.h"
#include "TProperty.h"
#include "ReflectionTypeTraits.h"


//저장/로드를 하기위해서는 이름으로 해당하는 클래스를 찾아올수있어야한다.
//->static으로 맵을 구성.
//std::unordered_map<std::string.XClass*> AllClass;
//FindClassFromName..



using FPropertyMap = std::unordered_map<std::string, Property*>;
using FChildContainer = std::vector<XClass*>;


class XClass
{	
protected:
	std::string		m_Name;
	ReflectionTags	m_Tags;

	XClass*			m_Parent;
	FChildContainer	m_Children;

	FPropertyMap			m_Properties;	

public:
	XClass() : m_Tags{}, m_Parent{ nullptr } {}
	XClass(const XClass&) = delete;
	XClass& operator=(const XClass&) = delete;
	virtual ~XClass();

	/** Type */
	const std::string& GetName() const { return m_Name; }
	bool HasTag(ReflectionTags InTag) const { return (m_Tags & InTag) > 0; }

public:

	template<typename Type>
	bool IsChildOf() const { return IsChildOf(Type::StaticClass()); }

	bool IsChildOf(const XClass* other) const;
	bool IsParentOf(const XClass* other) const { return other->IsChildOf(this); }

	bool IsA(XClass* other) const { return this == other; }

	bool HasChild() const { return m_Children.size() != 0; }

	virtual bool HasCustomCDO() const { return false; }
	/** Properties */

	const Property* FindProperty(const std::string& propertyName) const;
	void GetOwnProperties(FPropertyMap& outProperties) const;
	void GetAllProperties(FPropertyMap& outProperties) const;

	
	

	/** REGISTRY */

	/** Registry a class */
	void __Registry(const std::string& InName) { m_Name = InName; }

	/** Registry a class with a parent */
	template<typename Super>
	void __Registry(const std::string& InName)
	{
		m_Parent = Super::StaticClass();
		m_Parent->__RegistryChild(this);
		__Registry(InName);
	}

	/** Called internally to registry class tags */
	void __RegistryTags(ReflectionTags inTags) { m_Tags = inTags; }

	///** Called internally to registry a property of a class */
	template<typename VarType>
	void __RegistryProperty(const std::string& InName, std::function<VarType* (XObject*)>&& access, ReflectionTags tags, const std::string& GroupName = "Default")
	{
		Property* p = new TProperty<VarType>(this, GetReflectableName<VarType>(), InName, std::move(access), tags, GroupName);
		m_Properties.insert(std::make_pair(InName, p));
	}
	

public:
	// Get CDO
	virtual XObject* GetDefaultObject(const std::string& InCDOName = "") = 0;

	virtual XObject* CreateInstance(XObject* Owner, const std::string& InName = "", const std::string& InCustomCDOName = "") = 0;
	
	
	void GetAllChildren(FChildContainer& outChildren) const;

	void GetInstanceableChilren(std::vector<XClass*>& outInstaceableChildren);


	const FChildContainer& GetOwnChildren() const;

	XClass* FindChild(const std::string& InName) const;

	XClass* GetParent() const { return m_Parent; }


	virtual void RegisterCDO(const std::string& InCustomCDOName, XObject* LoadedInst,  const std::string& InPathKey) = 0;

	virtual void ReNameCDO(const std::string& InOrigin, const std::string& InRename) = 0;

	virtual XObject* GetCustomCDO(const std::string& InCDOName) = 0;

	virtual const std::string& GetCustomCDOPathKey() = 0;

	virtual size_t GetAllCustomCDO(std::vector<XObject*>& AllOfCustomCDO) = 0;	

	virtual size_t GetOwnCustomCDO(std::vector<XObject*>& OwnCustomCDO) = 0;

	virtual void DeleteCDO(const std::string& InCDOName) = 0;


	template<typename T>
	T* GetCustomCDO(const std::string& InCDOName) 
	{
		return (T*)GetCustomCDO(InCDOName);
	}




private:
	void __RegistryChild(XClass* child)
	{
		m_Children.push_back(child);
	}
};

