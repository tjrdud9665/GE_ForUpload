#pragma once

class AnimNotify : public XObject
{
	CLASS(AnimNotify,XObject)

	friend class AnimationSequence;
	friend class SkeletalMeshComponent;

public:
	AnimNotify();
	AnimNotify(const AnimNotify& Other);
	virtual ~AnimNotify();

	bool IsCalled() const
	{
		return m_Called;
	}

	void Reset()
	{
		m_Called = false;
	}

private:
	void Notify(class SkeletalMeshComponent* MeshComp, class AnimationSequence* AnimSeq);
	virtual void OnNotify(class SkeletalMeshComponent* MeshComp, class AnimationSequence* AnimSeq);
	virtual const std::string& GetNotifyName() const;


private:
	bool	m_Called;

};

