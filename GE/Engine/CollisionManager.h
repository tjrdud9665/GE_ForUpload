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
	//���������� ��..
	bool CreateChannel(const std::string& Name, ECollision_Interaction Interaction = ECollision_Interaction::Overlap);

	//GameFramework�ʿ��� ȣ��x
	SCollisionProfile* FindProfile(const std::string& Name) const;

	size_t GetAllProfileName(std::vector<std::string>& OutName);

	size_t GetCustomProfileName(std::vector<std::string>& OutName);

	//Return Custom Only..Custom0���� �����Ϳ��� ��Ʈ���մϴ�.
	size_t GetAllCustomChannelName(std::vector<std::string>& OutName);

	//Engine �⺻ ä���� �����ؼ� ��ȯ�մϴ�.
	size_t GetAllChannelName(std::vector<std::string>& OutName);

	void GetDefaultInteractions(std::vector<ECollision_Interaction>& OutInteractions);


	SCollisionChannel* GetCollisionChannel(int32 Idx);

	SCollisionChannel* GetCollisionChannel(const std::string& InChannelName);


	bool ModifyChannelName(const std::string& InChannelName, const std::string& toName, ECollision_Interaction DefaultInteraction);

	void DeleteChannel(const std::string& InChannelName);

	void DeleteProfile(const std::string& InProfileName);

	bool IsDefaultProfile(const std::string& InProfileName);



	//�����̸� ,�ٲ��̸�,�ٲ����ͷ���, �ٲ� ����
	bool ModifyProfile(const std::string& InProfileName, const std::string& toProfileName, const std::string& ChannelName, bool Enable, const std::vector<ECollision_Interaction>& Interactions, const std::string& Desc);


	virtual bool Serialize(Archive& ar);




	//���������� �뿩�մϴ�.
	SCollisionProfile* RentalProfile(class PrimitiveComponent* InRental, const std::string& InProfileName);

	//���������� �ݳ��մϴ�/.
	void ReturnProfile(class PrimitiveComponent* InRentaled);


	SCollisionProfile* CreateCustomProfile(class PrimitiveComponent* InProfileOwner, const SCollisionProfile* Copy = nullptr);

	SCollisionProfile* FindCustomProfile(class PrimitiveComponent* InProfileOwner) const;


private:
	/* ä���� �����Ȱ�� ��� �������Ͽ� �����Ͽ����Ѵ�.*/
	void ApplyProfile();

	/* ä���� ������ ��� ��� �������Ͽ��� �����Ͽ����Ѵ�.*/
	void DeleteChannelAllProfile(const SCollisionChannel* InDeleteChannel);


public:
	bool	Save();
	bool	Load();




private:
	FCollisionProfileMap	m_mapProfile;
	FCollisionChannelList	m_vecChannel;

	FProfileRetedList	m_ProfileRentalList;

	//���� ������Ʈ�� ���� �ø����Դϴ�.
	FCustomCollisionProfileMap	m_CustomProfiles;
};

