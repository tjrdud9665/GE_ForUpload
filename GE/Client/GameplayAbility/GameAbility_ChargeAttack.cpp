#include "PCH.h"
#include "GameAbility_ChargeAttack.h"
#include <Assets/Animation/AnimationSequence.h>
#include <Assets/Mesh/Skeleton.h>
#include <GameFramework/Components/AbilityComponent.h>
#include <GameFramework/Components/SkeletalMeshComponent.h>
#include "ObjectManager.h"
#include "../GameObject/HitBoxObject.h"
#include "GameplaySystem/GameplaySystem.h"
#include "GameFramework/GameObject/GameObject.h"
#include "GameFramework/GameObject/Character.h"
#include "Input/Input.h"
#include "Assets/GameplayAbility/GameplayAbility.h"


GameAbility_ChargeAttack::GameAbility_ChargeAttack()
{

}

GameAbility_ChargeAttack::GameAbility_ChargeAttack(const GameAbility_ChargeAttack& Other)
	: Super(Other)
	, m_ChargeTimer(Other.m_ChargeTimer)
	, m_ChargeMaxTimerID(Other.m_ChargeMaxTimerID)
	, m_ChargeLevelTimerID(Other.m_ChargeLevelTimerID)
{
}

GameAbility_ChargeAttack::~GameAbility_ChargeAttack()
{

}

void GameAbility_ChargeAttack::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{
	//Super::OnReceivedGameplayEvent(EventName, EventData, InSpec);

	// Animation Notify���� 
	// Skill01 Animation
	// Notify ChargeAttack  - 0.337
	// Notify ChargeReverse  - 0.307

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

void GameAbility_ChargeAttack::OnActivateAbility(const SGameplayAbilitySpec& InSpec)
{
	AbilityUtils::PlayAnimation(this, Attack[0], false, true, true);

	m_KeyPushCheck = true;
	m_KeyUpCheck = false;

	m_ChargeTimer.SetTimer(OUT m_ChargeMaxTimerID, 3.1f, false, std::bind(&GameAbility_ChargeAttack::MaxChargeTime, this));
	m_ChargeTimer.SetTimer(OUT m_ChargeLevelTimerID, 1.f, true, std::bind(&GameAbility_ChargeAttack::CheckChargeLevel, this));
}

void GameAbility_ChargeAttack::OnCancelAbility()
{

}

void GameAbility_ChargeAttack::OnEndAbility()
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

	m_ObjectInfo.OwnerAvatar->SetEnableRootMotion(false);
}

void GameAbility_ChargeAttack::InputPressed(const SGameplayAbilitySpec& InSpec)
{
	
}

void GameAbility_ChargeAttack::InputReleased(const SGameplayAbilitySpec& InSpec)
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

void GameAbility_ChargeAttack::MaxChargeTime()
{
	//�̰����� �����°� Ű�� ��� 3���̻� ���� ��� ������ �ִϸ��̼� ������ �����Ƽ ���� 


	// 3���̻� �����µ� ���� Ű�� ��� �ִϸ��̼��� ����ǰ��ִ� ��Ȳ�ϼ��� �ֱ⶧���� , üũ�� Return
	if (m_KeyUpCheck)
		return;

	//�װԾƴ϶�� �ִϸ��̼� ���� 

	m_Max = true;
	m_KeyPushCheck = false;
	m_Charge = true;

    m_ObjectInfo.OwnerAvatar->Play();
}

void GameAbility_ChargeAttack::CheckChargeLevel()
{
	// 1�ʸ��� �ش��Լ� ���� , Charge�ܰ� ���� , ���� ������ ����� ����

	if (m_Charge)
		++m_ChargeLevel;	


	
}
