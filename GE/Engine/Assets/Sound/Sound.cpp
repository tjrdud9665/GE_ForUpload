#include "EnginePCH.h"
#include "Sound.h"
#include "../../Path/PathManager.h"
#include "../../SoundManager.h"

Sound::Sound()
	: m_System(nullptr)
	, m_Sound(nullptr)
	, m_Group(nullptr)
	, m_Channel(nullptr)
	, m_Play(false)
	, m_Loop(false)
	, m_Pause(false)
{
}

Sound::Sound(const Sound& Other)
	:Super(Other)
	, m_System(Other.m_System)
	, m_Sound(Other.m_Sound)
	, m_Group(Other.m_Group)
	, m_Channel(Other.m_Channel)
	, m_Play(Other.m_Play)
	, m_Loop(Other.m_Loop)
	, m_Pause(Other.m_Pause)
	, m_GroupName(Other.m_GroupName)
{

}

Sound::~Sound()
{
}

const std::string& Sound::GetGroupName() const
{
	return m_GroupName;
}

bool Sound::GetPlay() const
{
	return m_Play;
}

bool Sound::GetLoop() const
{
	return m_Loop;
}

void Sound::SetLoop(bool InLoop)
{
	m_Loop = InLoop;
}

void Sound::SetGroupName(const std::string& GroupName)
{
	m_GroupName = GroupName;
}

void Sound::SetGroup(FMOD::ChannelGroup* InGroup)
{
	m_Group = InGroup;
}

bool Sound::LoadSound(FMOD::System* System, FMOD::ChannelGroup* Group, bool Loop, const char* FileName)
{
	m_System = System;
	m_Group = Group;
	m_Loop = Loop;

	char	FullPath[MAX_PATH] = {};

	const SPathInfo* Info = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);


	if (Info)
		strcpy_s(FullPath, Info->aPath.c_str());

	strcat_s(FullPath, FileName);



	FMOD_MODE	Mode = FMOD_DEFAULT;

	if (Loop)
		Mode = FMOD_LOOP_NORMAL;

	if (m_System->createSound(FullPath, Mode, nullptr, &m_Sound) != FMOD_OK)
		return false;

	return true;
}

bool Sound::LoadSoundFullPath(FMOD::System* System, FMOD::ChannelGroup* Group, bool Loop, const char* FullPath)
{
	m_System = System;
	m_Group = Group;
	m_Loop = Loop;


	FMOD_MODE	Mode = FMOD_DEFAULT;

	if (Loop)
		Mode = FMOD_LOOP_NORMAL;

	if (m_System->createSound(FullPath, Mode, nullptr, &m_Sound) != FMOD_OK)
		return false;

	return true;
}

void Sound::Play()
{
	m_System->playSound(m_Sound, m_Group, false, &m_Channel);

	m_Play = true;
	m_Pause = false;
}

void Sound::Stop()
{
	if (m_Channel)
	{
		bool	Playing = false;

		m_Channel->isPlaying(&Playing);


		if (Playing)
		{
			m_Channel->stop();
			m_Channel = nullptr;

			m_Play = false;
			m_Pause = false;
		}
	}

}

void Sound::Pause()
{
	if (m_Channel)
	{
		bool	Playing = false;

		m_Channel->isPlaying(&Playing);

		if (Playing)
			m_Channel->setPaused(true);

		m_Play = false;
		m_Pause = true;
	}

}

void Sound::Resume()
{
	if (m_Channel)
	{
		if (m_Pause)
		{
			m_Channel->setPaused(false);

			m_Play = true;
			m_Pause = false;
		}
	}
}

void Sound::TryToResumePlay()
{
	bool	Playing = false;

	if (m_Channel)
		m_Channel->isPlaying(&Playing);

	if (m_Pause)
	{
		Resume();
	}
	else if (!Playing)
	{
		Play();
	}
}

uint32 Sound::GetPosition() const
{
	uint32 position = 0;

	if (m_Channel)
		m_Channel->getPosition(&position, FMOD_TIMEUNIT_MS);

	return position;

}


uint32 Sound::GetLength() const
{
	uint32 length = 0;

	if (m_Sound)
		m_Sound->getLength(&length, FMOD_TIMEUNIT_MS);


	return length;
}

void Sound::SetPosition(uint32 InPos)
{
	if (m_Channel)
		m_Channel->setPosition(InPos, FMOD_TIMEUNIT_MS);
}


bool Sound::PostLoad()
{
	bool Result = Super::PostLoad();



	SoundManager* soundMgr = g_Engine->Get<SoundManager>();

	std::string RelPathWithName;
	GetRelativePathWithName(OUT RelPathWithName);
	if (LoadSound(soundMgr->GetSystem(), soundMgr->FindChannelGroup(m_GroupName), m_Loop, RelPathWithName.c_str()))
	{
		Result |= true;
	}

	return Result;
}
