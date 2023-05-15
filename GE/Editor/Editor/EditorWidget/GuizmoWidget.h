#pragma once
#include "EditorWidget.h"
#include "imgui.h"
#include "ImGuizmo.h"

enum class EAxisEditMode 
{
	Translate = ImGuizmo::OPERATION::TRANSLATE,
	Rotate = ImGuizmo::OPERATION::ROTATE,
	Scale = ImGuizmo::OPERATION::SCALE,
	Bounds
};

class GuizmoWidget :public EditorWidget
{
	CLASS(GuizmoWidget, EditorWidget)

protected:
	GuizmoWidget();
	virtual ~GuizmoWidget();

public:
	virtual void Update(float DeltaSeconds) override;

	void SetViewportSize(const SVector2& InVPSize);

	void SetWindowPos(const SVector2& InWindowPos);
	
	bool IsGuizmoEditing() const
	{
		return m_GuizmoEditing;
	}

	void SetGuizmoTarget(class SceneComponent* InGuizmoTarget);	

	class SceneComponent* GetGuizmoTarget() const;

	//Not Imple -> 에디터 쓰다가 불편하면 구현해서 쓰셈
	void AddGuizmoTarget(class SceneComponent* InGuizmoTarget);

	void SetSocketGuizmoTarget(class SkeletalMeshSocket* InMeshSocket);

private:
	void HandleAxisEditModeInput();

	
private:
	class SceneComponent* m_GuizmoTarget;		

	EAxisEditMode		m_AxisEditMode;

	ImVec2				m_VPSize;

	bool				m_GuizmoEditing;

	float				m_TargetDistance;

	ImVec2				m_WindowPos;

	class SkeletalMeshSocket* m_SocketTarget;

	//원하는 대상의 WorldMatrix의 포인터 형태로 수정하여 여러 에디터에서 호환 가능하도록 유도.

};

