#include "EnginePCH.h"
#include "EditorManager.h"
#include "Renderer/Renderer.h"

#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_internal.h"
#include "EngineWindow.h"
#include "../ImGUI/imgui_impl_dx11.h"
#include "../ImGUI/imgui_impl_win32.h"
#include "Engine.h"

EditorManager::EditorManager()
	:m_EditorBegun(false)
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
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigWindowsResizeFromEdges = true;
	io.ConfigViewportsNoTaskBarIcon = true;
	io.IniFilename = "editor.ini";

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(handle);
	ImGui_ImplDX11_Init(dev, context);

	//AddFont("DefaultFont", "NotoSansKR-Regular.otf", 20.f);
}

void EditorManager::BeginWindow()
{
	const ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;


	// Set window position and size
	float offset_y = 0;
		//_editor::widget_menu_bar ? (_editor::widget_menu_bar->GetHeight() + _editor::widget_menu_bar->GetPadding()) : 0;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + offset_y));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - offset_y));
	ImGui::SetNextWindowViewport(viewport->ID);

	// Set window style
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowBgAlpha(0.0f);

	// Begin window
	std::string name = "##main_window";
	bool open = true;
	m_EditorBegun = ImGui::Begin(name.c_str(), &open, window_flags);
	ImGui::PopStyleVar(3);

	// Begin dock space
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable && m_EditorBegun)
	{
		// Dock space
		const auto window_id = ImGui::GetID(name.c_str());
		if (!ImGui::DockBuilderGetNode(window_id))
		{
			// Reset current docking state
			ImGui::DockBuilderRemoveNode(window_id);
			ImGui::DockBuilderAddNode(window_id, ImGuiDockNodeFlags_None);
			ImGui::DockBuilderSetNodeSize(window_id, ImGui::GetMainViewport()->Size);

			// DockBuilderSplitNode(ImGuiID node_id, ImGuiDir split_dir, float size_ratio_for_node_at_dir, ImGuiID* out_id_dir, ImGuiID* out_id_other);
			ImGuiID dock_main_id = window_id;
			ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);
			const ImGuiID dock_right_down_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.6f, nullptr, &dock_right_id);
			ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);
			const ImGuiID dock_down_right_id = ImGui::DockBuilderSplitNode(dock_down_id, ImGuiDir_Right, 0.6f, nullptr, &dock_down_id);

			// Dock windows
			ImGui::DockBuilderDockWindow("World", dock_right_id);
			ImGui::DockBuilderDockWindow("Properties", dock_right_down_id);
			ImGui::DockBuilderDockWindow("Console", dock_down_id);
			ImGui::DockBuilderDockWindow("Assets", dock_down_right_id);
			ImGui::DockBuilderDockWindow("Viewport", dock_main_id);

			ImGui::DockBuilderFinish(dock_main_id);
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::DockSpace(window_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::PopStyleVar();
	}

}

void EditorManager::Update(float DeltaSeconds)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	BeginWindow();

	if(m_EditorBegun)
	{
		ImGui::End();
	}

	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}
