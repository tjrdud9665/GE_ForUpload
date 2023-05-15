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
	//���� ������ �ε� �Ͽ��� �ϴ� �����Ͱ� �ִٸ� �������� ������ ����� �ҷ����ִ� �ɼǵ��� ����.

}