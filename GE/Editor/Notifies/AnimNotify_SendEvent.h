#pragma once
#include <Assets/Animation/AnimNotify.h>

class AnimNotify_SendEvent :public AnimNotify
{
	CLASS(AnimNotify_SendEvent, AnimNotify)
protected:
	AnimNotify_SendEvent();
	AnimNotify_SendEvent(const AnimNotify_SendEvent& Other);
	virtual ~AnimNotify_SendEvent();

	virtual void OnNotify(class SkeletalMeshComponent* MeshComp, class AnimationSequence* AnimSeq) override;

	virtual const std::string& GetNotifyName() const;

private:
	
	PROP(std::string, EventName, DetailsEdit)
	std::string EventName;
};

