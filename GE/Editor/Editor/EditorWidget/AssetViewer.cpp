#include "PCH.h"
#include "AssetViewer.h"
#include "FileSystem/FileSystem.h"
#include "AssetManager.h"
#include "Path/PathManager.h"
#include "Assets/TextureAsset/TextureAsset.h"
#include "Renderer/Renderer.h"
#include "imgui_internal.h"
#include "imgui_stl.h"
#include "../ContentEditor.h"
#include "Assets/Material/Material.h"
#include "GameFramework/GameObject/GameObject.h"
#include "Assets/Scene/Scene.h"
#include <Assets/SceneInfo/SceneInfo.h>
#include <Assets/GameplayAbility/GameplayAbility.h>
#include <Assets/GameplayEffect/GameplayEffect.h>
#include "Assets/AI/BehaviorTree.h"
#include <Assets/AI/BlackBoard.h>
#include <Assets/Widget/UserWidget.h>
#include <Assets/Particle/ParticleAsset.h>


AssetViewer::AssetViewer()
	:m_SelectedGameObjectParent(nullptr)
{

}

AssetViewer::AssetViewer(const AssetViewer& Other)
	:Super(Other)
	, m_SelectedGameObjectParent(nullptr)
{

}

AssetViewer::~AssetViewer()
{

}

void AssetViewer::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
	m_CahcedMgr = g_Engine->Get<AssetManager>();
	m_CachedRenderer = g_Engine->Get<Renderer>();


	const SPathInfo* ResRootPath = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);

	m_CurrentDirectory = std::filesystem::path(ResRootPath->aPath);
	m_CurrentDirectory._Remove_filename_and_separator();
	m_ResRootPath = m_CurrentDirectory;

	const SPathInfo* TexturePathInfo = g_Engine->Get<PathManager>()->FindPath(TEXTURE_PATH);

	m_TexturePath = std::filesystem::path(TexturePathInfo->aPath);
	m_TexturePath._Remove_filename_and_separator();


	const SPathInfo* editResPathInfo = g_Engine->Get<PathManager>()->FindPath(EDIT_RES_PATH);	
	m_DirectoryIcon = m_CachedRenderer->CreateTextureFromFile("DirectoryIcon.png",editResPathInfo->aPath);
	m_FileIcon = m_CachedRenderer->CreateTextureFromFile("FileIcon.png", editResPathInfo->aPath);

	//////////////////////////////////////////////////////////////////////////
	const SPathInfo* materialPathInfo = g_Engine->Get<PathManager>()->FindPath(MATERIAL_PATH);
	m_MaterialPath = std::filesystem::path(materialPathInfo->aPath);
	m_MaterialPath._Remove_filename_and_separator();

	const SPathInfo* gameObjectPathInfo = g_Engine->Get<PathManager>()->FindPath(GAMEOBJECT_PATH);
	m_GameObjectPath = std::filesystem::path(gameObjectPathInfo->aPath);
	m_GameObjectPath._Remove_filename_and_separator();

	const SPathInfo* sceneObjectPathInfo = g_Engine->Get<PathManager>()->FindPath(SCENE_PATH);
	m_ScenePath = std::filesystem::path(sceneObjectPathInfo->aPath);
	m_ScenePath._Remove_filename_and_separator();

	const SPathInfo* sceneInfoPathInfo = g_Engine->Get<PathManager>()->FindPath(SCENEINFO_PATH);
	m_SceneInfoPath = std::filesystem::path(sceneInfoPathInfo->aPath);
	m_SceneInfoPath._Remove_filename_and_separator();


	const SPathInfo* abilityPathInfo = g_Engine->Get<PathManager>()->FindPath(ABILITYSYSTEM_PATH);
	m_AbilityPath = std::filesystem::path(abilityPathInfo->aPath);
	m_AbilityPath._Remove_filename_and_separator();

	const SPathInfo* aIPathInfo = g_Engine->Get<PathManager>()->FindPath(AI_PATH);
	m_AIPath = std::filesystem::path(aIPathInfo->aPath);
	m_AIPath._Remove_filename_and_separator();

	const SPathInfo* widgetPathInfo = g_Engine->Get<PathManager>()->FindPath(WIDGET_PATH);
	m_WidgetPath = std::filesystem::path(widgetPathInfo->aPath);
	m_WidgetPath._Remove_filename_and_separator();


	const SPathInfo* particlePathInfo = g_Engine->Get<PathManager>()->FindPath(PARTICLE_PATH);
	m_ParticlePath = std::filesystem::path(particlePathInfo->aPath);
	m_ParticlePath._Remove_filename_and_separator();
	

}

void AssetViewer::Update(float DeltaSeconds)
{
	if (m_Open)
	{
		BeginWindow();
		
		ShowAssetViewer();

		//UpdateChilds(DeltaSeconds);

		EndWindow();
	}
}

void AssetViewer::ShowAssetViewer()
{
	if (m_CurrentDirectory != std::filesystem::path(m_ResRootPath))
	{
		if (ImGui::Button("<-"))
		{
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}
	}

	static float padding = 10.0f;
	static float thumbnailSize = 50.0f;
	float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;

	ImGui::Columns(columnCount, 0, false);

	if (ImGui::BeginPopupContextWindow())
	{
		ImGui::Text("ContentMenu");
		ImGui::Separator();
		bool DesireClosePop = false;

		if (m_CurrentDirectory != std::filesystem::path(m_ResRootPath))
		{
			if (ImGui::Selectable("NewFolder"))
			{			
				FileSystem::newDirectory(m_CurrentDirectory, "newFolder");

			}
		}

		if (ImGui::BeginMenu("Asset Menu"))
		{
			fs::path lexcialMaterialPath = m_MaterialPath.lexically_relative(m_CurrentDirectory);
			if (std::string::npos == lexcialMaterialPath.string().find("Material"))
			{
				if (ImGui::Selectable("Material"))
				{
					//Create Material
					fs::path newPath = FileSystem::GetNewFileDirectory_Indexed(m_CurrentDirectory, "NewMaterial");
					AssetData* newMaterial = m_CahcedMgr->LoadOrCreate(AssetInfo(newPath.string()), Material::StaticClass());
					newMaterial->Save();
					
				}
			}

			fs::path lexcialScenePath = m_ScenePath.lexically_relative(m_CurrentDirectory);
			if (std::string::npos == lexcialScenePath.string().find("Scene"))
			{
				if (ImGui::Selectable("New Scene"))
				{
					//Create Material
					fs::path newPath = FileSystem::GetNewFileDirectory_Indexed(m_CurrentDirectory, "NewScene");
					AssetData* newScene = m_CahcedMgr->LoadOrCreate(AssetInfo(newPath.string()), Scene::StaticClass());
					newScene->Save();

				}
			}

			fs::path lexcialParticlePath = m_ParticlePath.lexically_relative(m_CurrentDirectory);
			if (std::string::npos == lexcialParticlePath.string().find("Particle"))
			{
				if (ImGui::Selectable("New Particle"))
				{
					//Create Material
					fs::path newPath = FileSystem::GetNewFileDirectory_Indexed(m_CurrentDirectory, "NewParticle");
					AssetData* newParticle = m_CahcedMgr->LoadOrCreate(AssetInfo(newPath.string()), ParticleAsset::StaticClass());
					newParticle->Save();

				}
			}

			fs::path lexcialSceneInfoPath = m_SceneInfoPath.lexically_relative(m_CurrentDirectory);
			if (std::string::npos == lexcialSceneInfoPath.string().find("SceneInfo"))
			{
				if (ImGui::Selectable("New SceneInfo",false ,ImGuiSelectableFlags_DontClosePopups))
				{
					ImGui::OpenPopup("SelectParentSceneInfo");
				}
			}

			fs::path lexcialWidgetPath = m_WidgetPath.lexically_relative(m_CurrentDirectory);
			if (std::string::npos == lexcialWidgetPath.string().find("Widget"))
			{
				if (ImGui::Selectable("New Widget", false, ImGuiSelectableFlags_DontClosePopups))
				{
					ImGui::OpenPopup("SelectParentUserWidget");
				}
			}

			fs::path lexcialAbilityPath = m_AbilityPath.lexically_relative(m_CurrentDirectory);
			if (std::string::npos == lexcialAbilityPath.string().find("GameplayAbility"))
			{
				if (ImGui::Selectable("GameplayAbility", false, ImGuiSelectableFlags_DontClosePopups))
				{
					ImGui::OpenPopup("SelectParentAbilityClass");
				}
				if (ImGui::Selectable("GameplayEffect"))
				{
					fs::path newGEPath = FileSystem::GetNewFileDirectory_Indexed(m_CurrentDirectory, "NewGameplayEffect");
					AssetData* newGameplayEffect = m_CahcedMgr->LoadOrCreate(AssetInfo(newGEPath.string()), GameplayEffect::StaticClass());
					newGameplayEffect->Save();
				}
			}
			
			fs::path lexcialAIPath = m_AIPath.lexically_relative(m_CurrentDirectory);
			if (std::string::npos == lexcialAIPath.string().find("AI"))
			{
				if (ImGui::Selectable("New BehaviorTree"))
				{				
					fs::path newPath = FileSystem::GetNewFileDirectory_Indexed(m_CurrentDirectory, "NewBTree");
					AssetData* newBTree = m_CahcedMgr->LoadOrCreate(AssetInfo(newPath.string()), BehaviorTree::StaticClass());
					newBTree->Save();
				}
				else if (ImGui::Selectable("New BlackBoard"))
				{
					fs::path newPath = FileSystem::GetNewFileDirectory_Indexed(m_CurrentDirectory, "NewBlackBoard");
					AssetData* newBB = m_CahcedMgr->LoadOrCreate(AssetInfo(newPath.string()), Blackboard::StaticClass());
					newBB->Save();
				}
			}

			fs::path lexcialGameObjectPath = m_GameObjectPath.lexically_relative(m_CurrentDirectory);
			if (std::string::npos == lexcialGameObjectPath.string().find("GameObject"))
			{
				if (ImGui::Selectable("GameObject", false, ImGuiSelectableFlags_DontClosePopups))
				{
					ImGui::OpenPopup("SelectParentGameObject");
				}
			}

			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal("SelectParentGameObject", NULL, 0))
			{
				FChildContainer goChildren;
				GameObject::StaticClass()->GetAllChildren(OUT goChildren);

				ImVec2 listSize = ImVec2(ImGui::GetContentRegionAvail().x, (goChildren.size() + 4) * ImGui::GetFontSize());
				if (ImGui::BeginListBox("##ParentClasses", listSize))
				{
					for (size_t i = 0; i < goChildren.size(); i++)
					{
						bool IsSelected = m_SelectedGameObjectParent == goChildren[i];
						if (ImGui::Selectable(goChildren[i]->GetName().c_str(), IsSelected))
						{
							m_SelectedGameObjectParent = goChildren[i];
						}

					}
					ImGui::EndListBox();
				}


				bool CreateDisable = m_SelectedGameObjectParent == nullptr;
				if (CreateDisable)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				if (ImGui::Button("Create"))
				{
					fs::path newGameObjectPath = FileSystem::GetNewFileDirectory_Indexed(m_CurrentDirectory, m_SelectedGameObjectParent->GetName());
					AssetData* newGameObject = m_CahcedMgr->LoadOrCreate(AssetInfo(newGameObjectPath.string()), m_SelectedGameObjectParent);
					newGameObject->Save();
					ImGui::CloseCurrentPopup();
					DesireClosePop = true;
				}
				if (CreateDisable)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::SameLine(0.0f);

				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
					DesireClosePop = true;
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginPopupModal("SelectParentUserWidget", NULL, 0))
			{
				FChildContainer widgetChildren;
				UserWidget::StaticClass()->GetAllChildren(OUT widgetChildren);
				widgetChildren.push_back(UserWidget::StaticClass());
				ImVec2 listSize = ImVec2(ImGui::GetContentRegionAvail().x, (widgetChildren.size() + 4) * ImGui::GetFontSize());
				if (ImGui::BeginListBox("##ParentClasses", listSize))
				{
					for (size_t i = 0; i < widgetChildren.size(); i++)
					{
						bool IsSelected = m_SelectedWidgetParent == widgetChildren[i];
						if (ImGui::Selectable(widgetChildren[i]->GetName().c_str(), IsSelected))
						{
							m_SelectedWidgetParent = widgetChildren[i];
						}

					}
					ImGui::EndListBox();
				}


				bool CreateDisable = m_SelectedWidgetParent == nullptr;
				if (CreateDisable)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				if (ImGui::Button("CreateUserWidget"))
				{
					std::string userWidgetName = "New" + m_SelectedWidgetParent->GetName();
					fs::path newUserwidgetPath = FileSystem::GetNewFileDirectory_Indexed(m_CurrentDirectory, userWidgetName);
					AssetData* newUserWidget = m_CahcedMgr->LoadOrCreate(AssetInfo(newUserwidgetPath.string()), m_SelectedWidgetParent);
					newUserWidget->Save();
					ImGui::CloseCurrentPopup();
					DesireClosePop = true;
				}
				if (CreateDisable)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::SameLine(0.0f);

				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
					DesireClosePop = true;
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginPopupModal("SeletectWidget", NULL, 0))
			{
				FChildContainer goChildren;
				GameObject::StaticClass()->GetAllChildren(OUT goChildren);

				ImVec2 listSize = ImVec2(ImGui::GetContentRegionAvail().x, (goChildren.size() + 4) * ImGui::GetFontSize());
				if (ImGui::BeginListBox("##ParentClasses", listSize))
				{
					for (size_t i = 0; i < goChildren.size(); i++)
					{
						bool IsSelected = m_SelectedGameObjectParent == goChildren[i];
						if (ImGui::Selectable(goChildren[i]->GetName().c_str(), IsSelected))
						{
							m_SelectedGameObjectParent = goChildren[i];
						}

					}
					ImGui::EndListBox();
				}


				bool CreateDisable = m_SelectedGameObjectParent == nullptr;
				if (CreateDisable)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				if (ImGui::Button("Create"))
				{
					fs::path newGameObjectPath = FileSystem::GetNewFileDirectory_Indexed(m_CurrentDirectory, m_SelectedGameObjectParent->GetName());
					AssetData* newGameObject = m_CahcedMgr->LoadOrCreate(AssetInfo(newGameObjectPath.string()), m_SelectedGameObjectParent);
					newGameObject->Save();
					ImGui::CloseCurrentPopup();
					DesireClosePop = true;
				}
				if (CreateDisable)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::SameLine(0.0f);

				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
					DesireClosePop = true;
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginPopupModal("SelectParentSceneInfo", NULL, 0))
			{
				FChildContainer sceneInfoChildren;
				SceneInfo::StaticClass()->GetAllChildren(OUT sceneInfoChildren);
				sceneInfoChildren.push_back(SceneInfo::StaticClass());		//Default SceneInfo
				ImVec2 listSize = ImVec2(ImGui::GetContentRegionAvail().x, (sceneInfoChildren.size() + 4) * ImGui::GetFontSize());
				if (ImGui::BeginListBox("##ParentClasses", listSize))
				{
					for (size_t i = 0; i < sceneInfoChildren.size(); i++)
					{
						bool IsSelected = m_SelectedSceneInfoParent == sceneInfoChildren[i];
						if (ImGui::Selectable(sceneInfoChildren[i]->GetName().c_str(), IsSelected))
						{
							m_SelectedSceneInfoParent = sceneInfoChildren[i];
						}

					}
					ImGui::EndListBox();
				}


				bool CreateDisable = m_SelectedSceneInfoParent == nullptr;
				if (CreateDisable)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				if (ImGui::Button("CreateSceneInfo"))
				{
					std::string sceneInfoName = "New" + m_SelectedSceneInfoParent->GetName();
					fs::path newSceneInfoPath = FileSystem::GetNewFileDirectory_Indexed(m_CurrentDirectory, sceneInfoName);
					AssetData* newSceneInfo = m_CahcedMgr->LoadOrCreate(AssetInfo(newSceneInfoPath.string()), m_SelectedSceneInfoParent);
					newSceneInfo->Save();
					ImGui::CloseCurrentPopup();
					DesireClosePop = true;
				}
				if (CreateDisable)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::SameLine(0.0f);

				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
					DesireClosePop = true;
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginPopupModal("SelectParentAbilityClass", NULL, 0))
			{
				FChildContainer gamplayAbilityChildren;
				GameplayAbility::StaticClass()->GetAllChildren(OUT gamplayAbilityChildren);	

				ImVec2 listSize = ImVec2(ImGui::GetContentRegionAvail().x, (gamplayAbilityChildren.size() + 4) * ImGui::GetFontSize());
				if (ImGui::BeginListBox("##ParentClasses", listSize))
				{
					for (size_t i = 0; i < gamplayAbilityChildren.size(); i++)
					{
						bool IsSelected = m_SelectedAbilityParent == gamplayAbilityChildren[i];
						if (ImGui::Selectable(gamplayAbilityChildren[i]->GetName().c_str(), IsSelected))
						{
							m_SelectedAbilityParent = gamplayAbilityChildren[i];
						}

					}
					ImGui::EndListBox();
				}


				bool CreateDisable = m_SelectedAbilityParent == nullptr;
				if (CreateDisable)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				if (ImGui::Button("Create"))
				{
					fs::path newAbilityPath = FileSystem::GetNewFileDirectory_Indexed(m_CurrentDirectory, m_SelectedAbilityParent->GetName());
					AssetData* newAbility = m_CahcedMgr->LoadOrCreate(AssetInfo(newAbilityPath.string()), m_SelectedAbilityParent);
					newAbility->Save();
					ImGui::CloseCurrentPopup();
					DesireClosePop = true;
				}
				if (CreateDisable)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::SameLine(0.0f);

				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
					DesireClosePop = true;
				}

				ImGui::EndPopup();
			}


			if (DesireClosePop)
				ImGui::CloseCurrentPopup();

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}


	for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
	{
		const auto& path = directoryEntry.path();
		//remove .meta extensions..
		std::string filenameString = path.filename().replace_extension("").string();

		if (!FileSystem::IsAssetFilePath(path) && !directoryEntry.is_directory())
			continue;;



		ImGui::PushID(filenameString.c_str());

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

		if (directoryEntry.is_directory())
		{			
			ImGui::ImageButton((ImTextureID)m_CachedRenderer->GetTextureObject(m_DirectoryIcon)->GetSRV(), { thumbnailSize, thumbnailSize });

			if (ImGui::BeginPopupContextItem())
			{
				ImGui::Text("Folder Menu");
				ImGui::Separator();
				bool EnginePath = g_Engine->Get<PathManager>()->IsEnginePath(directoryEntry.path());
				if (EnginePath)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				if (ImGui::BeginMenu("ReName Folder"))
				{
					if (!EnginePath)
					{
						ImGui::Text("Edit name:");
						std::string newFolderName;
						if (ImGui::InputText("##folderNameEdit", newFolderName, ImGuiInputTextFlags_EnterReturnsTrue))
						{
							fs::path renamePath = directoryEntry.path().parent_path();
							renamePath /= newFolderName;
							if (!FileSystem::FolderExists(renamePath))
							{
								fs::rename(directoryEntry.path(), renamePath);
								ImGui::CloseCurrentPopup();
							}

						}
					}
					ImGui::EndMenu();
				}



				vector<fs::path> vecFiles;
				FileSystem::DirectoryFileCount(directoryEntry.path(), OUT vecFiles);
				if (ImGui::Selectable("Delete Folder", false, ImGuiSelectableFlags_DontClosePopups))
				{
					if (vecFiles.size())
					{
						ImGui::OpenPopup("FolderDelete");
					} else
					{
						fs::remove(directoryEntry.path());
					}

				}

				ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				if (ImGui::BeginPopupModal("FolderDelete", NULL, 0))
				{
					ImGui::Text("Delete All File In this Path .\n\n");
					ImGui::Separator();


					if (ImGui::BeginListBox("##deleteFiles"))
					{
						for (size_t i = 0; i < vecFiles.size(); i++)
						{
							ImGui::Text(vecFiles[i].filename().string().c_str());
						}
						ImGui::EndListBox();
					}

					if (ImGui::Button("OK", ImVec2(120, 0)))
					{
						for (size_t i = 0; i < vecFiles.size(); i++)
						{
							m_CahcedMgr->Remove(vecFiles[i]);
						}

						fs::remove_all(directoryEntry.path());
						ImGui::CloseCurrentPopup();
					}
					ImGui::SetItemDefaultFocus();
					ImGui::SameLine();
					if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }

					ImGui::EndPopup();
				}
				if (EnginePath)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::EndPopup();

			}


		} else if (directoryEntry.path().extension() == ".meta")
		{
			//* Func
			const filesystem::path& filePath = directoryEntry.path();
			fs::path metaExtRemove = filePath;
			metaExtRemove.replace_extension("");
			std::string textureExt = metaExtRemove.extension().string();

			ImTextureID fileIcon = m_CachedRenderer->GetTextureObject(m_FileIcon)->GetSRV();				
			ImVec2 Uv0 = ImVec2(0.0f, 0.0f);
			ImVec2 Uv1 = ImVec2(1.0f, 1.0f);

			AssetData* currentAsset = m_CahcedMgr->GetLoadedAsset(AssetInfo(filePath.string(), false));

			if (currentAsset)
			{
				if (currentAsset->GetClass()->IsA(TextureAsset::StaticClass()))
				{
					TextureAsset* loadedTex = (TextureAsset*)m_CahcedMgr->GetLoadedAsset(AssetInfo(filePath.string(), false));
					if (loadedTex)
					{						
						ID3D11ShaderResourceView* srv = m_CachedRenderer->GetTextureObject(loadedTex->GetTextureID())->GetSRV();
						D3D11_SHADER_RESOURCE_VIEW_DESC desc;
						srv->GetDesc(&desc);										
						if(desc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D)
						{
							fileIcon = srv;
						}
					}
				} 
			}
			
			ImGui::ImageButton(fileIcon, { thumbnailSize, thumbnailSize }, Uv0, Uv1);			


			if (ImGui::BeginPopupContextItem())
			{
				ImGui::Text("File Menu");
				ImGui::Separator();

				bool RawFile = FileSystem::FileExists(FileSystem::FindRawFile(directoryEntry.path()));

				if (RawFile)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}
				if (ImGui::BeginMenu("Change FileName"))
				{
					ImGui::Text("Edit name:");
					std::string newFileName;
					if (ImGui::InputText("##folderNameEdit", newFileName, ImGuiInputTextFlags_EnterReturnsTrue))
					{
						fs::path renamePath = directoryEntry.path().parent_path();
						renamePath /= newFileName;
						renamePath.replace_extension(".meta");
						if (!FileSystem::FileExists(renamePath))
						{
							m_CahcedMgr->ReName(directoryEntry.path(), newFileName);
						}

					}

					ImGui::EndMenu();
				}

				if (ImGui::Selectable("Copy"))
				{
					fs::path CopyTarget = directoryEntry.path();

					std::string copyAssetNameTry = CopyTarget.replace_extension("").string();
					fs::path copyAssetPath = FileSystem::GetNewFileDirectory_Indexed(m_CurrentDirectory, copyAssetNameTry.c_str());

					std::string FileName = copyAssetPath.filename().replace_extension("").string();
					m_CahcedMgr->Copy(directoryEntry.path(), FileName);

				}

				if (ImGui::Selectable("Delete"))
				{
					//Delete TargetFile..
					if (FileSystem::FileExists(directoryEntry.path()))
					{
						m_CahcedMgr->Remove(directoryEntry.path());
						fs::remove(directoryEntry.path());
					}

				}

				if (RawFile)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}
				ImGui::EndPopup();
			}


		}


		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (directoryEntry.is_directory())
			{
				m_CurrentDirectory /= path.filename();
			} 
			else if (directoryEntry.path().extension() == ".meta")
			{
				//TypeSeletion from RawJsonFile
				std::string sPath = directoryEntry.path().string();
				Json LoadResult;
				bool AlreadyLoaded = false;
				AssetData* loaded = m_CahcedMgr->GetLoadedAsset(AssetInfo(sPath));
				if (loaded)
				{
					if (loaded->IsLoaded())
					{
						AlreadyLoaded = true;
					} else
					{
						AlreadyLoaded = FileSystem::LoadJsonFile(directoryEntry.path(), OUT LoadResult);
					}
				}
				else
				{
					loaded = m_CahcedMgr->Load(AssetInfo(sPath));
					//LoadResult = loaded->GetData();
					AlreadyLoaded = true;
				}

				if (AlreadyLoaded)
				{
					XClass* FoundAssetType = loaded->GetClass();
					if (FoundAssetType)
					{
						//Creat From Json
						AssetData* ClickedAsset = loaded;
						GetOwner<ContentEditor>()->OnClickedAsset(ClickedAsset);						
					}
				}


			}

		}

		ImGui::TextWrapped(filenameString.c_str());

		ImGui::NextColumn();

		ImGui::PopID();
	}

	ImGui::Columns(1);

	ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
	ImGui::SliderFloat("Padding", &padding, 0, 32);
}
