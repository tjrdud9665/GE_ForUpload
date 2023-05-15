#pragma once

#include "../Reflection/TClass.h"
#include "../Reflection/Reflection.h"
#include "../Serialization/Archive.h"

//XObject를 상속받아서 만든클래스는 오브젝트를 직접삭제하지못하게 막아야한다.

class XObject
{
	ORPHAN_CLASS(XObject)
	friend class Engine;

	template <class TypeA>
	friend class TClass;
	
	
	//모든 생성은 Globals 를통해서할수있도록

protected:
	/* ctor */
	XObject();
	
	/* dtor */
	virtual ~XObject();

public:
	/* copy ctor */
	XObject(const XObject& Other);

public:
	/* Call Update eve Update */
	virtual void PreUpdate(float DeltaSeconds);

	virtual void Update(float DeltaSeconds);

	/* Call Update After Update */
	virtual void PostUpdate(float DeltaSeconds);


	/* SetEnable State. */
	virtual void Enable(bool bEnable);

	/* Delete Next Tick. */
	void Kill();

	/* Is Delete Next Tick ?*/
	bool IsValidate() const;

	/* Is On Tick(). */
	bool IsEnable() const;

	/* Add Reference Counter. */
	void AddRef();

	/* Get Refernce Counter. */
	int32 GetRefCount() const;

	/* Decrease Refernce Count. */
	int32 Release();

	/* Set Object's Name */
	virtual void SetName(const std::string& InName);

	/* Get Object's Name. */
	const std::string& GetName()	const;

	virtual void GetRelativePathWithName(std::string& OutRelPathWithName) const;

	virtual void SetOwner(XObject* InOwner);

	bool	IsConstructed() const;



	virtual XObject* Clone();


public:
	virtual void PreLoad() {}
	virtual bool PostLoad() { return true; }
	virtual void OnUnload(){}



public:
	void PreConstruct(XObject* InSelf, XClass* InClass, XObject* InOwner)
	{				
		m_Self = InSelf;
		m_OwnClass = InClass;
		SetOwner(InOwner);
	}

	virtual void Construct(const std::string& InName = "")
	{		
		m_Constructed = true;
		m_Name = InName;
		if(m_Name.empty())
		{
			m_Name = GetClass()->GetName();
		}
	}

	virtual bool Serialize(Archive& ar)
	{
		SerializeReflection(ar);
		return true;
	}

	template<typename T>
	T* GetSelf() const { return dynamic_cast<T*>(m_Self); }

	template<typename T>
	T* GetOwner() const { return dynamic_cast<T*>(m_Owner); }


public:
	XClass*		GetClass() const;
	XObject*	GetSelf() const;
	XObject*	GetOwner()	const;


protected:
	//외부에서 호출 x
	virtual void OnDestroy();


private:
	PROP(std::string, m_Name,DetailsView)
	std::string		m_Name;



	//atomic
	int32			m_Refs;	


	bool			m_Enable;
	bool			m_PendingKill;	

	bool			m_Constructed;


private:
	XObject*		m_Self;

	PROP(XClass*, m_OwnClass,DetailsView)
	XClass*			m_OwnClass;

	XObject*		m_Owner;


	//mutable std::list<XObject*> m_Childs;
};