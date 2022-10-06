#pragma once
//#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include "../imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
//#define IMGUI_DEFINE_PLACEMENT_NEW
#include "../imgui/imgui_internal.h"
//#include "imgui/directx9/imgui_impl_dx9.h"
#include <Windows.h>
#include <string>
#include <functional>
#include <vector>

template<size_t N>
void render_tabs2(char* (&names)[N], int& activetab, float w, float h, bool Keybind)
{
	bool values[N] = { false };

	values[activetab] = true;

	for (auto i = 0; i < N; ++i)
	{

		if (i == 0)
		{
			if (ImGui::ToggleButtonSelect(names[i], &values[i], ImVec2{ w, h }, 1, Keybind))
				activetab = i;
		}
		else if (i == N - 1)
		{
			if (ImGui::ToggleButtonSelect(names[i], &values[i], ImVec2{ w, h }, 2, Keybind))
				activetab = i;
		}
		else
		{
			if (ImGui::ToggleButtonSelect(names[i], &values[i], ImVec2{ w, h }, 0, Keybind))
				activetab = i;
		}

	}
}


namespace ImGui
{
	void resetcurspos();
	bool ToggleButtonMain(const char* label, bool* v, const ImVec2& size_arg = ImVec2(0, 0), int side = 0);
	bool ToggleButtonSelect(const char* label, bool* v, const ImVec2& size_arg = ImVec2(0, 0), int side = 0, bool KeyBind = false);
	bool ToggleButton(const char* label, bool* v, const ImVec2& size_arg = ImVec2(0, 0), int side = 0);
	// Combo box helper allowing to pass an array of strings.
	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values);
	bool BeginGroupBox(const char* name, const ImVec2& size_arg = ImVec2(0, 0));
	void EndGroupBox();
	bool Hotkey(const char* label, int* k, const ImVec2& size_arg = ImVec2(0, 0));
	bool ListBox(const char* label, int* current_item, std::string items[], int items_count, int height_items);
	bool ListBox(const char* label, int* current_item, std::function<const char* (int)> lambda, int items_count, int height_in_items);
	bool Combo(const char* label, int* current_item, std::function<const char* (int)> lambda, int items_count, int height_in_items);
}