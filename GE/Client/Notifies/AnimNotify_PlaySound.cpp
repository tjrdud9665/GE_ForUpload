#include "PCH.h"
#include "AnimNotify_PlaySound.h"


AnimNotify_PlaySound::AnimNotify_PlaySound()
{

}

AnimNotify_PlaySound::AnimNotify_PlaySound(const AnimNotify_PlaySound& Other)
	:Super(Other)	
	, PlaySound(Other.PlaySound)
{

}

AnimNotify_PlaySound::~AnimNotify_PlaySound()
{

}

void AnimNotify_PlaySound::OnNotify(class SkeletalMeshComponent* MeshComp, class AnimationSequence* AnimSeq)
{
	if (PlaySound)
	{
		PlaySound->SetLoop(false);
		PlaySound->Play();
	}
}

const std::string& AnimNotify_PlaySound::GetNotifyName() const
{
	return GetClass()->GetName();
}
