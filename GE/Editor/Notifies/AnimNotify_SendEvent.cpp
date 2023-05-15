#include "PCH.h"
#include "AnimNotify_SendEvent.h"
#include <GameplaySystem/GameplaySystem.h>
#include <GameFramework/Components/SkeletalMeshComponent.h>
#include <GameFramework/GameObject/GameObject.h>

AnimNotify_SendEvent::AnimNotify_SendEvent()
{

}

AnimNotify_SendEvent::AnimNotify_SendEvent(const AnimNotify_SendEvent& Other)
	:Super(Other)
	,EventName(Other.EventName)
{

}

AnimNotify_SendEvent::~AnimNotify_SendEvent()
{

}

void AnimNotify_SendEvent::OnNotify(class SkeletalMeshComponent* MeshComp, class AnimationSequence* AnimSeq)
{
	SGameplayEventData* data = new SGameplayEventData();
	data->EventName = EventName;
	GameplaySystem::SendGameplayEvent(MeshComp->GetOwner<GameObject>(), data->EventName, data);
}

const std::string& AnimNotify_SendEvent::GetNotifyName() const
{
	return EventName;
}
