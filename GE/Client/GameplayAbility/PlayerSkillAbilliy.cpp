#include "PCH.h"
#include "PlayerSkillAbilliy.h"
#include <Assets/Animation/AnimationSequence.h>
#include <Assets/Mesh/Skeleton.h>
#include <GameFramework/Components/AbilityComponent.h>
#include <GameFramework/GameObject/GameObject.h>

PlayerSkillAbilliy::PlayerSkillAbilliy()
{

}

PlayerSkillAbilliy::PlayerSkillAbilliy(const PlayerSkillAbilliy& Other)
	:Super(Other)
{
	size_t Size = Other.SkillAnims.size();

	SkillAnims.resize(Size);

	for (int i = 0; i < Size; ++i)
	{
		SkillAnims[i] = Other.SkillAnims[i];
	}
}

PlayerSkillAbilliy::~PlayerSkillAbilliy()

{

}

void PlayerSkillAbilliy::OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec)
{

}

void PlayerSkillAbilliy::OnActivateAbility(const SGameplayAbilitySpec& InSpec)
{
	
	//AbilityUtils::PlayAnimation(this, SkillAnims[0], false, false);

}

void PlayerSkillAbilliy::OnCancelAbility()
{

}

void PlayerSkillAbilliy::OnEndAbility()
{

}

void PlayerSkillAbilliy::InputPressed(const SGameplayAbilitySpec& InSpec)
{

}
