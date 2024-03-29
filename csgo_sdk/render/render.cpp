#include "render.hpp"

#include <mutex>

#include "../functions/visuals.hpp"

#include "../sdk/csgostructs.hpp"
#include "../sdk/utils/input.hpp"
#include "../sdk/utils/math.hpp"

#include "menu.hpp"

#include "../configurations.hpp"
#include "../fonts/fonts.hpp"

ImFont* g_pDefaultFont;
ImFont* g_pSecondFont;
ImFont* g_IconEsp;
ImFont* g_SmallFont;

ImDrawListSharedData _data;

std::mutex render_mutex;

void Render::Initialize()
{
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(InputSys::Get().GetMainWindow());
	ImGui_ImplDX9_Init(g_D3DDevice9);

	draw_list = new ImDrawList(ImGui::GetDrawListSharedData());
	draw_list_act = new ImDrawList(ImGui::GetDrawListSharedData());
	draw_list_rendering = new ImDrawList(ImGui::GetDrawListSharedData());

	GetFonts();
}

void Render::GetFonts() 
{
	// menu font
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Fonts::Droid_compressed_data, Fonts::Droid_compressed_size, 14.f);
	
	// esp font
	g_pDefaultFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Fonts::Droid_compressed_data, Fonts::Droid_compressed_size, 18.f);
	
	// font for watermark; just example
	g_pSecondFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF( Fonts::Cousine_compressed_data, Fonts::Cousine_compressed_size, 18.f);

	// Weapon Icons font
	g_IconEsp = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
		Fonts::undefeated_compressed_data,
		Fonts::undefeated_compressed_size,
		16.f,
		nullptr,
		ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

	g_SmallFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
		Fonts::Droid_compressed_data,
		Fonts::Droid_compressed_size,
		12.f,
		nullptr,
		ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
}

void Render::ClearDrawList() 
{
	render_mutex.lock();
	draw_list_act->Clear();
	render_mutex.unlock();
}

void Render::BeginScene() 
{
	draw_list->Clear();
	draw_list->PushClipRectFullScreen();

	if (g_Configurations.misc_watermark)
		Render::Get().RenderText("csgo_sdk", 10, 5, 18.f, (Color)g_Configurations.color_watermark, false, true, g_pSecondFont);

	if (g_EngineClient->IsInGame() && g_LocalPlayer && g_Configurations.esp_enabled)
		Visuals::Get().AddToDrawList();

	if (g_EngineClient->IsInGame() && g_LocalPlayer && g_Configurations.esp_draw_weapon_fov) 
		Visuals::Get().DrawFOV();

	render_mutex.lock();
	*draw_list_act = *draw_list;
	render_mutex.unlock();
}

ImDrawList* Render::RenderScene() 
{
	if (render_mutex.try_lock()) 
	{
		*draw_list_rendering = *draw_list_act;
		render_mutex.unlock();
	}

	return draw_list_rendering;
}

float Render::RenderText(const std::string& text, ImVec2 pos, float size, Color color, bool center, bool outline, ImFont* pFont)
{
	ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());

	if (!pFont->ContainerAtlas) 
		return 0.f;

	draw_list->PushTextureID(pFont->ContainerAtlas->TexID);

	if (center)
		pos.x -= textSize.x / 2.0f;

	if (outline) 
	{
		draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y + 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
		draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y - 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
		draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y - 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
		draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y + 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
	}

	draw_list->AddText(pFont, size, pos, GetU32(color), text.c_str());
	draw_list->PopTextureID();

	return pos.y + textSize.y;
}

void Render::RenderCircle3D(Vector position, float points, float radius, Color color)
{
	float step = (float)M_PI * 2.0f / points;

	for (float a = 0; a < (M_PI * 2.0f); a += step)
	{
		Vector start(radius * cosf(a) + position.x, radius * sinf(a) + position.y, position.z);
		Vector end(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y, position.z);

		Vector start2d, end2d;
		if (g_DebugOverlay->ScreenPosition(start, start2d) || g_DebugOverlay->ScreenPosition(end, end2d))
			return;

		RenderLine(start2d.x, start2d.y, end2d.x, end2d.y, color);
	}
}
void Render::CircularProgressBar(int x, int y, int r1, int r2, int s, int d, Color col, bool inverse)

{

	for (int i = s; i < s + d; i++)

	{

		auto rad = i * M_PI / 180;

		if (!inverse)

			RenderLine(x + cos(rad) * r1, y + sin(rad) * r1, x + cos(rad) * r2, y + sin(rad) * r2, col);

		else

			RenderLine(x - sin(rad) * r1, y - cos(rad) * r1, x - sin(rad) * r2, y - cos(rad) * r2, col);

	}

}