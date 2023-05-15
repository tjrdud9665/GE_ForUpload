#pragma once
#include "EditorWindow.h"

/* Delete Socket  , ~~~ */


class MeshEditor :public EditorWindow
{
	CLASS(MeshEditor,EditorWindow)

public:
	MeshEditor();
	MeshEditor(const MeshEditor& Other);
	virtual ~MeshEditor();

	virtual void Build() override;

	virtual void Update(float DeltaSeconds) override;

	virtual void Construct(const std::string& InName = "") override;

	void SetTargetMesh(class MeshAsset* InTargetMesh);

	void Render();

	void PreRender();
	

private:
	void ShowMeshDetails();

	void ShowBoneViews();	

	void ShowMaterialViews();

	void ShowSocketProperties(float DeltaSeconds);

	void TreeBone(int32 parentIdx,class Skeleton* InSkeleton);

	void BoneViewHandleInput();

	void ComposePreview();

	void UpdateEditSub(float DeltaSeconds);

	void PostUpdateEditSub();

	void ShowMenu();

	void TargetViewHandleInput();


	class SkeletalMeshAsset* GetSkeletalMesh();


private:
	//Skeletal or Static
	class MeshAsset* m_MeshAsset;

	class TargetViewer* m_Viewer;
	class GuizmoWidget* m_SocketGuizmo;

	class Renderer* m_cachedRenderer;

	

	class CameraManager* m_ImitateCameraManager;
	class RenderManager* m_ImitateRenderManager;

	int32				m_HoveredBoneIndex;
	int32				m_SelectedBoneIndex;

	class SkeletalMeshSocket* m_HoveredSocket;
	class SkeletalMeshSocket* m_SelectedSocket;



	TSharedPtr<class PrimitiveComponent> m_PreviewComponent;
	TSharedPtr<class SceneComponent>	m_SocketPreview;
	TSharedPtr<class SkyComponent>		m_PreviewSky;

	TSharedPtr<class DirectionalLightComponent>	m_PreviewDirectionalLight;


	bool			m_ShowAddSocketPopUp;
	std::string		m_NewSocketName;

	class PropertySet* m_SocketPropertySet;		

	float			m_listStart;

	float			m_LeftDetailTabRatio;

};

