#pragma once
#include "EditorWindow.h"
class GameObjectEditor :public EditorWindow
{
	CLASS(GameObjectEditor,EditorWindow)

public:
	GameObjectEditor();
	GameObjectEditor(const GameObjectEditor& Other);
	virtual ~GameObjectEditor();

	virtual void Build() override;

	virtual void Update(float DeltaSeconds) override;

	virtual void Construct(const std::string& InName = "") override;

	void Render();

	void PreRender();

	void SetTargetObject(class GameObject* InObject);

private:
	void ShowMenu();

	void ComposePreview();

	void UpdateEditSub(float DeltaSeconds);

	void ShowObjectDetail(float DeltaSeconds);

	void HandleInput();

private:
	FRenderTargetID		m_ObjectEditTarget;

	class GameObject*	m_EditObject;

	class TargetViewer* m_Viewer;

	class GuizmoWidget* m_ObjectEditGuizmo;

	class CameraManager* m_ImitateCameraManager;
	class RenderManager* m_ImitateRenderManager;

	

	class SceneDetailWidget* m_ObjectDetails;
	


	TSharedPtr<class DirectionalLightComponent>	m_PreviewDirectionalLight;
	TSharedPtr<class SkyComponent>	m_PreviewSky;
	

	float			m_viewportSizeRatio;
	class Renderer* m_cachedRenderer;

};

