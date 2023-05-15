#include "EnginePCH.h"
#include "AnimationSequence.h"
#include "Path/PathManager.h"

AnimationSequence::AnimationSequence()
	:m_FramePerSec(0.0f)
	, m_PlayTime(1.0f)
{

}

AnimationSequence::AnimationSequence(const AnimationSequence& Other)
	:Super(Other)
	, m_FramePerSec(Other.m_FramePerSec)	
	, m_PlayTime(Other.m_PlayTime)
	, m_Skeleton(Other.m_Skeleton)	
{
	size_t notifySize = Notifies.size();
	for (size_t i = 0; i < notifySize; i++)
	{
		SAFE_DELETE(Notifies[i]);

	}
	Notifies.clear();

	size_t otherNotifySize = Other.Notifies.size();

	for (size_t i = 0; i < otherNotifySize; i++)
	{
		Notifies[i] = Other.Notifies[i]->Clone();
	}

}

AnimationSequence::~AnimationSequence()
{
	size_t keyFrameSize = m_vecKeyFrame.size();
	for(size_t i =0;i< keyFrameSize;i++)
	{
		SAFE_DELETE(m_vecKeyFrame[i]);
	}
	size_t notify_size = Notifies.size();
	for (size_t i = 0; i < notify_size; i++)
	{
		SAFE_DELETE(Notifies[i]);
	}
}


bool AnimationSequence::PostLoad()
{
	bool bResult = Super::PostLoad();

	//MakeFrameTrans(m_Skeleton);
	return bResult;
}

bool AnimationSequence::Serialize(Archive& ar)
{
	bool bResult = Super::Serialize(ar);

	//Save
	if (ar.IsSaving())
	{
		std::string RelPathWithName;
		GetRelativePathWithName(OUT RelPathWithName);
		fs::path animPath = RelPathWithName;
		animPath.replace_filename(GetName() + "_data.bin");
		std::string sPath = animPath.string();
		ar.Serialize("anim_file_path", sPath);
					
		const SPathInfo* Pathinfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
		fs::path fullPath = Pathinfo->aPath;
		fullPath /= animPath;
		SaveKeyFrameFile(fullPath.string().c_str(), m_vecKeyFrame);
	
	} else
	{
		std::string RelPathWithName;
		GetRelativePathWithName(OUT RelPathWithName);
		fs::path animPath = RelPathWithName;
		animPath.replace_filename(GetName() + "_data.bin");
		std::string sPath = animPath.string();
		ar.Serialize("anim_file_path", sPath);
		const SPathInfo* Pathinfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
		fs::path fullPath = Pathinfo->aPath;
		fullPath /= animPath;

		LoadKeyFrameFile(fullPath.string().c_str(), OUT m_vecKeyFrame,OUT m_FrameTrans,m_FrameLength);
	}

	
	return bResult;
}

bool AnimationSequence::SaveKeyFrameFile(const char* FullPath, const std::vector<SBoneKeyFrame*>& InAnimKeyFrames)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "wb");

	if (!File)
		return false;

	size_t	Count = InAnimKeyFrames.size();

	::fwrite(&Count, sizeof(size_t), 1, File);

	for (size_t i = 0; i < Count; ++i)
	{
		::fwrite(&InAnimKeyFrames[i]->BoneIndex, sizeof(int32), 1,File);

		size_t	FrameCount = InAnimKeyFrames[i]->vecKeyFrame.size();

		::fwrite(&FrameCount, sizeof(size_t), 1, File);

		for (size_t j = 0; j < FrameCount; ++j)
		{
			::fwrite(&InAnimKeyFrames[i]->vecKeyFrame[j]->Time, sizeof(double), 1, File);
			::fwrite(&InAnimKeyFrames[i]->vecKeyFrame[j]->Pos, sizeof(SVector3), 1, File);
			::fwrite(&InAnimKeyFrames[i]->vecKeyFrame[j]->Scale, sizeof(SVector3), 1, File);
			::fwrite(&InAnimKeyFrames[i]->vecKeyFrame[j]->Rot, sizeof(SVector4), 1, File);
		}
	}


	::fclose(File);

	return true;
}

bool AnimationSequence::LoadKeyFrameFile(const char* FullPath, std::vector<SBoneKeyFrame*>& OutAnimKeyFrames, std::vector<SAnimFrameTrans>& OutFrameTrans, int32 FrameLength)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "rb");

	if (!File)
		return false;

	size_t	Count = 0;

	::fread(&Count, sizeof(size_t), 1, File);	//BoneKeyFrameSize;	
	
	OutFrameTrans.resize(Count * FrameLength);
	for (size_t i = 0; i < Count; ++i)
	{
		SBoneKeyFrame* pBoneKeyFrame = new SBoneKeyFrame;
		OutAnimKeyFrames.push_back(pBoneKeyFrame);

		::fread(&pBoneKeyFrame->BoneIndex, sizeof(int32), 1, File);

		size_t	BoneFrameCount = 0;

		::fread(&BoneFrameCount, sizeof(size_t), 1, File);

		for (size_t j = 0; j < BoneFrameCount; ++j)
		{
			SKeyFrame* pKeyFrame = new SKeyFrame;
			pBoneKeyFrame->vecKeyFrame.push_back(pKeyFrame);

			::fread(&pKeyFrame->Time, sizeof(double), 1, File);
			::fread(&pKeyFrame->Pos, sizeof(SVector3), 1, File);
			::fread(&pKeyFrame->Scale, sizeof(SVector3), 1, File);
			::fread(&pKeyFrame->Rot, sizeof(SVector4), 1, File);	

			if (j < FrameLength)
			{
				SAnimFrameTrans	FrameTr = {};
				FrameTr.Scale = SVector4(pKeyFrame->Scale.x, pKeyFrame->Scale.y,pKeyFrame->Scale.z, 1.f);
				FrameTr.Translate = SVector4(pKeyFrame->Pos.x, pKeyFrame->Pos.y,pKeyFrame->Pos.z, 1.f);
				FrameTr.Rot = pKeyFrame->Rot;
				OutFrameTrans[i * FrameLength + j] = FrameTr;
			}
		}
	}
	

	::fclose(File);
	
	return true;
}

float AnimationSequence::GetTotalDuration() const
{
	if (m_FramePerSec != 0)
	{
		return m_FrameTime * m_FrameLength;
	}

	return 0.0f;

}

size_t AnimationSequence::GetNotifyInfos(std::vector<class AnimNotifyInfo*>& OutNotifies) const
{
	for (size_t i = 0; i < Notifies.size(); i++)
	{
		OutNotifies.push_back(Notifies[i]);

	}

	return Notifies.size();
}

void AnimationSequence::GetAnimationInfo(SAnimationIfno& OutAnimInfo,float Time, int32 frameIdx, bool bCycling) const
{
	float FrameTime = ::max(0.01f,m_FrameTime);
	float Ratio = Time / GetTotalDuration();


	if(m_Skeleton)
	{
		OutAnimInfo.AnimBoneCount = m_Skeleton->GetBoneCount();
	}

	if (frameIdx >= m_FrameLength)
		frameIdx = m_FrameLength - 1;

	int32 NextFrameIndex = frameIdx + 1;

	if (NextFrameIndex >= m_FrameLength)
	{
		if (bCycling)
			NextFrameIndex = 0;
		else
			NextFrameIndex = frameIdx;
	}

	OutAnimInfo.AnimFrameCount = m_FrameLength;
	OutAnimInfo.AnimRatio = Ratio;
	OutAnimInfo.AnimCurrentFrame = frameIdx;
	OutAnimInfo.AnimNextFrame = NextFrameIndex;

	
}

void AnimationSequence::GetChangeAnimationInfo(SAnimationIfno& OutAnimInfo, float Ratio) const
{
	//float FrameTime = ::max(0.01f, m_FrameTime);
	//float Ratio = Time / GetTotalDuration();
	OutAnimInfo.AnimChangeFrameCount = m_FrameLength;
	OutAnimInfo.AnimChangeRatio = Ratio;
}

int32 AnimationSequence::GetKeyFrameIndexAtTime(float Time, bool bClampToEnds /*= false*/) const
{
	if ((Time < 0.0f) && !bClampToEnds)
	{
		return INDEX_NONE;
	}

	if (m_FramePerSec > 0.0f)
	{
		return (int32)(Time * m_FramePerSec);
		
		//float SumTime = 0.0f;

		//for (int32 KeyFrameIndex = 0; KeyFrameIndex < m_FrameLength; ++KeyFrameIndex)
		//{
		//	SumTime += 1 / m_FramePerSec;

		//	if (Time <= SumTime)
		//	{
		//		return KeyFrameIndex;
		//	}
		//}

		//// Return the last frame (note: relies on INDEX_NONE = -1 if there are no key frames)
		//return (int32)m_FrameLength - 1;
	} 
	else
	{
		return INDEX_NONE; //NOKeyframes..
	}

	return INDEX_NONE;
}

size_t AnimationSequence::MakeRuntimeNotifyInfos(std::vector<AnimNotifyInfo*>& OutNotifies)
{
	size_t notifySize = Notifies.size();

	OutNotifies.resize(notifySize);

	for (size_t i = 0; i < notifySize; i++)
	{
		OutNotifies[i] = Notifies[i]->Clone();
	}

	std::sort(OutNotifies.begin(), OutNotifies.end(), &AnimationSequence::SortNotifyInfoByTime);

	return notifySize;
}

void AnimationSequence::RemoveRuntimeNotifies(std::vector<AnimNotifyInfo*>& DeleteNotifies)
{
	size_t notifySize = DeleteNotifies.size();

	for (size_t i = 0; i < notifySize; i++)
	{
		SAFE_DELETE(DeleteNotifies[i]);
	}
	DeleteNotifies.clear();
}

void AnimationSequence::ResetNotifies(const std::vector<AnimNotifyInfo*>& InResetNotifies)
{
	size_t NotifiySize = InResetNotifies.size();
	for (size_t i = 0; i < NotifiySize; i++)
	{
		InResetNotifies[i]->NotifyInst->Reset();
	}
}

bool AnimationSequence::SortNotifyInfoByTime(const AnimNotifyInfo* src, const AnimNotifyInfo* dest)
{
	return src->GetNotifyTimeRate() < dest->GetNotifyTimeRate();
}



