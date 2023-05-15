#pragma once
#include "../AssetData.h"

class Sound : public AssetData
{
	CLASS(Sound, AssetData)
	friend class SoundManager;

private:
	Sound();
	Sound(const Sound& Other);
	virtual ~Sound();

public:
	const std::string& GetGroupName()   const;

	bool GetPlay()   const;

	bool GetLoop()  const;

	void SetLoop(bool InLoop);


public:
	void SetGroupName(const std::string& GroupName);
	void SetGroup(FMOD::ChannelGroup* InGroup);

public:
	bool LoadSound(FMOD::System* System, FMOD::ChannelGroup* Group, bool Loop, const char* FileName);

	bool LoadSoundFullPath(FMOD::System* System, FMOD::ChannelGroup* Group, bool Loop, const char* FullPath);

	void Play();

	void Stop();

	void Pause();

	void Resume();

	void TryToResumePlay();

	uint32 GetPosition() const;

	uint32 GetLength() const;

	void SetPosition(uint32 InPos);







private:
	FMOD::System*		m_System;
	FMOD::Sound*		m_Sound;
	FMOD::ChannelGroup* m_Group;
	FMOD::Channel*		m_Channel;

	PROP(std::string, m_GroupName, DetailsView, "Sound")
	std::string     m_GroupName;

	bool        m_Play;

	bool        m_Loop;
	bool        m_Pause;

protected:
	virtual bool PostLoad() override;
};

