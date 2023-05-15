#include "PCH.h"
#include "CollisionPropertyWidget.h"
#include <CollisionManager.h>
#include "imgui.h"
#include "imgui_internal.h"
#include <GameFramework/Components/PrimitiveComponent.h>

CollisionPropertyWidget::CollisionPropertyWidget()
{

}

CollisionPropertyWidget::~CollisionPropertyWidget()
{

}

void CollisionPropertyWidget::Update(float DeltaSeconds)
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

	SCollisionProfile** profile = GetHandle()->GetValuePtr();

	std::vector<std::string> CustomChannelNames;
	size_t customChanneldSize = g_Engine->Get<CollisionManager>()->GetAllCustomChannelName(OUT CustomChannelNames);

	std::vector<std::string> AllChannelNames;
	size_t allChannelSize = g_Engine->Get<CollisionManager>()->GetAllChannelName(OUT AllChannelNames);


	bool IsCustomProfile = (*profile)->Name == "Custom";

	if (ImGui::TreeNodeEx("Collision", ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected))
	{
		if (ImGui::BeginTable("CollisionTable", 2, ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			bool treeOpen = false;
			if (treeOpen = ImGui::TreeNodeEx("Collision Preset", ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding))
				ImGui::TreePop();


			ImGui::TableSetColumnIndex(1);
			ShowCollisionProfilesCombo(profile);
			if (!IsCustomProfile)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			if (treeOpen)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Collision Enabled");
				ImGui::TableSetColumnIndex(1);
				std::string checkName = "##" + GetName() + "Enabled";

				ImGui::Checkbox(checkName.c_str(), &(*profile)->Enable);


				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Object Type");
				ImGui::TableSetColumnIndex(1);
				ShowCollisionChannelCombo(profile);

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Response");
				ImGui::TableSetColumnIndex(1);
				if (ImGui::BeginTable("InterationTable", 3, ImGuiTableFlags_Hideable))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::AlignTextToFramePadding();
					ImGui::Text("Ignore");
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("Overlap");
					ImGui::EndTable();
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("All");
				ImGui::TableSetColumnIndex(1);
				if (ImGui::BeginTable("InterationTable", 3, ImGuiTableFlags_Hideable))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::AlignTextToFramePadding();

					ShowCollisionChannelIgnoreCheck(profile, INDEX_NONE);

					ImGui::TableSetColumnIndex(1);

					ShowCollisionChannelOverlapCheck(profile, INDEX_NONE);

					ImGui::EndTable();
				}

				for (size_t i = 0; i < allChannelSize; i++)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::AlignTextToFramePadding();
					ImGui::Text(AllChannelNames[i].c_str());
					ImGui::TableSetColumnIndex(1);

					if (ImGui::BeginTable("InterationTable", 3, ImGuiTableFlags_Hideable))
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);						
						ShowCollisionChannelIgnoreCheck(profile, (int32)i);
						ImGui::TableSetColumnIndex(1);
						ShowCollisionChannelOverlapCheck(profile, (int32)i);
						ImGui::TableSetColumnIndex(2);
						ShowCollisionChannelBlockCheck(profile, (int32)i);
						ImGui::EndTable();
					}


				}
			}
			if (!IsCustomProfile)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

			ImGui::EndTable();

		}

		ImGui::TreePop();
	}


	ImGui::PopStyleVar();
}

void CollisionPropertyWidget::ShowCollisionChannelCombo(SCollisionProfile** Profile)
{
	std::string name = "##" + GetName() + "Channels";
	SCollisionProfile** val = Profile;

	std::vector<std::string> channelNames;
	size_t channelSize = g_Engine->Get<CollisionManager>()->GetAllChannelName(OUT channelNames);


	if (ImGui::BeginCombo(name.c_str(), (*val)->Channel->Name.c_str()))
	{
		for (size_t i = 0; i < channelSize; i++)
		{
			bool is_selected = (channelNames[i] == (*val)->Channel->Name);
			if (ImGui::Selectable(channelNames[i].c_str(), is_selected))
			{
				(*val)->Channel = g_Engine->Get<CollisionManager>()->GetCollisionChannel(channelNames[i]);

			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

void CollisionPropertyWidget::ShowCollisionProfilesCombo(SCollisionProfile** Profile)
{
	std::string name = "##" + GetName();
	SCollisionProfile** val = Profile;

	std::vector<std::string> profileNames;
	size_t profileSize = g_Engine->Get<CollisionManager>()->GetAllProfileName(OUT profileNames);


	if (ImGui::BeginCombo(name.c_str(), (*val)->Name.c_str()))
	{

		for (size_t i = 0; i < profileSize; i++)
		{
			bool is_selected = (profileNames[i] == (*val)->Name);
			if (ImGui::Selectable(profileNames[i].c_str(), is_selected))
			{
				*val = g_Engine->Get<CollisionManager>()->FindProfile(profileNames[i]);
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		bool IsCustom = (*val)->Name == "Custom";

		if (ImGui::Selectable("Custom", IsCustom))
		{
			PrimitiveComponent* inst = (PrimitiveComponent*)m_Prop->GetInstance();
			if (inst && !IsCustom)
			{
				*val = g_Engine->Get<CollisionManager>()->CreateCustomProfile(inst);
			}
		}
		ImGui::EndCombo();
	}
}

void CollisionPropertyWidget::ShowCollisionChannelIgnoreCheck(SCollisionProfile** Profile, int32 idx)
{
	SCollisionProfile** val = Profile;

	std::string name = "##" + GetName() + to_string(idx) + "Ig";
	size_t interactionSize = (*val)->vecCollisionInteraction.size();

	if (idx != INDEX_NONE)
	{
		bool IsIgnore = (*val)->vecCollisionInteraction[idx] == ECollision_Interaction::Ignore;

		if (ImGui::Checkbox(name.c_str(), &IsIgnore))
		{
			(*val)->vecCollisionInteraction[idx] = ECollision_Interaction::Ignore;

		}
	} else
	{
		bool IsIgnore = true;

		for (size_t i = 0; i < interactionSize; i++)
		{
			if (((*val)->vecCollisionInteraction[i] != ECollision_Interaction::Ignore))
			{
				IsIgnore = false;
				break;
			}
		}

		if (ImGui::Checkbox(name.c_str(), &IsIgnore))
		{
			for (size_t i = 0; i < interactionSize; i++)
			{
				(*val)->vecCollisionInteraction[i] = ECollision_Interaction::Ignore;
			}
		}
	}
}

void CollisionPropertyWidget::ShowCollisionChannelOverlapCheck(SCollisionProfile** Profile, int32 idx)
{
	SCollisionProfile** val = Profile;

	size_t interactionSize = (*val)->vecCollisionInteraction.size();
	std::string name = "##" + GetName() + to_string(idx) + "Col";

	if (idx != INDEX_NONE)
	{
		bool IsCollision = (*val)->vecCollisionInteraction[idx] == ECollision_Interaction::Overlap;

		if (ImGui::Checkbox(name.c_str(), &IsCollision))
		{
			(*val)->vecCollisionInteraction[idx] = ECollision_Interaction::Overlap;

		}
	} else
	{

		bool IsCollision = true;

		for (size_t i = 0; i < interactionSize; i++)
		{
			if ((*val)->vecCollisionInteraction[i] != ECollision_Interaction::Overlap)
			{
				IsCollision = false;
				break;
			}
		}


		if (ImGui::Checkbox(name.c_str(), &IsCollision))
		{
			for (size_t i = 0; i < interactionSize; i++)
			{
				(*val)->vecCollisionInteraction[i] = ECollision_Interaction::Overlap;
			}

		}
	}

}

void CollisionPropertyWidget::ShowCollisionChannelBlockCheck(SCollisionProfile** Profile, int32 idx)
{
	SCollisionProfile** val = Profile;

	size_t interactionSize = (*val)->vecCollisionInteraction.size();
	std::string name = "##" + GetName() + to_string(idx) + "Blk";

	if (idx != INDEX_NONE)
	{
		bool IsBlock = (*val)->vecCollisionInteraction[idx] == ECollision_Interaction::Block;

		if (ImGui::Checkbox(name.c_str(), &IsBlock))
		{
			(*val)->vecCollisionInteraction[idx] = ECollision_Interaction::Block;

		}
	} else
	{

		bool IsCollision = true;

		for (size_t i = 0; i < interactionSize; i++)
		{
			if ((*val)->vecCollisionInteraction[i] != ECollision_Interaction::Block)
			{
				IsCollision = false;
				break;
			}
		}


		if (ImGui::Checkbox(name.c_str(), &IsCollision))
		{
			for (size_t i = 0; i < interactionSize; i++)
			{
				(*val)->vecCollisionInteraction[i] = ECollision_Interaction::Block;
			}

		}
	}

}
