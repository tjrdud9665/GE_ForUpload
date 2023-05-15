#include "PCH.h"
#include "BlackboardEditor.h"
#include "imgui.h"
#include "imgui_stl.h"

BlackboardEditor::BlackboardEditor()
	:m_EditingBlackBoard(nullptr)
	, m_NewBBKeyType(EBlackBoardValType::Bool)
	, m_SelectedKeyType(EBlackBoardValType::Bool)
{

}

BlackboardEditor::~BlackboardEditor()
{

}

void BlackboardEditor::Build()
{
	Super::Build();

	AddFlag(ImGuiWindowFlags_MenuBar);
	AddFlag(ImGuiWindowFlags_NoDocking);
	AddFlag(ImGuiWindowFlags_NoCollapse);
}

void BlackboardEditor::SetEditBlackBoard(class Blackboard* InBB)
{
	m_EditingBlackBoard = InBB;
}

void BlackboardEditor::Update(float DeltaSeconds)
{
	if (m_Open)
	{
		BeginWindow();
		ShowMenu();

		UpdateBlackboardEditing();
		EndWindow();
	}
}

void BlackboardEditor::UpdateBlackboardEditing()
{
	if (m_EditingBlackBoard)
	{
		//Select KEy Type
		const char* Preview = "";
		if (m_NewBBKeyType == EBlackBoardValType::Bool)
			Preview = "Bool";
		else if (m_NewBBKeyType == EBlackBoardValType::Float)
			Preview = "Float";
		else if (m_NewBBKeyType == EBlackBoardValType::Int)
			Preview = "Int";
		else if (m_NewBBKeyType == EBlackBoardValType::Object)
			Preview = "Object";
		else if (m_NewBBKeyType == EBlackBoardValType::Vector)
			Preview = "Vector";
		else if (m_NewBBKeyType == EBlackBoardValType::Ability)
			Preview = "Ability";



		if (ImGui::BeginCombo("##BBValType", Preview))
		{
			bool IsSelected = (m_NewBBKeyType == EBlackBoardValType::Bool);
			if (ImGui::Selectable("Bool", IsSelected))
			{
				m_NewBBKeyType = EBlackBoardValType::Bool;
			}
			IsSelected = (m_NewBBKeyType == EBlackBoardValType::Float);
			if (ImGui::Selectable("Float", IsSelected))
			{
				m_NewBBKeyType = EBlackBoardValType::Float;
			}
			IsSelected = (m_NewBBKeyType == EBlackBoardValType::Int);
			if (ImGui::Selectable("Int", IsSelected))
			{
				m_NewBBKeyType = EBlackBoardValType::Int;
			}
			IsSelected = (m_NewBBKeyType == EBlackBoardValType::Vector);
			if (ImGui::Selectable("Vector", IsSelected))
			{
				m_NewBBKeyType = EBlackBoardValType::Vector;
			}
			IsSelected = (m_NewBBKeyType == EBlackBoardValType::Object);
			if (ImGui::Selectable("Object", IsSelected))
			{
				m_NewBBKeyType = EBlackBoardValType::Object;
			}
			IsSelected = (m_NewBBKeyType == EBlackBoardValType::Ability);
			if (ImGui::Selectable("Ability", IsSelected))
			{
				m_NewBBKeyType = EBlackBoardValType::Ability;
			}

			ImGui::EndCombo();
		}

		ImGui::InputText("##BlackBoardKeyName", m_newKeyName);

		ImGui::SameLine();

		if (ImGui::Button("Add Key"))
		{
			if (!m_newKeyName.empty())
			{
				m_EditingBlackBoard->AddKey(m_newKeyName, m_NewBBKeyType);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Key Delete"))
		{
			if (!m_selectedKeyName.empty())
			{
				m_EditingBlackBoard->RemoveKey(m_selectedKeyName, m_SelectedKeyType);
			}
		}

		//List Up All BlackBoard KEYs		
		if (ImGui::BeginListBox("##BBKeys", ImVec2(400, ImGui::GetContentRegionAvail().y)))
		{

			//반복..AllOfKeys.
			//Bool 부터 반복..
			std::unordered_map<std::string, bool> bools;
			std::unordered_map<std::string, float> floats;
			std::unordered_map<std::string, SVector3> vectors;
			std::unordered_map<std::string, int32> ints;
			std::unordered_map<std::string, TSharedPtr<XObject>> objects;
			std::unordered_map<std::string, TSubclassOf<GameplayAbility>> abilities;

			size_t boolSize = m_EditingBlackBoard->GetBoolValues(OUT bools);
			size_t floatSize = m_EditingBlackBoard->GetFloatValues(OUT floats);
			size_t vectorSize = m_EditingBlackBoard->GetVectorValues(OUT vectors);
			size_t intSize = m_EditingBlackBoard->GetIntValues(OUT ints);
			size_t objectSize = m_EditingBlackBoard->GetObjectValues(OUT objects);
			size_t abilitySize = m_EditingBlackBoard->GetAbilityValues(OUT abilities);


			if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
			{
				ImGui::TableSetupColumn("Type");
				ImGui::TableSetupColumn("Name");
				ImGui::TableHeadersRow();
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();

				Blackboard::FBoolKeyIter boolIter = bools.begin();
				Blackboard::FBoolKeyIter boolIterEnd = bools.end();

				for (; boolIter != boolIterEnd; boolIter++)
				{
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Bool");
					ImGui::TableSetColumnIndex(1);
					std::string selLabel = "(Bool)" + (*boolIter).first;
					bool IsSel = (m_selectedKeyName == (*boolIter).first);
					if (ImGui::Selectable(selLabel.c_str(), IsSel))
					{
						m_selectedKeyName = (*boolIter).first;
						m_SelectedKeyType = EBlackBoardValType::Bool;
					}
					//ImGui::Text((*boolIter).first.c_str());
					ImGui::TableNextRow();
				}

				Blackboard::FFloatKeyIter floatIter = floats.begin();
				Blackboard::FFloatKeyIter floatlIterEnd = floats.end();

				for (; floatIter != floatlIterEnd; floatIter++)
				{
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Float");
					ImGui::TableSetColumnIndex(1);
					std::string selLabel = "(Float)" + (*floatIter).first;
					bool IsSel = (m_selectedKeyName == (*floatIter).first);
					if (ImGui::Selectable(selLabel.c_str(), IsSel))
					{
						m_selectedKeyName = (*floatIter).first;
						m_SelectedKeyType = EBlackBoardValType::Float;
					}
					//ImGui::Text((*boolIter).first.c_str());
					ImGui::TableNextRow();
				}

				Blackboard::FIntKeyIter intIter = ints.begin();
				Blackboard::FIntKeyIter intIterEnd = ints.end();

				for (; intIter != intIterEnd; intIter++)
				{
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Int");
					ImGui::TableSetColumnIndex(1);
					std::string selLabel = "(Int)" + (*intIter).first;
					bool IsSel = (m_selectedKeyName == (*intIter).first);
					if (ImGui::Selectable(selLabel.c_str(), IsSel))
					{
						m_selectedKeyName = (*intIter).first;
						m_SelectedKeyType = EBlackBoardValType::Int;
					}
					//ImGui::Text((*boolIter).first.c_str());
					ImGui::TableNextRow();
				}

				Blackboard::FObjectKeyIter objIter = objects.begin();
				Blackboard::FObjectKeyIter objIterEnd = objects.end();

				for (; objIter != objIterEnd; objIter++)
				{
					ImGui::TableSetColumnIndex(0);
					ImGui::Text(u8"Object");
					ImGui::TableSetColumnIndex(1);
					std::string selLabel = "(Object)" + (*objIter).first;
					bool IsSel = (m_selectedKeyName == (*objIter).first);
					if (ImGui::Selectable(selLabel.c_str(), IsSel))
					{
						m_selectedKeyName = (*objIter).first;
						m_SelectedKeyType = EBlackBoardValType::Object;
					}
					//ImGui::Text((*boolIter).first.c_str());
					ImGui::TableNextRow();
				}

				Blackboard::FVectorKeyIter vectorIter = vectors.begin();
				Blackboard::FVectorKeyIter vectorIterEnd = vectors.end();

				for (; vectorIter != vectorIterEnd; vectorIter++)
				{
					ImGui::TableSetColumnIndex(0);
					ImGui::Text(u8"Vector");
					ImGui::TableSetColumnIndex(1);
					std::string selLabel = "(Vector)" + (*vectorIter).first;
					bool IsSel = (m_selectedKeyName == (*vectorIter).first);
					if (ImGui::Selectable(selLabel.c_str(), IsSel))
					{
						m_selectedKeyName = (*vectorIter).first;
						m_SelectedKeyType = EBlackBoardValType::Vector;
					}
					//ImGui::Text((*boolIter).first.c_str());
					ImGui::TableNextRow();
				}

				Blackboard::FAbilityKeyIter abilityIter = abilities.begin();
				Blackboard::FAbilityKeyIter abilityIterEnd = abilities.end();

				for (; abilityIter != abilityIterEnd; abilityIter++)
				{
					ImGui::TableSetColumnIndex(0);
					ImGui::Text(u8"Ability");
					ImGui::TableSetColumnIndex(1);
					std::string selLabel = "(Ability)" + (*abilityIter).first;
					bool IsSel = (m_selectedKeyName == (*abilityIter).first);
					if (ImGui::Selectable(selLabel.c_str(), IsSel))
					{
						m_selectedKeyName = (*abilityIter).first;
						m_SelectedKeyType = EBlackBoardValType::Ability;
					}
					//ImGui::Text((*boolIter).first.c_str());
					ImGui::TableNextRow();
				}



				ImGui::EndTable();
			}



			ImGui::EndListBox();
		}
	}
}

void BlackboardEditor::ShowMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem("Save"))
			{
				if (m_EditingBlackBoard)
				{
					m_EditingBlackBoard->Save();
				}
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}
