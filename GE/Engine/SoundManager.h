#pragma once
#include "XObject/XObject.h"

class SoundManager : public XObject
{
	friend class Globals;
	using FSoundMap = std::unordered_map<std::string, TSharedPtr<class Sound>>;
	using FChannelGroupMap = std::unordered_map<std::string, FMOD::ChannelGroup*>;

private:
	SoundManager();
	virtual ~SoundManager();


public:
	bool Initialize();

	virtual void Update(float DeltaSeconds)	override;

	bool CreateSoundChannel(const std::string& InName);

	bool LoadSound(const std::string& GroupName, const std::string& Name, bool Loop, const char* FileName, const std::string& PathName = SOUND_PATH);

	bool SetVolume(int32 Volume);

	bool SetVolume(const std::string& GroupName, int32 Volume);
	bool SoundPlay(const std::string& InName);
	bool SoundStop(const std::string& InName);
	bool SoundPause(const std::string& InName);
	bool SoundResume(const std::string& InName);


	FMOD::ChannelGroup* FindChannelGroup(const std::string& InName);

	size_t GetAllChannelGroups(std::vector<std::string>& OutGroupNames);


	class Sound* FindSound(const std::string& InName);
	void ReleaseSound(const std::string& InName);

	FMOD::System* GetSystem() const;

private:
	FSoundMap			m_mapSound;
	FMOD::System*		m_System;
	FMOD::ChannelGroup* m_MasterGroup;
	FChannelGroupMap	m_mapChannelGroup;
};

