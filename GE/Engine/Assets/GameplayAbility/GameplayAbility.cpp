#include "EnginePCH.h"
#include "GameplayAbility.h"
#include "GameFramework/Components/AbilityComponent.h"
#include "GameFramework/Components/SkeletalMeshComponent.h"
#include "GameFramework/GameObject/GameObject.h"

GameplayAbility::GameplayAbility()
	:m_OwningScene(nullptr)
	, m_InstancingPolicy(EGameplayAbilityInstancingPolicy::InstancedPerObject)
	, m_Activated(false)
	, m_CoolTimeID(0)
	, AbilityEndOnAnimEnd(true)
{

}

GameplayAbility::GameplayAbility(const GameplayAbility& Other)
	: Super(Other)
	, m_OwningScene(Other.m_OwningScene)
	, CoolTime(Other.CoolTime)
	, CostGameplayEffectClass(Other.CostGameplayEffectClass)
	, m_InstancingPolicy(Other.m_InstancingPolicy)
	, m_Activated(Other.m_Activated)
	, m_CoolTimeID(0)
	, AbilityEndOnAnimEnd(Other.AbilityEndOnAnimEnd)
	,BindKeyName(Other.BindKeyName)
{
	size_t blockTagSize = Other.ActivationBlockTag.size();
	ActivationBlockTag.resize(blockTagSize);
	for(size_t i=0;i<blockTagSize;i++)
	{
		ActivationBlockTag[i] = Other.ActivationBlockTag[i];
	}
	size_t reqTagSize = Other.ActivationRequireTag.size();
	ActivationRequireTag.resize(reqTagSize);
	for (size_t i = 0; i < reqTagSize; i++)
	{
		ActivationRequireTag[i] = Other.ActivationRequireTag[i];
	}
}

GameplayAbility::~GameplayAbility()
{
	if (!m_OwningScene)
	{
		std::string sPath = GetInfo().GetSPath();
		if (!sPath.empty())
		{
			std::string CDOName = FileSystem::GetRelativePath(FileSystem::FromString(sPath)) + GetName();
			GetClass()->DeleteCDO(CDOName);
		}
	}
}

void GameplayAbility::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{

}

bool GameplayAbility::CallActivateAbility(const SGameplayAbilitySpec& InSpec)
{
	PreActivate(InSpec);
	// *Tag Check TO DO Block Ability. * TAG */
	if (CheckActivationTags(InSpec))
		return ActivateAbility(InSpec);

	return false;
}

void GameplayAbility::OnActivateAbility(const SGameplayAbilitySpec& InSpec)
{

}

void GameplayAbility::PreActivate(const SGameplayAbilitySpec& InSpec)
{
	m_ObjectInfo = InSpec.ObjectInfo;

}

bool GameplayAbility::ActivateAbility(const SGameplayAbilitySpec& InSpec)
{
	if (CommitAbility(InSpec))
	{
		m_Activated = true;
		OnActivateAbility(InSpec);
		return true;
	}

	return false;
}

bool GameplayAbility::CommitAbility(const SGameplayAbilitySpec& InSpec)
{
	if (!CommitCheck(InSpec))//실행가능한지 체크
	{
		return false;
	}

	CommitExecute(InSpec); // 집행 




	// Broadcast this commitment
	//InSpec->AbilitySystemComponent->NotifyAbilityCommit(this);

	return true;
}

bool GameplayAbility::CommitCheck(const SGameplayAbilitySpec& InSpec)
{
	if (!CheckCooldown(InSpec))
	{
		return false;
	}

	if (!CheckCost(InSpec))
	{
		return false;
	}

	if (m_Activated)
	{
		return false;
	}

	return true;
}

bool GameplayAbility::CheckCooldown(const SGameplayAbilitySpec& InSpec)
{
	if (InSpec.ObjectInfo.OwnerComponent->GetEffectRemainingTime(m_CoolTimeID) > 0)
	{
		return false;
	}

	return true;
}

bool GameplayAbility::CheckCost(const SGameplayAbilitySpec& InSpec)
{
	
	if (CostGameplayEffectClass)
	{
		AbilityComponent*  comp = InSpec.ObjectInfo.OwnerComponent;
		if(comp)
		{				
			//CostGameplayEffectClass.GetDefaultObject();
			if (!comp->CanApplyAttributeModifiers(CostGameplayEffectClass))
			{
				
				return false;
			}
		}
		
	
	}
	return true;	
}

void GameplayAbility::CommitExecute(const SGameplayAbilitySpec& InSpec)
{
	ApplyCoolDown(InSpec);
	ApplyCost(InSpec);
}

void GameplayAbility::ApplyCoolDown(const SGameplayAbilitySpec& InSpec)
{
	SGameplayEffectSpec* spec = InSpec.ObjectInfo.OwnerComponent->ApplyGameplayEffectToTarget(CoolTime, InSpec.ObjectInfo.OwnerComponent);
	if (spec)
	{
		m_CoolTimeID = spec->Handle;
	}

}

void GameplayAbility::ApplyCost(const SGameplayAbilitySpec& InSpec)
{
	//Apply Cost GameplayEffect
}

bool GameplayAbility::CheckActivationTags(const SGameplayAbilitySpec& InSpec)
{
	//Check Blocking 
	size_t blockSize = ActivationBlockTag.size();
	for(size_t i =0;i<blockSize;i++)
	{
		bool BlockFound = false;
		InSpec.ObjectInfo.OwnerObject->FindTag(ActivationBlockTag[i], OUT BlockFound);
		if (BlockFound)
		{
			//If want Block Interface OnBlocked Here
			return false;
		}
	}

	size_t reqSzie = ActivationRequireTag.size();
	for (size_t i = 0; i < reqSzie; i++)
	{
		bool ReqFound = false;
		InSpec.ObjectInfo.OwnerObject->FindTag(ActivationRequireTag[i], OUT ReqFound);
		if (!ReqFound)
		{
			return false;
		}
	}


	return true;
}



void GameplayAbility::EndAbility()
{
	m_Activated = false;
	OnEndAbility();
}

void GameplayAbility::OnEndAbility()
{
	//m_ObjectInfo.PlayingAnimSeq = nullptr;
}

bool GameplayAbility::IsActivating() const
{
	return m_Activated;
}

void GameplayAbility::CancelAbility()
{
	m_Activated = false;
	OnCancelAbility();
}

void GameplayAbility::OnCancelAbility()
{
	//Remove Tags.
}

bool GameplayAbility::IsAbilityEndOnAnimationEnd() const
{
	return AbilityEndOnAnimEnd;
}

uint64 GameplayAbility::GetCoolTimeID() const
{
	return m_CoolTimeID;
}



void GameplayAbility::InputPressed(const SGameplayAbilitySpec& InSpec)
{
	//Make Combo ,Or Charge Ability

	int i = 0;
}

void GameplayAbility::InputReleased(const SGameplayAbilitySpec& InSpec)
{
	
}

bool GameplayAbility::PostLoad()
{
	bool Result = Super::PostLoad();

	std::string CDOName = FileSystem::GetRelativePath(FileSystem::FromString(GetInfo().GetSPath())) + GetName();

	GetClass()->RegisterCDO(CDOName, this, ABILITYSYSTEM_PATH);

	return Result;
}

void GameplayAbility::SetOwner(XObject* InOwner)
{
	Super::SetOwner(InOwner);

	if (InOwner->GetClass()->IsChildOf(AbilityComponent::StaticClass()) ||
		InOwner->GetClass()->IsA(AbilityComponent::StaticClass()))
	{
		m_OwningScene = ((AbilityComponent*)InOwner)->GetOwningScene();
	}
}

void AbilityUtils::PlayAnimation(class GameplayAbility* InAbility, class AnimationSequence* InAnimSeq, bool bLoop, bool bRootMotionEable, bool PlayFromStart, bool IsReverse )
{
	if (InAnimSeq && InAbility)
	{
		GameObject* abilityOwner = InAbility->m_ObjectInfo.OwnerObject;
		SkeletalMeshComponent* abilityAvatar = InAbility->m_ObjectInfo.OwnerAvatar;
		if (abilityAvatar)
		{
			InAbility->m_ObjectInfo.PlayingAnimSeq = InAnimSeq;
			abilityAvatar->SetLooping(bLoop);
			abilityAvatar->SetAnimationSequence(InAnimSeq);
			if (PlayFromStart)
			{
				if(IsReverse)
				{
					abilityAvatar->ReverseFromEnd();
				}
				else
				{
					abilityAvatar->PlayFromStart();
				}
				
			}
			else
			{
				if(IsReverse)
				{
					abilityAvatar->Reverse();
				}
				else
				{
					abilityAvatar->Play();
				}
			}
			

			abilityAvatar->SetEnableRootMotion(bRootMotionEable);
			
			InAbility->m_ObjectInfo.OwnerComponent->ModifyGameplayAbilitySpecInfo(InAbility, InAbility->m_ObjectInfo);
		}
	}
}
