#pragma once
#include "../AssetData.h"
#include "AnimNotify.h"
#include "../Mesh/Skeleton.h"


struct SKeyFrame
{
	double  Time;
	SVector3 Pos;
	SVector3 Scale;
	SVector4 Rot;
};

struct SBoneKeyFrame
{
	int32    BoneIndex;
	std::vector<SKeyFrame*>  vecKeyFrame;
	int32     RefCount;

	SBoneKeyFrame() :
		RefCount(1)
	{}

	~SBoneKeyFrame()
	{
		size_t  Size = vecKeyFrame.size();

		for (size_t i = 0; i < Size; ++i)
		{
			SAFE_DELETE(vecKeyFrame[i]);
		}

		vecKeyFrame.clear();
	}
};

//어떤 노티파이를 
// 어느 타이밍에 
// 어떤 값을가지고
class AnimNotifyInfo : public XObject
{
	CLASS(AnimNotifyInfo, XObject)
	
	friend class SkeletalMeshComponent;
	friend class AnimationSequence;

public:
	AnimNotifyInfo()
		:NotifyTimeRate(0.0f)
		, NotifyInst(nullptr)
	{

	}
	AnimNotifyInfo(const AnimNotifyInfo& Other)
		:Super(Other)
		, NotifyTimeRate(Other.NotifyTimeRate)
		, NotifyInst(nullptr)
	{

		if (Other.IsConstructed())
		{
			if (Other.NotifyInst)
			{
				SAFE_DELETE(NotifyInst);
				NotifyInst = Other.NotifyInst->Clone();
			}

		}

	}

	virtual ~AnimNotifyInfo()
	{
		SAFE_DELETE(NotifyInst);
	}

	TSubclassOf<class AnimNotify>& GetNotifyClass()
	{
		return NotifyClass;
	}

	void SetNotifyClass(XClass* InNotifyClass)
	{
		SAFE_DELETE(NotifyInst);

		NotifyClass = InNotifyClass;
		NotifyInst = Globals::CreateX<AnimNotify>(InNotifyClass, this);

	}

	float GetNotifyTimeRate() const
	{
		return NotifyTimeRate;
	}

	void SetNotifyTimeRate(float InTime)
	{
		NotifyTimeRate = InTime;
	}

	class AnimNotify* GetNotifyInst() const
	{
		return NotifyInst;
	}


	virtual bool Serialize(Archive& ar) override
	{
		bool result = Super::Serialize(ar);

		//SetNotifyClass()
		if (NotifyClass && !NotifyInst)
		{
			NotifyInst = Globals::CreateX<AnimNotify>(NotifyClass.GetClass(), this);
		}

		ar.Serialize("NotifyInst", NotifyInst);

		return result;
	}

private:
	PROP(TSubclassOf<class AnimNotify>, NotifyClass, DetailsEdit)
		TSubclassOf<class AnimNotify>	NotifyClass;

	PROP(float, NotifyTimeRate, DetailsView)
		float		NotifyTimeRate;

	PROP(class AnimNotify*, NotifyInst, DetailsEdit | Transient)
	class AnimNotify* NotifyInst;


};







class AnimationSequence :public AssetData
{
	CLASS(AnimationSequence, AssetData)
	friend class AssimpLoader;
	friend class FBXLoader;

protected:
	AnimationSequence();
	AnimationSequence(const AnimationSequence& Other);
	virtual ~AnimationSequence();


public:	
	virtual bool PostLoad() override;

	virtual bool Serialize(Archive& ar) override;

	static bool SaveKeyFrameFile(const char* FullPath, const std::vector<SBoneKeyFrame*>& InAnimKeyFrames);
	static bool LoadKeyFrameFile(const char* FullPath, std::vector<SBoneKeyFrame*>& OutAnimKeyFrames, std::vector<SAnimFrameTrans>& OutFrameTrans , int32 FrameLength);

	
public:
	float GetTotalDuration() const;

	size_t GetNotifyInfos(std::vector<class AnimNotifyInfo*>& OutNotifies) const;	

	void GetAnimationInfo(SAnimationIfno& OutAnimInfo,float Time,int32 frameIdx , bool bCycling) const;

	void GetChangeAnimationInfo(SAnimationIfno& OutAnimInfo, float Ratio) const;

	int32 GetKeyFrameIndexAtTime(float Time, bool bClampToEnds = false) const;

public:
	/* Notify Utility */
	size_t MakeRuntimeNotifyInfos(std::vector<AnimNotifyInfo*>& OutNotifies);
	
	static void RemoveRuntimeNotifies(std::vector<AnimNotifyInfo*>& DeleteNotifies);
	static void ResetNotifies(const std::vector<AnimNotifyInfo*>& InResetNotifies);

	static bool SortNotifyInfoByTime(const AnimNotifyInfo* src, const AnimNotifyInfo* dest);


public:
	int32 GetKeyFrameCount()	const
	{
		return m_FrameLength;
	}

	float GetFramePerSec() const
	{
		return m_FramePerSec;
	}

	float GetFrameTime() const
	{
		return m_FrameTime;
	}
	

	const std::vector<SAnimFrameTrans>& GetAnimationTrans() const
	{
		return m_FrameTrans;
	}

	class Skeleton* GetSkeleton() const
	{
		return m_Skeleton;
	}

private:	
	PROP(TSharedPtr<Skeleton>, m_Skeleton, DetailsView)
	TSharedPtr<Skeleton> m_Skeleton;	
	
	std::vector<SAnimFrameTrans>	m_FrameTrans;
	std::vector<SBoneKeyFrame*>		m_vecKeyFrame;

		
	int32							m_StartFrame;
	int32							m_EndFrame;

	PROP(float, m_FramePerSec, DetailsView, "Animation")
	float m_FramePerSec;

	PROP(float, m_PlayTime, DetailsView, "Animation")
	float	m_PlayTime;

	PROP(float, m_FrameTime, DetailsView, "Animation")
	float	m_FrameTime;

	PROP(int32, m_FrameLength, DetailsView, "Animation")
	int32	m_FrameLength;


	int32	m_ChangeFrame;

	bool							m_End;


private:
	//ProtoType으로만사용할것.. 2개이상의 오브젝트에서 같은 AnimSeq을 사용시에 노티파이정보를 공유하게됨.
	//컴포넌트별로 런타임 노티파이를  가지고 있어야 한다.
	PROP(std::vector<class AnimNotifyInfo*>, Notifies, DetailsEdit,"Notify")
	std::vector<class AnimNotifyInfo*>		Notifies;




	

	

};

