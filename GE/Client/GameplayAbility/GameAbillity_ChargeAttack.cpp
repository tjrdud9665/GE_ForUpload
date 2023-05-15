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
		// 스킬키를 누르고 있다면 애니메이션이 뒤로 진행되고 , ChargeOn Charge 상태 돌입 

		if (m_KeyPushCheck)
		{
			m_ObjectInfo.OwnerAvatar->Reverse();

			m_ChargeOn = true;
			m_Charge = true;
		}	
	}

	if (EventName == "ChargeReverse")
	{
		//현재 ChargeOn 상태이고 , 키가 눌려있다면 다시 원래대로 애니메이션을 진행한다 .  계속 반복

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
	// 어빌리티 종료시 멤버 리셋 

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
	// 키를 뗏을 경우 

	m_KeyPushCheck = false;

	m_KeyUpCheck = true;


	// 만약에 현재 타이머로 인한 맥스차지 애니메이션이 진행되었을경우 Return
	if (m_Max)
	{
		m_Max = false;
		return;
	}
	m_Charge = true;

	//그게 아니라면 나머지 애니메이션 진행

	m_ObjectInfo.OwnerAvatar->Play();

}

void GameAbillity_ChargeAttack::MaxChargeTime()
{
	//이곳으로 들어오는건 키를 계속 3초이상 누른 경우 


	// 3초이상 눌렀는데 현재 키를 뗴고 애니메이션이 진행되고있는 상황일수도 있기때문에 , 체크후 Return
	if (m_KeyUpCheck)
		return;

	//그게아니라면 애니메이션 진행 

	m_Max = true;
	m_KeyPushCheck = false;
	m_Charge = true;

    m_ObjectInfo.OwnerAvatar->Play();
}

void GameAbillity_ChargeAttack::CheckChargeLevel()
{
	// 1초마다 해당함수 실행 , Charge단계 증가 , 추후 데미지 계산을 위함

	if (m_Charge)
		++m_ChargeLevel;	


	
}
