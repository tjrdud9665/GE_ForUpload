#include "PCH.h"
#include "GameAbillity_ChargeAttack.h"
#include <Assets/Animation/AnimationSequence.h>
#include <Assets/Mesh/Skeleton.h>
#include <GameFramework/Components/AbilityComponent.h>
#include <GameFramework/Components/SkeletalMeshComponent.h>
#include "ObjectManager.h"
#include "GameFramework/GameObject/HitBoxObject.h"
#include "GameplaySystem/GameplaySystem.h"
#include "GameFramework/GameObject/GameObject.h"
#include "GameFramework/GameObject/Character.h"
#include "Input/Input.h"
#include "Assets/GameplayAbility/GameplayAbility.h"


GameAbillity_ChargeAttack::GameAbillity_ChargeAttack()
{

}

GameAbillity_ChargeAttack::GameAbillity_ChargeAttack(const GameAbillity_ChargeAttack& Other)
	: Super(Other)
	, m_ChargeTimer(Other.m_ChargeTimer)
	, m_ChargeMaxTimerID(Other.m_ChargeMaxTimerID)
	, m_ChargeLevelTimerID(Other.m_ChargeLevelTimerID)
{
}

GameAbillity_ChargeAttack::~GameAbillity_ChargeAttack()
{

}

void GameAbillity_ChargeAttack::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{
	Super::OnReceivedGameplayEvent(EventName, EventData, InSpec);

	if (EventName == "ChargeAttack")
	{
		// ��ųŰ�� ������ �ִٸ� �ִϸ��̼��� �ڷ� ����ǰ� , ChargeOn Charge ���� ���� 

		if (m_KeyPushCheck)
		{
			m_ObjectInfo.OwnerAvatar->Reverse();

			m_ChargeOn = true;
			m_Charge = true;
		}	
	}

	if (EventName == "ChargeReverse")
	{
		//���� ChargeOn �����̰� , Ű�� �����ִٸ� �ٽ� ������� �ִϸ��̼��� �����Ѵ� .  ��� �ݺ�

		if (m_ChargeOn && m_KeyPushCheck)
		{
			m_ObjectInfo.OwnerAvatar->Play();

			m_ChargeOn = false;
			m_Charge = true;
		}
	}
}

void GameAbillity_ChargeAttack::OnActivateAbility(const SGameplayAbilitySpec& InSpec)
{
	AbilityUtils::PlayAnimation(this, Attack[0], false, true, true);

	m_KeyPushCheck = true;
	m_KeyUpCheck = false;

	m_ChargeTimer.SetTimer(OUT m_ChargeMaxTimerID, 3.1f, false, std::bind(&GameAbillity_ChargeAttack::MaxChargeTime, this));
	m_ChargeTimer.SetTimer(OUT m_ChargeLevelTimerID, 1.f, true, std::bind(&GameAbillity_ChargeAttack::CheckChargeLevel, this));
}

void GameAbillity_ChargeAttack::OnCancelAbility()
{

}

void GameAbillity_ChargeAttack::OnEndAbility()
{
	// �����Ƽ ����� ��� ���� 

	m_ChargeOn = false;
	m_KeyPushCheck = false;
	m_KeyUpCheck = false;
	m_Max = false;

	m_Charge = false;
	m_ChargeLevel = 0;

	m_ChargeTimer.RemoveTimer(m_ChargeMaxTimerID);
	m_ChargeTimer.RemoveTimer(m_ChargeLevelTimerID);
}

void GameAbillity_ChargeAttack::InputPressed(const SGameplayAbilitySpec& InSpec)
{
	
}

void GameAbillity_ChargeAttack::InputReleased(const SGameplayAbilitySpec& InSpec)
{
	// Ű�� ���� ��� 

	m_KeyPushCheck = false;

	m_KeyUpCheck = true;


	// ���࿡ ���� Ÿ�̸ӷ� ���� �ƽ����� �ִϸ��̼��� ����Ǿ������ Return
	if (m_Max)
	{
		m_Max = false;
		return;
	}
	m_Charge = true;

	//�װ� �ƴ϶�� ������ �ִϸ��̼� ����

	m_ObjectInfo.OwnerAvatar->Play();

}

void GameAbillity_ChargeAttack::MaxChargeTime()
{
	//�̰����� �����°� Ű�� ��� 3���̻� ���� ��� 


	// 3���̻� �����µ� ���� Ű�� ��� �ִϸ��̼��� ����ǰ��ִ� ��Ȳ�ϼ��� �ֱ⶧���� , üũ�� Return
	if (m_KeyUpCheck)
		return;

	//�װԾƴ϶�� �ִϸ��̼� ���� 

	m_Max = true;
	m_KeyPushCheck = false;
	m_Charge = true;

    m_ObjectInfo.OwnerAvatar->Play();
}

void GameAbillity_ChargeAttack::CheckChargeLevel()
{
	// 1�ʸ��� �ش��Լ� ���� , Charge�ܰ� ���� , ���� ������ ����� ����

	if (m_Charge)
		++m_ChargeLevel;	


	
}
