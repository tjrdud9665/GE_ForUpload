#include "PCH.h"
#include "SoundWidget.h"
#include <Assets/Sound/Sound.h>
#include "SoundManager.h"
#include "imgui.h"
#include <ImSequencer.h>

SoundWidget::SoundWidget()
{

}

SoundWidget::SoundWidget(const SoundWidget& Other)
{
	//Not use.
}

SoundWidget::~SoundWidget()
{

}

void SoundWidget::Build()
{
	Super::Build();

	m_cachedMgr = g_Engine->Get<SoundManager>();
}

void SoundWidget::Update(float DeltaSeconds)
{
	if (m_EditSound)
	{
		std::vector<std::string> vecGroups;

		size_t GroupSize = m_cachedMgr->GetAllChannelGroups(OUT vecGroups);


		std::string name = "##" + GetName();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

		if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("SoundGroupname");
			ImGui::TableSetColumnIndex(1);


			if (ImGui::BeginCombo("##SoundChannlGroups", m_EditSound->GetGroupName().c_str()))
			{
				for (size_t i = 0; i < GroupSize; i++)
				{
					if (ImGui::Selectable(vecGroups[i].c_str()))
					{
						m_EditSound->SetGroupName(vecGroups[i]);
						m_EditSound->SetGroup(m_cachedMgr->FindChannelGroup(vecGroups[i]));
					}

				}

				ImGui::EndCombo();
			}


			ImGui::EndTable();
		}

		int32 pos = m_EditSound->GetPosition();

		ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
		ImGui::SetNextItemWidth(canvas_sz.x);

		if (ImGui::SliderInt("##soundPlayBar", &pos, 0, m_EditSound->GetLength()))
		{
			m_EditSound->Pause();
			m_EditSound->SetPosition((uint32)pos);
		}

		if (ImGui::Button("Play"))
		{
			m_EditSound->TryToResumePlay();
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			m_EditSound->Pause();
		}

		ImGui::PopStyleVar();
	}

}

void SoundWidget::SetEditSound(Sound* InSound)
{
	if(m_EditSound)
	{
		m_EditSound->Stop();
	}
	m_EditSound = InSound;
}
