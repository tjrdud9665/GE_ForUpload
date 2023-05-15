#include "EnginePCH.h"
#include "SoundManager.h"
#include "Path/PathManager.h"
#include "AssetManager.h"
#include "Assets/Sound/Sound.h"

SoundManager::SoundManager()
	:m_System(nullptr)
	, m_MasterGroup(nullptr)
{

}

SoundManager::~SoundManager()
{
	m_mapSound.clear();

	FChannelGroupMap::iterator	iter = m_mapChannelGroup.begin();
	FChannelGroupMap::iterator	iterEnd = m_mapChannelGroup.end();

	for (; iter != iterEnd; ++iter)
	{
		iter->second->release();
	}

	if (m_System)
	{
		m_System->release();
		m_System->close();
	}
}

bool SoundManager::Initialize()
{
	// FMOD_RESULT : FMOD에서 제공하는 타입으로 FMOD의 기능을 사용할때 함수가 성공했는지 실패햇는지를
	// 판단하기 위한 값의 타입이다.
	FMOD_RESULT result = FMOD::System_Create(&m_System);

	if (result != FMOD_OK)
		return false;

	result = m_System->init(500, FMOD_INIT_NORMAL, nullptr);

	if (result != FMOD_OK)
		return false;

	result = m_System->getMasterChannelGroup(&m_MasterGroup);

	if (result != FMOD_OK)
		return false;

	m_mapChannelGroup.insert(std::make_pair("Master", m_MasterGroup));

	CreateSoundChannel("BGM");
	CreateSoundChannel("Effect");
	CreateSoundChannel("UI");

	return true;
}

void SoundManager::Update(float DeltaSeconds)
{
	m_System->update();
}

bool SoundManager::CreateSoundChannel(const std::string& InName)
{
	if (FindChannelGroup(InName))
		return false;

	FMOD::ChannelGroup* Group = nullptr;

	FMOD_RESULT	result = m_System->createChannelGroup(InName.c_str(), &Group);

	if (result != FMOD_OK)
		return false;

	// 생성한 그룹을 마스터 그룹에 추가해준다.
	m_MasterGroup->addGroup(Group, false);

	m_mapChannelGroup.insert(std::make_pair(InName, Group));

	return true;
}

bool SoundManager::LoadSound(const std::string& GroupName, const std::string& Name, bool Loop, const char* FileName, const std::string& PathName /*= SOUND_PATH*/)
{
	Sound* sound = FindSound(Name);

	if (sound)
		return true;

	FMOD::ChannelGroup* Group = FindChannelGroup(GroupName);

	if (!Group)
		return false;

	sound = Globals::CreateX<Sound>(this, Name);

	sound->SetName(Name);

	if (!sound->LoadSound(m_System, Group, Loop, FileName))
	{
		SAFE_DELETE(sound);
		return false;
	}

	sound->SetGroupName(GroupName);

	m_mapSound.insert(std::make_pair(Name, sound));

	return true;
}

bool SoundManager::SetVolume(int32 Volume)
{
	m_MasterGroup->setVolume(Volume / 100.f);

	return true;

}

bool SoundManager::SetVolume(const std::string& GroupName, int32 Volume)
{
	FMOD::ChannelGroup* Group = FindChannelGroup(GroupName);

	if (!Group)
		return false;

	Group->setVolume(Volume / 100.f);

	return true;
}

bool SoundManager::SoundPlay(const std::string& InName)
{
	Sound* sound = FindSound(InName);

	if (!sound)
		return false;

	sound->Play();

	return true;
}

bool SoundManager::SoundStop(const std::string& InName)
{
	Sound* sound = FindSound(InName);

	if (!sound)
		return false;

	sound->Stop();

	return true;
}

bool SoundManager::SoundPause(const std::string& InName)
{
	Sound* sound = FindSound(InName);

	if (!sound)
		return false;

	sound->Pause();

	return true;
}

bool SoundManager::SoundResume(const std::string& InName)
{
	Sound* sound = FindSound(InName);

	if (!sound)
		return false;

	sound->Resume();

	return true;
}

FMOD::ChannelGroup* SoundManager::FindChannelGroup(const std::string& InName)
{
	FChannelGroupMap::iterator	iter = m_mapChannelGroup.find(InName);

	if (iter == m_mapChannelGroup.end())
		return nullptr;

	return iter->second;
}

size_t SoundManager::GetAllChannelGroups(std::vector<std::string>& OutGroupNames)
{
	size_t GroupSize = m_mapChannelGroup.size();
	FChannelGroupMap::iterator iter = m_mapChannelGroup.begin();
	FChannelGroupMap::iterator End = m_mapChannelGroup.end();

	for (; iter != End; iter++)
	{
		OutGroupNames.push_back((*iter).first);
	}

	return GroupSize;
}

class Sound* SoundManager::FindSound(const std::string& InName)
{
	FSoundMap::iterator	iter = m_mapSound.find(InName);

	if (iter == m_mapSound.end())
		return nullptr;

	return iter->second;
}

void SoundManager::ReleaseSound(const std::string& InName)
{
	FSoundMap::iterator	iter = m_mapSound.find(InName);

	if (iter == m_mapSound.end())
		return;

	if (iter->second->GetRefCount() == 1)
		m_mapSound.erase(iter);
}

FMOD::System* SoundManager::GetSystem() const
{
	return m_System;
}