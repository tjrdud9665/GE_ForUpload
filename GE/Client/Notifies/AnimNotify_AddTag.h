#pragma once
#include "Assets\Animation\AnimNotify.h"

class AnimNotify_AddTag : public AnimNotify
{
	CLASS(AnimNotify_AddTag, AnimNotify)
protected:
	AnimNotify_AddTag();
	AnimNotify_AddTag(const AnimNotify_AddTag& Other);
	virtual ~AnimNotify_AddTag();

	virtual void OnNotify(class SkeletalMeshComponent* MeshComp, class AnimationSequence* AnimSeq) override;

	virtual const std::string& GetNotifyName() const;

private:
	PROP(std::string, TagName, DetailsEdit)
		std::string		TagName;

	PROP(bool, TagValue, DetailsEdit)
		bool TagValue;
};

