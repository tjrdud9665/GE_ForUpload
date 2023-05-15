#include "PCH.h"
#include "CollisionEditorWidget.h"
#include "imgui.h"
#include "imgui_stl.h"
#include "imgui_internal.h"
#include <CollisionManager.h>

CollisionEditorWidget::CollisionEditorWidget()
{

}

CollisionEditorWidget::~CollisionEditorWidget()
{

}

void CollisionEditorWidget::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);
}

void CollisionEditorWidget::Update(float DeltaSeconds)
{
	std::vector<std::string> customChannelNames;
	std::vector<std::string> allChannelNames;
	std::vector<std::string> profileNames;

	//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
	//ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));

	
	//1번은 수정 불가
	CollisionManager* colMgr = g_Engine->Get<CollisionManager>();
	size_t CustomChannelSize = colMgr->GetAllCustomChannelName(OUT customChannelNames);

	size_t AllChannelSize = colMgr->GetAllChannelName(OUT allChannelNames);
	size_t ProfileSize = colMgr->GetAllProfileName(OUT profileNames);

	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;
	float fontSize = ImGui::GetFontSize();

	ImVec2 channelListSize = ImVec2(ImGui::GetContentRegionAvail().x, fontSize * (CustomChannelSize + 8));

	ImVec2 profileListSize = ImVec2(ImGui::GetContentRegionAvail().x, fontSize * (ProfileSize + 8));

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();

	if (ImGui::TreeNodeEx("CollisionChannel", flag))
	{

		if (ImGui::BeginListBox("##ChannelBorder", channelListSize))
		{
			int32 MaxChannelSize = (int32)ECollision_Channel::Max - (int32)ECollision_Channel::Custom0;

			float width = ImGui::GetContentRegionAvail().x;
			ImGui::Text("Maximum Count of custom channels is %d.", MaxChannelSize);
			const ImVec2 label_size = ImGui::CalcTextSize("NewChannel Modify Delete", NULL, true);
			ImGui::SameLine(width - label_size.x - 70);

			bool EnableNewChannel = AllChannelSize - (int32)ECollision_Channel::Custom0 >= MaxChannelSize;

			if (EnableNewChannel)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			if (ImGui::Button("NewChannel"))
			{
				ImGui::OpenPopup("NewChannelPop");
				m_newChannelName = "";
				m_newChannelDefaultInteraction = ECollision_Interaction::Overlap;
			}
			if (EnableNewChannel)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

			bool ChannelSelected = !m_SelectedChannelName.empty();

			if (!ChannelSelected)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			ImGui::SameLine();
			if (ImGui::Button("Modify"))
			{
				ImGui::OpenPopup("ModChannelPop");
				m_newChannelName = m_SelectedChannelName;
				m_newChannelDefaultInteraction = colMgr->GetCollisionChannel(m_SelectedChannelName)->Interaction;
			}

			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				colMgr->DeleteChannel(m_SelectedChannelName);
				CustomChannelSize = colMgr->GetAllCustomChannelName(OUT customChannelNames);
				AllChannelSize = colMgr->GetAllChannelName(OUT allChannelNames);

				m_SelectedChannelName = "";
			}

			if (!ChannelSelected)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

			//Custom0~~16까지 모두 커스텀채널
			//List Up CollisionChannels.
			if (ImGui::BeginTable("ChannelTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
			{

				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Default-Response");
				ImGui::TableHeadersRow();

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				//ImGui::AlignTextToFramePadding();



				for (int32 i = 0; i < customChannelNames.size(); i++)
				{
					bool selected = m_SelectedChannelName == customChannelNames[i];

					if (ImGui::Selectable(customChannelNames[i].c_str(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
					{
						m_SelectedChannelName = customChannelNames[i];
						m_SelectedIndex = i;
					}


				}

				ImGui::TableSetColumnIndex(1);

				for (int32 i = 0; i < customChannelNames.size(); i++)
				{
					switch(colMgr->GetCollisionChannel(i + (int32)ECollision_Channel::Custom0)->Interaction)
					{
					case ECollision_Interaction::Overlap:
						ImGui::Text("Overlap");
						break;
					case ECollision_Interaction::Ignore:
						ImGui::Text("Ignore");
						break;
					case ECollision_Interaction::Block:
						ImGui::Text("Block");
						break;
					}

				}


				ImGui::EndTable();
			}

			//PopUps....

			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("NewChannelPop", NULL, 0))
			{

				ImGui::Text("Name ");
				ImGui::SameLine();
				ImGui::InputText("##newChannelName", m_newChannelName);
				ImGui::Text("Default-Response");

				std::string ComboLabel = "Overlap";
				if (m_newChannelDefaultInteraction == ECollision_Interaction::Ignore)
				{
					ComboLabel = "Ignore";
				}
				else if (m_newChannelDefaultInteraction == ECollision_Interaction::Block)
				{
					ComboLabel = "Block";
				}


				if (ImGui::BeginCombo("##NewChannelInteraction", ComboLabel.c_str()))
				{
					bool defaultCollion = true;
					if (ImGui::Selectable("Overlap", defaultCollion))
					{
						m_newChannelDefaultInteraction = ECollision_Interaction::Overlap;
					}
					if (ImGui::Selectable("Ignore", !defaultCollion))
					{
						m_newChannelDefaultInteraction = ECollision_Interaction::Ignore;
					}
					if (ImGui::Selectable("Block", !defaultCollion))
					{
						m_newChannelDefaultInteraction = ECollision_Interaction::Block;
					}

					ImGui::EndCombo();
				}


				if (ImGui::Button("Apply"))
				{
					if (!m_newChannelName.empty())
						colMgr->CreateChannel(m_newChannelName, m_newChannelDefaultInteraction);

					colMgr->Save();
					ImGui::CloseCurrentPopup();
				}
				float width = ImGui::GetContentRegionAvail().x;
				const ImVec2 label_size = ImGui::CalcTextSize("Cancel", NULL, true);
				ImGui::SameLine(width - label_size.x);
				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal("ModChannelPop", NULL, 0))
			{

				ImGui::Text("Name ");
				ImGui::SameLine();

				ImGui::InputText("##ModChannelName", m_newChannelName);

				ImGui::Text("Default-Response");

				std::string ComboLabel = "Overlap";




				if (m_newChannelDefaultInteraction == ECollision_Interaction::Ignore)
				{
					ComboLabel = "Ignore";
				}
				else if (m_newChannelDefaultInteraction == ECollision_Interaction::Block)
				{
					ComboLabel = "Block";
				}

				if (ImGui::BeginCombo("##NewChannelInteraction", ComboLabel.c_str()))
				{
					bool defaultCollion = true;
					if (ImGui::Selectable("Overlap", defaultCollion))
					{
						m_newChannelDefaultInteraction = ECollision_Interaction::Overlap;
					}
					if (ImGui::Selectable("Ignore", !defaultCollion))
					{
						m_newChannelDefaultInteraction = ECollision_Interaction::Ignore;
					}
					if (ImGui::Selectable("Block", !defaultCollion))
					{
						m_newChannelDefaultInteraction = ECollision_Interaction::Block;
					}

					ImGui::EndCombo();
				}


				if (ImGui::Button("Apply"))
				{
					colMgr->ModifyChannelName(m_SelectedChannelName, m_newChannelName, m_newChannelDefaultInteraction);
					m_SelectedChannelName = "";
					colMgr->Save();
					ImGui::CloseCurrentPopup();
				}
				float width = ImGui::GetContentRegionAvail().x;
				const ImVec2 label_size = ImGui::CalcTextSize("Cancel", NULL, true);
				ImGui::SameLine(width - label_size.x);
				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}



			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("CollisionProfiles", flag))
	{
		if (ImGui::BeginListBox("##ProfileBorder", profileListSize))
		{
			float width = ImGui::GetContentRegionAvail().x;
			ImGui::Text("Profile ");
			const ImVec2 label_size = ImGui::CalcTextSize("New Profile Modify Delete", NULL, true);
			ImGui::SameLine(width - label_size.x - 70);
			if (ImGui::Button("New Profile"))
			{
				//SetUp ProfileDefulats..
				m_SelectedChannelName = allChannelNames[0];
				m_newProfileName = "";
				m_newProfileEnabled = true;
				m_newProfileDesc = "";

				colMgr->GetDefaultInteractions(OUT m_newProfileInteractions);
				ImGui::OpenPopup("NewProfilePop");

			}
			bool ProfileSelected = !m_SelectedProfileName.empty();

			bool IsEngineProfile = colMgr->IsDefaultProfile(m_SelectedProfileName);

			if (!ProfileSelected)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			ImGui::SameLine();
			if (ImGui::Button("Modify"))
			{
				SCollisionProfile* modifyProfile = colMgr->FindProfile(m_SelectedProfileName);
				m_newProfileEnabled = modifyProfile->Enable;
				m_newProfileName = modifyProfile->Name;
				m_SelectedChannelName = modifyProfile->Channel->Name;
				m_newProfileDesc = modifyProfile->Descrition;
				m_newProfileInteractions = modifyProfile->vecCollisionInteraction;

				ImGui::OpenPopup("ModProfilePop");
			}
			ImGui::SameLine();
			if (IsEngineProfile)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			if (ImGui::Button("Delete"))
			{
				colMgr->DeleteProfile(m_SelectedProfileName);
				ProfileSize = colMgr->GetAllProfileName(OUT profileNames);
				colMgr->Save();

			}
			if (!ProfileSelected)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			if (IsEngineProfile)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}


			if (ImGui::BeginTable("ProfileTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
			{
				
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Enabled");
				ImGui::TableSetupColumn("CnannelType");
				ImGui::TableSetupColumn("Description");

				ImGui::TableHeadersRow();

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				//ImGui::AlignTextToFramePadding();



				for (int32 i = 0; i < ProfileSize; i++)
				{
					bool selected = m_SelectedProfileName == profileNames[i];

					if (ImGui::Selectable(profileNames[i].c_str(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
					{
						m_SelectedProfileName = profileNames[i];
						m_SelectedIndex = i;
					}


				}

				ImGui::TableSetColumnIndex(1);

				for (int32 i = 0; i < ProfileSize; i++)
				{
					SCollisionProfile* profile = colMgr->FindProfile(profileNames[i]);
					if (profile->Enable)
						ImGui::Text("On");
					else
						ImGui::Text("Off");
				}
				ImGui::TableSetColumnIndex(2);

				for (int32 i = 0; i < ProfileSize; i++)
				{
					SCollisionProfile* profile = colMgr->FindProfile(profileNames[i]);
					ImGui::Text(profile->Channel->Name.c_str());
				}

				ImGui::TableSetColumnIndex(3);
				for (int32 i = 0; i < ProfileSize; i++)
				{
					SCollisionProfile* profile = colMgr->FindProfile(profileNames[i]);
					ImGui::Text(profile->Descrition.c_str());
				}


				ImGui::EndTable();
			}

			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("NewProfilePop", NULL, 0))
			{
				if (ImGui::BeginTable("NewProfilePopSortTable", 2, ImGuiTableFlags_Resizable))
				{

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					//ImGui::AlignTextToFramePadding();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Name ");
					ImGui::TableSetColumnIndex(1);
					ImGui::InputText("##NewProfieName", m_newProfileName);
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("CollisionEnabled");
					ImGui::TableSetColumnIndex(1);
					ImGui::Checkbox("##CollisionEnabled", &m_newProfileEnabled);
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("ChannelType");
					ImGui::TableSetColumnIndex(1);
					if (ImGui::BeginCombo("##newProfileChannel", m_SelectedChannelName.c_str()))
					{

						for (size_t i = 0; i < AllChannelSize; i++)
						{
							bool seletedChannel = m_SelectedChannelName == allChannelNames[i];
							if (ImGui::Selectable(allChannelNames[i].c_str(), seletedChannel))
							{
								m_SelectedChannelName = allChannelNames[i];
							}
						}
						ImGui::EndCombo();
					}
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Description");
					ImGui::TableSetColumnIndex(1);
					ImGui::InputText("##NewProfieDesc", m_newProfileDesc);
					//Show Collision Respose Tables.
					ImGui::Separator();

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Default-Response");
					ImGui::TableSetColumnIndex(1);

					if (ImGui::BeginTable("InterationTable", 3, 0))
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						//ImGui::AlignTextToFramePadding();
						ImGui::Text("Ignore");
						ImGui::TableSetColumnIndex(1);
						ImGui::Text("Overlap");
						ImGui::TableSetColumnIndex(2);
						ImGui::Text("Block");
						ImGui::EndTable();
					}


					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("All");
					ImGui::TableSetColumnIndex(1);

					if (ImGui::BeginTable("InterationTable", 3, 0))
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::AlignTextToFramePadding();
						std::string chkNameCol = "##_col";
						std::string chkNameIg = "##_ig";
						std::string chkNamebk = "##_bk";

						bool IsIgnore = true;
						size_t interactionSize = m_newProfileInteractions.size();
						int32 collSize = 0;
						int32 blockSize = 0;

						for (size_t i = 0; i < interactionSize; i++)
						{
							if (m_newProfileInteractions[i] == ECollision_Interaction::Overlap)
							{
								IsIgnore = false;
								collSize++;
							}
							else if (m_newProfileInteractions[i] == ECollision_Interaction::Block)
							{
								IsIgnore = false;
								blockSize++;
							}

						}

						if (ImGui::Checkbox(chkNameCol.c_str(), &IsIgnore))
						{
							for (size_t i = 0; i < interactionSize; i++)
							{
								m_newProfileInteractions[i] = ECollision_Interaction::Ignore;
							}

						}
						ImGui::TableSetColumnIndex(1);

						bool IsCollision = collSize == interactionSize;

						if (ImGui::Checkbox(chkNameIg.c_str(), &IsCollision))
						{
							for (size_t i = 0; i < interactionSize; i++)
							{
								m_newProfileInteractions[i] = ECollision_Interaction::Overlap;
							}
						}	
						ImGui::TableSetColumnIndex(2);

						bool IsBlock = blockSize == interactionSize;

						if (ImGui::Checkbox(chkNamebk.c_str(), &IsBlock))
						{
							for (size_t i = 0; i < interactionSize; i++)
							{
								m_newProfileInteractions[i] = ECollision_Interaction::Block;
							}
						}
			

						ImGui::EndTable();
					}




					for (size_t i = 0; i < AllChannelSize; i++)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						//ImGui::AlignTextToFramePadding();
						ImGui::Text(allChannelNames[i].c_str());
						ImGui::TableSetColumnIndex(1);

						if (ImGui::BeginTable("InterationTable", 3, 0))
						{
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							//ImGui::AlignTextToFramePadding();
							std::string chkNameCol = "##_col" + allChannelNames[i];
							std::string chkNameIg = "##_ig" + allChannelNames[i];
							std::string chkNamebk = "##_bk" + allChannelNames[i];

							bool IsIgnore = m_newProfileInteractions[i] == ECollision_Interaction::Ignore;

							ImGui::Checkbox(chkNameCol.c_str(), &IsIgnore);
							if (IsIgnore)
								m_newProfileInteractions[i] = ECollision_Interaction::Ignore;

							ImGui::TableSetColumnIndex(1);
							bool IsCollision = m_newProfileInteractions[i] == ECollision_Interaction::Overlap;
							ImGui::Checkbox(chkNameIg.c_str(), &IsCollision);
							if (IsCollision)
								m_newProfileInteractions[i] = ECollision_Interaction::Overlap;

							ImGui::TableSetColumnIndex(2);
							bool IsBlock = m_newProfileInteractions[i] == ECollision_Interaction::Block;
							ImGui::Checkbox(chkNamebk.c_str(), &IsBlock);
							if (IsBlock)
								m_newProfileInteractions[i] = ECollision_Interaction::Block;

							ImGui::EndTable();
						}
					}
					ImGui::EndTable();
				}



				if (ImGui::Button("Apply"))
				{

					colMgr->CreateProfile(m_newProfileName, m_SelectedChannelName, m_newProfileEnabled, m_newProfileInteractions, m_newProfileDesc);
					colMgr->Save();
					ImGui::CloseCurrentPopup();
				}
				float width = ImGui::GetContentRegionAvail().x;
				const ImVec2 label_size = ImGui::CalcTextSize("Cancel", NULL, true);
				ImGui::SameLine(width - label_size.x);

				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}



			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("ModProfilePop", NULL, 0))
			{
				if (ImGui::BeginTable("ModProfilePopSortTable", 2, ImGuiTableFlags_Resizable))
				{

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					//ImGui::AlignTextToFramePadding();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Name ");
					ImGui::TableSetColumnIndex(1);

					bool EngineDefaultProfile = colMgr->FindProfile(m_newProfileName)->Defaults;
					if (EngineDefaultProfile)
					{
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
					}
					ImGui::InputText("##ModProfieName", m_SelectedProfileName);

					if (EngineDefaultProfile)
					{
						ImGui::PopItemFlag();
						ImGui::PopStyleVar();
					}

					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("CollisionEnabled");
					ImGui::TableSetColumnIndex(1);

					ImGui::Checkbox("##CollisionEnabled", &m_newProfileEnabled);
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("ChannelType");
					ImGui::TableSetColumnIndex(1);
					if (ImGui::BeginCombo("##newProfileChannel", m_SelectedChannelName.c_str()))
					{

						for (size_t i = 0; i < AllChannelSize; i++)
						{
							bool seletedChannel = m_SelectedChannelName == allChannelNames[i];
							if (ImGui::Selectable(allChannelNames[i].c_str(), seletedChannel))
							{
								m_SelectedChannelName = allChannelNames[i];
							}
						}
						ImGui::EndCombo();
					}
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Description");
					ImGui::TableSetColumnIndex(1);
					ImGui::InputText("##NewProfieDesc", m_newProfileDesc);
					//Show Collision Respose Tables.
					ImGui::Separator();

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("CollisionResponse");
					ImGui::TableSetColumnIndex(1);

					if (ImGui::BeginTable("InterationTable", 3, 0))
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						//ImGui::AlignTextToFramePadding();
						ImGui::Text("Ignore");
						ImGui::TableSetColumnIndex(1);
						ImGui::Text("Overlap");
						ImGui::TableSetColumnIndex(2);
						ImGui::Text("Block");
						ImGui::EndTable();
					}


					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					//ImGui::AlignTextToFramePadding();
					ImGui::Text("All");
					ImGui::TableSetColumnIndex(1);


					if (ImGui::BeginTable("InterationTable", 3, 0))
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						//ImGui::AlignTextToFramePadding();
						std::string chkNameCol = "##_col";
						std::string chkNameIg = "##_ig";
						std::string chkNamebk = "##_bk";

						bool IsIgnore = true;
						size_t interactionSize = m_newProfileInteractions.size();
						int32 collSize = 0;
						int32 blockSize = 0;

						for (size_t i = 0; i < interactionSize; i++)
						{
							if (m_newProfileInteractions[i] == ECollision_Interaction::Ignore)
							{
								IsIgnore = false;
								collSize++;
							}
							else if (m_newProfileInteractions[i] == ECollision_Interaction::Block)
							{
								IsIgnore = false;
								blockSize++;
							}
							

						}

						if (ImGui::Checkbox(chkNameCol.c_str(), &IsIgnore))
						{
							for (size_t i = 0; i < interactionSize; i++)
							{
								m_newProfileInteractions[i] = ECollision_Interaction::Ignore;
							}

						}
						ImGui::TableSetColumnIndex(1);

						bool IsCollision = collSize == interactionSize;

						if (ImGui::Checkbox(chkNameIg.c_str(), &IsCollision))
						{
							for (size_t i = 0; i < interactionSize; i++)
							{
								m_newProfileInteractions[i] = ECollision_Interaction::Overlap;
							}
						}

						bool IsBlock = blockSize == interactionSize;

						if (ImGui::Checkbox(chkNamebk.c_str(), &IsBlock))
						{
							for (size_t i = 0; i < interactionSize; i++)
							{
								m_newProfileInteractions[i] = ECollision_Interaction::Block;
							}
						}

						ImGui::EndTable();
					}


					for (size_t i = 0; i < AllChannelSize; i++)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						//ImGui::AlignTextToFramePadding();
						ImGui::Text(allChannelNames[i].c_str());
						ImGui::TableSetColumnIndex(1);
						if (ImGui::BeginTable("InterationTable", 3, 0))
						{
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							//ImGui::AlignTextToFramePadding();
							std::string chkNameCol = "##_col" + allChannelNames[i];
							std::string chkNameIg = "##_ig" + allChannelNames[i];
							std::string chkNamebk = "##_bk" + allChannelNames[i];

							bool IsIgnore = m_newProfileInteractions[i] == ECollision_Interaction::Ignore;

							ImGui::Checkbox(chkNameCol.c_str(), &IsIgnore);
							if (IsIgnore)
								m_newProfileInteractions[i] = ECollision_Interaction::Ignore;


							ImGui::TableSetColumnIndex(1);
							bool IsCollision = m_newProfileInteractions[i] == ECollision_Interaction::Overlap;
							ImGui::Checkbox(chkNameIg.c_str(), &IsCollision);
							if (IsCollision)
								m_newProfileInteractions[i] = ECollision_Interaction::Overlap;

							ImGui::TableSetColumnIndex(2);
							bool IsBlock = m_newProfileInteractions[i] == ECollision_Interaction::Block;
							ImGui::Checkbox(chkNamebk.c_str(), &IsBlock);
							if (IsBlock)
								m_newProfileInteractions[i] = ECollision_Interaction::Block;


							ImGui::EndTable();
						}


					}
					ImGui::EndTable();
				}

				if (ImGui::Button("Apply"))
				{
					// m_newProfileName< 원래이름
					// m_SelectedProfileName  <<바꿀이름
					colMgr->ModifyProfile(m_newProfileName, m_SelectedProfileName, m_SelectedChannelName, m_newProfileEnabled, m_newProfileInteractions, m_newProfileDesc);
					colMgr->Save();
					ImGui::CloseCurrentPopup();
				}
				float width = ImGui::GetContentRegionAvail().x;
				const ImVec2 label_size = ImGui::CalcTextSize("Cancel", NULL, true);
				ImGui::SameLine(width - label_size.x);

				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
			ImGui::EndListBox();
		}

		ImGui::TreePop();
	}

	//ImGui::PopStyleVar();
	
}
