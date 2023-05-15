#include "PCH.h"
#include "AnimNotify_KillOwner.h"
#include <GameFramework/Components/SkeletalMeshComponent.h>
#include <GameFramework/GameObject/GameObject.h>

AnimNotify_KillOwner::AnimNotify_KillOwner()
{
}

AnimNotify_KillOwner::AnimNotify_KillOwner(const AnimNotify_KillOwner& Other)
	: Super(Other)
{
}

AnimNotify_KillOwner::~AnimNotify_KillOwner()
{
}

void AnimNotify_KillOwner::OnNotify(SkeletalMeshComponent* MeshComp, AnimationSequence* AnimSeq)
{
	if (MeshComp)
	{
		GameObject* OwnerGO = MeshComp->GetOwner<GameObject>();
		if (OwnerGO)
			OwnerGO->Kill();
	}
}
