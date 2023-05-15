#pragma once
#include "../XObject/XObject.h"

class GameInstance : public XObject
{
	friend class Globals;
	friend class Engine;

protected:
	GameInstance();
	virtual ~GameInstance();

public:
	void SaveData(const std::string& InName, const Json& InJson);

	bool GetData(const std::string& InName, Json& OutData) const;

	void RemoveData(const std::string& InName);

	void Initialize();


private:
	std::unordered_map<std::string, Json>	m_Data;
};

