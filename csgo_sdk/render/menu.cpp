#include "Menu.hpp"

#define NOMINMAX

#include <Windows.h>
#include <chrono>
#include <filesystem>

#include "../functions/item_definitions.hpp"
#include "../sdk/utils/input.hpp"
#include "../config.hpp"
#include "../functions/skins.hpp"
#include "../functions/kit_parser.hpp"
#include "ui.hpp"


#define IMGUI_DEFINE_MATH_OPERATORS

#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

#include "../functions/misc.hpp"

extern ImFont* g_pDefaultFont;

IDirect3DTexture9* m_skin_texture = nullptr;
static std::string old_name_skin = "";
static std::string old_name_weap = "";

constexpr static float get_sidebar_item_width() { return 150.0f; }
constexpr static float get_sidebar_item_height() { return  50.0f; }

std::string timestamp_to_date(const time_t rawtime)
{
    struct tm* dt;
    char buffer[30];
    dt = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%d/%m/%y %H:%M", dt);
    return std::string(buffer);
}

enum 
{
	TAB_ESP,
    TAB_GLOW,
    TAB_CHAMS,
	TAB_AIMBOT,
	TAB_MISC,
	TAB_SKINS,
	TAB_CONFIG
};

namespace ImGuiEx
{
    bool Hotkey(const char* label, int* k, const ImVec2& size_arg) 
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        ImGuiIO& io = g.IO;
        const ImGuiStyle& style = g.Style;

        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
        ImVec2 size = ImGui::CalcItemSize(size_arg, ImGui::CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f);
        const ImRect frame_bb(window->DC.CursorPos + ImVec2(label_size.x + style.ItemInnerSpacing.x, 0.0f), window->DC.CursorPos + size);
        const ImRect total_bb(window->DC.CursorPos, frame_bb.Max);

        ImGui::ItemSize(total_bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(total_bb, id)) return false;

        const bool focus_requested = ImGui::FocusableItemRegister(window, g.ActiveId == id, false);
        const bool focus_requested_by_code = focus_requested && (window->FocusIdxAllCounter == window->FocusIdxAllRequestCurrent);
        const bool focus_requested_by_tab = focus_requested && !focus_requested_by_code;

        const bool hovered = ImGui::ItemHoverable(frame_bb, id);

        if (hovered) 
        {
            ImGui::SetHoveredID(id);
            g.MouseCursor = ImGuiMouseCursor_TextInput;
        }

        const bool user_clicked = hovered && io.MouseClicked[0];

        if (focus_requested || user_clicked) 
        {
            if (g.ActiveId != id) 
            {
                memset(io.MouseDown, 0, sizeof(io.MouseDown));
                memset(io.KeysDown, 0, sizeof(io.KeysDown));
                *k = 0;
            }
            ImGui::SetActiveID(id, window);
            ImGui::FocusWindow(window);
        }
        else if (io.MouseClicked[0]) 
        {
            if (g.ActiveId == id)
                ImGui::ClearActiveID();
        }

        bool value_changed = false;
        int key = *k;

        if (g.ActiveId == id) 
        {
            for (auto i = 0; i < 5; i++) 
            {
                if (io.MouseDown[i]) 
                {
                    switch (i) 
                    {
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
            if (!value_changed) 
            {
                for (auto i = VK_BACK; i <= VK_RMENU; i++) 
                {
                    if (io.KeysDown[i]) 
                    {
                        key = i;
                        value_changed = true;
                        ImGui::ClearActiveID();
                    }
                }
            }

            if (ImGui::IsKeyPressedMap(ImGuiKey_Escape))
            {
                *k = 0;
                ImGui::ClearActiveID();
            }
            else 
            {
                *k = key;
            }
        }

        char buf_display[64] = "Key";

        ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImColor(75, 103, 210, 0), true, style.FrameRounding);

        if (*k != 0 && g.ActiveId != id) strcpy_s(buf_display, KeyNames[*k]);
        else if (g.ActiveId == id) strcpy_s(buf_display, "Press key");

        const ImRect clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x, frame_bb.Min.y + size.y);
        ImVec2 render_pos = frame_bb.Min + style.FramePadding;
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
        ImGui::RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, buf_display, NULL, NULL, style.ButtonTextAlign, &clip_rect);
        ImGui::PopStyleColor();

        if (label_size.x > 0) ImGui::RenderText(ImVec2(total_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), label);

        return value_changed;
    }

    std::map<ImGuiID, float> config_alpha;

    bool Config_Button(const char* label, const char* creation_date, bool selected, ImVec2 size_arg)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
        ImGuiButtonFlags flags = ImGuiButtonFlags_None;
        ImVec2 pos = window->DC.CursorPos;
        if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
            pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;
        ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

        const ImRect bb(pos, pos + size);
        ImGui::ItemSize(size, style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id))
            return false;

        if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat)
            flags |= ImGuiButtonFlags_Repeat;

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);
        float* alpha = &config_alpha[id];

        if (*alpha >= ImGui::GetStyle().Alpha) 
            *alpha = ImGui::GetStyle().Alpha;

        if (*alpha <= 0.f) 
            *alpha = 0.f;

        if (selected) 
        {
            if (*alpha <= ImGui::GetStyle().Alpha) 
                *alpha += ImGui::GetIO().DeltaTime * 4;
        }
        else 
        {
            if (*alpha > 0.f) 
                *alpha -= ImGui::GetIO().DeltaTime * 4;
        }

        ImU32 col = IM_COL32(255, 255, 255, 110);
        const ImU32 frame_color = ImGui::GetColorU32((ImGui::IsItemActive() && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

        ImGui::RenderFrame(bb.Min, bb.Max, frame_color, true, 3.f);

        window->DrawList->AddText(g_pDefaultFont, 18.f, bb.Min + ImVec2(10, 3), col, label);
        window->DrawList->AddText(bb.Min + ImVec2(10, 20), col, creation_date);
        window->DrawList->AddRect(bb.Min, bb.Max, ImGui::GetColorU32(ImVec4(0.260f, 0.590f, 0.980f, *alpha)), 3.f, 15, 1.5f);

        window->DrawList->AddText(g_pDefaultFont, 18.f, bb.Min + ImVec2(10, 3), ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, *alpha)), label);
        window->DrawList->AddText(bb.Min + ImVec2(10, 20), ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, *alpha)), creation_date);

        return pressed;
    }

    bool InputTextWithHint(const char* label, char* hint, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL)
    {
        IM_ASSERT(!(flags & ImGuiInputTextFlags_Multiline));
        return ImGui::InputTextEx(label, hint, buf, (int)buf_size, ImVec2(0, 0), flags, callback, user_data);
    }

    void TextCentered(std::string text) {
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text(text.c_str());
    }
}
template<size_t N>
void render_tabsS(char* (&names)[N], int& activetab, float w, float h)
{
    bool values[N] = { false };
    values[activetab] = true;
    for (auto i = 0; i < N; ++i)
    {
        if (i == 0)
        {
            if (ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h }, 1))
                activetab = i;
        }
        else if (i == N - 1)
        {
            if (ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h }, 2))
                activetab = i;
        }
        else
        {
            if (ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h }, 0))
                activetab = i;
        }
        if (i < N - 1) ImGui::SameLine();
    }
}


template<size_t N>
void render_tabs(char* (&names)[N], int& activetab, float w, float h, float space, bool sameline)
{
    for(auto i = 0; i < N; ++i)
    {
        if(ImGui::Button(names[i], ImVec2{ w, h }))
        {
            activetab = i;
        }

        if(sameline && i < N - 1)
            ImGui::SameLine();
        
        ImVec2 windowpos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(windowpos.x + space, windowpos.y));
    }
}

ImVec2 get_sidebar_size()
{
    constexpr float padding = 10.0f;
    constexpr auto size_w = padding * 2.0f + get_sidebar_item_width();
    constexpr auto size_h = padding * 2.0f + (sizeof(sidebar_tabs) / sizeof(char*)) * get_sidebar_item_height();

    return ImVec2{ size_w, ImMax(325.0f, size_h) };
}

void RenderEspTab()
{
    auto& style = ImGui::GetStyle();

    ImGui::BeginGroup();
    {
        ImGui::Columns(3, nullptr, false);

        ImGui::Checkbox("Enabled", &g_Configurations.esp_enabled);
        ImGui::Checkbox("Team check", &g_Configurations.esp_enemies_only);
        ImGui::Checkbox("Boxes", &g_Configurations.esp_player_boxes);
        ImGui::Checkbox("Names", &g_Configurations.esp_player_names);
        ImGui::Checkbox("Health", &g_Configurations.esp_player_health);
        ImGui::Checkbox("Armour", &g_Configurations.esp_player_armour);
        ImGui::Text("Weapon");
        ImGui::Combo("##Weaponesptype", &g_Configurations.esp_player_weapons_type, weaponesptype, IM_ARRAYSIZE(weaponesptype));
        ImGui::Checkbox("Snaplines", &g_Configurations.esp_player_snaplines);

        ImGui::NextColumn();

        ImGui::Checkbox("Crosshair", &g_Configurations.esp_crosshair);
        ImGui::Checkbox("Dropped Weapons", &g_Configurations.esp_dropped_weapons);
        if (g_Configurations.esp_dropped_weapons)
        {
            ImGui::Text("Type");
            ImGui::Combo("##Weaponespdroppedtype", &g_Configurations.esp_dropped_weapons_type, weaponesptype, IM_ARRAYSIZE(weaponesptype));
        }
        ImGui::Checkbox("Defuse Kit", &g_Configurations.esp_defuse_kit);
        ImGui::Checkbox("Planted C4", &g_Configurations.esp_planted_c4);
        ImGui::Checkbox("Item Esp", &g_Configurations.esp_items);
        ImGui::Checkbox("Grenade prediction", &g_Configurations.esp_grenade_prediction);
        ImGui::Checkbox("Draw weapon FOV", &g_Configurations.esp_draw_weapon_fov);

        ImGui::NextColumn();

        ImGui::PushItemWidth(100);
        ImGui::ColorEdit4("Allies Visible", g_Configurations.color_esp_ally_visible);
        ImGui::ColorEdit4("Enemies Visible", g_Configurations.color_esp_enemy_visible);
        ImGui::ColorEdit4("Allies Occluded", g_Configurations.color_esp_ally_occluded);
        ImGui::ColorEdit4("Enemies Occluded", g_Configurations.color_esp_enemy_occluded);
        ImGui::ColorEdit4("Crosshair", g_Configurations.color_esp_crosshair);
        ImGui::ColorEdit4("Dropped Weapons", g_Configurations.color_esp_weapons);
        ImGui::ColorEdit4("Defuse Kit", g_Configurations.color_esp_defuse);
        ImGui::ColorEdit4("Planted C4", g_Configurations.color_esp_c4);
        ImGui::ColorEdit4("Item Esp", g_Configurations.color_esp_item);
        ImGui::ColorEdit4("Grenade prediction", g_Configurations.color_grenade_prediction);
        ImGui::PopItemWidth();

        ImGui::Columns(1, nullptr, false);
    }
    ImGui::EndGroup();
}

void RednerGlowTab()
{
    ImGui::BeginGroup();
    {
        ImGui::Columns(3, nullptr, false);

        ImGui::Checkbox("Enabled", &g_Configurations.glow_enabled);
        ImGui::Checkbox("Team check", &g_Configurations.glow_enemies_only);
        ImGui::Checkbox("Players", &g_Configurations.glow_players);
        ImGui::Checkbox("Chickens", &g_Configurations.glow_chickens);
        ImGui::Checkbox("C4 Carrier", &g_Configurations.glow_c4_carrier);
        ImGui::Checkbox("Planted C4", &g_Configurations.glow_planted_c4);
        ImGui::Checkbox("Defuse Kits", &g_Configurations.glow_defuse_kits);
        ImGui::Checkbox("Weapons", &g_Configurations.glow_weapons);
        ImGui::SliderInt("Glow Style", &g_Configurations.glow_style, 0, 3);
        ImGui::Checkbox("Full Bloom", &g_Configurations.glow_fullbloom);

        ImGui::NextColumn();

        ImGui::PushItemWidth(100);
        ImGui::ColorEdit4("Ally", g_Configurations.color_glow_ally);
        ImGui::ColorEdit4("Enemy", g_Configurations.color_glow_enemy);
        ImGui::ColorEdit4("Chickens", g_Configurations.color_glow_chickens);
        ImGui::ColorEdit4("C4 Carrier", g_Configurations.color_glow_c4_carrier);
        ImGui::ColorEdit4("Planted C4", g_Configurations.color_glow_planted_c4);
        ImGui::ColorEdit4("Defuse Kits", g_Configurations.color_glow_defuse);
        ImGui::ColorEdit4("Weapons", g_Configurations.color_glow_weapons);
        ImGui::PopItemWidth();

        ImGui::NextColumn();

        ImGui::Columns(1, nullptr, false);
    }
    ImGui::EndGroup();
}

void RenderChamsTab()
{
    ImGui::BeginGroup();
    {
        ImGui::Columns(4, nullptr, false);

        ImGui::BeginGroup();
        {
            ImGui::Text("Players");
            ImGui::Spacing();
            ImGui::Checkbox("Enabled ##players", &g_Configurations.chams_player_enabled);
            ImGui::Checkbox("Team Check ##players", &g_Configurations.chams_player_enemies_only);
            ImGui::Checkbox("Wireframe ##players", &g_Configurations.chams_player_wireframe);
            ImGui::Checkbox("Flat ##players", &g_Configurations.chams_player_flat);
            ImGui::Checkbox("Behind wall ##players", &g_Configurations.chams_player_ignorez);
            ImGui::Checkbox("Glass ##players", &g_Configurations.chams_player_glass);
            ImGui::PushItemWidth(110);
            ImGui::ColorEdit4("Ally (Visible) ##players", g_Configurations.color_chams_player_ally_visible);
            ImGui::ColorEdit4("Ally (Occluded) ##players", g_Configurations.color_chams_player_ally_occluded);
            ImGui::ColorEdit4("Enemy (Visible) ##players", g_Configurations.color_chams_player_enemy_visible);
            ImGui::ColorEdit4("Enemy (Occluded) ##players", g_Configurations.color_chams_player_enemy_occluded);
            ImGui::PopItemWidth();
        }
        ImGui::EndGroup();

        ImGui::NextColumn();

        ImGui::BeginGroup();
        {
            ImGui::Text("Arms");
            ImGui::Spacing();
            ImGui::Checkbox("Enabled ##arms", &g_Configurations.chams_arms_enabled);
            ImGui::Checkbox("Wireframe ##arms", &g_Configurations.chams_arms_wireframe);
            ImGui::Checkbox("Flat ##arms", &g_Configurations.chams_arms_flat);
            ImGui::Checkbox("Ignore-Z ##arms", &g_Configurations.chams_arms_ignorez);
            ImGui::Checkbox("Glass ##arms", &g_Configurations.chams_arms_glass);
            ImGui::PushItemWidth(110);
            ImGui::ColorEdit4("Color (Visible) ##arms", g_Configurations.color_chams_arms_visible);
            ImGui::ColorEdit4("Color (Occluded) ##arms", g_Configurations.color_chams_arms_occluded);
            ImGui::PopItemWidth();
        }
        ImGui::EndGroup();

        ImGui::NextColumn();

        ImGui::BeginGroup();
        {
            ImGui::Text("Weapon");
            ImGui::Spacing();
            ImGui::Checkbox("Enabled ##weapon", &g_Configurations.chams_weapon_enabled);
            ImGui::Checkbox("Wireframe ##weapon", &g_Configurations.chams_weapon_wireframe);
            ImGui::Checkbox("Flat ##weapon", &g_Configurations.chams_weapon_flat);
            ImGui::Checkbox("Ignore-Z ##weapon", &g_Configurations.chams_weapon_ignorez);
            ImGui::Checkbox("Glass ##weapon", &g_Configurations.chams_weapon_glass);
            ImGui::PushItemWidth(110);
            ImGui::ColorEdit4("Color (Visible) ##weapon", g_Configurations.color_chams_weapon_visible);
            ImGui::ColorEdit4("Color (Occluded) ##weapon", g_Configurations.color_chams_weapon_occluded);
            ImGui::PopItemWidth();
        }
        ImGui::EndGroup();

        ImGui::NextColumn();

        ImGui::BeginGroup();
        {
            ImGui::Text("Sleeve");
            ImGui::Spacing();
            ImGui::Checkbox("Enabled ##sleeve", &g_Configurations.chams_sleeve_enabled);
            ImGui::Checkbox("Wireframe ##sleeve", &g_Configurations.chams_sleeve_wireframe);
            ImGui::Checkbox("Flat ##sleeve", &g_Configurations.chams_sleeve_flat);
            ImGui::Checkbox("Ignore-Z ##sleeve", &g_Configurations.chams_sleeve_ignorez);
            ImGui::Checkbox("Glass ##sleeve", &g_Configurations.chams_sleeve_glass);
            ImGui::PushItemWidth(110);
            ImGui::ColorEdit4("Color (Visible) ##sleeve", g_Configurations.color_chams_sleeve_visible);
            ImGui::ColorEdit4("Color (Occluded) ##sleeve", g_Configurations.color_chams_sleeve_occluded);
            ImGui::PopItemWidth();
        }
        ImGui::EndGroup();

        ImGui::Columns(1, nullptr, false);
    }
    ImGui::EndGroup();
}

void RenderMiscTab()
{
    ImGui::BeginGroup();
    {
        ImGui::Columns(2, nullptr, false);

        ImGui::Checkbox("Bunny hop", &g_Configurations.misc_bhop);
        ImGui::Checkbox("Auto strafe", &g_Configurations.misc_autostrafe);
		ImGui::Checkbox("Third Person", &g_Configurations.misc_thirdperson);

		if(g_Configurations.misc_thirdperson)
			ImGui::SliderFloat("Distance", &g_Configurations.misc_thirdperson_dist, 0.f, 150.f);

        ImGui::Checkbox("EdgeJump", &g_Configurations.misc_edgejump);
        if (g_Configurations.misc_edgejump)
        {
            ImGui::SameLine();
            ImGuiEx::Hotkey("##EJKeybind", &g_Configurations.misc_edgejumpkey, ImVec2(100, 20));
        }
        ImGui::Checkbox("JumpBug", &g_Configurations.misc_jumpbug);
        if (g_Configurations.misc_jumpbug)
        {
            ImGui::SameLine();
            ImGuiEx::Hotkey("##JBKeybind", &g_Configurations.misc_jumpbugkey, ImVec2(100, 20));
        }
        ImGui::Checkbox("EdgeBug", &g_Configurations.misc_edgebug);
        if (g_Configurations.misc_edgebug)
        {
            ImGui::SameLine();
            ImGuiEx::Hotkey("##EBKeybind", &g_Configurations.misc_edgebugkey, ImVec2(100, 20));
        }
        ImGui::Checkbox("Blockbot", &g_Configurations.misc_blockbot);
        if (g_Configurations.misc_blockbot)
        {
            ImGui::SameLine();
            ImGuiEx::Hotkey("##BBKeybind", &g_Configurations.misc_blockbotkey, ImVec2(100, 20));
        }
        ImGui::Checkbox("Fake Backwards", &g_Configurations.misc_fakebackwards);
        if (ImGui::ItemsToolTipBegin("##molotovcolorchanger"))
        {
            ImGui::SliderInt("Turn Smoothness", &g_Configurations.misc_fakebackwardsturnsmoothness, 1, 20);
            ImGui::ItemsToolTipEnd();
        }
        if (g_Configurations.misc_fakebackwards)
        {
            ImGui::SameLine();
            ImGuiEx::Hotkey("##FBKeybind", &g_Configurations.misc_fakebackwardskey, ImVec2(100, 20));
        }

        ImGui::Checkbox("No hands", &g_Configurations.misc_no_hands);
		ImGui::Checkbox("Rank reveal", &g_Configurations.misc_showranks);
		ImGui::Checkbox("Watermark", &g_Configurations.misc_watermark);
		ImGui::Checkbox("No Fog", &g_Configurations.misc_no_fog);
        ImGui::Checkbox("Nightmode", &g_Configurations.misc_nightmode);

        if (g_Configurations.misc_nightmode) {
            ImGui::ColorEdit3("Color World", g_Configurations.nightmode_color_world);
            ImGui::ColorEdit3("Color Prop", g_Configurations.nightmode_color_prop);
            ImGui::ColorEdit3("Color Sky", g_Configurations.nightmode_color_sky);
        }

        if (ImGui::Button("Unhook"))
            g_Unload = true;

		ImGui::NextColumn();

        ImGui::Text("Viewmodel:");
        ImGui::SliderInt("FOV", &g_Configurations.misc_viewmodel_fov, 68, 120);
		ImGui::Text("Postprocessing:");
        ImGui::SliderFloat("Red", &g_Configurations.misc_mat_ambient_light_r, 0, 1);
        ImGui::SliderFloat("Green", &g_Configurations.misc_mat_ambient_light_g, 0, 1);
        ImGui::SliderFloat("Blue", &g_Configurations.misc_mat_ambient_light_b, 0, 1);
        ImGui::Checkbox("Particle color editor", &g_Configurations.misc_editparticle);
        if (g_Configurations.misc_editparticle)
        {
            ImGui::Text("Molotov");
            if (ImGui::ItemsToolTipBegin("##molotovcolorchanger"))
            {
                ImGui::Checkbox("no smoke", &g_Configurations.misc_changemolotov_nosmoke);
                ImGui::ItemsToolTipEnd();
            }
            ImGui::SameLine();
            ImGui::ColorEdit4("Color##molotov", g_Configurations.misc_changemolotov_color);

            ImGui::Text("Blood   ");
            ImGui::SameLine();
            ImGui::ColorEdit4("Color##blood", g_Configurations.misc_changeblood_color);

            ImGui::Text("Smoke ");
            ImGui::SameLine();
            ImGui::ColorEdit4("Color##smoke", g_Configurations.misc_changesmoke_color);

        }

        ImGui::Columns(1, nullptr, false);
    }
    ImGui::EndGroup();
}

struct hud_weapons_t {
    std::int32_t* GetWeaponCount() {
        return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
    }
};

static std::string selected_weapon_name = "";
static std::string selected_skin_name = "";

void RenderSkinsTab()
{
    auto& entries = g_Configurations.m_items;
    ImGui::Columns(2, nullptr, false);
    ImGui::BeginChild("weapon select##skin window", ImVec2(0.f, 0.f), true, ImGuiWindowFlags_ChildWindowTitle);
    {
        ImGui::ListBoxHeader("weapons##skinstab", ImVec2(0, 306));
        {
            for (size_t w = 0; w < k_WeaponNames.size(); w++)
            {
                switch (w)
                {
                case 0:
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "knife");
                    break;
                case 2:
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "glove");
                    break;
                case 4:
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "pistols");
                    break;
                case 14:
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "semi-rifle");
                    break;
                case 21:
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "rifle");
                    break;
                case 28:
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "sniper-rifle");
                    break;
                case 32:
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "machingun");
                    break;
                case 34:
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "shotgun");
                    break;
                }

                if (ImGui::Selectable(k_WeaponNames[w].name.c_str(), definition_vector_index == w))
                {
                    definition_vector_index = w;
                }
            }
        }
        ImGui::ListBoxFooter();

        static float next_enb_time = 0;

        float time_to_next_up = g_GlobalVars->curtime;

        time_to_next_up = std::clamp(next_enb_time - g_GlobalVars->curtime, 0.f, 1.f);

        std::string name = "update (";
        name += std::to_string(time_to_next_up);
        name.erase(12, 16);
        name += ")";

        if (ImGui::Button(name.c_str(), ImVec2(221, 0)))
        {
            if (next_enb_time <= g_GlobalVars->curtime)
            {
                static auto clear_hud_weapon_icon_fn =
                    reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(
                        Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 34"));

                auto element = Utils::FindHudElement<std::uintptr_t*>("CCSGO_HudWeaponSelection");

                if (element)
                {
                    auto hud_weapons = reinterpret_cast<hud_weapons_t*>(std::uintptr_t(element) - 0xa0);
                    if (hud_weapons != nullptr)
                    {

                        if (*hud_weapons->GetWeaponCount())
                        {
                            for (std::int32_t i = 0; i < *hud_weapons->GetWeaponCount(); i++)
                                i = clear_hud_weapon_icon_fn(hud_weapons, i);

                            typedef void(*ForceUpdate) (void);
                            static ForceUpdate FullUpdate = (ForceUpdate)Utils::PatternScan(GetModuleHandleA("engine.dll"), "A1 ? ? ? ? B9 ? ? ? ? 56 FF 50 14 8B 34 85");
                            FullUpdate();

                            g_ClientState->ForceFullUpdate();
                        }
                    }
                }

                next_enb_time = g_GlobalVars->curtime + 1.f;
            }
        }
    }
    ImGui::EndChild("weapon select");

    ImGui::NextColumn();

    ImGui::BeginChild("skin select##skin tab", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindowTitle);
    {
        static int selected_tab_skins = 0;

        auto& selected_entry = entries[k_WeaponNames[definition_vector_index].definition_index];
        auto& satatt = g_Configurations.statrack_items[k_WeaponNames[definition_vector_index].definition_index];
        selected_entry.definition_index = k_WeaponNames[definition_vector_index].definition_index;
        selected_entry.definition_vector_index = definition_vector_index;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        {
            static char* Players_tabs[] = { "general", "paint kit" };

            auto TabsW_players_tab = (ImGui::GetCurrentWindow()->Size.x - Menu::Get()._style.WindowPadding.x * 2.0f) / _countof(Players_tabs);

            render_tabsS(Players_tabs, selected_tab_skins, TabsW_players_tab, 20.0f);
        }
        ImGui::PopStyleVar();

        if (selected_entry.definition_index != GLOVE_T_SIDE &&
            selected_entry.definition_index != GLOVE_CT_SIDE &&
            selected_entry.definition_index != WEAPON_KNIFE &&
            selected_entry.definition_index != WEAPON_KNIFE_T)
        {
            selected_weapon_name = k_WeaponNamesPreview[definition_vector_index].name;
        }
        else
        {
            if (selected_entry.definition_index == GLOVE_T_SIDE ||
                selected_entry.definition_index == GLOVE_CT_SIDE)
            {
                selected_weapon_name = k_GloveNamesPreview.at(selected_entry.definition_override_vector_index).name;
            }
            if (selected_entry.definition_index == WEAPON_KNIFE ||
                selected_entry.definition_index == WEAPON_KNIFE_T)
            {
                selected_weapon_name = k_KnifeNamesPreview.at(selected_entry.definition_override_vector_index).name;
            }
        }

        if (selected_tab_skins == 0)
        {
            ////if (ImGui::Button("Current", ImVec2(-1, 19)))
            //	PickCurrentWeapon(&selected_entry.definition_index, &selected_entry.definition_vector_index, k_weapon_names);
            //
            //int iter = 0;
            //for (auto& weapon : k_weapon_names) {
            //	const char fufu = *weapon.name.c_str();
            //
            //	//if (ImGui::ButtonT(&fufu, ImVec2(-1, 19), selected_entry.paint_kit_index, weapon.definition_index, false, false)) {
            //		selected_entry.definition_index = weapon.definition_index;
            //		selected_entry.definition_vector_index = iter;
            //	//}
            //	iter++;
            //}

            //ImGui::Checkbox12("Enabled##skinschanger" ,&selected_entry.enableskinchanger);
            ImGui::Checkbox("skin preview", &g_Configurations.skin_preview);
            ImGui::Checkbox("stattrak##21312", &selected_entry.stat_trak);
            //if (ImGui::ItemsToolTipBegin("##stat_trak_live"))
            //{
            //	ImGui::Checkbox12("live statrak", &settings::changers::skin::live_stat_trak);
            //	ImGui::ItemsToolTipEnd();
            //}
            ImGui::InputInt("seed", &selected_entry.seed);
            ImGui::InputInt("stattrak##1312321", &satatt.statrack_new.counter);
            ImGui::SliderFloat("wear", &selected_entry.wear, FLT_MIN, 1.f, "%.2f");

            ////Sticker
            //ImGui::PushID("sticker");
            //
            //static auto selected_sticker_slot = 0;
            //
            //auto& selected_sticker = selected_entry.stickers[selected_sticker_slot];
            //
            //ImGui::PushItemWidth(-1);
            //
            //char element_name[64];
            //
            //ImGui::ListBox("", &selected_sticker_slot, [&selected_entry, &element_name](int idx)
            //	{
            //		auto kit_vector_index = selected_entry.stickers[idx].kit_vector_index;
            //		sprintf_s(element_name, "#%d (%s)", idx + 1, k_stickers.at(kit_vector_index).name.c_str());
            //		return element_name;
            //	}, 5, 5);
            //ImGui::PopItemWidth();
            //
            //ImGui::NextColumn();
            //
            //ImGui::Combo("Sticker Kit", &selected_sticker.kit_vector_index, [](void* data, int idx, const char** out_text)
            //	{
            //		*out_text = k_stickers.at(idx).name.c_str();
            //		return true;
            //	}, nullptr, k_stickers.size(), 10);
            //
            ////End Sticker

            if (selected_entry.definition_index == WEAPON_KNIFE || selected_entry.definition_index == WEAPON_KNIFE_T)
            {
                ImGui::ListBoxHeader("knife##sdsdadsdadas", ImVec2(0, 152));
                {
                    for (int i = 0; i < k_KnifeNames.size(); i++)
                    {
                        if (ImGui::Selectable(k_KnifeNames[i].name.c_str(), selected_entry.definition_override_vector_index == i))
                        {
                            selected_entry.definition_override_vector_index = i;

                            selected_skin_name = "";
                        }
                    }

                    selected_entry.definition_override_index = k_KnifeNames.at(selected_entry.definition_override_vector_index).definition_index;
                }
                ImGui::ListBoxFooter();
            }
            else if (selected_entry.definition_index == GLOVE_T_SIDE || selected_entry.definition_index == GLOVE_CT_SIDE)
            {
                ImGui::ListBoxHeader("glove##sdsdadsdadas", ImVec2(0, 152));
                {
                    for (int i = 0; i < k_GloveNames.size(); i++)
                    {
                        if (ImGui::Selectable(k_GloveNames[i].name.c_str(), selected_entry.definition_override_vector_index == i))
                        {
                            selected_entry.definition_override_vector_index = i;
                        }
                    }

                    selected_entry.definition_override_index = k_GloveNames.at(selected_entry.definition_override_vector_index).definition_index;
                }
                ImGui::ListBoxFooter();
            }
            else {
                static auto unused_value = 0;
                selected_entry.definition_override_vector_index = 0;
            }

            char wpnname[32];
            ImGui::InputText("Name##21312", selected_entry.custom_name, sizeof(wpnname));
        }

        else if (selected_tab_skins == 1)
        {
            if (SkinsParsed)
            {
                static char filter_name[32];
                std::string filter = filter_name;

                bool is_glove = selected_entry.definition_index == GLOVE_T_SIDE ||
                    selected_entry.definition_index == GLOVE_CT_SIDE;

                bool is_knife = selected_entry.definition_index == WEAPON_KNIFE ||
                    selected_entry.definition_index == WEAPON_KNIFE_T;

                int cur_weapidx = 0;
                if (!is_glove && !is_knife)
                {
                    cur_weapidx = k_WeaponNames[definition_vector_index].definition_index;
                    //selected_weapon_name = k_weapon_names_preview[definition_vector_index].name;
                }
                else
                {
                    if (selected_entry.definition_index == GLOVE_T_SIDE ||
                        selected_entry.definition_index == GLOVE_CT_SIDE)
                    {
                        cur_weapidx = k_GloveNames.at(selected_entry.definition_override_vector_index).definition_index;
                    }
                    if (selected_entry.definition_index == WEAPON_KNIFE ||
                        selected_entry.definition_index == WEAPON_KNIFE_T)
                    {
                        cur_weapidx = k_KnifeNames.at(selected_entry.definition_override_vector_index).definition_index;

                    }
                }

                ImGui::InputText("name filter [?]", filter_name, sizeof(filter_name));
                if (ImGui::ItemsToolTipBegin("##skinfilter"))
                {
                    ImGui::Checkbox("show skins for selected weapon", &g_Configurations.show_cur);
                    ImGui::ItemsToolTipEnd();
                }

                auto weaponName = WeaponNames(cur_weapidx);

                ImGui::ListBoxHeader("skins##sdsdadsdadas", ImVec2(0, 261));
                {
                    if (selected_entry.definition_index != GLOVE_T_SIDE && selected_entry.definition_index != GLOVE_CT_SIDE)
                    {
                        if (ImGui::Selectable(" - ", selected_entry.paint_kit_index == -1))
                        {
                            selected_entry.paint_kit_vector_index = -1;
                            selected_entry.paint_kit_index = -1;
                            selected_skin_name = "";
                        }

                        int lastID = ImGui::GetItemID();
                        for (size_t w = 0; w < k_Skins.size(); w++)
                        {
                            for (auto names : k_Skins[w].WeaponName)
                            {
                                std::string name = k_Skins[w].Name;

                                if (g_Configurations.show_cur)
                                {
                                    if (names != weaponName)
                                        continue;
                                }

                                if (name.find(filter) != name.npos)
                                {
                                    ImGui::PushID(lastID++);

                                    ImGui::PushStyleColor(ImGuiCol_Text, skins::GetColorRatiry(is_knife && g_Configurations.show_cur ? 6 : k_Skins[w].Rarity));
                                    {
                                        if (ImGui::Selectable(name.c_str(), selected_entry.paint_kit_vector_index == w))
                                        {
                                            selected_entry.paint_kit_vector_index = w;
                                            selected_entry.paint_kit_index = k_Skins[selected_entry.paint_kit_vector_index].Id;
                                            selected_skin_name = k_Skins[w].NameShort;
                                        }
                                    }
                                    ImGui::PopStyleColor();

                                    ImGui::PopID();
                                }
                            }
                        }
                    }
                    else
                    {
                        int lastID = ImGui::GetItemID();

                        if (ImGui::Selectable(" - ", selected_entry.paint_kit_index == -1))
                        {
                            selected_entry.paint_kit_vector_index = -1;
                            selected_entry.paint_kit_index = -1;
                            selected_skin_name = "";
                        }

                        for (size_t w = 0; w < k_Gloves.size(); w++)
                        {
                            for (auto names : k_Gloves[w].WeaponName)
                            {
                                std::string name = k_Gloves[w].Name;
                                //name += " | ";
                                //name += names;

                                if (g_Configurations.show_cur)
                                {
                                    if (names != weaponName)
                                        continue;
                                }

                                if (name.find(filter) != name.npos)
                                {
                                    ImGui::PushID(lastID++);

                                    ImGui::PushStyleColor(ImGuiCol_Text, skins::GetColorRatiry(6));
                                    {
                                        if (ImGui::Selectable(name.c_str(), selected_entry.paint_kit_vector_index == w))
                                        {
                                            selected_entry.paint_kit_vector_index = w;
                                            selected_entry.paint_kit_index = k_Gloves[selected_entry.paint_kit_vector_index].Id;
                                            selected_skin_name = k_Gloves[selected_entry.paint_kit_vector_index].NameShort;
                                        }
                                    }
                                    ImGui::PopStyleColor();

                                    ImGui::PopID();
                                }
                            }
                        }
                    }
                }
                ImGui::ListBoxFooter();
            }
            else
            {
                ImGui::Text("skins parsing, wait...");
            }
        }
    }
    ImGui::EndChild("skin select");
}

static int weapon_index = 7;
static int weapon_vector_index = 0;

struct WeaponNameT
{
    constexpr WeaponNameT(int32_t definition_index, const char* name) : definition_index(definition_index), name(name) {}

    int32_t definition_index = 0;
    const char* name = nullptr;
};

std::vector <WeaponNameT> WeaponNamesA =
{
    {61, "USP-S"},
    {32, "P2000"},
    {4, "Glock-18"},
    {2, "Dual berettas"},
    {36, "P250"},
    {3, "Five-Seven"},
    {30, "Tec-9"},
    {63, "CZ75A"},
    {64, "R8 revolver"},
    {1, "Deagle"},

    {34, "MP9"},
    {17, "Mac-10"},
    {23, "MP5-SD"},
    {33, "MP7"},
    {24, "UMP-45"},
    {19, "P90"},
    {26, "PP-Bizon"},

    {7, "AK-47"},
    {60, "M4A1-S"},
    {16, "M4A4"},
    {8, "AUG"},
    {39, "SG553"},
    {10, "Famas"},
    {13, "Galil"},

    {40, "SSG-08"},
    {38, "Scar-20"},
    {9,  "AWP"},
    {11, "G3SG1"},

    {14, "M249"},
    {28, "Negev"},

    {27, "Mag-7"},
    {35, "Nova"},
    {29, "Sawed-off"},
    {25, "XM1014"},
};

void RenderCurrentWeaponButton()
{
    if (!g_EngineClient->IsConnected() || !g_LocalPlayer || !g_LocalPlayer->IsAlive())
        return;

    auto weapon = g_LocalPlayer->m_hActiveWeapon();
    if (!weapon)
        return;

    short wpn_idx = weapon->m_Item().m_iItemDefinitionIndex();

    auto wpn_it = std::find_if(WeaponNamesA.begin(), WeaponNamesA.end(), [wpn_idx](const WeaponNameT& a)
    {
        return a.definition_index == wpn_idx;
    });

    if (wpn_it != WeaponNamesA.end())
    {
        weapon_index = wpn_idx;
        weapon_vector_index = std::abs(std::distance(WeaponNamesA.begin(), wpn_it));
    }
}

void RenderAimbotTab()
{
    auto settings = &g_Configurations.legitbot_items[weapon_index];

	ImGui::BeginGroup();
	{
        ImGui::Columns(3, nullptr, false);

        ImGui::PushItemWidth(220);
        ImGui::Text("Weapon");
        if (ImGui::Combo("##weapon", &weapon_vector_index, [](void* data, int32_t idx, const char** out_text)
           {auto vec = reinterpret_cast<std::vector <WeaponNameT>*>(data);	*out_text = vec->at(idx).name; return true; },
            (void*)(&WeaponNamesA), WeaponNamesA.size()))
        {
            weapon_index = WeaponNamesA[weapon_vector_index].definition_index;
        }

        RenderCurrentWeaponButton();

        ImGui::Checkbox("Enabled", &settings->enabled);
        ImGui::Checkbox("On Key", &settings->on_key);
        if (settings->on_key) 
        {
            ImGui::SameLine();
            ImGuiEx::Hotkey("##Key bind", &settings->key, ImVec2(100, 20));
        }

        ImGui::Checkbox("Deathmatch", &settings->deathmatch);
        if (weapon_index == WEAPON_P250 || weapon_index == WEAPON_USP_SILENCER || weapon_index == WEAPON_GLOCK ||
            weapon_index == WEAPON_FIVESEVEN || weapon_index == WEAPON_TEC9 || weapon_index == WEAPON_DEAGLE ||
            weapon_index == WEAPON_ELITE || weapon_index == WEAPON_HKP2000) 
        {
            ImGui::Checkbox("Autopistol", &settings->autopistol);
        }

        ImGui::Checkbox("Ignore Wall", &settings->autowall);

        ImGui::Checkbox("Silent", &settings->silent);
        if (weapon_index == WEAPON_AWP || weapon_index == WEAPON_SSG08 ||
            weapon_index == WEAPON_G3SG1 || weapon_index == WEAPON_SCAR20) 
        {
            ImGui::Checkbox("Only In Zoom", &settings->only_in_zoom);
        }

        ImGui::Checkbox("Auto Fire", &settings->autofire);
        if (settings->autofire) 
        {
            ImGui::SameLine();
            ImGuiEx::Hotkey("##Auto Fire bind", &settings->autofire_key, ImVec2(100, 20));
        }

        if (settings->autofire)
            ImGui::Checkbox("Auto Stop", &settings->autostop);

        ImGui::Checkbox("Smoke check", &settings->smoke_check);
        ImGui::Checkbox("Flash check", &settings->flash_check);
        ImGui::Checkbox("Jump check", &settings->jump_check);

        ImGui::NextColumn();

        ImGui::PushItemWidth(220);
        ImGui::Text("Aim Type:");
        ImGui::Combo("##aimbot.aim_type", &settings->aim_type, aim_types, IM_ARRAYSIZE(aim_types));

        if (settings->aim_type == 0) 
        {
            ImGui::Text("Hitbox:");
            ImGui::Combo("##aimbot.hitbox", &settings->hitbox, hitbox_list, IM_ARRAYSIZE(hitbox_list));
        }

        ImGui::Text("Priority:");
        ImGui::Combo("##aimbot.priority", &settings->priority, priorities, IM_ARRAYSIZE(priorities));
        ImGui::PopItemWidth();
        ImGui::SliderFloat("FOV", &settings->fov, 0, 30, "%.1f");

        if (settings->silent)
            ImGui::SliderFloat("Fov Silent", &settings->silent_fov, 0, 20, "%.1f");

        ImGui::SliderFloat("Smooth", &settings->smooth, 1, 20, "%.1f");

        if (settings->autowall)
            ImGui::SliderInt("Min Damage", &settings->min_damage, 1, 100, "%.0f");

        ImGui::SliderFloat("Backtrack", &settings->backtrack_time, 0.f, 0.2f, "%.2f");
        ImGui::SliderInt("Shot Delay", &settings->shot_delay, 0, 100, "%.0f");
        ImGui::SliderInt("Kill Delay", &settings->kill_delay, 0, 1000, "%.0f");

        ImGui::NextColumn();

        ImGui::Checkbox("Enabled RCS##aimbot.rcs", &settings->rcs);

        ImGui::PushItemWidth(220);
        ImGui::Text("Type");
        ImGui::Combo("##type", &settings->rcs_type, rcs_types, IM_ARRAYSIZE(rcs_types));

        ImGui::Checkbox("RCS Custom Fov", &settings->rcs_fov_enabled);
        ImGui::Checkbox("RCS Custom Smooth", &settings->rcs_smooth_enabled);

        ImGui::SliderInt("RCS Start", &settings->rcs_start, 1, 30, "%.0f shots");
        ImGui::SliderInt("RCS X", &settings->rcs_x, 0, 100, "%.0f");
        ImGui::SliderInt("RCS Y", &settings->rcs_y, 0, 100, "%.0f");

        if (settings->rcs_fov_enabled)
            ImGui::SliderFloat("RCS Fov", &settings->rcs_fov, 0, 20, "%.2f");

        if (settings->rcs_smooth_enabled)
            ImGui::SliderFloat("RCS Smooth", &settings->rcs_smooth, 1, 15, "%.2f");

        ImGui::Columns(1, nullptr, false);
	}
	ImGui::EndGroup();
}

void RenderConfigTab()
{
    static std::vector<std::string> configs;

    static auto loadConfigs = []()
    {
        std::vector<std::string> items = {};

        std::string path("C:\\csgo_sdk");
        if (!std::filesystem::is_directory(path))
            std::filesystem::create_directories(path);

        for (auto& p : std::filesystem::directory_iterator(path))
            items.push_back(p.path().string().substr(path.length() + 1));

        return items;
    };

    static auto is_configs_loaded = false;

    if (!is_configs_loaded)
    {
        is_configs_loaded = true;
        configs = loadConfigs();
    }

    static std::string current_config;

    static char config_name[32];

    static auto subtab = 0;

    ImGui::BeginGroup();
    {
        ImGui::Columns(2, nullptr, false);

        time_t mod_time{};
        for (auto& config : configs) 
        {
            struct stat result;
            auto file_path = "C:\\csgo_sdk\\" + config;

            if (stat(file_path.c_str(), &result) == 0)
                mod_time = result.st_mtime;

            auto file_creation_date = "Created at: " + timestamp_to_date(mod_time);
            if (ImGuiEx::Config_Button(config.c_str(), file_creation_date.c_str(), config == current_config, ImVec2(220, 40))) 
            {
                current_config = config;
            }
        }

        ImGui::NextColumn();

        ImGui::Text("Config Name");
        ImGui::PushItemWidth(220);
        ImGuiEx::InputTextWithHint("##config_name", "Enter config name", config_name, sizeof(config_name));

        if (ImGui::Button("Create", ImVec2(220, 30))) 
        {
            current_config = std::string(config_name);

            CConfig::Get().Save(current_config + ".ini");
            is_configs_loaded = false;
            memset(config_name, 0, 32);
        }

        if (!current_config.empty()) 
        {
            if (ImGui::Button("Load", ImVec2(220, 30)))
            {
                CConfig::Get().Load(current_config);
            }

            if (ImGui::Button("Save", ImVec2(220, 30)))
            {
                CConfig::Get().Save(current_config);
            }

            if (ImGui::Button("Delete", ImVec2(220, 30)) && std::filesystem::remove("C:\\csgo_sdk\\" + current_config))
            {
                current_config.clear();
                is_configs_loaded = false;
            }
        }

        if (ImGui::Button("Refresh", ImVec2(220, 30)))
            is_configs_loaded = false;

        ImGui::Columns(1, nullptr, false);
    }
    ImGui::EndGroup();
}

void Menu::Initialize()
{
	CreateStyle();
    _visible = true;
}

void Menu::Shutdown()
{
    ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Menu::OnDeviceLost()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
    ImGui_ImplDX9_CreateDeviceObjects();
}

void Menu::Render()
{
    //ImGui::GetIO().MouseDrawCursor = _visible;

    if (!_visible)
        return;

    const auto sidebar_size = get_sidebar_size();
    static int active_sidebar_tab = 0;

    ImVec2 main_pos;
    ImVec2 main_size;

    ImGui::SetNextWindowSize(ImVec2{ 1085, 5 }, ImGuiSetCond_Once);

    if (ImGui::Begin("csgo_sdk_main", &_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar))
    {
        main_pos = ImGui::GetWindowPos();
        main_size = ImGui::GetWindowSize();

        ImGui::BeginGroup();
        {
            ImGuiEx::TextCentered("csgo_sdk");
        }
        ImGui::EndGroup();
        ImGui::End();
    }

    ImGui::SetNextWindowSize(ImVec2{ 1085, 600 }, ImGuiSetCond_Once);
    ImGui::SetNextWindowPos(ImVec2(main_pos.x, main_pos.y + 35.f));

    if (ImGui::Begin("csgo_sdk_sub", &_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        {
            ImGui::BeginGroup();
            {
                render_tabs(sidebar_tabs, active_sidebar_tab, get_sidebar_item_width(), get_sidebar_item_height(), 3, true);
            }
            ImGui::EndGroup();
        }
        ImGui::PopStyleVar();
        ImGui::Dummy(ImVec2(0,5));

        auto size = ImVec2{ 0.0f, sidebar_size.y };

        ImGui::BeginGroup();
        if (active_sidebar_tab == TAB_ESP)
        {
            RenderEspTab();
        }
        else if (active_sidebar_tab == TAB_GLOW)
        {
            RednerGlowTab();
        }
        else if (active_sidebar_tab == TAB_CHAMS)
        {
            RenderChamsTab();
        }
        else if (active_sidebar_tab == TAB_AIMBOT)
        {
            RenderAimbotTab();
        }
        else if (active_sidebar_tab == TAB_MISC)
        {
            RenderMiscTab();
        } 
        else if (active_sidebar_tab == TAB_SKINS)
        {
            RenderSkinsTab();
        }
        else if (active_sidebar_tab == TAB_CONFIG)
        {
            RenderConfigTab();
        }
        ImGui::EndGroup();

        ImGui::End();
    }

    if (g_Configurations.skin_preview && active_sidebar_tab == TAB_SKINS && g_Configurations.show_cur)
    {
        if ((selected_skin_name != old_name_skin) || (selected_weapon_name != old_name_weap))
        {
            std::string filename = selected_skin_name == "" ? "resource/flash/econ/weapons/base_weapons/" + std::string(selected_weapon_name) + ".png" : "resource/flash/econ/default_generated/" + std::string(selected_weapon_name) + "_" + std::string(selected_skin_name) + "_light_large.png";

            const auto handle = g_BaseFileSystem->open(filename.c_str(), "r", "GAME");
            if (handle)
            {
                int file_len = g_BaseFileSystem->size(handle);
                char* image = new char[file_len];

                g_BaseFileSystem->read(image, file_len, handle);
                g_BaseFileSystem->close(handle);

                D3DXCreateTextureFromFileInMemory(g_D3DDevice9, image, file_len, &m_skin_texture);

                delete[] image;

                old_name_skin = selected_skin_name;
                old_name_weap = selected_weapon_name;
            }
        }

        ImGui::SetNextWindowPos(ImVec2(main_pos.x + main_size.x + 5.f, main_pos.y + main_size.y + 5.f));
        ImGui::SetNextWindowSize(ImVec2{ 300, 250 });

        if (ImGui::Begin("Preview##window", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_RainbowTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
        {
            if (m_skin_texture)
            {
                ImGui::Image(m_skin_texture, { 256, 192 });
            }

            ImGui::End();
        }
    }
}

void Menu::Toggle()
{
    _visible = !_visible;
}

void Menu::CreateStyle()
{
	ImGui::StyleColorsDark();
	ImGui::SetColorEditOptions(ImGuiColorEditFlags_HEX);
	_style.FrameRounding = 0.f;
	_style.WindowRounding = 0.f;
	_style.ChildRounding = 0.f;
	_style.Colors[ImGuiCol_Button] = ImVec4(0.260f, 0.590f, 0.980f, 0.670f);
	_style.Colors[ImGuiCol_Header] = ImVec4(0.260f, 0.590f, 0.980f, 0.670f);
	_style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.260f, 0.590f, 0.980f, 1.000f);
	_style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.30f, 1.0f);
	_style.Colors[ImGuiCol_WindowBg] = ImVec4(0.000f, 0.009f, 0.120f, 0.940f);
	_style.Colors[ImGuiCol_PopupBg] = ImVec4(0.076f, 0.143f, 0.209f, 1.000f);
	ImGui::GetStyle() = _style;
}

