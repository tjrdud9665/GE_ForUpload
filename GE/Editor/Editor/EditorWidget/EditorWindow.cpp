#include "PCH.h"
#include "EditorWindow.h"



EditorWindow::EditorWindow()
	:m_Open(false)
	,m_WindowFlags(0)
{

}

EditorWindow::~EditorWindow()
{

}

void EditorWindow::Update(float DeltaSeconds)
{
	if(m_Open)
	{
		BeginWindow();
		UpdateChilds(DeltaSeconds);
		EndWindow();	
	}
}

bool EditorWindow::BeginWindow()
{
	return ImGui::Begin(GetName().c_str(), &m_Open, m_WindowFlags);	
}

void EditorWindow::EndWindow()
{
	ImGui::End();

	//AfterEnd
}

void EditorWindow::Build()
{
	m_Open = true;
	m_WindowFlags = 0;
}

void EditorWindow::AddFlag(ImGuiWindowFlags InFlag)
{
	m_WindowFlags |= InFlag;
}

void EditorWindow::RemoveFlag(ImGuiWindowFlags InFlag)
{
	m_WindowFlags &= ~InFlag;
}
