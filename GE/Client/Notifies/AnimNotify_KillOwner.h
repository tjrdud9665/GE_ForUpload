#pragma once
#include <Assets\Animation\AnimNotify.h>

class AnimNotify_KillOwner : public AnimNotify
{
	CLASS(AnimNotify_KillOwner, AnimNotify)

protected:
	AnimNotify_KillOwner();
	AnimNotify_KillOwner(const AnimNotify_KillOwner& Other);
	virtual ~AnimNotify_KillOwner();

	virtual void OnNotify(class SkeletalMeshComponent* MeshComp, class AnimationSequence* AnimSeq) override;
};

