#pragma once
#include "EditorWidget.h"
#include "imgui.h"



class EditorWindow :public EditorWidget
{
	CLASS(EditorWindow,EditorWidget)

protected:
	EditorWindow();
	virtual ~EditorWindow();

protected:
	bool m_Open;
	ImGuiWindowFlags m_WindowFlags;




	virtual void Update(float DeltaSeconds) override;

	bool BeginWindow();
	void EndWindow();

	virtual void Build() override;


public:
	void SetOpen(bool Open) { m_Open = Open; }
	bool IsOpened() const { return m_Open; }
	bool* IsOpenedPtr() { return &m_Open; }


public:
	void AddFlag(ImGuiWindowFlags InFlag);
	void RemoveFlag(ImGuiWindowFlags InFlag);


};

