#pragma once
#include <Assets/Animation/AnimNotify.h>
#include "Assets/Sound/Sound.h"

class AnimNotify_PlaySound :public AnimNotify
{
	CLASS(AnimNotify_PlaySound, AnimNotify)
protected:
	AnimNotify_PlaySound();
	AnimNotify_PlaySound(const AnimNotify_PlaySound& Other);
	virtual ~AnimNotify_PlaySound();

	virtual void OnNotify(class SkeletalMeshComponent* MeshComp, class AnimationSequence* AnimSeq) override;

	virtual const std::string& GetNotifyName() const;

private:
	PROP(TSharedPtr<Sound>, PlaySound, DetailsEdit)
	TSharedPtr<Sound>		PlaySound;

};

