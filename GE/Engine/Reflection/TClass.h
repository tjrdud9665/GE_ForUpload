#pragma once

#include "XClass.h"
#include "TProperty.h"

template<typename Type>
class TClass : public XClass
{
private:
	template <class TypeA>
	friend class TClass;

	template <class TypeA>
	friend class TSubclassOf;

private:
	static TClass _class;
	Type* _CDO;		

	std::unordered_map<std::string, XObject*>	_loadedCDO;
	std::string									_CustomCDOPathKey;



public:
	TClass() : XClass()
	{
		Type::__meta_Registry();
		Type::__meta_RegistryProperties();		
		_CDO = new Type();
		_CDO->m_OwnClass = this;		

	}
	virtual ~TClass()
	{		
		SAFE_DELETE(_CDO);
		_loadedCDO.clear();
		//std::unordered_map<std::string, Type*>::iterator iter = _loadedCDO.begin();
		//std::unordered_map<std::string, Type*>::iterator End = _loadedCDO.end();
		//for(;iter != End;iter++)
		//{			
		//}		

	}

	virtual void RegisterCDO(const std::string& InCustomCDOName, XObject* LoadedInst , const std::string& InPathKey) override
	{
		_loadedCDO.insert(std::make_pair(InCustomCDOName, LoadedInst));
		_CustomCDOPathKey = InPathKey;
	}
	virtual void ReNameCDO(const std::string& InOrigin, const std::string& InRename) override
	{
		std::unordered_map<std::string, XObject*>::iterator findIter = _loadedCDO.find(InOrigin);

		if(findIter != _loadedCDO.end())
		{		
			_loadedCDO.insert(std::make_pair(InRename,findIter->second));			
			_loadedCDO.erase(findIter);
		}

		
	}

	virtual XObject* GetCustomCDO(const std::string& InCDOName)  override
	{
		std::unordered_map<std::string, XObject*>::iterator iter = _loadedCDO.find(InCDOName);

		if (iter != _loadedCDO.end())
			return (*iter).second;
		
		return nullptr;		
	}

	virtual size_t GetOwnCustomCDO(std::vector<XObject*>& OwnCustomCDO) override
	{
		std::unordered_map<std::string, XObject*>::iterator iter = _loadedCDO.begin();
		std::unordered_map<std::string, XObject*>::iterator iter_end = _loadedCDO.end();

		for(;iter != iter_end;iter++)
		{
			if(_CDO)
			{
				if ((*iter).second->GetClass()->IsA(_CDO->GetClass()))
				{
					OwnCustomCDO.push_back((*iter).second);
				}
			}
			
		}

		return OwnCustomCDO.size();
	}

	virtual const std::string& GetCustomCDOPathKey() override
	{
		return _CustomCDOPathKey;
	}

	virtual size_t GetAllCustomCDO(std::vector<XObject*>& AllOfCustomCDO) override
	{
		std::unordered_map<std::string, XObject*>::iterator iter = _loadedCDO.begin();
		std::unordered_map<std::string, XObject*>::iterator End = _loadedCDO.end();
		for(;iter != End;iter++)
		{			
			AllOfCustomCDO.push_back((*iter).second);
		}		

		return AllOfCustomCDO.size();
	}

	virtual void DeleteCDO(const std::string& InCDOName) override
	{
		std::unordered_map<std::string, XObject*>::iterator iter = _loadedCDO.find(InCDOName);
		if (iter != _loadedCDO.end())
			_loadedCDO.erase(iter);
	}

	virtual bool HasCustomCDO() const override
	{
		return _loadedCDO.size() != 0;
	}

public:
	virtual Type* GetDefaultObject(const std::string& InCDOName = "") override
	{
		if(InCDOName.empty())
			return _CDO;

		Type* customCDO = XClass::GetCustomCDO<Type>(InCDOName);

		if (customCDO)
			return customCDO;

		return _CDO;
	}

	virtual Type* CreateInstance(XObject* InOwner, const std::string& InName = "",const std::string& InCustomCDOName = "") override
	{		
		Type* ptr = Create<Type>(InOwner, InName, InCustomCDOName);
		return ptr;
	}


	static TClass* GetStatic() { return &_class; }



private:
	template<typename T, typename = std::enable_if_t<std::is_base_of<XObject, T>::value>>
	T* Create(XObject* InOwner,const std::string& InName , const std::string& InCustomCDOName)
	{
		XObject* cdo = GetCustomCDO(InCustomCDOName);// == nullptr; ? _CDO : GetCustomCDO(InCustomCDOName);
		
		if(!cdo)
		{			
			std::vector<XClass*> cilds;
			Type::StaticClass()->GetAllChildren(OUT cilds);
			size_t childsSize = cilds.size();
			for (size_t i = 0; i < childsSize; i++)
			{
				cdo = cilds[i]->GetCustomCDO(InCustomCDOName);
				if (cdo)
					break;
			}
			if(!cdo)
				cdo = _CDO;
		}

		XObject* ptr = cdo->Clone();	

		if (!ptr->IsConstructed())
		{
			ptr->PreConstruct(ptr->GetSelf(), Type::StaticClass(), InOwner);
			ptr->Construct(InName);
		}
		else
		{
			ptr->SetOwner(InOwner);
			ptr->SetName(InName);
		}

		return (T*)ptr;
	}

};

template <typename Type>
TClass<Type> TClass<Type>::_class{};