#include "EnginePCH.h"
#include "CollisionManager.h"
#include <Path/PathManager.h>
#include <GameFramework/Components/PrimitiveComponent.h>

CollisionManager::CollisionManager()
{

}

CollisionManager::~CollisionManager()
{
	{
		FCollisionProfileMap::iterator	iter = m_mapProfile.begin();
		FCollisionProfileMap::iterator	iterEnd = m_mapProfile.end();

		for (; iter != iterEnd; ++iter)
		{
			SAFE_DELETE(iter->second);
		}
	}

	{
		FCollisionChannelList::iterator	iter = m_vecChannel.begin();
		FCollisionChannelList::iterator	iterEnd = m_vecChannel.end();

		for (; iter != iterEnd; ++iter)
		{
			SAFE_DELETE((*iter));
		}
	}

	{
		FCustomCollisionProfileMap::iterator iter = m_CustomProfiles.begin();
		FCustomCollisionProfileMap::iterator iterEnd = m_CustomProfiles.end();

		for (; iter != iterEnd; ++iter)
		{
			SAFE_DELETE(iter->second);
		}
	}


	m_mapProfile.clear();
	m_CustomProfiles.clear();
	m_vecChannel.clear();

}

void CollisionManager::Initialize()
{
	CreateChannel("Default");
	CreateChannel("Mouse");
	//CreateChannel("Custom0_Channel");
	//CreateChannel("Custom1_Channel");
	//CreateChannel("Custom2_Channel");
	//CreateChannel("Custom3_Channel");
	//CreateChannel("Custom4_Channel");



	CreateProfile("Default", "Default", true, ECollision_Interaction::Overlap, "Default Collision Profile.");
	CreateProfile("Mouse", "Mouse", true, ECollision_Interaction::Overlap, "Mouse Collision Profile.");

	//CreateProfile("Custom0", "Custom0_Channel", true,ECollision_Interaction::Ignore);
	//SetCollisionInteraction("Custom0", "Custom0_Channel", ECollision_Interaction::Collision);

	Load();



}

bool CollisionManager::CreateProfile(const std::string& Name, const std::string& ChannelName, bool Enable, ECollision_Interaction BaseInteraction, const std::string& Desc)
{
	SCollisionProfile* Profile = FindProfile(Name);

	if (Profile)
		return false;

	SCollisionChannel* Channel = nullptr;

	size_t	Count = m_vecChannel.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (m_vecChannel[i]->Name == ChannelName)
		{
			Channel = m_vecChannel[i];
			break;
		}
	}

	if (!Channel)
		return false;

	Profile = new SCollisionProfile;

	Profile->Name = Name;
	Profile->Channel = Channel;
	Profile->Enable = Enable;
	Profile->vecCollisionInteraction.resize(m_vecChannel.size());

	Count = Profile->vecCollisionInteraction.size();

	for (size_t i = 0; i < Count; ++i)
	{
		Profile->vecCollisionInteraction[i] = BaseInteraction;
	}

	Profile->Descrition = Desc;
	Profile->Defaults = true;
	m_mapProfile.insert(std::make_pair(Name, Profile));


	return true;
}

bool CollisionManager::CreateProfile(const std::string& Name, const std::string& ChannelName, bool Enable, const std::vector<ECollision_Interaction>& Interactions, const std::string& Desc /*= ""*/)
{
	if (Name.empty())
		return false;

	if (Name == "Custom")
	{
		return false;
	}

	SCollisionProfile* Profile = FindProfile(Name);

	if (Profile)
		return false;

	SCollisionChannel* Channel = nullptr;

	size_t	Count = m_vecChannel.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (m_vecChannel[i]->Name == ChannelName)
		{
			Channel = m_vecChannel[i];
			break;
		}
	}

	if (!Channel)
		return false;

	Profile = new SCollisionProfile;

	Profile->Name = Name;
	Profile->Channel = Channel;
	Profile->Enable = Enable;
	Profile->vecCollisionInteraction.resize(m_vecChannel.size());

	Count = Profile->vecCollisionInteraction.size();

	for (size_t i = 0; i < Count; ++i)
	{
		Profile->vecCollisionInteraction[i] = Interactions[i];
	}

	Profile->Descrition = Desc;
	Profile->Defaults = false;
	m_mapProfile.insert(std::make_pair(Name, Profile));


	return true;
}

bool CollisionManager::SetCollisionInteraction(const std::string& Name, const std::string& ChannelName, ECollision_Interaction Interaction)
{
	SCollisionProfile* Profile = FindProfile(Name);

	if (!Profile)
		return false;

	SCollisionChannel* Channel = nullptr;

	size_t	Count = m_vecChannel.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (m_vecChannel[i]->Name == ChannelName)
		{
			Channel = m_vecChannel[i];
			break;
		}
	}

	if (!Channel)
		return false;

	Profile->vecCollisionInteraction[(int32)Channel->Channel] = Interaction;

	return true;
}

bool CollisionManager::CreateChannel(const std::string& Name, ECollision_Interaction Interaction /*= ECollision_Interaction::Collision*/)
{
	size_t	Count = m_vecChannel.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (m_vecChannel[i]->Name == Name)
			return false;
	}

	SCollisionChannel* NewChannel = new SCollisionChannel;

	NewChannel->Name = Name;
	NewChannel->Channel = (ECollision_Channel)m_vecChannel.size();
	NewChannel->Interaction = Interaction;

	m_vecChannel.push_back(NewChannel);

	FCollisionProfileMap::iterator	iter = m_mapProfile.begin();
	FCollisionProfileMap::iterator	iterEnd = m_mapProfile.end();

	for (; iter != iterEnd; ++iter)
	{
		iter->second->vecCollisionInteraction.push_back(Interaction);
	}

	ApplyProfile();
	return true;
}

SCollisionProfile* CollisionManager::FindProfile(const std::string& Name) const
{
	FCollisionProfileMap::const_iterator	iter = m_mapProfile.find(Name);

	if (iter == m_mapProfile.cend())
	{
		return nullptr;
	}

	return iter->second;
}

size_t CollisionManager::GetAllProfileName(std::vector<std::string>& OutName)
{
	size_t profileSize = m_mapProfile.size();
	OutName.clear();

	OutName.resize(profileSize);
	FCollisionProfileMap::const_iterator	iter = m_mapProfile.begin();
	FCollisionProfileMap::const_iterator	End = m_mapProfile.end();

	int32 i = 0;
	for (; iter != End; iter++)
	{
		OutName[i++] = iter->first;
	}

	return profileSize;
}

size_t CollisionManager::GetCustomProfileName(std::vector<std::string>& OutName)
{
	size_t profileSize = m_mapProfile.size();
	OutName.clear();


	FCollisionProfileMap::const_iterator	iter = m_mapProfile.begin();
	FCollisionProfileMap::const_iterator	End = m_mapProfile.end();

	int32 i = 0;
	for (; iter != End; iter++)
	{
		if (!iter->second->Defaults)
			OutName.push_back(iter->first);
	}

	return OutName.size();
}

size_t CollisionManager::GetAllCustomChannelName(std::vector<std::string>& OutName)
{
	size_t channelSize = m_vecChannel.size() - (int32)ECollision_Channel::Custom0;
	OutName.clear();

	OutName.resize(channelSize);

	FCollisionChannelList::const_iterator	iter = m_vecChannel.begin() + (int32)ECollision_Channel::Custom0;
	FCollisionChannelList::const_iterator	End = m_vecChannel.end();

	int32 i = 0;
	for (; iter != End; iter++)
	{
		OutName[i++] = (*iter)->Name;
	}

	return channelSize;
}

size_t CollisionManager::GetAllChannelName(std::vector<std::string>& OutName)
{
	size_t channelSize = m_vecChannel.size();
	OutName.clear();

	OutName.resize(channelSize);

	FCollisionChannelList::const_iterator	iter = m_vecChannel.begin();
	FCollisionChannelList::const_iterator	End = m_vecChannel.end();

	int32 i = 0;
	for (; iter != End; iter++)
	{
		OutName[i++] = (*iter)->Name;
	}

	return channelSize;
}

void CollisionManager::GetDefaultInteractions(std::vector<ECollision_Interaction>& OutInteractions)
{
	size_t channelSize = m_vecChannel.size();
	OutInteractions.resize(channelSize);

	for (size_t i = 0; i < channelSize; i++)
	{
		OutInteractions[i] = m_vecChannel[i]->Interaction;
	}


}

SCollisionChannel* CollisionManager::GetCollisionChannel(int32 Idx)
{
	if (Idx < m_vecChannel.size())
		return m_vecChannel[Idx];

	return nullptr;
}

SCollisionChannel* CollisionManager::GetCollisionChannel(const std::string& InChannelName)
{
	size_t	Count = m_vecChannel.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (m_vecChannel[i]->Name == InChannelName)
			return m_vecChannel[i];
	}

	return nullptr;

}

bool CollisionManager::ModifyChannelName(const std::string& InChannelName, const std::string& toName, ECollision_Interaction DefaultInteraction)
{
	bool SetOnlyInteration = InChannelName == toName;

	//고치려고 하는이름이 이미있따면 false.
	size_t	Count = m_vecChannel.size();

	if (!SetOnlyInteration)
	{
		for (size_t i = 0; i < Count; ++i)
		{
			if (m_vecChannel[i]->Name == toName)
			{
				return true;
			}
		}
	}


	for (size_t i = 0; i < Count; ++i)
	{
		if (m_vecChannel[i]->Name == InChannelName)
		{
			m_vecChannel[i]->Name = toName;
			m_vecChannel[i]->Interaction = DefaultInteraction;
		}
	}


	//Apply All Profiles
	ApplyProfile();





	return false;
}

void CollisionManager::DeleteChannel(const std::string& InChannelName)
{
	size_t channelSize = m_vecChannel.size();
	for (size_t i = 0; i < channelSize; i++)
	{
		if (m_vecChannel[i]->Name == InChannelName)
		{
			DeleteChannelAllProfile(m_vecChannel[i]);
			SAFE_DELETE(m_vecChannel[i]);
			m_vecChannel.erase(m_vecChannel.begin() + i);
			break;
		}
	}


	ApplyProfile();
	Save();
}

void CollisionManager::DeleteProfile(const std::string& InProfileName)
{
	FCollisionProfileMap::iterator	findIter = m_mapProfile.find(InProfileName);
	SCollisionProfile* deleteProfile = nullptr;

	if (findIter != m_mapProfile.end())
	{
		deleteProfile = findIter->second;
		m_mapProfile.erase(findIter);

	}

	//Profile을 참조하는 모든객체들에게 삭제를 통지하여야한다.
	FProfileRetedList::iterator iter = m_ProfileRentalList.begin();
	FProfileRetedList::iterator End = m_ProfileRentalList.end();

	for (; iter != End; iter++)
	{
		if ((*iter)->GetCollisionProfile() == deleteProfile)
			(*iter)->OnProfileLost(FindProfile("Default"));
	}



	SAFE_DELETE(deleteProfile);

}

bool CollisionManager::IsDefaultProfile(const std::string& InProfileName)
{
	SCollisionProfile* profile = FindProfile(InProfileName);

	if (profile)
	{
		return profile->Defaults;
	}

	return false;
}

bool CollisionManager::ModifyProfile(const std::string& InProfileName, const std::string& toProfileName, const std::string& ChannelName, bool Enable, const std::vector<ECollision_Interaction>& Interactions, const std::string& Desc)
{
	SCollisionProfile* Profile = FindProfile(InProfileName);

	if (!Profile)
		return false;


	SCollisionChannel* Channel = nullptr;

	size_t	Count = m_vecChannel.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (m_vecChannel[i]->Name == ChannelName)
		{
			Channel = m_vecChannel[i];
			break;
		}
	}

	if (!Channel)
		return false;


	Profile->Name = toProfileName;
	Profile->Channel = Channel;
	Profile->Enable = Enable;
	Profile->vecCollisionInteraction.resize(m_vecChannel.size());

	Count = Profile->vecCollisionInteraction.size();

	for (size_t i = 0; i < Count; ++i)
	{
		Profile->vecCollisionInteraction[i] = Interactions[i];
	}

	Profile->Descrition = Desc;

	m_mapProfile.erase(InProfileName);
	m_mapProfile.insert(std::make_pair(toProfileName, Profile));
	return true;
}

bool CollisionManager::Serialize(Archive& ar)
{
	//Serailzize Datas.
	size_t	Count = m_vecChannel.size();
	std::vector<std::string>	profileNames;
	ar.Serialize("CollisionChannels", m_vecChannel);


	int32 profileSize = (int32)GetAllProfileName(OUT profileNames);

	ar.Serialize("profileSize", profileSize);
	ar.Serialize("profileNames", profileNames);

	FCollisionProfileMap::iterator iter = m_mapProfile.begin();
	FCollisionProfileMap::iterator End = m_mapProfile.end();

	for (int32 i = 0; i < profileSize; i++)
	{
		//ar.BeginObject(profileNames[i]);		

		SCollisionProfile* profile = FindProfile(profileNames[i]);
		if (!profile)
		{
			profile = new SCollisionProfile();

			m_mapProfile.insert(make_pair(profileNames[i], profile));

		}
		ar.Serialize(profileNames[i].c_str(), profile);


		//ar.EndObject();

	}

	return true;
}

SCollisionProfile* CollisionManager::RentalProfile(class PrimitiveComponent* InRental, const std::string& InProfileName)
{
	SCollisionProfile* rentalProfile = FindProfile(InProfileName);

	m_ProfileRentalList.push_back(InRental);


	if (!rentalProfile)
	{
		rentalProfile = FindProfile("Default");
	}

	return rentalProfile;
}

void CollisionManager::ReturnProfile(class PrimitiveComponent* InRentaled)
{
	m_ProfileRentalList.remove(InRentaled);


	FCustomCollisionProfileMap::iterator findIter = m_CustomProfiles.find(InRentaled);

	if (findIter != m_CustomProfiles.end())
	{
		SAFE_DELETE(findIter->second);
		m_CustomProfiles.erase(findIter);
	}


}

SCollisionProfile* CollisionManager::CreateCustomProfile(class PrimitiveComponent* InProfileOwner, const SCollisionProfile* Copy)
{

	FCustomCollisionProfileMap::iterator findIter = m_CustomProfiles.find(InProfileOwner);

	if (findIter != m_CustomProfiles.end())
	{
		return findIter->second;

	}

	SCollisionProfile* customProfile = new SCollisionProfile();

	std::string desiredDefaultChannelName = "Default";
	bool desiredDefaultEnable = true;
	std::vector<ECollision_Interaction> desiredInteractions;

	if (Copy)
	{
		desiredDefaultChannelName = Copy->Channel->Name;
		desiredDefaultEnable = Copy->Enable;
		desiredInteractions = Copy->vecCollisionInteraction;
	} else
	{
		GetDefaultInteractions(OUT desiredInteractions);
	}

	size_t	Count = m_vecChannel.size();
	for (size_t i = 0; i < Count; ++i)
	{
		if (m_vecChannel[i]->Name == desiredDefaultChannelName)
		{
			customProfile->Channel = m_vecChannel[i];
			break;
		}
	}

	customProfile->Defaults = false;//Custom Never True

	customProfile->Enable = desiredDefaultEnable;
	customProfile->Name = "Custom";

	//std::vector<ECollision_Interaction> Interactions;
	//GetDefaultInteractions(OUT Interactions);
	Count = desiredInteractions.size();
	for (size_t i = 0; i < Count; ++i)
	{
		customProfile->vecCollisionInteraction.push_back(desiredInteractions[i]);
	}



	m_CustomProfiles.insert(std::make_pair(InProfileOwner, customProfile));

	return customProfile;
}

SCollisionProfile* CollisionManager::FindCustomProfile(class PrimitiveComponent* InProfileOwner) const
{
	FCustomCollisionProfileMap::const_iterator findIter = m_CustomProfiles.find(InProfileOwner);

	if (findIter != m_CustomProfiles.cend())
	{
		return findIter->second;
	}

	return nullptr;
}

void CollisionManager::ApplyProfile()
{

	{
		FCollisionProfileMap::iterator iter = m_mapProfile.begin();
		FCollisionProfileMap::iterator End = m_mapProfile.end();



		for (; iter != End; iter++)
		{
			if ((*iter).second->vecCollisionInteraction.size() != m_vecChannel.size())
			{
				(*iter).second->vecCollisionInteraction.resize(m_vecChannel.size());
			}

			if ((*iter).second->Defaults)
			{
				for (size_t i = 0; i < m_vecChannel.size(); i++)
				{
					(*iter).second->vecCollisionInteraction[i] = m_vecChannel[i]->Interaction;
				}
			}

		}
	}


	{

		FCustomCollisionProfileMap::iterator iter = m_CustomProfiles.begin();
		FCustomCollisionProfileMap::iterator End = m_CustomProfiles.end();

		for (; iter != End; iter++)
		{
			if ((*iter).second->vecCollisionInteraction.size() != m_vecChannel.size())
			{
				(*iter).second->vecCollisionInteraction.resize(m_vecChannel.size());
			}

			//if ((*iter).second->Defaults)
			//{
			//	for (size_t i = 0; i < m_vecChannel.size(); i++)
			//	{
			//		(*iter).second->vecCollisionInteraction[i] = m_vecChannel[i]->Interaction;
			//	}
			//}



		}

	}


}

void CollisionManager::DeleteChannelAllProfile(const SCollisionChannel* InDeleteChannel)
{
	FCollisionProfileMap::iterator iter = m_mapProfile.begin();
	FCollisionProfileMap::iterator End = m_mapProfile.end();

	for (; iter != End; iter++)
	{
		if ((*iter).second->Channel == InDeleteChannel)
		{
			(*iter).second->Channel = m_vecChannel[0];
		}
	}
}

bool CollisionManager::Save()
{
	JsonArchive ar{};

	const SPathInfo* settingPath = g_Engine->Get<PathManager>()->FindPath(SETTING_PATH);

	std::string CollisionSettingPath = settingPath->aPath + "Collision.meta";


	Serialize(ar);

	return FileSystem::SaveJsonFile(CollisionSettingPath, ar.GetData(), ar.GetIndent());
}

bool CollisionManager::Load()
{

	Json data;
	const SPathInfo* settingPath = g_Engine->Get<PathManager>()->FindPath(SETTING_PATH);

	std::string CollisionSettingPath = settingPath->aPath + "Collision.meta";

	bool jsonLoad = FileSystem::LoadJsonFile(CollisionSettingPath, OUT data);

	JsonArchive ar(data);

	Serialize(ar);

	//ApplyProfile();

	return jsonLoad;
}

