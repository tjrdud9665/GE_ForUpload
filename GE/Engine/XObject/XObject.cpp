#include "EnginePCH.h"
#include "XObject.h"

XObject::XObject()
	:m_Refs(0)
	, m_Enable(true)
	, m_PendingKill(false)
	, m_Constructed(false)
	, m_Self(this)
	, m_OwnClass(nullptr)
	, m_Owner(nullptr)
{

}

XObject::XObject(const XObject& Other)
	:m_Refs(0)
	, m_Enable(Other.m_Enable)
	, m_PendingKill(false)
	, m_Name(Other.m_Name)
	, m_Self(Other.m_Self)
	, m_OwnClass(Other.m_OwnClass)
	, m_Owner(Other.m_Owner)
	, m_Constructed(Other.m_Constructed)
{



}

void XObject::PreUpdate(float DeltaSeconds)
{

}

void XObject::Update(float DeltaSeconds)
{
	
}

XObject::~XObject()
{
	m_Constructed = false;
}

void XObject::PostUpdate(float DeltaSeconds)
{

}

void XObject::Enable(bool bEnable)
{
	m_Enable = bEnable;
}

void XObject::Kill()
{
	m_PendingKill = true;
	//OnDestroy();// ∞¢ º“∏Í¿⁄ø°º≠ »£√‚ ø‰∏¡!!
}

bool XObject::IsValidate() const
{
	return !m_PendingKill;
}

bool XObject::IsEnable() const
{
	return m_Enable;
}

void XObject::AddRef()
{
	++m_Refs;
}

int32 XObject::GetRefCount() const
{
	return m_Refs;
}

int32 XObject::Release()
{
	--m_Refs;
	if (m_Refs <= 0)
	{
		OnDestroy();
		delete this;
		return 0;
	}

	return m_Refs;
}

void XObject::SetName(const std::string& InName)
{
	m_Name = InName;
}

const std::string& XObject::GetName() const
{
	return m_Name;
}

void XObject::GetRelativePathWithName(std::string& OutRelPathWithName) const
{
	OutRelPathWithName = m_Name;
}

XObject* XObject::Clone()
{
	return new XObject(*this);
}

XClass* XObject::GetClass() const
{
	return m_OwnClass;
}

XObject* XObject::GetSelf() const
{
	return m_Self;
}

XObject* XObject::GetOwner() const
{
	return m_Owner;
}



void XObject::OnDestroy()
{

}

void XObject::SetOwner(XObject* InOwner)
{
	m_Owner = InOwner;
}

bool XObject::IsConstructed() const
{
	return m_Constructed;
}

