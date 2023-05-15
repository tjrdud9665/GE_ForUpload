#include "EnginePCH.h"
#include "InputHandle.h"



InputHandle::InputHandle()
{

}

InputHandle::~InputHandle()
{
	if(g_Engine)
	{
		{
			InputIDList::iterator iter = m_ActionHandles.begin();
			InputIDList::iterator iterEnd = m_ActionHandles.end();

			for (; iter != iterEnd; iter++)
			{
				g_Engine->Get<Input>()->ClearActionBind((*iter).InputName, EInputEventType::IE_Pressed, (*iter).InputID);
				g_Engine->Get<Input>()->ClearActionBind((*iter).InputName, EInputEventType::IE_Released, (*iter).InputID);
			}
		}

		{
			InputIDList::iterator iter = m_AxisHandles.begin();
			InputIDList::iterator iterEnd = m_AxisHandles.end();

			for (; iter != iterEnd; iter++)
			{
				g_Engine->Get<Input>()->ClearAxisBind((*iter).InputName,(*iter).InputID);
				g_Engine->Get<Input>()->ClearAxisBind((*iter).InputName, (*iter).InputID);
			}

		}
	}

}

uint64 InputHandle::AddActionBind(const std::string& InName, EInputEventType EventType, std::function<void(const std::string&,uint64)> callback)
{
	uint64 Id = g_Engine->Get<Input>()->AddActionBind(InName, EventType, callback);

	m_ActionHandles.push_back(SInputHandle(Id,InName));

	return Id;
}

void InputHandle::ClearActionBind(const std::string& InName, EInputEventType EventType, uint64 ID /*= INDEX_NONE*/)
{
	{
		InputIDList::iterator iter = m_ActionHandles.begin();
		InputIDList::iterator iterEnd = m_ActionHandles.end();

		for (; iter != iterEnd; iter++)
		{
			if (ID == (*iter).InputID)
			{
				m_ActionHandles.erase(iter);
				break;
			}
		}
	}
	g_Engine->Get<Input>()->ClearActionBind(InName, EventType,ID);
}

uint64 InputHandle::AddAxisBind(const std::string& InName, std::function<void(float)> callbck)
{
	uint64 Id =  g_Engine->Get<Input>()->AddAxisBind(InName, callbck);
	m_AxisHandles.push_back(SInputHandle(Id, InName));

	return Id;
}

void InputHandle::ClearAxisBind(const std::string& InName, uint64 ID)
{
	{
		InputIDList::iterator iter = m_AxisHandles.begin();
		InputIDList::iterator iterEnd = m_AxisHandles.end();

		for (; iter != iterEnd; iter++)
		{
			if (ID == (*iter).InputID)
			{
				m_AxisHandles.erase(iter);
				break;
			}
		}
	}

	g_Engine->Get<Input>()->ClearAxisBind(InName, ID);
}

void InputHandle::ClearAllBinds()
{
	{
		InputIDList::iterator iter = m_ActionHandles.begin();
		InputIDList::iterator iterEnd = m_ActionHandles.end();

		for (; iter != iterEnd; iter++)
		{
			g_Engine->Get<Input>()->ClearActionBind((*iter).InputName, EInputEventType::IE_Pressed, 0);
			g_Engine->Get<Input>()->ClearActionBind((*iter).InputName, EInputEventType::IE_Released, 0);
		}
	}

	{
		InputIDList::iterator iter = m_AxisHandles.begin();
		InputIDList::iterator iterEnd = m_AxisHandles.end();

		for (; iter != iterEnd; iter++)
		{
			g_Engine->Get<Input>()->ClearAxisBind((*iter).InputName, (*iter).InputID);
			g_Engine->Get<Input>()->ClearAxisBind((*iter).InputName, (*iter).InputID);
		}

	}
}
