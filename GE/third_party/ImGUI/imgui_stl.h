

#pragma once

#include <string>
#include "imgui.h"

namespace ImGui
{
	IMGUI_API void Text(std::string&& txt);
	IMGUI_API void Text(const std::string& txt);



	IMGUI_API bool  InputText(const char* label, std::string& str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
	IMGUI_API bool  InputTextMultiline(const char* label, std::string& str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);

	IMGUI_API bool  InputTextWithHint(const char* label, const char* hint,std::string& str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);

	
	
}


