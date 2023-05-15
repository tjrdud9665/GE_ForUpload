#include "PCH.h"
#include "GameplayAbility_Evade.h"
#include <Assets/Animation/AnimationSequence.h>
#include <Assets/Mesh/Skeleton.h>
#include <GameFramework/GameObject/GameObject.h>
#include <GameFramework/Components/SkeletalMeshComponent.h>
#include <CameraManager.h>
#include <GameFramework/Components/CameraComponent.h>
#include <Input/Input.h>

GameplayAbility_Evade::GameplayAbility_Evade()
	: AnimPlayRate(1.f)
	, ChasingMouse(false)
{
}

GameplayAbility_Evade::GameplayAbility_Evade(const GameplayAbility_Evade& Other)
	: Super(Other)
	, Evade(Other.Evade)
	, AnimPlayRate(Other.AnimPlayRate)
	, ChasingMouse(Other.ChasingMouse)
{
}

GameplayAbility_Evade::~GameplayAbility_Evade()
{
}

void GameplayAbility_Evade::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{
	if (IsActivating())
	{
		if ("End" == EventName)
		{
			EndAbility();
		}
	}
}

void GameplayAbility_Evade::OnActivateAbility(const SGameplayAbilitySpec& InSpec)
{
	// 조건없이 무조건 발동

	m_ObjectInfo.OwnerObject->AddTag("Evade", true);
	m_ObjectInfo.OwnerObject->AddTag("CanAbility", false);

	if (ChasingMouse)
		ChangeDirection();

	m_ObjectInfo.OwnerAvatar->SetPlayRate(AnimPlayRate);
	AbilityUtils::PlayAnimation(this, Evade, false, true, true);
}

void GameplayAbility_Evade::OnEndAbility()
{
	m_ObjectInfo.OwnerObject->AddTag("Evade", false);
}

void GameplayAbility_Evade::ChangeDirection()
{
	D3D11_VIEWPORT viewport = g_Engine->Get<Renderer>()->GetCurrentViewport();

	if (g_Engine->IsEditMode())
	{
		viewport = g_Engine->GetEditViewport();
	}

	SVector2 mousePos = g_Engine->Get<Input>()->GetMousePosition();

	SVector3 origin;
	SVector3 direction;

	// Ray Picking
	SHitResult hit = g_Engine->Get<CameraManager>()->GetCurrentCamera()->pickRay(viewport, mousePos.x, mousePos.y, OUT origin, OUT direction);

	SVector3 End = hit.point;

	SVector3 Rot = SMathUtils::FindLookAtRotation(m_ObjectInfo.OwnerObject->GetWorldLocation(), End);

	m_ObjectInfo.OwnerObject->SetWorldRotation(SVector3(0.f, 0.f, Rot.y));
}
