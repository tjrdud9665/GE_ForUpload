#include "PCH.h"
#include "InputEditorWidget.h"
#include "imgui.h"
#include "imgui_stl.h"
#include "imgui_internal.h"
#include "Input/Input.h"


InputEditorWidget::InputEditorWidget()
	:m_OpenCreateAction(false)
	, m_OpenAddActionKey(false)
	, m_Selectedkey(0)
	, m_OpenChangeActionKey(false)
	, m_cachedInput(nullptr)
	, m_OpenCreateAxis(false)
{

}

InputEditorWidget::~InputEditorWidget()
{

}

void InputEditorWidget::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);
	m_cachedInput = g_Engine->Get<Input>();

}

void InputEditorWidget::Update(float DeltaSeconds)
{
	ShowActionBinds();
	ShowAxisBinds();

}

void InputEditorWidget::ShowActionBinds()
{
	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;

	std::vector<std::string> bindingNames;
	size_t bindingSize = m_cachedInput->GetActionBindingNames(OUT bindingNames);

	if (ImGui::TreeNodeEx("ActionMapping", flag))
	{
		ImGui::InputText("##newActionName", m_newActionName);
		ImGui::SameLine();

		bool IsActionCreateDisable = (m_newActionName.empty()) || (m_cachedInput->m_ActionNameToEvent.find(m_newActionName) != m_cachedInput->m_ActionNameToEvent.end());

		if (IsActionCreateDisable)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		if (ImGui::Button("AddAction"))
		{
			if (!IsActionCreateDisable)
				m_OpenCreateAction = true;
		}

		if (m_OpenCreateAction)
		{
			if (ImGui::BeginPopupContextWindow("createActionMapping", ImGuiPopupFlags_AnyPopup))
			{
				ImGui::Text("Input Your Bind Key!!");

				ImGui::GetIO().KeysDown[VK_LBUTTON] = ImGui::GetIO().MouseDown[0];
				ImGui::GetIO().KeysDown[VK_RBUTTON] = ImGui::GetIO().MouseDown[1];
				ImGui::GetIO().KeysDown[VK_MBUTTON] = ImGui::GetIO().MouseDown[2];

				for (int32 i = 0; i < 256; i++)
				{

					if (ImGui::GetIO().KeysDown[i])
					{
						m_cachedInput->CreateActionKey(m_newActionName, (uchar8)i);
						m_OpenCreateAction = false;
						break;
					}
				}
				/// Mouse buttons: 0=left, 1=right, 2=middle 


				ImGui::EndPopup();
			}
		}



		if (IsActionCreateDisable)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}


		std::vector<std::string>::iterator iter = bindingNames.begin();
		std::vector<std::string>::iterator iterEnd = bindingNames.end();

		for (; iter != iterEnd; iter++)
		{
			if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
			{

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				std::string labelName = "##" + (*iter);
				std::string originName = (*iter);
				if (ImGui::InputText(labelName.c_str(), (*iter), ImGuiInputTextFlags_EnterReturnsTrue))
				{
					m_cachedInput->ReNameActionKey(originName, (*iter));
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					//Delete ActionBind					
					m_cachedInput->DeleteActionKey(originName);
				}

				ImGui::TableSetColumnIndex(1);
				//////////////////////////////////////////					
				if (ImGui::Button("Add Key"))
				{
					m_OpenAddActionKey = true;
					m_SelectedActionName = *iter;
				}
				if (m_OpenAddActionKey)
				{
					if (ImGui::BeginPopupContextWindow("AddAtionKey", ImGuiPopupFlags_AnyPopup))
					{
						ImGui::Text("Input Your Bind Key!!");

						ImGui::GetIO().KeysDown[VK_LBUTTON] = ImGui::GetIO().MouseDown[0];
						ImGui::GetIO().KeysDown[VK_RBUTTON] = ImGui::GetIO().MouseDown[1];
						ImGui::GetIO().KeysDown[VK_MBUTTON] = ImGui::GetIO().MouseDown[2];

						for (int32 i = 0; i < 256; i++)
						{

							if (ImGui::GetIO().KeysDown[i])
							{
								m_cachedInput->CreateActionKey(m_SelectedActionName, (uchar8)i);
								m_OpenAddActionKey = false;
								break;
							}
						}

						ImGui::EndPopup();
					}
				}

				ImGui::SameLine();
				if (ImGui::Button("Mod Key"))
				{
					m_OpenChangeActionKey = true;
				}
				if (m_OpenChangeActionKey && m_Selectedkey && m_SelectedActionName == (*iter))
				{
					if (ImGui::BeginPopupContextWindow("createActionMapping", ImGuiPopupFlags_AnyPopup))
					{
						ImGui::Text("Input Your Bind Key!!");

						ImGui::GetIO().KeysDown[VK_LBUTTON] = ImGui::GetIO().MouseDown[0];
						ImGui::GetIO().KeysDown[VK_RBUTTON] = ImGui::GetIO().MouseDown[1];
						ImGui::GetIO().KeysDown[VK_MBUTTON] = ImGui::GetIO().MouseDown[2];

						for (int32 i = 0; i < 256; i++)
						{
							if (ImGui::GetIO().KeysDown[i])
							{
								m_Selectedkey = m_cachedInput->ChangeActionKey(m_SelectedActionName, m_Selectedkey, (uchar8)i);
								m_OpenChangeActionKey = false;
								break;
							}
						}

						ImGui::EndPopup();

					}
				}


				ImGui::SameLine();

				if (ImGui::Button("Remove Key"))
				{
					m_cachedInput->DeleteActionKey(*iter, m_Selectedkey);

				}
				std::list<uchar8> boundKeys;
				size_t keySize = m_cachedInput->GetKeyFromActionName((*iter), OUT boundKeys);

				ImVec2 listSize = ImVec2(ImGui::GetFontSize() * 6, (keySize + 1) * ImGui::GetFontSize());
				std::string bindKeyList = "##" + (*iter);

				std::list<uchar8>::iterator keyIter = boundKeys.begin();
				std::list<uchar8>::iterator keyIterEnd = boundKeys.end();

				if (ImGui::BeginTable("ActionMapping", 4, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInner))
				{
					for (; keyIter != keyIterEnd; keyIter++)
					{
						bool IsSel = (m_Selectedkey == (*keyIter) && m_SelectedActionName == *iter);
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::AlignTextToFramePadding();
						std::string displayKeyName = m_cachedInput->ConvertKeyToString((*keyIter));
						if (ImGui::Selectable(displayKeyName.c_str(), IsSel))
						{
							m_SelectedActionName = (*iter);
							m_Selectedkey = (*keyIter);
						}
						ImGui::TableSetColumnIndex(1);//Shift
						ImGui::Text("Shift");
						ImGui::SameLine();
						bool shitf = m_cachedInput->GetKeyShift(*iter, *keyIter);
						std::string shiftLabel = "##" + (*iter) + "shift" + displayKeyName;
						if (ImGui::Checkbox(shiftLabel.c_str(), &shitf))
						{
							m_cachedInput->SetKeyShift(*iter, *keyIter, shitf);

						}

						ImGui::TableSetColumnIndex(2);//Ctrl

						ImGui::Text("Ctrl");
						ImGui::SameLine();
						bool ctrl = m_cachedInput->GetKeyCtrl(*iter, *keyIter);
						std::string ctrlLabel = "##" + (*iter) + "ctrl" + displayKeyName;
						if (ImGui::Checkbox(ctrlLabel.c_str(), &ctrl))
						{
							m_cachedInput->SetKeyCtrl(*iter, *keyIter, ctrl);

						}
						ImGui::TableSetColumnIndex(3);//Alt

						ImGui::Text("Alt");
						ImGui::SameLine();
						bool alt = m_cachedInput->GetKeyAlt(*iter, *keyIter);
						std::string altLabel = "##" + (*iter) + "alt" + displayKeyName;
						if (ImGui::Checkbox(altLabel.c_str(), &alt))
						{
							m_cachedInput->SetKeyAlt(*iter, *keyIter, alt);

						}

					}
					ImGui::EndTable();
				}

				//////////////////////////////////////////
				ImGui::EndTable();
			}
		}


		ImGui::TreePop();
	}
}

void InputEditorWidget::ShowAxisBinds()
{
	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;

	std::vector<std::string> bindingNames;
	size_t bindingSize = m_cachedInput->GetAxisBindingNames(OUT bindingNames);


	if (ImGui::TreeNodeEx("Axis Mapping", flag))
	{
		ImGui::InputText("##newActionName", m_newAxisName);
		ImGui::SameLine();

		bool IsAxisCreateDisable = (m_newAxisName.empty()) || (m_cachedInput->m_AxisEvents.find(m_newAxisName) != m_cachedInput->m_AxisEvents.end());

		if (IsAxisCreateDisable)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		if (ImGui::Button("Add Axis"))
		{
			if (!IsAxisCreateDisable)
				m_OpenCreateAxis = true;
		}

		if (m_OpenCreateAxis)
		{
			if (ImGui::BeginPopupContextWindow("createAxisMapping", ImGuiPopupFlags_AnyPopup))
			{
				ImGui::Text("Input Your Bind Key!!");

				ImGui::GetIO().KeysDown[VK_LBUTTON] = ImGui::GetIO().MouseDown[0];
				ImGui::GetIO().KeysDown[VK_RBUTTON] = ImGui::GetIO().MouseDown[1];
				ImGui::GetIO().KeysDown[VK_MBUTTON] = ImGui::GetIO().MouseDown[2];

				for (int32 i = 0; i < 256; i++)
				{
					if (ImGui::GetIO().KeysDown[i])
					{
						m_cachedInput->CreateAxisKey(m_newAxisName, (uchar8)i, 0.0f);
						m_OpenCreateAxis = false;
						break;
					}
				}

				ImGui::EndPopup();
			}
		}
		if (IsAxisCreateDisable)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		std::vector<std::string>::iterator iter = bindingNames.begin();
		std::vector<std::string>::iterator iterEnd = bindingNames.end();

		for (; iter != iterEnd; iter++)
		{
			if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				std::string labelName = "##" + (*iter);
				std::string originName = (*iter);
				if (ImGui::InputText(labelName.c_str(), (*iter), ImGuiInputTextFlags_EnterReturnsTrue))
				{
					m_cachedInput->ReNameAxisKey(originName, (*iter));
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					m_cachedInput->DeleteAxisBind(originName);
				}
				ImGui::TableSetColumnIndex(1);
				ImGui::TableSetColumnIndex(1);
				//////////////////////////////////////////					
				if (ImGui::Button("Add Key"))
				{
					m_OpenAddAxisKey = true;
					m_SelectedAxisName = *iter;
				}
				if (m_OpenAddAxisKey)
				{
					if (ImGui::BeginPopupContextWindow("AddAxisKey", ImGuiPopupFlags_AnyPopup))
					{
						ImGui::Text("Input Your Bind Key!!");

						ImGui::GetIO().KeysDown[VK_LBUTTON] = ImGui::GetIO().MouseDown[0];
						ImGui::GetIO().KeysDown[VK_RBUTTON] = ImGui::GetIO().MouseDown[1];
						ImGui::GetIO().KeysDown[VK_MBUTTON] = ImGui::GetIO().MouseDown[2];

						for (int32 i = 0; i < 256; i++)
						{
							if (ImGui::GetIO().KeysDown[i])
							{
								m_cachedInput->CreateAxisKey(m_SelectedAxisName, (uchar8)i, 0.0f);
								m_OpenAddAxisKey = false;
								break;
							}
						}

						ImGui::EndPopup();
					}
				}

				ImGui::SameLine();
				if (ImGui::Button("Mod Key"))
				{
					m_OpenChangeAxisKey = true;
				}
				if (m_OpenChangeAxisKey && m_SelectedAxisKey)
				{
					if (ImGui::BeginPopupContextWindow("ChangeAxisKey", ImGuiPopupFlags_AnyPopup))
					{
						ImGui::Text("Input Your Bind Key!!");
						ImGui::GetIO().KeysDown[VK_LBUTTON] = ImGui::GetIO().MouseDown[0];
						ImGui::GetIO().KeysDown[VK_RBUTTON] = ImGui::GetIO().MouseDown[1];
						ImGui::GetIO().KeysDown[VK_MBUTTON] = ImGui::GetIO().MouseDown[2];

						for (int32 i = 0; i < 256; i++)
						{
							if (ImGui::GetIO().KeysDown[i])
							{
								m_SelectedAxisKey = m_cachedInput->ChangeAxisKey(m_SelectedAxisName, m_SelectedAxisKey, (uchar8)i);
								m_OpenChangeAxisKey = false;
								break;
							}
						}

						ImGui::EndPopup();

					}
				}


				ImGui::SameLine();

				if (ImGui::Button("Remove Key"))
				{
					if (m_SelectedAxisKey)
						m_cachedInput->DeleteAxisKey(*iter, m_SelectedAxisKey);

				}
				if (ImGui::BeginTable("AxisMapping", 4, ImGuiTableFlags_BordersInner))
				{
					std::vector<uchar8>	axisKeys;
					size_t keySize = m_cachedInput->GetAxisKeys((*iter), OUT axisKeys);
					for (size_t i = 0; i < keySize; i++)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::AlignTextToFramePadding();

						if (ImGui::BeginTable("AxisValueMapping", 2, ImGuiTableFlags_BordersInner))
						{
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::AlignTextToFramePadding();

							bool IsSel = (m_SelectedAxisKey == axisKeys[i] && m_SelectedAxisName == (*iter));
							if (ImGui::Selectable(m_cachedInput->ConvertKeyToString(axisKeys[i]).c_str(), IsSel))
							{
								m_SelectedAxisName = (*iter);
								m_SelectedAxisKey = axisKeys[i];
							}

							std::string scaleLabel = "##" + (*iter) + "scale" + to_string(i);
							float v = m_cachedInput->GetAxisScale((*iter), axisKeys[i]);
							ImGui::TableSetColumnIndex(1);
							if (ImGui::InputFloat(scaleLabel.c_str(), &v))
							{
								m_cachedInput->SetAxisScale((*iter), axisKeys[i], v);
							}


							ImGui::EndTable();
						}



						ImGui::TableSetColumnIndex(1);
						ImGui::Text("Shift");
						ImGui::SameLine();
						bool shitf = m_cachedInput->GetAxisKeyShift(*iter, axisKeys[i]);
						std::string shiftLabel = "##" + (*iter) + "shift" + to_string(axisKeys[i]);
						if (ImGui::Checkbox(shiftLabel.c_str(), &shitf))
						{
							m_cachedInput->SetAxisKeyShift(*iter, axisKeys[i], shitf);

						}
						ImGui::TableSetColumnIndex(2);//Ctrl
						ImGui::Text("Ctrl");
						ImGui::SameLine();
						bool ctrl = m_cachedInput->GetAxisKeyCtrl(*iter, axisKeys[i]);
						std::string ctrlLabel = "##" + (*iter) + "ctrl" + to_string(axisKeys[i]);
						if (ImGui::Checkbox(ctrlLabel.c_str(), &ctrl))
						{
							m_cachedInput->SetAxisKeyCtrl(*iter, axisKeys[i], ctrl);

						}
						ImGui::TableSetColumnIndex(3);//Alt
						ImGui::Text("Alt");
						ImGui::SameLine();
						bool alt = m_cachedInput->GetAxisKeyAlt(*iter, axisKeys[i]);
						std::string altLabel = "##" + (*iter) + "alt" + to_string(axisKeys[i]);
						if (ImGui::Checkbox(altLabel.c_str(), &alt))
						{
							m_cachedInput->SetAxisKeyAlt(*iter, axisKeys[i], alt);

						}
					}


					ImGui::EndTable();
				}


				ImGui::EndTable();

			}
		}




		ImGui::TreePop();
	}
}
