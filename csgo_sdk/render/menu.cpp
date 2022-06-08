#include "Menu.hpp"

#define NOMINMAX

#include <Windows.h>
#include <chrono>
#include <filesystem>

#include "../sdk/csgostructs.hpp"
#include "../sdk/utils/input.hpp"
#include "../configurations.hpp"
#include "../config.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS

#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

#include "../functions/misc.hpp"

extern ImFont* g_pDefaultFont;

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
}

template<size_t N>
void render_tabs(char* (&names)[N], int& activetab, float w, float h, bool sameline)
{
    for(auto i = 0; i < N; ++i)
    {
        if(ImGui::Button(names[i], ImVec2{ w, h })) 
        {
            activetab = i;
        }

        if(sameline && i < N - 1)
            ImGui::SameLine();
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
        ImGui::Checkbox("Weapon", &g_Configurations.esp_player_weapons);
        ImGui::Checkbox("Snaplines", &g_Configurations.esp_player_snaplines);

        ImGui::NextColumn();

        ImGui::Checkbox("Crosshair", &g_Configurations.esp_crosshair);
        ImGui::Checkbox("Dropped Weapons", &g_Configurations.esp_dropped_weapons);
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

		if(&g_Configurations.misc_thirdperson)
			ImGui::SliderFloat("Distance", &g_Configurations.misc_thirdperson_dist, 0.f, 150.f);

        ImGui::Checkbox("No hands", &g_Configurations.misc_no_hands);
		ImGui::Checkbox("Rank reveal", &g_Configurations.misc_showranks);
		ImGui::Checkbox("Watermark", &g_Configurations.misc_watermark);

		ImGui::NextColumn();

        ImGui::SliderInt("Viewmodel FOV:", &g_Configurations.misc_viewmodel_fov, 68, 120);
		ImGui::Text("Postprocessing:");
        ImGui::SliderFloat("Red", &g_Configurations.misc_mat_ambient_light_r, 0, 1);
        ImGui::SliderFloat("Green", &g_Configurations.misc_mat_ambient_light_g, 0, 1);
        ImGui::SliderFloat("Blue", &g_Configurations.misc_mat_ambient_light_b, 0, 1);

        ImGui::Columns(1, nullptr, false);
    }
    ImGui::EndGroup();
}

static int weapon_index = 7;
static int weapon_vector_index = 0;

struct WeaponNameT
{
    constexpr WeaponNameT(int32_t definition_index, const char* name) : definition_index(definition_index), name(name) {}

    int32_t definition_index = 0;
    const char* name = nullptr;
};

std::vector <WeaponNameT> WeaponNames =
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

    auto wpn_it = std::find_if(WeaponNames.begin(), WeaponNames.end(), [wpn_idx](const WeaponNameT& a)
    {
        return a.definition_index == wpn_idx;
    });

    if (wpn_it != WeaponNames.end())
    {
        weapon_index = wpn_idx;
        weapon_vector_index = std::abs(std::distance(WeaponNames.begin(), wpn_it));
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
            (void*)(&WeaponNames), WeaponNames.size())) 
        {
            weapon_index = WeaponNames[weapon_vector_index].definition_index;
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
    ImGui::GetIO().MouseDrawCursor = _visible;

    if (!_visible)
        return;

    const auto sidebar_size = get_sidebar_size();
    static int active_sidebar_tab = 0;

    if (ImGui::Begin("csgo_sdk", &_visible, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        {
            ImGui::BeginGroup();
            {
                render_tabs(sidebar_tabs, active_sidebar_tab, get_sidebar_item_width(), get_sidebar_item_height(), false);
            }
            ImGui::EndGroup();
        }
        ImGui::PopStyleVar();
        ImGui::SameLine();

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
        else if (active_sidebar_tab == TAB_CONFIG)
        {
            RenderConfigTab();
        }
        ImGui::EndGroup();

        ImGui::End();
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

