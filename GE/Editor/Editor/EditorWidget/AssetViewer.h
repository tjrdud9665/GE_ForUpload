#pragma once
#include "EditorWindow.h"

/*
*	AssetViewer 에	Child Widget 붙이지 마시오.
*/


class AssetViewer :public EditorWindow
{
	CLASS(AssetViewer, EditorWindow)

protected:
	AssetViewer();
	AssetViewer(const AssetViewer& Other);
	virtual ~AssetViewer();

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void Update(float DeltaSeconds) override;

private:
	void ShowAssetViewer();


private:
	filesystem::path		m_CurrentDirectory;
	filesystem::path		m_ResRootPath;

	//CustomAsset Paths.
	filesystem::path		m_ScenePath;
	filesystem::path		m_MaterialPath;
	filesystem::path		m_TexturePath;
	filesystem::path		m_MeshPath;
	filesystem::path		m_GameObjectPath;

	filesystem::path		m_AbilityPath;

	filesystem::path		m_SceneInfoPath;

	filesystem::path		m_AIPath;

	filesystem::path		m_WidgetPath;

	filesystem::path		m_ParticlePath;


	bool					m_Dirty;

	XClass* m_SelectedGameObjectParent;

	XClass* m_SelectedSceneInfoParent;

	XClass* m_SelectedWidgetParent;

	XClass* m_SelectedAbilityParent;

	class AssetManager* m_CahcedMgr;
	class Renderer* m_CachedRenderer;

	FTextureID			m_DirectoryIcon;
	FTextureID			m_FileIcon;

};

