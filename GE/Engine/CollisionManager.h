#pragma once
#include "XObject/XObject.h"


class CollisionManager :public XObject
{

	using FCollisionProfileMap = std::unordered_map<std::string, SCollisionProfile*>;
	using FCollisionChannelList = std::vector<SCollisionChannel*>;
	using FProfileRetedList = std::list<class PrimitiveComponent*>;
	using FCustomCollisionProfileMap = std::unordered_map<class PrimitiveComponent*, SCollisionProfile*>;

	friend class Globals;



private:
	CollisionManager();
	~CollisionManager();

public:
	void Initialize();

	bool CreateProfile(const std::string& Name, const std::string& ChannelName, bool Enable, ECollision_Interaction BaseInteraction = ECollision_Interaction::Overlap, const std::string& Desc = "");

	bool CreateProfile(const std::string& Name, const std::string& ChannelName, bool Enable, const std::vector<ECollision_Interaction>& Interactions, const std::string& Desc = "");

	bool SetCollisionInteraction(const std::string& Name, const std::string& ChannelName, ECollision_Interaction Interaction);
	//만든순서대로 들어감..
	bool CreateChannel(const std::string& Name, ECollision_Interaction Interaction = ECollision_Interaction::Overlap);

	//GameFramework쪽에서 호출x
	SCollisionProfile* FindProfile(const std::string& Name) const;

	size_t GetAllProfileName(std::vector<std::string>& OutName);

	size_t GetCustomProfileName(std::vector<std::string>& OutName);

	//Return Custom Only..Custom0부터 에디터에서 컨트롤합니다.
	size_t GetAllCustomChannelName(std::vector<std::string>& OutName);

	//Engine 기본 채널을 포함해서 반환합니다.
	size_t GetAllChannelName(std::vector<std::string>& OutName);

	void GetDefaultInteractions(std::vector<ECollision_Interaction>& OutInteractions);


	SCollisionChannel* GetCollisionChannel(int32 Idx);

	SCollisionChannel* GetCollisionChannel(const std::string& InChannelName);


	bool ModifyChannelName(const std::string& InChannelName, const std::string& toName, ECollision_Interaction DefaultInteraction);

	void DeleteChannel(const std::string& InChannelName);

	void DeleteProfile(const std::string& InProfileName);

	bool IsDefaultProfile(const std::string& InProfileName);



	//원래이름 ,바꿀이름,바꿀인터렉션, 바꿀 설명
	bool ModifyProfile(const std::string& InProfileName, const std::string& toProfileName, const std::string& ChannelName, bool Enable, const std::vector<ECollision_Interaction>& Interactions, const std::string& Desc);


	virtual bool Serialize(Archive& ar);




	//프로파일을 대여합니다.
	SCollisionProfile* RentalProfile(class PrimitiveComponent* InRental, const std::string& InProfileName);

	//프로파일을 반납합니다/.
	void ReturnProfile(class PrimitiveComponent* InRentaled);


	SCollisionProfile* CreateCustomProfile(class PrimitiveComponent* InProfileOwner, const SCollisionProfile* Copy = nullptr);

	SCollisionProfile* FindCustomProfile(class PrimitiveComponent* InProfileOwner) const;


private:
	/* 채널이 수정된경우 모든 프로파일에 통지하여야한다.*/
	void ApplyProfile();

	/* 채널이 삭제된 경우 모든 프로파일에서 삭제하여야한다.*/
	void DeleteChannelAllProfile(const SCollisionChannel* InDeleteChannel);


public:
	bool	Save();
	bool	Load();




private:
	FCollisionProfileMap	m_mapProfile;
	FCollisionChannelList	m_vecChannel;

	FProfileRetedList	m_ProfileRentalList;

	//개별 오브젝트가 가진 컬리전입니다.
	FCustomCollisionProfileMap	m_CustomProfiles;
};

