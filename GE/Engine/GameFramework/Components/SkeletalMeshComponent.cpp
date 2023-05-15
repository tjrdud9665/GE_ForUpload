#include "EnginePCH.h"
#include "SkeletalMeshComponent.h"
#include "Renderer/Renderer.h"
#include "Assets/Mesh/SkeletalMeshAsset.h"
#include "Assets/Mesh/SkeletalMeshSocket.h"

SkeletalMeshComponent::SkeletalMeshComponent()
	: m_CachedRenderer(nullptr)
	, m_CachedFrameIndex(0)
	, m_PlayRate(1.0f)
	, m_Looping(true)
	, m_Playing(true)
	,m_BoneMatrixArray(INDEX_NONE)
	, m_BoneSocketMatrixArray(INDEX_NONE)
	,SkeletalMesh(nullptr)
	, m_NotifiyInit(false)
	, m_BlendThreshold(0.0f)
	, m_BlendTimerID(INVALID_TIMER_ID)
	, PlayFromStartOnChanged(true)
	, m_AccumulatedTime(-1)
	, EnableRootMotion(false)
{
	
}

SkeletalMeshComponent::SkeletalMeshComponent(const SkeletalMeshComponent& Other)
	:Super(Other)
	, m_CachedRenderer(Other.m_CachedRenderer)		
	, m_PlayRate(Other.m_PlayRate)
	, m_CachedFrameIndex(Other.m_CachedFrameIndex)
	, m_Looping(Other.m_Looping)
	, m_Playing(Other.m_Playing)
	, m_BoneMatrixArray(INDEX_NONE)
	, m_BoneSocketMatrixArray(INDEX_NONE)
	, m_Animation(Other.m_Animation)
	, m_NotifiyInit(false)
	, m_BlendThreshold(Other.m_BlendThreshold)
	, m_BlendTimerID(INVALID_TIMER_ID)
	, PlayFromStartOnChanged(Other.PlayFromStartOnChanged)
	, m_AccumulatedTime(Other.m_AccumulatedTime)
	, EnableRootMotion(Other.EnableRootMotion)
{

	if (GetMeshAsset())
	{
		SkeletalMesh = GetMeshAssetAs<SkeletalMeshAsset>();	
	}	
}

SkeletalMeshComponent::~SkeletalMeshComponent()
{
	UnBindOnFinished();
	AnimationSequence::RemoveRuntimeNotifies(OUT m_RuntimeNotifies);
}

void SkeletalMeshComponent::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);	

	m_CachedRenderer = g_Engine->Get<Renderer>();
	

}

void SkeletalMeshComponent::BeginPlay()
{
	Super::BeginPlay();	
	PlayFromStart();
}

void SkeletalMeshComponent::Render(class Renderer* pRenderer)
{	
	SkeletalMeshAsset* skeletalMesh = GetMeshAssetAs<SkeletalMeshAsset>();
	if (skeletalMesh)
	{	
		pRenderer->SetConstant1i("UseSkinning", m_Animation != nullptr );	
		//Animation Inplace
		SVector3 rootOffset = SVector3(0.0f);
	
		{
			Skeleton* skel = skeletalMesh->GetSkeleton();
			if (skel)
			{				
				STransform socketTr = GetBoneTransform(skel->GetRootBoneIndex());
				STransform comp = GetComponentTransform();
				rootOffset = comp.InverseTransformPosition(socketTr.Translation);

			}			
			
		}	
		
		pRenderer->SetConstant3f("rootOffset", rootOffset);
	}

	Super::Render(pRenderer);
	


}

void SkeletalMeshComponent::RenderDepth(class Renderer* pRenderer, const Matrix& ViewProj)
{
	SkeletalMeshAsset* skeletalMesh = GetMeshAssetAs<SkeletalMeshAsset>();
	if (skeletalMesh)
	{
		pRenderer->SetConstant1i("UseSkinning", m_Animation != nullptr);
		SVector3 rootOffset = SVector3(0.0f);
		//if (EnableRootMotion)
		{
			Skeleton* skel = skeletalMesh->GetSkeleton();
			if (skel)
			{
				STransform socketTr = GetBoneTransform(skel->GetRootBoneIndex());
				STransform comp = GetComponentTransform();
				rootOffset = comp.InverseTransformPosition(socketTr.Translation);

			}
		}

		pRenderer->SetConstant3f("rootOffset", rootOffset);
	}

	Super::RenderDepth(pRenderer, ViewProj);
}

void SkeletalMeshComponent::GatherRenderInstance(class Renderer* pRenderer, std::vector<SInstanceInfo>& InstanceInfos, int32 InstanceID)
{
	Super::GatherRenderInstance(pRenderer, InstanceInfos, InstanceID);

	SkeletalMeshAsset* skeletalMesh = GetMeshAssetAs<SkeletalMeshAsset>();
	if (skeletalMesh)
	{

		pRenderer->SetConstant1i("InstancingBoneCount", skeletalMesh->GetBoneCount());
		InstanceInfos[InstanceID].useSkinning = m_Animation != nullptr;
		SVector3 rootOffset = SVector3::ZeroVector;
		//if(EnableRootMotion)
		{
			Skeleton* skel = skeletalMesh->GetSkeleton();
			if (skel)
			{
				STransform socketTr = GetBoneTransform(skel->GetRootBoneIndex());
				STransform comp = GetComponentTransform();

				rootOffset = comp.InverseTransformPosition(socketTr.Translation);
			}
		}
		InstanceInfos[InstanceID].rootOffset = rootOffset;

		//if (skeletalMesh->GetInstancingBoneMatrixArray() != INDEX_NONE)
		//{
		//	pRenderer->SetStructured("InstancingComputedMatrix", skeletalMesh->GetInstancingBoneMatrixArray());
		//	//pRenderer->SetStructured();		
		//}		
		
	}
}

void SkeletalMeshComponent::GatherRenderDeptthInstance(class Renderer* pRenderer, const Matrix& ViewProj, std::vector<SDepthInstanceInfo>& InstanceInfos, int32 InstanceID)
{
	Super::GatherRenderDeptthInstance(pRenderer, ViewProj, InstanceInfos, InstanceID);

	SkeletalMeshAsset* skeletalMesh = GetMeshAssetAs<SkeletalMeshAsset>();
	if (skeletalMesh)
	{
		//pRenderer->SetConstant1i("UseSkinning", 0);
		//pRenderer->SetConstant1i("AnimRowIndex", InstanceID)
		
		SVector3 rootOffset = SVector3::ZeroVector;
		//if (EnableRootMotion)
		{
			Skeleton* skel = skeletalMesh->GetSkeleton();
			if (skel)
			{
				STransform socketTr = GetBoneTransform(skel->GetRootBoneIndex());
				STransform comp = GetComponentTransform();

				rootOffset = comp.InverseTransformPosition(socketTr.Translation);
			}
		};
		pRenderer->SetConstant1i("InstancingBoneCount", skeletalMesh->GetBoneCount());
		InstanceInfos[InstanceID].useSkinning = m_Animation != nullptr;
		InstanceInfos[InstanceID].rootOffset = rootOffset;

		//if (skeletalMesh->GetInstancingBoneMatrixArray() != INDEX_NONE)
		//	pRenderer->SetStructured("InstancingComputedMatrix", skeletalMesh->GetInstancingBoneMatrixArray());

	}
}

void SkeletalMeshComponent::PreUpdate(float DeltaSeconds)
{
	Super::PreUpdate(DeltaSeconds);
	//For ChangeAnimation Detection
	m_PrevAnimation = m_Animation;
	

}

void SkeletalMeshComponent::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);
	ComputeAnimation(m_CachedRenderer);
	UpdateAnimation(DeltaSeconds);
	
		
	if(m_AnimationStarted && IsPlaying() && m_Animation)
		OnStarted();

	if(EnableRootMotion)
	{		
		if (SkeletalMesh)
		{
			Skeleton* skel = SkeletalMesh->GetSkeleton();
			if (skel && m_Animation && IsPlaying())
			{
				STransform socketTr = GetBoneTransform(skel->GetRootBoneIndex(), m_RootMotionStarted);
				//m_Parent->SetWorldLocation(socketTr.Translation);		
				//m_Parent->AddWorldLocation(socketTr.Translation - m_Parent->GetWorldLocation());
				if(m_Parent)
					m_Parent->AddRootMotaionAcc((socketTr.Translation - m_Parent->GetWorldLocation()) * 1/ 60.0f);

				
			}
		}
	}


	
}


void SkeletalMeshComponent::PostUpdate(float DeltaSeconds)
{	
	Super::PostUpdate(DeltaSeconds);	
	if(GetMeshAsset() != SkeletalMesh)
		SetMeshAsset(SkeletalMesh);


	if (m_Animation != m_PrevAnimation)
	{
		AnimationChanged();
		m_PrevAnimation = m_Animation;
	}

	if (!m_NotifiyInit)
	{
		AnimationSequence::RemoveRuntimeNotifies(m_RuntimeNotifies);
		if (m_Animation)
		{
			m_Animation->MakeRuntimeNotifyInfos(OUT m_RuntimeNotifies);
			m_NotifiyInit = true;
		}
	}

	m_AnimEnded = false;

	
}

STransform SkeletalMeshComponent::GetSocketTransform(const std::string& InSocketName) const
{
	STransform OutSocketTransform = GetComponentTransform();

	if (InSocketName != "")
	{
		int32 SocketBoneIndex;
		STransform SocketLocalTransform;
		SkeletalMeshSocket*  Socket = GetSocketInfoByName(InSocketName, OUT SocketLocalTransform, OUT SocketBoneIndex);
		// apply the socket transform first if we find a matching socket
		if (Socket)
		{
			if (SocketBoneIndex != INDEX_NONE)
			{
				STransform BoneTransform = GetBoneTransform(SocketBoneIndex);		
				STransform RootTr = GetRootBoneTransform();
				STransform comp = GetComponentTransform();
				SVector3 worldRootOffset = comp.InverseTransformPosition(RootTr.Translation);
				worldRootOffset *= comp.Scale3D;
				worldRootOffset = worldRootOffset.TransformNormal(Matrix::StaticRotationQuaternion(comp.Rotation));
				BoneTransform.Translation -= worldRootOffset;

				OutSocketTransform = SocketLocalTransform * BoneTransform;		
			}
		}
		else
		{
			int32 BoneIndex = GetBoneIndex(InSocketName);
			if (BoneIndex != INDEX_NONE)
			{
				STransform BoneTransform = GetBoneTransform(SocketBoneIndex);

				STransform RootTr = GetRootBoneTransform();
				STransform comp = GetComponentTransform();
				SVector3 worldRootOffset = comp.InverseTransformPosition(RootTr.Translation);
				worldRootOffset = worldRootOffset.TransformNormal(Matrix::StaticRotationQuaternion(comp.Rotation));			
				BoneTransform.Translation -= worldRootOffset;

				OutSocketTransform = BoneTransform;
			}
		}
	}
		

	return OutSocketTransform;
}

void SkeletalMeshComponent::SetMeshAsset(class MeshAsset* InMeshAsset)
{
	Super::SetMeshAsset(InMeshAsset);
	SkeletalMesh = GetMeshAssetAs<SkeletalMeshAsset>();
}

void SkeletalMeshComponent::AdditionalBindPerIA(class Renderer* pRenderer)
{
	if (m_BoneMatrixArray != INDEX_NONE)
		pRenderer->SetStructured("ComputedMatrix", m_BoneMatrixArray);
}

bool SkeletalMeshComponent::Serialize(Archive& ar)
{
	bool bResult = Super::Serialize(ar);
	
	if(ar.IsLoading())
	{
		SetMeshAsset(SkeletalMesh);
	}

	return bResult;
}

void SkeletalMeshComponent::UpdateAnimation(float DeltaSeconds)
{
	bool bIsFinished = false;

	if (m_Playing)
	{
		const float TimelineLength = GetAnimationLength();
		const float EffectiveDeltaTime = DeltaSeconds * (m_ReversePlayback ? (-m_PlayRate) : (m_PlayRate));

		float NewPosition = m_AccumulatedTime + EffectiveDeltaTime;


		if (EffectiveDeltaTime > 0.0f)
		{
			if (NewPosition > TimelineLength)
			{
				if (m_Looping)
				{					
					SetPlaybackPosition(TimelineLength);
					SetPlaybackPosition(0.0f);

					if (TimelineLength > 0.0f)
					{					
						NewPosition = TimelineLength;
						while (NewPosition >= TimelineLength)
						{
							NewPosition -= TimelineLength;
						}
					} else
					{
						NewPosition = 0.0f;
					}
				} else
				{
					// If not looping, snap to end and stop playing.					
					NewPosition = TimelineLength;
					Stop();
					bIsFinished = true;
				}
			}
		}
		//Reverse
		else
		{
			if (NewPosition < 0.0f)
			{
				if (m_Looping)
				{
					// If looping, play to start, jump to end, and set target to somewhere near the end.
					SetPlaybackPosition(0.0f);
					SetPlaybackPosition(TimelineLength);

					if (TimelineLength > 0.0f)
					{
						while (NewPosition < 0.0f)
						{
							NewPosition += TimelineLength;
						}
					} else
					{
						NewPosition = 0.0f;
					}
				} 
				else
				{
					// If not looping, snap to start and stop playing.
					NewPosition = 0.0f;
					Stop();
					bIsFinished = true;
				}
			}
		}

		SetPlaybackPosition(NewPosition);

		if (bIsFinished)
		{
			OnFinished();
		}
	}
}

void SkeletalMeshComponent::CalculateCurrentFrame()
{
	const int32 LastCachedFrame = m_CachedFrameIndex;
	m_CachedFrameIndex = (m_Animation != nullptr) ? m_Animation->GetKeyFrameIndexAtTime(m_AccumulatedTime) : INDEX_NONE;
}

void SkeletalMeshComponent::ComputeAnimation(class Renderer* pRenderer)
{
	SkeletalMeshAsset* skeletalMesh = GetMeshAssetAs<SkeletalMeshAsset>();
	if (skeletalMesh)
	{
		Skeleton* boneSkeleton =  skeletalMesh->GetSkeleton();	

		if (m_Animation && boneSkeleton)
		{				

			pRenderer->SetShader(EShaders::COMPUTE_ANIMATION);		
			uint32 BoneCount = (uint32)skeletalMesh->GetBoneCount();

			SAnimationIfno animInfo;

			animInfo.AnimBoneCount = BoneCount;
			//마지막프래임을 강제로 밀어준다		
			
			m_Animation->GetAnimationInfo(OUT animInfo, m_AccumulatedTime, m_CachedFrameIndex,false);
				

			animInfo.AnimChangeAnimation = (int32)m_BlendTimerID;
			if (m_BlendTimerID)
				m_ChangeBlendAnimation->GetChangeAnimationInfo(OUT animInfo, m_TimerHandle.GetRemainTimeRate(m_BlendTimerID));
			else
				animInfo.AnimChangeRatio = 0;		
						

			animInfo.AnimRowIndex = GetInstanceID();
			pRenderer->SetConstantStruct("animInfo", (void*)&animInfo);					

			std::vector<Matrix> offsetMatrics = boneSkeleton->GetOffsetMatrics();

			if(m_BoneMatrixArray == INDEX_NONE)
				m_BoneMatrixArray =  pRenderer->CreateRWStructured("BoneMatrixArray", EShaders::COMPUTE_ANIMATION, nullptr, BoneCount);

			if(m_BoneSocketMatrixArray == INDEX_NONE)
				m_BoneSocketMatrixArray = pRenderer->CreateRWStructured("BoneSocketMatrixArray", EShaders::COMPUTE_ANIMATION, nullptr, BoneCount,ECPUAccess::CPU_R);

			
			if (skeletalMesh->GetInstancingBoneMatrixArray() == INDEX_NONE)
				skeletalMesh->CreateInstancingBoneMatrix(pRenderer);

			// Output Binding! 
			pRenderer->SetRWStructured(m_BoneMatrixArray, BoneCount);
			pRenderer->SetRWStructured(m_BoneSocketMatrixArray, BoneCount);
			pRenderer->SetRWStructured(skeletalMesh->GetInstancingBoneMatrixArray(), (uint32)skeletalMesh->m_RenderCount * (uint32)BoneCount);					

			pRenderer->SetStructured("OffsetArray", (void*)offsetMatrics.data(), sizeof(Matrix) * BoneCount);
			pRenderer->SetStructured("FrameTransArray", (void*)m_Animation->GetAnimationTrans().data(), m_Animation->GetAnimationTrans().size() * sizeof(SAnimFrameTrans));
			if(m_BlendTimerID)
				pRenderer->SetStructured("ChangeFrameTransArray", (void*)m_ChangeBlendAnimation->GetAnimationTrans().data(), m_ChangeBlendAnimation->GetAnimationTrans().size() * sizeof(SAnimFrameTrans));

			pRenderer->Apply();
			uint32 GroupX = BoneCount / 256 + 1;
			pRenderer->Dispatch(GroupX, 1, 1);
			pRenderer->ClearSRV();

			m_ComputedBoneMatrix.resize(BoneCount);			
			void* data = (void*)m_ComputedBoneMatrix.data();
			pRenderer->ReadBuffer(m_BoneSocketMatrixArray, data);
			//소켓		
		}
		else if (boneSkeleton)
		{
			uint32 BoneCount = (uint32)skeletalMesh->GetBoneCount();
			m_ComputedBoneMatrix.resize(BoneCount);
			void* dest = (void*)m_ComputedBoneMatrix.data();
			void* src = (void*)boneSkeleton->GetOffsetMatrics().data();
			::memcpy(dest, src, sizeof(Matrix) * BoneCount);
			for(uint32 i =0;i<BoneCount;i++)
			{
				m_ComputedBoneMatrix[i].Inverse();
				m_ComputedBoneMatrix[i].Transpose();
				//Normal 행렬
			}

		}
	}

}

void SkeletalMeshComponent::CallNotifies(float NewPosition)
{
	if (!m_Animation)
		return;

	const float TimelinePercent = NewPosition / GetAnimationLength();
	for (size_t i = 0; i < m_RuntimeNotifies.size(); i++)
	{
		//정방향 재생
		if (!m_ReversePlayback)
		{
			if (!m_RuntimeNotifies[i]->NotifyInst->IsCalled())
			{
				if (m_RuntimeNotifies[i]->NotifyTimeRate <= TimelinePercent)
				{
					m_RuntimeNotifies[i]->NotifyInst->Notify(this, m_Animation);
				}
			}
		}
		//역방향 재생
		else
		{
			if (!m_RuntimeNotifies[i]->NotifyInst->IsCalled())
			{
				if (m_RuntimeNotifies[i]->NotifyTimeRate >= TimelinePercent)
				{
					m_RuntimeNotifies[i]->NotifyInst->Notify(this, m_Animation);
				}
			}
		}
	}
}

void SkeletalMeshComponent::OnFinished()
{
	m_Animation->ResetNotifies(m_RuntimeNotifies);

	if(m_OnFinished)
	{
		m_OnFinished();
	}

}

void SkeletalMeshComponent::AnimationStarted()
{	
	m_AnimationStarted = true;	
}

void SkeletalMeshComponent::OnStarted()
{
	if (SkeletalMesh)
	{
		Skeleton* skel = SkeletalMesh->GetSkeleton();
		if (skel && m_Animation)
		{
			if(m_Parent)
			{
				m_Parent->ConditionalUpdateComponentToWorld();

				STransform parentTr = m_Parent->GetComponentTransform();
				parentTr.Rotation = GetComponentTransform().Rotation;
				m_RootMotionStarted = parentTr;				
			}
	
		}
	}
	
	m_AnimationStarted = false;
}

void SkeletalMeshComponent::OnEnded()
{
	if (SkeletalMesh)
	{
		Skeleton* skel = SkeletalMesh->GetSkeleton();
		if (skel && m_Animation && m_Parent)
		{			
			STransform socketT = GetBoneTransform(skel->GetRootBoneIndex());		
		}
	}
	
	m_AnimEnded = true;
}

void SkeletalMeshComponent::SetPlaybackPosition(float NewPosition)
{
	float OldPosition = m_AccumulatedTime;
	m_AccumulatedTime = NewPosition;

	bool PreNotifyReverse = m_ReversePlayback;

	if (OldPosition != m_AccumulatedTime)
	{
		CalculateCurrentFrame();
		CallNotifies(NewPosition);

		if(NewPosition <= 0.0f)
		{
			AnimationStarted();
		}
		else if (NewPosition >= GetAnimationLength())
		{
			OnEnded();
		}
	}	


	if (m_AccumulatedTime >= GetAnimationLength() || m_AccumulatedTime <= 0.0f || PreNotifyReverse != m_ReversePlayback)
	{
		if (m_Animation)
		{
			m_Animation->ResetNotifies(m_RuntimeNotifies);
		}
	}
}

float SkeletalMeshComponent::GetPlaybackPosition() const
{
	return m_AccumulatedTime;
}

void SkeletalMeshComponent::SetLooping(bool bNewLooping)
{
	m_Looping = bNewLooping;
}

bool SkeletalMeshComponent::IsLooping() const
{
	return m_Looping;
}

void SkeletalMeshComponent::SetPlayRate(float NewRate)
{
	m_PlayRate = NewRate;
}

float SkeletalMeshComponent::GetPlayRate() const
{
	return m_PlayRate;
}

void SkeletalMeshComponent::SetNewTime(float NewTime)
{
	SetPlaybackPosition(NewTime);
}

float SkeletalMeshComponent::GetAnimationLength() const
{
	return (m_Animation != nullptr) ? m_Animation->GetTotalDuration() : 0.0f;
}

int32 SkeletalMeshComponent::GetAnimLengthInFrames() const
{
	return (m_Animation != nullptr) ? m_Animation->GetKeyFrameCount() : 0;
}

bool SkeletalMeshComponent::IsPlaying() const
{
	return m_Playing;
}

bool SkeletalMeshComponent::IsReversing() const
{
	return m_Playing && m_ReversePlayback;
}

void SkeletalMeshComponent::Play()
{
	m_ReversePlayback = false;
	m_Playing = true;
}

void SkeletalMeshComponent::PlayFromStart()
{
	SetPlaybackPosition(0.0f);
	Play();
}

void SkeletalMeshComponent::Reverse()
{
	m_ReversePlayback = true;
	m_Playing = true;
}

void SkeletalMeshComponent::ReverseFromEnd()
{
	SetPlaybackPosition(GetAnimationLength());
	Reverse();
}

void SkeletalMeshComponent::Stop()
{
	m_Playing = false;
}

void SkeletalMeshComponent::BindOnFinished(std::function<void(void)> Func)
{
	m_OnFinished = Func;
}

void SkeletalMeshComponent::UnBindOnFinished()
{
	m_OnFinished = nullptr;
}

void SkeletalMeshComponent::AnimationChanged()
{
	m_ChangeBlendAnimation = m_PrevAnimation;

	if (GetOwner<GameObject>() && m_ChangeBlendAnimation && m_Animation && m_BlendThreshold)
	{
		if (m_BlendTimerID != INVALID_TIMER_ID)
		{
			m_TimerHandle.RemoveTimer(OUT m_BlendTimerID);
		}		
		m_TimerHandle.SetTimer(OUT m_BlendTimerID, m_BlendThreshold, false, std::bind(&SkeletalMeshComponent::OnChangeEnded, this));
	}
	if (m_Animation && PlayFromStartOnChanged)
	{			
		PlayFromStart();
	}

	m_NotifiyInit = false;
}

void SkeletalMeshComponent::OnChangeEnded()
{
	m_BlendTimerID = INVALID_TIMER_ID;
}

class AnimationSequence* SkeletalMeshComponent::GetCurrentAnimationSequence() const
{
	return m_Animation;
}

bool SkeletalMeshComponent::IsEnableRootMotion() const
{
	return EnableRootMotion;
}

void SkeletalMeshComponent::SetEnableRootMotion(bool InEnableRootMotion)
{	
	EnableRootMotion = InEnableRootMotion;	
}

void SkeletalMeshComponent::ResetAllNotifies()
{
	if(m_Animation)
		m_Animation->ResetNotifies(m_RuntimeNotifies);
}

class SkeletalMeshSocket* SkeletalMeshComponent::FindSocketInfo(const std::string& InSocketName, STransform& OutTransform, int32& OutBoneIndex, int32& OutIndex) const
{
	SkeletalMeshSocket* Socket = nullptr;

	if (SkeletalMesh)
	{
		int32 SocketIndex;
		Socket = SkeletalMesh->FindSocketInfo(InSocketName, OutTransform, OutBoneIndex, SocketIndex);
	} 

	return Socket;
}

class SkeletalMeshSocket* SkeletalMeshComponent::GetSocketInfoByName(const std::string& InSocketName, STransform& OutTransform, int32& OutBoneIndex) const
{		

	SkeletalMeshSocket* Socket = nullptr;

	if (SkeletalMesh)
	{
		int32 SocketIndex;
		Socket = SkeletalMesh->FindSocketInfo(InSocketName, OutTransform, OutBoneIndex, SocketIndex);
	} 
	

	return Socket;
}

Matrix SkeletalMeshComponent::GetBoneMatrix(int32 BoneIndex) const
{
	if(SkeletalMesh)
	{
		Skeleton* skeleton = SkeletalMesh->GetSkeleton();
		if(skeleton)
		{
			if (BoneIndex < skeleton->GetBoneCount())
			{
				return m_ComputedBoneMatrix[BoneIndex] * GetComponentTransform().ToMatrixWithScale();
				//int32 ParentBoneIndex = MasterBoneMap[BoneIdx];

				//if (ParentBoneIndex != INDEX_NONE && ParentBoneIndex < MasterPoseComponentInst->GetNumComponentSpaceTransforms())
				//{
				//	return GetComponentSpaceTransforms()[ParentBoneIndex].ToMatrixWithScale() * GetComponentTransform().ToMatrixWithScale();
				//}
				
			}
		}

	}

	return Matrix::StaticIdentity();
}

SVector3 SkeletalMeshComponent::GetBoneAxis(const std::string& BoneName, EAxis Axis) const
{
	int32 BoneIndex = GetBoneIndex(BoneName);
	if (BoneIndex == INDEX_NONE)
	{		
		return SVector3::ZeroVector;
	} 
	else if (Axis == EAxis::MAX)
	{
		return SVector3::ZeroVector;
	} 
	else
	{
		return GetBoneMatrix(BoneIndex).GetUnitAxis(Axis);
	}
}

int32 SkeletalMeshComponent::GetNumBones() const
{
	if(SkeletalMesh)
	{
		Skeleton* skel = SkeletalMesh->GetSkeleton();
		if(skel)
		{
			return skel->GetBoneCount();
		}

	}
	return 0;
}

int32 SkeletalMeshComponent::GetBoneIndex(const std::string& BoneName) const
{
	int32 BoneIndex = INDEX_NONE;
	if (BoneName != "" && SkeletalMesh)
	{
		Skeleton* skel = SkeletalMesh->GetSkeleton();
		if(skel)
			BoneIndex = skel->GetBoneIndex(BoneName);
	}

	return BoneIndex;
}

STransform SkeletalMeshComponent::GetBoneTransform(int32 BoneIndex) const
{
	return GetBoneTransform(BoneIndex, GetComponentTransform());
}

STransform SkeletalMeshComponent::GetBoneTransform(int32 BoneIndex, const STransform& LocalToWorld) const
{
	
	if (BoneIndex < m_ComputedBoneMatrix.size())
	{
		
		STransform bonetransform;
		Matrix computed = m_ComputedBoneMatrix[BoneIndex];
		computed.Transpose();
		bonetransform.FromMatrix(computed);
		return bonetransform * LocalToWorld;		
		
	}

	return STransform::Identity;
}

STransform SkeletalMeshComponent::GetRootBoneTransform() const
{
	Skeleton* skel = SkeletalMesh->GetSkeleton();
	if (skel)
	{
		return GetBoneTransform(skel->GetRootBoneIndex());
	}

	return STransform::Identity;
}

std::string SkeletalMeshComponent::GetParentBone(const std::string& InBoneName) const
{
	std::string Result = "";

	int32 BoneIndex = GetBoneIndex(InBoneName);
	if ((BoneIndex != INDEX_NONE) && (BoneIndex > 0)) 
	{
		Skeleton* skel = SkeletalMesh->GetSkeleton();
		if(skel)
			Result = skel->GetBoneName(skel->GetParentIndex(InBoneName));
		
	}
	return Result;
}

void SkeletalMeshComponent::SetAnimationSequence(class AnimationSequence* InAnimSeq)
{	
	if(m_Animation != InAnimSeq)
	{
		AnimationChanged();
	}
	m_Animation = InAnimSeq;	
}

bool SkeletalMeshComponent::IsAnimEnded() const
{
	return m_AnimEnded;
}
