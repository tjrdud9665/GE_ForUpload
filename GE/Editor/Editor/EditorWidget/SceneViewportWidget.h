#pragma once
#include "EditorWindow.h"
#include <Renderer/RenderCommands.h>

class SceneViewportWidget :public EditorWindow
{
	CLASS(SceneViewportWidget,EditorWindow)

protected:
	SceneViewportWidget();
	virtual ~SceneViewportWidget();

	virtual void Build() override;

	virtual void Update(float DeltaSeconds) override;

	void HandleInput();

	void MousePickComponent();

	void AdjustViewport();

public:
	void	SetGuizmoTarget(class SceneComponent* InGuizmoTarget);

	bool	IsGuizmoActivated() const;

	const SVector2& GetSize();


private:
	class TargetViewer* m_TargetViewer;

	class GuizmoWidget* m_Guizmo;

	class D3DManager* m_cachedD3DManager;

	bool m_EnablePick;

};

