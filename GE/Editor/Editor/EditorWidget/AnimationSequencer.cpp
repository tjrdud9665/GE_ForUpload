#include "PCH.h"
#include "AnimationSequencer.h"
#include <GameFramework/Components/SkeletalMeshComponent.h>
#include <Assets/Animation/AnimationSequence.h>
#include "imgui.h"

AnimationSequencer::AnimationSequencer()
{

}

AnimationSequencer::AnimationSequencer(const AnimationSequencer& Other)
{
	//Not use.
}

AnimationSequencer::~AnimationSequencer()
{

}

void AnimationSequencer::Build()
{
	Super::Build();
}

void AnimationSequencer::Update(float DeltaSeconds)
{
	if (!m_TargetAnimation || !m_AnimPriviewComponent)
		return;

	ImGui::BeginGroup();	

	float TotalTime = m_TargetAnimation->GetTotalDuration();
		

	ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!	
	ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available

	if (ImGui::Button("Play"))
	{
		m_Play = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
	{
		m_Play = false;
	}
	ImGui::SetNextItemWidth(canvas_sz.x);
	if(m_Play)
	{
		m_CurrentTime = m_AnimPriviewComponent->GetPlaybackPosition();
	}
	if (ImGui::SliderFloat("##AnimSeqTimeLine", &m_CurrentTime, 0.0f, TotalTime))
	{
		m_AnimPriviewComponent->SetPlaybackPosition(m_CurrentTime);
		m_Play = false;		
	}



	ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, 32);

	ImGuiIO& io = ImGui::GetIO();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();


	std::vector<AnimNotifyInfo*> Notifies;
	size_t cnt = m_TargetAnimation->GetNotifyInfos(OUT Notifies);

	for (size_t i = 0; i < cnt; i++)
	{
		std::string sliderName = "##AnimNotify_Cnt" + std::to_string(i);

		float NotifyTimeRate = Notifies[i]->GetNotifyTimeRate();

		//Notify 는 실제 시간이 아닌 에니메이션 전체시간의 %로 계산


		ImGui::SetNextItemWidth(canvas_sz.x);
		if (ImGui::DragFloat(sliderName.c_str(), &NotifyTimeRate, 1 / canvas_sz.x, 0.0f, 1.0f))
		{
			
		}

		Notifies[i]->SetNotifyTimeRate(NotifyTimeRate);

		float itemSize = ImGui::GetItemRectMax().x - ImGui::GetItemRectMin().x;
		float realPos = itemSize * NotifyTimeRate;

		ImVec2 NotifyLineStart = ImVec2(ImGui::GetItemRectMin().x + realPos, ImGui::GetItemRectMin().y);
		ImVec2 NotifyLineEnd = ImVec2(ImGui::GetItemRectMin().x + realPos, ImGui::GetItemRectMax().y);

		std::string indexName = Notifies[i]->GetNotifyClass().Class->GetName() + to_string(i);
		draw_list->AddText(NotifyLineStart, IM_COL32(255, 255, 255, 255), indexName.c_str());

		draw_list->AddLine(NotifyLineStart, NotifyLineEnd, IM_COL32(255, 0, 0, 255));
		//draw_list->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 255, 255));

	}
	ImGui::EndGroup();


	if(m_Play)
	{
		m_AnimPriviewComponent->Play();				
	}
	else
	{
		m_AnimPriviewComponent->Stop();
	}
	
}

void AnimationSequencer::SetTargetAnimation(class AnimationSequence* animSeq)
{
	m_TargetAnimation = animSeq;
}

void AnimationSequencer::InitPreviewComponent(class SkeletalMeshComponent* OwnerPreview)
{
	m_AnimPriviewComponent = OwnerPreview;
}
