#include "PCH.h"
#include "EditorManager.h"
#include "Renderer/Renderer.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "EngineWindow.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Engine.h"
#include "RenderManager.h"
#include "EditorWidget/BoolPropertyWidget.h"
#include "GameFramework/Components/SceneComponent.h"
#include "EditorWidget/TransformPropertyWidget.h"
#include "GameFramework/Components/SceneComponent.h"
#include "EditorWidget/SceneComponentWidget.h"
#include "ImGuizmo.h"

#include "Assets/TextureAsset/TextureAsset.h"
#include "Assets/Mesh/Mesh.h"
//#include "Assets/Mesh/StaticMesh.h"
#include "Assets/Animation/AnimationSequence.h"
#include "Assets/Mesh/MeshAsset.h"
#include "Assets/Mesh/SkeletalMeshAsset.h"
#include "Assets/Mesh/Skeleton.h"
#include "Assets/Material/Material.h"
#include "Assets/Sound/Sound.h"
#include "Assets/AI/BlackBoard.h"
#include "Assets/AI/BehaviorTree.h"
#include "Assets/Font/FontAsset.h"
#include "Assets/Particle/ParticleAsset.h"

#include "EditorWidget/TAssetPropertyWidget.h"
#include "EditorWidget/FloatPropertyWidget.h"
#include "EditorWidget/Vector2PropertyWidget.h"
#include "EditorWidget/Vector3PropertyWidget.h"
#include "EditorWidget/ColorPropertyWidget.h"
#include "EditorWidget/StringPropertyWidget.h"
#include <SceneManager.h>
#include "imgui_stl.h"
#include <AssetManager.h>
#include "EditorWidget/EnumPropertyWidget.h"
#include "EditorWidget/SubClassWidget.h"
#include "EditorWidget/SignedPropertyWidget.h"
#include "EditorWidget/CollisionPropertyWidget.h"
#include "EditorWidget/AnimNotifyInfoWidget.h"


#include "Includes.h"
#include <Assets/GameplayEffect/GameplayEffect.h>
#include <Assets/GameplayAbility/GameplayAbility.h>
#include "EditorWidget/AttributePropertyWidget.h"
#include "EditorWidget/AttributeModiferWidget.h"
#include "EditorWidget/BBKeySelectorWidget.h"
#include <Path/PathManager.h>
#include "EditorWidget/WidgetTransformWidget.h"
#include "EditorWidget/AnchorPropertyWidget.h"







//= EDITOR OPTIONS ========================================================================================
// Shapes
const float k_roundness = 2.0f;
// Font
const float k_font_size = 16.0f;
const float k_font_scale = 0.7f;
// Color
const ImVec4 k_color_text = ImVec4(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f, 1.0f);
const ImVec4 k_color_text_disabled = ImVec4(54.0f / 255.0f, 54.0f / 255.0f, 54.0f / 255.0f, 1.0f);
const ImVec4 k_color_dark_very = ImVec4(15.0f / 255.0f, 15.0f / 255.0f, 15.0f / 255.0f, 1.0f);
const ImVec4 k_color_dark = ImVec4(21.0f / 255.0f, 21.0f / 255.0f, 21.0f / 255.0f, 1.0f);
const ImVec4 k_color_mid = ImVec4(36.0f / 255.0f, 36.0f / 255.0f, 36.0f / 255.0f, 1.0f);
const ImVec4 k_color_light = ImVec4(47.0f / 255.0f, 47.0f / 255.0f, 47.0f / 255.0f, 1.0f);
const ImVec4 k_color_shadow = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
const ImVec4 k_color_interactive = ImVec4(56.0f / 255.0f, 56.0f / 255.0f, 56.0f / 255.0f, 1.0f);
const ImVec4 k_color_interactive_hovered = ImVec4(0.450f, 0.450f, 0.450f, 1.000f);
const ImVec4 k_color_check = ImVec4(26.0f / 255.0f, 140.0f / 255.0f, 192.0f / 255.0f, 1.0f);

const ImVec4 k_color_black = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

static void ImGui_ApplyStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.WindowBorderSize = 1.0f;
	style.FrameBorderSize = 1.0f;
	style.ScrollbarSize = 20.0f;
	style.FramePadding = ImVec2(5, 5);
	style.ItemSpacing = ImVec2(6, 5);
	style.WindowMenuButtonPosition = ImGuiDir_Right;
	style.WindowRounding = k_roundness;
	style.FrameRounding = k_roundness;
	style.PopupRounding = k_roundness;
	style.GrabRounding = k_roundness;
	style.ScrollbarRounding = k_roundness;
	style.Alpha = 1.0f;
}

static void ImGui_ApplyColors()
{
	// Use default dark style as a base
	ImGui::StyleColorsDark();
	ImVec4* colors = ImGui::GetStyle().Colors;

	// Colors
	colors[ImGuiCol_Text] = k_color_text;
	colors[ImGuiCol_TextDisabled] = k_color_text_disabled;
	colors[ImGuiCol_WindowBg] = k_color_mid;                   // Background of normal windows
	colors[ImGuiCol_ChildBg] = k_color_mid;                   // Background of child windows
	colors[ImGuiCol_PopupBg] = k_color_dark;                  // Background of popups, menus, tooltips windows
	colors[ImGuiCol_Border] = k_color_interactive;
	colors[ImGuiCol_BorderShadow] = k_color_shadow;
	colors[ImGuiCol_FrameBg] = k_color_dark_very;             // Background of checkbox, radio button, plot, slider, text input
	colors[ImGuiCol_FrameBgHovered] = k_color_black;
	colors[ImGuiCol_FrameBgActive] = k_color_dark_very;
	colors[ImGuiCol_TitleBg] = k_color_dark;
	colors[ImGuiCol_TitleBgActive] = k_color_dark;
	colors[ImGuiCol_TitleBgCollapsed] = k_color_light;
	colors[ImGuiCol_MenuBarBg] = k_color_dark;
	colors[ImGuiCol_ScrollbarBg] = k_color_mid;
	colors[ImGuiCol_ScrollbarGrab] = k_color_interactive;
	colors[ImGuiCol_ScrollbarGrabHovered] = k_color_interactive_hovered;
	colors[ImGuiCol_ScrollbarGrabActive] = k_color_dark_very;
	colors[ImGuiCol_CheckMark] = k_color_check;
	colors[ImGuiCol_SliderGrab] = k_color_interactive;
	colors[ImGuiCol_SliderGrabActive] = k_color_check;
	colors[ImGuiCol_Button] = k_color_interactive;
	colors[ImGuiCol_ButtonHovered] = k_color_interactive_hovered;
	colors[ImGuiCol_ButtonActive] = k_color_dark_very;
	colors[ImGuiCol_Header] = k_color_light;                 // Header colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
	colors[ImGuiCol_HeaderHovered] = k_color_interactive_hovered;
	colors[ImGuiCol_HeaderActive] = k_color_dark_very;

	colors[ImGuiCol_Separator] = k_color_black;

	colors[ImGuiCol_SeparatorHovered] = k_color_light;
	colors[ImGuiCol_SeparatorActive] = k_color_light;
	colors[ImGuiCol_ResizeGrip] = k_color_interactive;
	colors[ImGuiCol_ResizeGripHovered] = k_color_interactive_hovered;
	colors[ImGuiCol_ResizeGripActive] = k_color_dark_very;
	colors[ImGuiCol_Tab] = k_color_light;
	colors[ImGuiCol_TabHovered] = k_color_interactive_hovered;
	colors[ImGuiCol_TabActive] = k_color_dark_very;
	colors[ImGuiCol_TabUnfocused] = k_color_light;
	colors[ImGuiCol_TabUnfocusedActive] = k_color_light;                 // Might be called active, but it's active only because it's it's the only tab available, the user didn't really activate it
	colors[ImGuiCol_DockingPreview] = k_color_dark_very;             // Preview overlay color when about to docking something
	colors[ImGuiCol_DockingEmptyBg] = k_color_interactive;           // Background color for empty node (e.g. CentralNode with no window docked into it)
	colors[ImGuiCol_PlotLines] = k_color_interactive;
	colors[ImGuiCol_PlotLinesHovered] = k_color_interactive_hovered;
	colors[ImGuiCol_PlotHistogram] = k_color_interactive;
	colors[ImGuiCol_PlotHistogramHovered] = k_color_interactive_hovered;
	colors[ImGuiCol_TextSelectedBg] = k_color_dark;
	colors[ImGuiCol_DragDropTarget] = k_color_interactive_hovered;   // Color when hovering over target
	colors[ImGuiCol_NavHighlight] = k_color_dark;                  // Gamepad/keyboard: current highlighted item
	colors[ImGuiCol_NavWindowingHighlight] = k_color_dark;                  // Highlight window when using CTRL+TAB
	colors[ImGuiCol_NavWindowingDimBg] = k_color_dark;                  // Darken/colorize entire screen behind the CTRL+TAB window list, when active
	colors[ImGuiCol_ModalWindowDimBg] = k_color_dark;                  // Darken/colorize entire screen behind a modal window, when one is active
}


EditorManager::EditorManager()
	:m_EditorBegun(false)
	,m_ShowDemo(true)
{

}

EditorManager::~EditorManager()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void EditorManager::Initialize()
{
	ID3D11Device* dev = g_Engine->Get<Renderer>()->GetDeivce();
	ID3D11DeviceContext* context = g_Engine->Get<Renderer>()->GetDeviceContext();
	HWND handle = g_Engine->Get<EngineWindow>()->Handle();

	ImGui::CreateContext();

	// Configuration
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigWindowsResizeFromEdges = true;
	io.ConfigViewportsNoTaskBarIcon = true;
	io.IniFilename = "editor.ini";

	ImGui_ApplyStyle();
	ImGui_ApplyColors();

	ImGui_ImplWin32_Init(handle);
	ImGui_ImplDX11_Init(dev, context);

	PathManager* pathMgr = g_Engine->Get<PathManager>();
	const SPathInfo* fontPath = pathMgr->FindPath(EDIT_RES_PATH);
	

	io.Fonts->AddFontFromFileTTF((fontPath->aPath + "NotoSansKR-Regular.otf").c_str(), k_font_size,nullptr,io.Fonts->GetGlyphRangesKorean());


	
	RegisterPropertyDetails();	

	g_Engine->AddRenderCall<EditorManager>(this,std::bind(&EditorManager::PreRender,this), std::bind(&EditorManager::Render, this));
}


void EditorManager::Update(float DeltaSeconds)
{
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();	

	
	ImGui::NewFrame();

	UpdateMainNavBar();
	UpdateDocking();	

	size_t editorSize = m_Editors.size();
	for(size_t i =0;i<editorSize;i++)
	{
		m_Editors[i]->Update(DeltaSeconds);
	}
	
	ImGui::Render();	

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

}

void EditorManager::UpdateDocking()
{
	static bool opt_fullscreen_persistant = true;	
	static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;;
	bool opt_fullscreen = opt_fullscreen_persistant;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	/*if (opt_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;*/

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	bool b_dockspace_open = true;
	ImGui::Begin("EditorLayoutWindow", &b_dockspace_open, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(1);

	// Dock space
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		if (ImGui::DockBuilderGetNode(ImGui::GetID("EditorLayoutWindow")) == NULL)
		{
			ImGuiID dockspace_id = ImGui::GetID("EditorLayoutWindow");
			ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
			ImGui::DockBuilderAddNode(dockspace_id, 0); // Add empty node
			ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

			ImGuiID dock_main_id = dockspace_id;
			ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);
			const ImGuiID dock_right_down_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.6f, nullptr, &dock_right_id);
			ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);
			const ImGuiID dock_down_right_id = ImGui::DockBuilderSplitNode(dock_down_id, ImGuiDir_Right, 0.6f, nullptr, &dock_down_id);

			// Dock windows
			//ImGui::DockBuilderDockWindow("World", dock_right_id);
			//ImGui::DockBuilderDockWindow("Properties", dock_right_down_id);
			//ImGui::DockBuilderDockWindow("Console", dock_down_id);			
			ImGui::DockBuilderDockWindow("Sceneviewport", dock_right_id);
		}

		ImGuiID dockspace_id = ImGui::GetID("EditorLayout");	
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);

	}
	ImGui::End();
}

void EditorManager::UpdateMainNavBar()
{	
	if (ImGui::BeginMainMenuBar())
	{		
		if (ImGui::BeginMenu("Editors"))
		{
			size_t editorSize = m_Editors.size();
			for (size_t i = 0; i < editorSize; i++)
			{
				m_Editors[i]->ExpandViewsMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Save"))
		{			
			
			ImGui::BeginChild("child", ImVec2(128, 60), true);
			if(m_sceneName.empty())
				m_sceneName = g_Engine->Get<SceneManager>()->GetCurrentScene()->GetName();
			ImGui::InputText("SceneSave", m_sceneName);
			
			if (ImGui::Button("Save"))
			{
				Scene* currentScene = g_Engine->Get<SceneManager>()->GetCurrentScene();
				currentScene->SetName(m_sceneName);
				g_Engine->Get<AssetManager>()->ReName(currentScene->GetRawPath(), m_sceneName);
				g_Engine->Get<SceneManager>()->GetCurrentScene()->Save();
				
			}
			ImGui::EndChild();
			//g_Engine->Get<SceneManager>()->GetCurrentScene()->Save();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Play/Stop"))
		{
			Scene* currentScene = g_Engine->Get<SceneManager>()->GetCurrentScene();
			if (!currentScene->IsBegan())
			{
				if (ImGui::MenuItem("Play"))
				{
					JsonArchive ar(false);
					currentScene->Serialize(ar);
					currentScene->SetData(ar.GetData());
					currentScene->BeginPlay();
					m_PlayedScene = currentScene;
					m_PlayedSceneData = ar.GetData();
				}
			} 
			else
			{
				if (ImGui::MenuItem("Stop"))
				{
					Scene* currentScene = g_Engine->Get<SceneManager>()->GetCurrentScene();

					g_Engine->Get<SceneManager>()->CreateNextScene(m_PlayedScene, true);
					m_PlayedScene->SetData(m_PlayedSceneData);
					g_Engine->Get<SceneManager>()->SetAutoPlay(false);

				}
			}

			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu("Exit"))
		{
			if (ImGui::MenuItem("Exit")) 
			{
				g_Engine->ShutDown();
			}


			ImGui::EndMenu();
		}
		
		ImGui::EndMainMenuBar();
	}
}


void EditorManager::Render()
{
	size_t editorSize = m_Editors.size();
	for (size_t i = 0; i < editorSize; i++)
	{
		m_Editors[i]->Render();
	}
}

void EditorManager::PreRender()
{
	size_t editorSize = m_Editors.size();
	for (size_t i = 0; i < editorSize; i++)
	{
		m_Editors[i]->PreRender();
	}
}

void EditorManager::RegisterPropertyDetails()
{	
	Property::RegisterDetailWidget<bool>(BoolPropertyWidget::StaticClass());
	Property::RegisterDetailWidget<int32>(SignedPropertyWidget::StaticClass());
	Property::RegisterDetailWidget<float>(FloatPropertyWidget::StaticClass());
	Property::RegisterDetailWidget<SVector3>(Vector3PropertyWidget::StaticClass());
	Property::RegisterDetailWidget<SVector2>(Vector2PropertyWidget::StaticClass());
	Property::RegisterDetailWidget<SLinearColor>(ColorPropertyWidget::StaticClass());

	Property::RegisterDetailWidget<SCollisionProfile*>(CollisionPropertyWidget::StaticClass());

	Property::RegisterDetailWidget<std::string>(StringPropertyWidget::StaticClass());

	Property::RegisterDetailWidget<XEnum>(EnumPropertyWidget::StaticClass());
	Property::RegisterDetailWidget<XSubclassOf>(SubClassWidget::StaticClass());

	Property::RegisterDetailWidget<SceneComponent*>(TransformPropertyWidget::StaticClass());
	Property::RegisterDetailWidget<TSharedPtr<SceneComponent>>(SceneComponentWidget::StaticClass());

	//For Animation Notify Prop

	Property::RegisterDetailWidget<AnimNotifyInfo*>(AnimNotifyInfoWidget::StaticClass());


	//GameplayEffects
	Property::RegisterDetailWidget<GameplayEffect*>(TAssetPropertyWidget<GameplayEffect>::StaticClass());
	Property::RegisterDetailWidget<GameplayAbility*>(TAssetPropertyWidget<GameplayAbility>::StaticClass());

	Property::RegisterDetailWidget<SGameplayAttribute>(AttributePropertyWidget::StaticClass());
	Property::RegisterDetailWidget<AttributeModifer*>(AttributeModiferWidget::StaticClass());

	

	Property::RegisterDetailWidget<TextureAsset*>(TAssetPropertyWidget<TextureAsset>::StaticClass());
	Property::RegisterDetailWidget<TSharedPtr<TextureAsset>>(TAssetPropertyWidget<TextureAsset>::StaticClass());	

	Property::RegisterDetailWidget<SkeletalMeshAsset*>(TAssetPropertyWidget<SkeletalMeshAsset>::StaticClass());
	Property::RegisterDetailWidget<TSharedPtr<SkeletalMeshAsset>>(TAssetPropertyWidget<SkeletalMeshAsset>::StaticClass());

	Property::RegisterDetailWidget<MeshAsset*>(TAssetPropertyWidget<MeshAsset>::StaticClass());
	Property::RegisterDetailWidget<TSharedPtr<MeshAsset>>(TAssetPropertyWidget<MeshAsset>::StaticClass());

	Property::RegisterDetailWidget<AnimationSequence*>(TAssetPropertyWidget<AnimationSequence>::StaticClass());
	Property::RegisterDetailWidget<TSharedPtr<AnimationSequence>>(TAssetPropertyWidget<AnimationSequence>::StaticClass());

	Property::RegisterDetailWidget<Material*>(TAssetPropertyWidget<Material>::StaticClass());
	Property::RegisterDetailWidget<TSharedPtr<Material>>(TAssetPropertyWidget<Material>::StaticClass());

	Property::RegisterDetailWidget<Sound*>(TAssetPropertyWidget<Sound>::StaticClass());
	Property::RegisterDetailWidget<TSharedPtr<Sound>>(TAssetPropertyWidget<Sound>::StaticClass());
	Property::RegisterDetailWidget<FontAsset*>(TAssetPropertyWidget<FontAsset>::StaticClass());
	Property::RegisterDetailWidget<TSharedPtr<FontAsset>>(TAssetPropertyWidget<FontAsset>::StaticClass());



	//AI ASSETs
	Property::RegisterDetailWidget<BehaviorTree*>(TAssetPropertyWidget<BehaviorTree>::StaticClass());
	Property::RegisterDetailWidget<TSharedPtr<BehaviorTree>>(TAssetPropertyWidget<BehaviorTree>::StaticClass());

	Property::RegisterDetailWidget<Blackboard*>(TAssetPropertyWidget<Blackboard>::StaticClass());
	Property::RegisterDetailWidget<TSharedPtr<Blackboard>>(TAssetPropertyWidget<Blackboard>::StaticClass());



	Property::RegisterDetailWidget<SBlackboardKeySelector>(BBKeySelectorWidget::StaticClass());

	Property::RegisterDetailWidget<SAnchorData>(AnchorPropertyWidget::StaticClass());

	Property::RegisterDetailWidget<SWidgetTransform>(WidgetTransformWidget::StaticClass());
	
	Property::RegisterDetailWidget<Scene*>(TAssetPropertyWidget<Scene>::StaticClass());
	Property::RegisterDetailWidget<TSharedPtr<Scene>>(TAssetPropertyWidget<Scene>::StaticClass());

	Property::RegisterDetailWidget<ParticleAsset*>(TAssetPropertyWidget<ParticleAsset>::StaticClass());
	Property::RegisterDetailWidget<TSharedPtr<ParticleAsset>>(TAssetPropertyWidget<ParticleAsset>::StaticClass());
}

