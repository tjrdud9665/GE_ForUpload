#pragma once
#include "PrimitiveComponent.h"
#include "Assets/Mesh/Skeleton.h"
#include "Assets/Animation/AnimationSequence.h"

class SkeletalMeshComponent :public PrimitiveComponent
{
	CLASS(SkeletalMeshComponent,PrimitiveComponent)

protected:
	SkeletalMeshComponent();
	SkeletalMeshComponent(const SkeletalMeshComponent& Other);
	virtual ~SkeletalMeshComponent();
	
	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void BeginPlay() override;
	
	virtual void Render(class Renderer* pRenderer) override;

	virtual void RenderDepth(class Renderer* pRenderer, const Matrix& ViewProj) override;

	virtual void GatherRenderInstance(class Renderer* pRenderer, std::vector<SInstanceInfo>& InstanceInfos, int32 InstanceID) override;

	virtual void GatherRenderDeptthInstance(class Renderer* pRenderer, const Matrix& ViewProj, std::vector<SDepthInstanceInfo>& InstanceInfos, int32 InstanceID) override;
	
public:
	virtual void PreUpdate(float DeltaSeconds) override;

	virtual void Update(float DeltaSeconds) override;	

	virtual void PostUpdate(float DeltaSeconds) override;

	virtual STransform GetSocketTransform(const std::string& InSocketName) const override;

	virtual void SetMeshAsset(class MeshAsset* InMeshAsset)	override;

	// Primitive
	virtual void AdditionalBindPerIA(class Renderer* pRenderer) override;

public:
	virtual bool Serialize(Archive& ar) override;


private:
	void UpdateAnimation(float DeltaSeconds);

	void CalculateCurrentFrame();

	void ComputeAnimation(class Renderer* pRenderer);

private:
	void CallNotifies(float NewPosition);

	void OnFinished(); //반복이 아닌 즉 진짜 끝나고 가만히있을때

	void AnimationStarted();		//ComputeAnimation 하기전의 시작

	void OnStarted();				//ComputeAnimation 이후의 Started

	void OnEnded();		//반복여부와 상관없이 끝에도달하면 콜

public:
	/*Animation Interface */
	void SetPlaybackPosition(float NewPosition);

	float GetPlaybackPosition() const;

	void SetLooping(bool bNewLooping);

	bool IsLooping() const;

	void SetPlayRate(float NewRate);

	float GetPlayRate() const;

	void SetNewTime(float NewTime);

	float GetAnimationLength() const;

	int32 GetAnimLengthInFrames() const;

	bool IsPlaying() const;

	bool IsReversing() const;

	void Play();

	void PlayFromStart();

	void Reverse();

	void ReverseFromEnd();

	void Stop();

	void BindOnFinished(std::function<void(void)> Func);

	void UnBindOnFinished();

	virtual void AnimationChanged();

	void OnChangeEnded();

	class AnimationSequence* GetCurrentAnimationSequence() const;

	bool IsEnableRootMotion() const;

	void SetEnableRootMotion(bool InEnableRootMotion);

	void ResetAllNotifies();

public:
	class SkeletalMeshSocket* FindSocketInfo(const std::string& InSocketName, STransform& OutTransform, int32& OutBoneIndex, int32& OutIndex) const;

	class SkeletalMeshSocket* GetSocketInfoByName(const std::string& InSocketName, STransform& OutTransform, int32& OutBoneIndex) const;

	Matrix GetBoneMatrix(int32 BoneIndex) const;

	SVector3 GetBoneAxis(const std::string& BoneName, EAxis Axis) const;

	int32 GetNumBones() const;

	int32 GetBoneIndex(const std::string& BoneName) const;

	STransform GetBoneTransform(int32 BoneIndex, const STransform& LocalToWorld) const;

	STransform GetBoneTransform(int32 BoneIndex) const;

	STransform GetRootBoneTransform() const;


	std::string GetParentBone(const std::string& InBoneName) const;	

	void SetAnimationSequence(class AnimationSequence* InAnimSeq);

	//Only Valid In Update
	bool IsAnimEnded() const;

protected:
	PROP(TSharedPtr<AnimationSequence>, m_Animation,DetailsEdit,"Animation")
	TSharedPtr<AnimationSequence>		m_Animation;

	//For Blending In HLSL (Lerp)
	AnimationSequence*					m_PrevAnimation;			//For Change Detection

	AnimationSequence*					m_ChangeBlendAnimation;		//For Blending

	//
	//* 에니메이션이 바뀌고 일정시간동안 만 이전 에니메이션을 유지시키면서 Lerp 한다
	//* 즉 에니메이션을 즉각적으로 교체하되 이전의 바뀌기 전의것을 Prev 로 유지하면서 m_BlendThreshold 시간 만큼 2개를 섞는다.
	uint64								m_BlendTimerID;

	PROP(float, m_BlendThreshold, DetailsEdit, "Animation")
		float m_BlendThreshold;

	////Runtime Copied Asset Not Origin -> Cause SocketTranform
	//PROP(TSharedPtr<class Skeleton>, m_Skeleton, DetailsView | Transient, "Animation")
	//TSharedPtr<class Skeleton> m_Skeleton;


	PROP(float, m_PlayRate, DetailsEdit, "Animation")
	float m_PlayRate;

	PROP(bool, m_Looping, DetailsEdit, "Animation")
	bool m_Looping;

	PROP(bool, m_ReversePlayback, DetailsEdit, "Animation")
	bool m_ReversePlayback;

	PROP(bool, PlayFromStartOnChanged, DetailsEdit,"Animation")
	bool PlayFromStartOnChanged;

	PROP(bool, EnableRootMotion, DetailsEdit, "Animation")
	bool EnableRootMotion;

	bool m_Playing;

	PROP(float, m_AccumulatedTime, DetailsView | Transient, "Animation")
	float m_AccumulatedTime;

	int32 m_CachedFrameIndex;

	

	
	PROP(class SkeletalMeshAsset*, SkeletalMesh, DetailsEdit , "Primitive")
	class SkeletalMeshAsset* SkeletalMesh;


private:		
	class Renderer* m_CachedRenderer;			

	FBufferID			m_BoneMatrixArray;
	FBufferID			m_BoneSocketMatrixArray;

	std::vector<Matrix>		m_ComputedBoneMatrix;
	

private:
	//Notify
	std::vector<class AnimNotifyInfo*>		m_RuntimeNotifies;
	bool									m_NotifiyInit;

	//End Func
	std::function<void(void)>	m_OnFinished;

	STransform				m_RootMotionStarted;	
	
	bool					m_AnimationStarted;

	bool					m_AnimEnded;
	


};

