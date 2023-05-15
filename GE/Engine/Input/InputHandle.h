#pragma once
#include "Input.h"

struct SInputHandle 
{
	int64 InputID;
	std::string InputName;

	SInputHandle(int64 Id, std::string Name)
		:InputID(Id),
		InputName(Name)
	{

	}
};

class InputHandle
{
	using InputIDList = std::vector<SInputHandle>;	
public:
	InputHandle();
	~InputHandle();

public:
	uint64 AddActionBind(const std::string& InName, EInputEventType EventType, std::function<void(const std::string&,uint64)> callback);
	void ClearActionBind(const std::string& InName, EInputEventType EventType, uint64 ID = 0);

	uint64 AddAxisBind(const std::string& InName, std::function<void(float)> callbck);
	void ClearAxisBind(const std::string& InName, uint64 ID = 0);

	void ClearAllBinds();


private:
	InputIDList			m_ActionHandles;
	InputIDList			m_AxisHandles;
};

