#pragma once
#include "EditorWidget.h"
#include <ImSequencer.h>



//Seq의 아이템이 Notify로 구성되길 원함.




class AnimationSequencer :public EditorWidget
{
	CLASS(AnimationSequencer, EditorWidget)

public:
	AnimationSequencer();
	AnimationSequencer(const AnimationSequencer& Other);
	virtual ~AnimationSequencer();

	virtual void Build() override;

	virtual void Update(float DeltaSeconds) override;

	void SetTargetAnimation(class AnimationSequence* animSeq);
	
	void InitPreviewComponent(class SkeletalMeshComponent* OwnerPreview);

private:
	//SAnimSequence	m_SeqInterface;
	class AnimationSequence* m_TargetAnimation;

	//From Owner
	class SkeletalMeshComponent*	m_AnimPriviewComponent;

	float	m_CurrentTime;

	bool	m_Play;

};

