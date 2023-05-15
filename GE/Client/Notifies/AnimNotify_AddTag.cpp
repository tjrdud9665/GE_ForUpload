#include "PCH.h"
#include "AnimNotify_AddTag.h"
#include "GameFramework/Components/SkeletalMeshComponent.h"
#include "GameFramework/GameObject/GameObject.h"

AnimNotify_AddTag::AnimNotify_AddTag()
	: TagValue(false)
{
}

AnimNotify_AddTag::AnimNotify_AddTag(const AnimNotify_AddTag& Other)
	: Super(Other)
	, TagName(Other.TagName)
	, TagValue(Other.TagValue)
{
}

AnimNotify_AddTag::~AnimNotify_AddTag()
{
}

void AnimNotify_AddTag::OnNotify(SkeletalMeshComponent* MeshComp, AnimationSequence* AnimSeq)
{
	if (MeshComp)
	{
		GameObject* OwnerGO = MeshComp->GetOwner<GameObject>();
		if (OwnerGO && !TagName.empty())
			OwnerGO->AddTag(TagName, TagValue);
	}
}

const std::string& AnimNotify_AddTag::GetNotifyName() const
{
	return TagName;
}
