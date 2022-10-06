#include "ui.hpp"
#include "menu.hpp"
#include <deque>
#include <algorithm>
#include <vector>

//template <class T>
//bool ComboForSkins(const char* label, std::vector current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1) {
//	
//}
void ImGui::resetcurspos()
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	window->DC.CursorPos = ImVec2(window->Pos.x, window->Pos.y + 18);


	window->DrawList->AddText(ImVec2(window->Pos.x, window->Pos.y + 18), GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)), "1");
}
bool ImGui::ToggleButtonMain(const char* label, bool* v, const ImVec2& size_arg, int side)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	int flags = 0;
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat) flags |= ImGuiButtonFlags_Repeat;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

	// Render
	const ImU32 col = ImGui::GetColorU32(*v ? ImGuiCol_TitleBg : (hovered ? ImGuiCol_ChildBg : ImGuiCol_WindowBg));
	//ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
	switch (side)
	{
	case 0:
		window->DrawList->AddRectFilled(bb.Min, bb.Max, col, 0.f);
		break;
	case 1:
		window->DrawList->AddRectFilled(bb.Min, bb.Max, col, style.FrameRounding, ImDrawCornerFlags_BotLeft);
		break;
	case 2:
		window->DrawList->AddRectFilled(bb.Min, bb.Max, col, style.FrameRounding, ImDrawCornerFlags_BotRight);
		break;
	}

	if (*v)
	{
		ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding - ImVec2(0, 8), label, NULL, &label_size, style.ButtonTextAlign, &bb);
	}
	else
	{
		ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	}

	if (*v)
	{
		switch (side)
		{
		case 0:
			window->DrawList->AddRectFilled(ImVec2(bb.Min.x, bb.Max.y - 4), bb.Max, ImGui::GetColorU32(ImGuiCol_SliderGrab), 0.f);
			break;
		case 1:
			window->DrawList->AddRectFilled(ImVec2(bb.Min.x, bb.Max.y - 4), bb.Max, ImGui::GetColorU32(ImGuiCol_SliderGrab), style.FrameRounding, ImDrawCornerFlags_BotLeft);
			break;
		case 2:
			window->DrawList->AddRectFilled(ImVec2(bb.Min.x, bb.Max.y - 4), bb.Max, ImGui::GetColorU32(ImGuiCol_SliderGrab), style.FrameRounding, ImDrawCornerFlags_BotRight);
			break;
		}
		//window->DrawList->AddRectFilledMultiColor(ImVec2(bb.Min.x, bb.Max.y - 1), bb.Max, ImGui::GetColorU32(ImGuiCol_SliderGrab), ImGui::GetColorU32(ImGuiCol_SliderGrabActive), ImGui::GetColorU32(ImGuiCol_SliderGrabActive), ImGui::GetColorU32(ImGuiCol_SliderGrab));
	}
	if (pressed)
		*v = !*v;

	return pressed;

	/*ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	int flags = 0;
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat) flags |= ImGuiButtonFlags_Repeat;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

	// Render
	const ImU32 col = ImGui::GetColorU32(*v ? ImGuiCol_WindowBg : ((hovered || held) ? ImGuiCol_ButtonHovered : ImGuiCol_Button));

	ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
	ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

	if (pressed)
		*v = !*v;

	return pressed;*/
}

bool ImGui::ToggleButtonSelect(const char* label, bool* v, const ImVec2& size_arg, int side, bool KeyBind)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	int flags = 0;
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat) flags |= ImGuiButtonFlags_Repeat;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

	// Render
	const ImU32 col = KeyBind ? (ImGui::GetColorU32(*v ? ImGuiCol_ButtonActive : (hovered ? ImGuiCol_TitleBg : ImGuiCol_ButtonHovered))) : (ImGui::GetColorU32(*v ? ImGuiCol_TitleBg : (hovered ? ImGuiCol_ChildBg : ImGuiCol_WindowBg)));
	//ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
	switch (side)
	{
	case 0:
		window->DrawList->AddRectFilled(bb.Min, bb.Max, col, 0.f);
		break;
	case 1:
		window->DrawList->AddRectFilled(bb.Min, bb.Max, col, style.FrameRounding, ImDrawCornerFlags_Top);
		break;
	case 2:
		window->DrawList->AddRectFilled(bb.Min, bb.Max, col, style.FrameRounding, ImDrawCornerFlags_Bot);
		break;
	}

	if (!KeyBind)
	{
		if (*v)
		{
			ImGui::RenderTextClipped(ImVec2(bb.Min.x + 16.f, bb.Min.y + style.FramePadding.y), ImVec2(bb.Min.x + 16.f, bb.Max.y - style.FramePadding.y), label, NULL, &label_size, style.ButtonTextAlign, &bb);
		}
		else
		{
			ImGui::RenderTextClipped(ImVec2(bb.Min.x + 8.f, bb.Min.y + style.FramePadding.y), ImVec2(bb.Min.x + 8.f, bb.Max.y - style.FramePadding.y), label, NULL, &label_size, style.ButtonTextAlign, &bb);
		}
	}
	else
	{
		if (*v)
		{
			ImGui::RenderTextClipped(ImVec2(bb.Min.x, bb.Min.y + style.FramePadding.y), ImVec2(bb.Max.x, bb.Max.y - style.FramePadding.y), label, NULL, &label_size, style.ButtonTextAlign, &bb);
		}
		else
		{
			ImGui::RenderTextClipped(ImVec2(bb.Min.x, bb.Min.y + style.FramePadding.y), ImVec2(bb.Max.x, bb.Max.y - style.FramePadding.y), label, NULL, &label_size, style.ButtonTextAlign, &bb);
		}
	}

	if (*v)
	{
		switch (side)
		{
		case 0:
			window->DrawList->AddRectFilled(ImVec2(bb.Min.x, bb.Min.y), ImVec2(bb.Min.x + 4.f, bb.Max.y), ImGui::GetColorU32(ImGuiCol_SliderGrab), 0.f);
			break;
		case 1:
			window->DrawList->AddRectFilled(ImVec2(bb.Min.x, bb.Min.y), ImVec2(bb.Min.x + 4.f, bb.Max.y), ImGui::GetColorU32(ImGuiCol_SliderGrab), style.FrameRounding, ImDrawCornerFlags_TopLeft);
			break;
		case 2:
			window->DrawList->AddRectFilled(ImVec2(bb.Min.x, bb.Min.y), ImVec2(bb.Min.x + 4.f, bb.Max.y), ImGui::GetColorU32(ImGuiCol_SliderGrab), style.FrameRounding, ImDrawCornerFlags_BotLeft);
			break;
		}
		//window->DrawList->AddRectFilledMultiColor(ImVec2(bb.Min.x, bb.Max.y - 1), bb.Max, ImGui::GetColorU32(ImGuiCol_SliderGrab), ImGui::GetColorU32(ImGuiCol_SliderGrabActive), ImGui::GetColorU32(ImGuiCol_SliderGrabActive), ImGui::GetColorU32(ImGuiCol_SliderGrab));
	}
	if (pressed)
		*v = !*v;

	return pressed;

	/*ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	int flags = 0;
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat) flags |= ImGuiButtonFlags_Repeat;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

	// Render
	const ImU32 col = ImGui::GetColorU32(*v ? ImGuiCol_WindowBg : ((hovered || held) ? ImGuiCol_ButtonHovered : ImGuiCol_Button));

	ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
	ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

	if (pressed)
		*v = !*v;

	return pressed;*/
}

bool ImGui::ToggleButton(const char* label, bool* v, const ImVec2& size_arg, int side /* 1 - left; 2 - right*/)
{

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	int flags = 0;
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat) flags |= ImGuiButtonFlags_Repeat;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

	// Render
	const ImU32 col = ImGui::GetColorU32((hovered && held || *v) ? ImGuiCol_ButtonActive : (hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button));
	//ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
	switch (side)
	{
	case 0:
		window->DrawList->AddRectFilled(bb.Min, bb.Max, col, 0.f);
		break;
	case 1:
		window->DrawList->AddRectFilled(bb.Min, bb.Max, col, style.FrameRounding, ImDrawCornerFlags_Left);
		break;
	case 2:
		window->DrawList->AddRectFilled(bb.Min, bb.Max, col, style.FrameRounding, ImDrawCornerFlags_Right);
		break;
	}

	if (*v)
	{
		ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding - ImVec2(0, 8), label, NULL, &label_size, style.ButtonTextAlign, &bb);
	}
	else
	{
		ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	}

	if (*v)
	{
		switch (side)
		{
		case 0:
			window->DrawList->AddRectFilled(ImVec2(bb.Min.x, bb.Max.y - 4), bb.Max, ImGui::GetColorU32(ImGuiCol_SliderGrab), 0.f);
			break;
		case 1:
			window->DrawList->AddRectFilled(ImVec2(bb.Min.x, bb.Max.y - 4), bb.Max, ImGui::GetColorU32(ImGuiCol_SliderGrab), style.FrameRounding, ImDrawCornerFlags_BotLeft);
			break;
		case 2:
			window->DrawList->AddRectFilled(ImVec2(bb.Min.x, bb.Max.y - 4), bb.Max, ImGui::GetColorU32(ImGuiCol_SliderGrab), style.FrameRounding, ImDrawCornerFlags_BotRight);
			break;
		}
		//window->DrawList->AddRectFilledMultiColor(ImVec2(bb.Min.x, bb.Max.y - 1), bb.Max, ImGui::GetColorU32(ImGuiCol_SliderGrab), ImGui::GetColorU32(ImGuiCol_SliderGrabActive), ImGui::GetColorU32(ImGuiCol_SliderGrabActive), ImGui::GetColorU32(ImGuiCol_SliderGrab));
	}
	if (pressed)
		*v = !*v;

	return pressed;
}

static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
	const char* const* items = (const char* const*)data;
	if (out_text)
		*out_text = items[idx];
	return true;
}

static auto vector_getter = [](void* vec, int idx, const char** out_text) {
	auto& vector = *static_cast<std::vector<std::string>*>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*out_text = vector.at(idx).c_str();
	return true;
};

bool ImGui::Combo(const char* label, int* currIndex, std::vector<std::string>& values) {
	if (values.empty()) { return false; }
	return ImGui::Combo(label, currIndex, vector_getter,
		static_cast<void*>(&values), values.size());
}

bool ImGui::BeginGroupBox(const char* name, const ImVec2& size_arg)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;

	window->DC.CursorPos.y += GImGui->FontSize / 2;
	const ImVec2 content_avail = ImGui::GetContentRegionAvail();
	ImVec2 size = ImFloor(size_arg);
	if (size.x <= 0.0f) {
		size.x = ImMax(content_avail.x, 4.0f) - fabsf(size.x); // Arbitrary minimum zero-ish child size of 4.0f (0.0f causing too much issues)
	}
	if (size.y <= 0.0f) {
		size.y = ImMax(content_avail.y, 4.0f) - fabsf(size.y);
	}

	ImVec4 becup_color_body = g.Style.Colors[ImGuiCol_ChildBg];
	ImVec4 becup_color_bord = g.Style.Colors[ImGuiCol_Border];
	//float becup_child_bord = g.Style.ChildBorderSize;

	g.Style.Colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	g.Style.Colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	//g.Style.ChildBorderSize = 0.f;

	ImGui::SetNextWindowSize(size);
	bool ret = ImGui::BeginChild(name, size, true);
	//ImGui::Begin(name, &ret, flags);

	//g.Style.ChildBorderSize = becup_child_bord;
	g.Style.Colors[ImGuiCol_Border] = becup_color_bord;
	g.Style.Colors[ImGuiCol_ChildBg] = becup_color_body;

	window = ImGui::GetCurrentWindow();

	auto padding = ImGui::GetStyle().WindowPadding;

	auto text_size = ImGui::CalcTextSize(name, NULL, true);

	if (text_size.x > 1.0f) {
		window->DrawList->PushClipRectFullScreen();
		//window->DrawList->AddRectFilled(window->DC.CursorPos - ImVec2{ 4, 0 }, window->DC.CursorPos + (text_size + ImVec2{ 4, 0 }), GetColorU32(ImGuiCol_ChildWindowBg));
		//RenderTextClipped(pos, pos + text_size, name, NULL, NULL, GetColorU32(ImGuiCol_Text));
		window->DrawList->PopClipRect();
	}
	//if (!(window->Flags & ImGuiWindowFlags_ShowBorders))
	//	ImGui::GetCurrentWindow()->Flags &= ~ImGuiWindowFlags_ShowBorders;

	return ret;
}

void ImGui::EndGroupBox()
{
	ImGui::EndChild();
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	window->DC.CursorPosPrevLine.y -= GImGui->FontSize / 2;
}

bool ImGui::Hotkey(const char* label, int* k, const ImVec2& size_arg)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	ImGuiIO& io = g.IO;
	const ImGuiStyle& style = g.Style;

	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	ImVec2 size = ImGui::CalcItemSize(size_arg, ImGui::CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f);
	const ImRect frame_bb(window->DC.CursorPos + ImVec2(label_size.x + style.ItemInnerSpacing.x, 0.0f), window->DC.CursorPos + size);
	const ImRect total_bb(window->DC.CursorPos, frame_bb.Max);

	ImGui::ItemSize(total_bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(total_bb, id))
		return false;

	const bool focus_requested = ImGui::FocusableItemRegister(window, g.ActiveId == id, false);
	const bool focus_requested_by_code = focus_requested && (window->FocusIdxAllCounter == window->FocusIdxAllRequestCurrent);
	const bool focus_requested_by_tab = focus_requested && !focus_requested_by_code;

	const bool hovered = ImGui::ItemHoverable(frame_bb, id);

	if (hovered) {
		ImGui::SetHoveredID(id);
		g.MouseCursor = ImGuiMouseCursor_TextInput;
	}

	const bool user_clicked = hovered && io.MouseClicked[0];

	if (focus_requested || user_clicked) {
		if (g.ActiveId != id) {
			// Start edition
			memset(io.MouseDown, 0, sizeof(io.MouseDown));
			memset(io.KeysDown, 0, sizeof(io.KeysDown));
			*k = 0;
		}
		ImGui::SetActiveID(id, window);
		ImGui::FocusWindow(window);
	}
	else if (io.MouseClicked[0]) {
		// Release focus when we click outside
		if (g.ActiveId == id)
			ImGui::ClearActiveID();
	}

	bool value_changed = false;
	int key = *k;

	if (g.ActiveId == id) {
		for (auto i = 0; i < 5; i++) {
			if (io.MouseDown[i]) {
				switch (i) {
				case 0:
					key = VK_LBUTTON;
					break;
				case 1:
					key = VK_RBUTTON;
					break;
				case 2:
					key = VK_MBUTTON;
					break;
				case 3:
					key = VK_XBUTTON1;
					break;
				case 4:
					key = VK_XBUTTON2;
					break;
				}
				value_changed = true;
				ImGui::ClearActiveID();
			}
		}
		if (!value_changed) {
			for (auto i = VK_BACK; i <= VK_RMENU; i++) {
				if (io.KeysDown[i]) {
					key = i;
					value_changed = true;
					ImGui::ClearActiveID();
				}
			}
		}

		if (IsKeyPressedMap(ImGuiKey_Escape)) {
			*k = 0;
			ImGui::ClearActiveID();
		}
		else {
			*k = key;
		}
	}

	// Render
	// Select which buffer we are going to display. When ImGuiInputTextFlags_NoLiveEdit is Set 'buf' might still be the old value. We Set buf to NULL to prevent accidental usage from now on.

	char buf_display[64] = "None";

	ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), true, style.FrameRounding);

	if (*k != 0 && g.ActiveId != id) {
		strcpy_s(buf_display, KeyNames[*k]);
	}
	else if (g.ActiveId == id) {
		strcpy_s(buf_display, "<Press a key>");
	}

	const ImRect clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x, frame_bb.Min.y + size.y); // Not using frame_bb.Max because we have adjusted size
	ImVec2 render_pos = frame_bb.Min + style.FramePadding;
	ImGui::RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, buf_display, NULL, NULL, style.ButtonTextAlign, &clip_rect);
	//RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, buf_display, NULL, NULL, GetColorU32(ImGuiCol_Text), style.ButtonTextAlign, &clip_rect);
	//draw_window->DrawList->AddText(g.Font, g.FontSize, render_pos, GetColorU32(ImGuiCol_Text), buf_display, NULL, 0.0f, &clip_rect);

	if (label_size.x > 0)
		ImGui::RenderText(ImVec2(total_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), label);

	return value_changed;
}


bool ImGui::ListBox(const char* label, int* current_item, std::string items[], int items_count, int height_items) {
	char** tmp;
	tmp = new char* [items_count];//(char**)malloc(sizeof(char*) * items_count);
	for (int i = 0; i < items_count; i++) {
		//tmp[i] = new char[items[i].size()];//(char*)malloc(sizeof(char*));
		tmp[i] = const_cast<char*>(items[i].c_str());
	}

	const bool value_changed = ImGui::ListBox(label, current_item, Items_ArrayGetter, static_cast<void*>(tmp), items_count, height_items);
	return value_changed;
}

bool ImGui::ListBox(const char* label, int* current_item, std::function<const char* (int)> lambda, int items_count, int height_in_items)
{
	return ImGui::ListBox(label, current_item, [](void* data, int idx, const char** out_text)
		{
			*out_text = (*reinterpret_cast<std::function<const char* (int)>*>(data))(idx);
			return true;
		}, &lambda, items_count, height_in_items);
}

bool ImGui::Combo(const char* label, int* current_item, std::function<const char* (int)> lambda, int items_count, int height_in_items)
{
	return ImGui::Combo(label, current_item, [](void* data, int idx, const char** out_text)
		{
			*out_text = (*reinterpret_cast<std::function<const char* (int)>*>(data))(idx);
			return true;
		}, &lambda, items_count, height_in_items);
}
