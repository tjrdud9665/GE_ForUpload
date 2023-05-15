#include "EnginePCH.h"
#include "GameInstance.h"

GameInstance::GameInstance()
{

}

GameInstance::~GameInstance()
{

}

void GameInstance::SaveData(const std::string& InName, const Json& InJson)
{
	m_Data.insert(std::make_pair(InName, InJson));

}

bool GameInstance::GetData(const std::string& InName, Json& OutData) const
{
	std::unordered_map<std::string, Json>::const_iterator findIter = m_Data.find(InName);

	if (findIter != m_Data.end())
	{
		OutData = findIter->second;
		return true;
	}

	return false;
}

void GameInstance::RemoveData(const std::string& InName)
{
	std::unordered_map<std::string, Json>::iterator findIter = m_Data.find(InName);

	if (findIter != m_Data.end())
	{
		m_Data.erase(findIter);
	}
}

void GameInstance::Initialize()
{
	//만약 종료후 로드 하여야 하는 데이터가 있다면 종료전에 파일을 만들고 불러와주는 옵션등을 제작.

}