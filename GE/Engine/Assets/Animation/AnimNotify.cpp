#include "EnginePCH.h"
#include "AnimNotify.h"

AnimNotify::AnimNotify()
	:m_Called(false)
{
	

}

AnimNotify::AnimNotify(const AnimNotify& Other)
	:Super(Other)
	,m_Called(false)
{

}

AnimNotify::~AnimNotify()
{

}

void AnimNotify::Notify(class SkeletalMeshComponent* MeshComp, class AnimationSequence* AnimSeq)
{
	if (MeshComp && AnimSeq)
	{
		m_Called = true;
		OnNotify(MeshComp, AnimSeq);		
	}
}

void AnimNotify::OnNotify(class SkeletalMeshComponent* MeshComp, class AnimationSequence* AnimSeq)
{
	//Implement "OnNotify"  Derived Class like -> AimNotify_SSibal
}

const std::string& AnimNotify::GetNotifyName() const
{
	//Edit 에서 노출되는 이름
	return GetName();
}
