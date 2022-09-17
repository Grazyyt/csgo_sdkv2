#include "hooks.hpp"
#include <intrin.h>  

#include "render/render.hpp"
#include "render/menu.hpp"
#include "configurations.hpp"
#include "sdk/utils/input.hpp"
#include "sdk/utils/utils.hpp"
#include "functions/chams.hpp"
#include "functions/visuals.hpp"
#include "functions/glow.hpp"
#include "functions/misc.hpp"
#include "functions/prediction.hpp"
#include "functions/grenade_pred.hpp"
#include "functions/aimbot.hpp"
#include "functions/backtrack.hpp"
#include "functions/knifechanger.hpp"
#include "helpers/eventlistener.h"

#pragma intrinsic(_ReturnAddress)

void anti_cheat_fix()
{
	const char* modules[] { "client.dll", "engine.dll", "server.dll", "studiorender.dll", "materialsystem.dll", "shaderapidx9.dll", "vstdlib.dll", "vguimatsurface.dll" };
	long long long_long = 0x69690004C201B0;
	for (auto test : modules)
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)Utils::PatternScan(GetModuleHandleA(test), "55 8B EC 56 8B F1 33 C0 57 8B 7D 08"), &long_long, 7, 0);
}

namespace Hooks 
{
	void Initialize()
	{
		hlclient_hook.setup(g_CHLClient);
		direct3d_hook.setup(g_D3DDevice9);
		vguipanel_hook.setup(g_VGuiPanel);
		vguisurf_hook.setup(g_VGuiSurface);
		sound_hook.setup(g_EngineSound);
		mdlrender_hook.setup(g_MdlRender);
		clientmode_hook.setup(g_ClientMode);
		ConVar* sv_cheats_con = g_CVar->FindVar("sv_cheats");
		sv_cheats.setup(sv_cheats_con);
		gameevents_vhook.setup(g_GameEvents);

		g_CustomEventsManager.Init();

		direct3d_hook.hook_index(index::EndScene, hkEndScene);
		direct3d_hook.hook_index(index::Reset, hkReset);
		hlclient_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
		hlclient_hook.hook_index(index::CreateMove, hkCreateMove_Proxy);
		vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);
		sound_hook.hook_index(index::EmitSound1, hkEmitSound1);
		vguisurf_hook.hook_index(index::LockCursor, hkLockCursor);
		mdlrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);
		clientmode_hook.hook_index(index::DoPostScreenSpaceEffects, hkDoPostScreenEffects);
		clientmode_hook.hook_index(index::OverrideView, hkOverrideView);
		sv_cheats.hook_index(index::SvCheatsGetBool, hkSvCheatsGetBool);
		clientmode_hook.hook_index(index::ShouldDrawFog, hkShouldDrawFog);
		gameevents_vhook.hook_index(index::FireEvent, hkFireEvent);
		sequence_vhook = new RecvPropHook(C_BaseViewModel::m_nSequence(), hkRecvProxy);

		anti_cheat_fix();
	}

	void Shutdown()
	{
		hlclient_hook.unhook_all();
		direct3d_hook.unhook_all();
		vguipanel_hook.unhook_all();
		vguisurf_hook.unhook_all();
		mdlrender_hook.unhook_all();
		clientmode_hook.unhook_all();
		sound_hook.unhook_all();
		sv_cheats.unhook_all();
		sequence_vhook->~RecvPropHook();

		g_CustomEventsManager.Unload();

		Glow::Get().Shutdown();
	}

	long __stdcall hkEndScene(IDirect3DDevice9* pDevice)
	{
		static auto oEndScene = direct3d_hook.get_original<decltype(&hkEndScene)>(index::EndScene);

		static auto viewmodel_fov = g_CVar->FindVar("viewmodel_fov");
		static auto mat_ambient_light_r = g_CVar->FindVar("mat_ambient_light_r");
		static auto mat_ambient_light_g = g_CVar->FindVar("mat_ambient_light_g");
		static auto mat_ambient_light_b = g_CVar->FindVar("mat_ambient_light_b");
		static auto crosshair_cvar = g_CVar->FindVar("crosshair");

		viewmodel_fov->m_fnChangeCallbacks.m_Size = 0;
		viewmodel_fov->SetValue(g_Configurations.misc_viewmodel_fov);
		mat_ambient_light_r->SetValue(g_Configurations.misc_mat_ambient_light_r);
		mat_ambient_light_g->SetValue(g_Configurations.misc_mat_ambient_light_g);
		mat_ambient_light_b->SetValue(g_Configurations.misc_mat_ambient_light_b);
		
		crosshair_cvar->SetValue(!(g_Configurations.esp_enabled && g_Configurations.esp_crosshair));

		DWORD colorwrite, srgbwrite;
		IDirect3DVertexDeclaration9* vert_dec = nullptr;
		IDirect3DVertexShader9* vert_shader = nullptr;
		DWORD dwOld_D3DRS_COLORWRITEENABLE = NULL;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
		pDevice->GetVertexDeclaration(&vert_dec);
		pDevice->GetVertexShader(&vert_shader);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);
	
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		auto esp_drawlist = Render::Get().RenderScene();

		Menu::Get().Render();
	
		ImGui::Render(esp_drawlist);

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
		pDevice->SetVertexDeclaration(vert_dec);
		pDevice->SetVertexShader(vert_shader);

		return oEndScene(pDevice);
	}

	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static auto oReset = direct3d_hook.get_original<decltype(&hkReset)>(index::Reset);

		Menu::Get().OnDeviceLost();

		auto hr = oReset(device, pPresentationParameters);

		if (hr >= 0)
			Menu::Get().OnDeviceReset();

		return hr;
	}

	void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
	{
		static auto oCreateMove = hlclient_hook.get_original<decltype(&hkCreateMove_Proxy)>(index::CreateMove);

		oCreateMove(g_CHLClient, 0, sequence_number, input_sample_frametime, active);

		auto cmd = g_Input->GetUserCmd(sequence_number);
		auto verified = g_Input->GetVerifiedCmd(sequence_number);
		auto flags = g_LocalPlayer->m_fFlags();

		if (!cmd || !cmd->command_number)
			return;
		
		if (Menu::Get().IsVisible())
			cmd->buttons &= ~IN_ATTACK;

		if (g_Configurations.misc_bhop)
			Misc::Get().Bhop(cmd);

		if (g_Configurations.misc_autostrafe)
			Misc::Get().AutoStrafe(cmd);

		if (g_Configurations.misc_showranks && cmd->buttons & IN_SCORE)
			g_CHLClient->DispatchUserMessage(CS_UM_ServerRankRevealAll, 0, 0, nullptr);

		Engine_Prediction::Get().Begin(cmd);
		{
			Misc::Get().JumpBug(cmd);
			LegitBot::Get().OnMove(cmd);
			Backtrack::Get().OnMove(cmd);

			if (g_Configurations.esp_grenade_prediction)
				Grenade_Pred::Get().Trace(cmd);
		}
		Engine_Prediction::Get().End();

		if (g_Configurations.misc_edgejump && InputSys::Get().IsKeyDown(g_Configurations.misc_edgejumpkey))
		{
			if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND) && (flags & FL_ONGROUND))
				cmd->buttons |= IN_JUMP;
		}

		verified->m_cmd = *cmd;
		verified->m_crc = cmd->GetChecksum();
	}

	__declspec(naked) void __fastcall hkCreateMove_Proxy(void* _this, int, int sequence_number, float input_sample_frametime, bool active)
	{
		__asm
		{
			push ebp
			mov  ebp, esp
			push ebx; not sure if we need this
			push esp
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call Hooks::hkCreateMove
			pop  ebx
			pop  ebp
			retn 0Ch
		}
	}

	void __fastcall hkPaintTraverse(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce)
	{
		static auto panelId = vgui::VPANEL{ 0 };
		static auto oPaintTraverse = vguipanel_hook.get_original<decltype(&hkPaintTraverse)>(index::PaintTraverse);

		oPaintTraverse(g_VGuiPanel, edx, panel, forceRepaint, allowForce);

		if (!panelId) {
			const auto panelName = g_VGuiPanel->GetName(panel);
			if (!strcmp(panelName, "FocusOverlayPanel")) {
				panelId = panel;
			}
		}
		else if (panelId == panel) 
		{
			static bool bSkip = false;
			bSkip = !bSkip;

			if (bSkip)
				return;

			Render::Get().BeginScene();
		}
	}

	void __fastcall hkEmitSound1(void* _this, int edx, IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk) {
		static auto ofunc = sound_hook.get_original<decltype(&hkEmitSound1)>(index::EmitSound1);


		if (!strcmp(pSoundEntry, "UIPanorama.popup_accept_match_beep")) {
			static auto fnAccept = reinterpret_cast<bool(__stdcall*)(const char*)>(Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12"));

			if (fnAccept) {

				fnAccept("");

				FLASHWINFO fi;
				fi.cbSize = sizeof(FLASHWINFO);
				fi.hwnd = InputSys::Get().GetMainWindow();
				fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
				fi.uCount = 0;
				fi.dwTimeout = 0;
				FlashWindowEx(&fi);
			}
		}

		ofunc(g_EngineSound, edx, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unk);

	}

	int __fastcall hkDoPostScreenEffects(void* _this, int edx, int a1)
	{
		static auto oDoPostScreenEffects = clientmode_hook.get_original<decltype(&hkDoPostScreenEffects)>(index::DoPostScreenSpaceEffects);

		if (g_LocalPlayer && g_Configurations.glow_enabled)
			Glow::Get().Run();

		return oDoPostScreenEffects(g_ClientMode, edx, a1);
	}

	void __stdcall hkFrameStageNotify(ClientFrameStage_t stage)
	{
		static auto ofunc = hlclient_hook.get_original<void(__thiscall*)(IBaseClientDLL*, ClientFrameStage_t)>(index::FrameStageNotify);

		if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		{
			if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
				skins::knifes::UpdateKnife();
		}

		Visuals::Get().Nightmode();

		return ofunc(g_CHLClient, stage);
	}

	void __fastcall hkOverrideView(void* _this, int edx, CViewSetup* vsView)
	{
		static auto ofunc = clientmode_hook.get_original<decltype(&hkOverrideView)>(index::OverrideView);

		if (g_EngineClient->IsInGame() && vsView)
			Visuals::Get().ThirdPerson();

		ofunc(g_ClientMode, edx, vsView);
	}

	void __fastcall hkLockCursor(void* _this)
	{
		static auto ofunc = vguisurf_hook.get_original<decltype(&hkLockCursor)>(index::LockCursor);

		if (Menu::Get().IsVisible()) {
			g_VGuiSurface->UnlockCursor();
			g_InputSystem->ResetInputState();
			return;
		}
		ofunc(g_VGuiSurface);

	}

	void __fastcall hkDrawModelExecute(void* _this, int edx, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		static auto ofunc = mdlrender_hook.get_original<decltype(&hkDrawModelExecute)>(index::DrawModelExecute);

		if (g_MdlRender->IsForcedMaterialOverride() &&
			!strstr(pInfo.pModel->szName, "arms") &&
			!strstr(pInfo.pModel->szName, "weapons/v_")) {
			return ofunc(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);
		}

		Chams::Get().OnDrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);

		ofunc(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);

		g_MdlRender->ForcedMaterialOverride(nullptr);
	}

	bool __fastcall hkSvCheatsGetBool(PVOID pConVar, void* edx)
	{
		static auto dwCAM_Think = Utils::PatternScan(GetModuleHandleW(L"client.dll"), "85 C0 75 30 38 86");
		static auto ofunc = sv_cheats.get_original<bool(__thiscall *)(PVOID)>(13);
		if (!ofunc)
			return false;

		if (reinterpret_cast<DWORD>(_ReturnAddress()) == reinterpret_cast<DWORD>(dwCAM_Think))
			return true;
		return ofunc(pConVar);
	}
	bool __fastcall hkShouldDrawFog(void* ecx, void* edx) {
		return !g_Configurations.misc_no_fog;
	}
	bool __fastcall hkSendNetMsg(void* ecx, void* edx, INetMessage* msg, bool reliable, bool voice)
	{
		static auto oSNMEvent = netchannel_vhook.get_original<sendnetmsg_fn>(index::SendNetMessage);

		if (!msg)
			return original_sendnetmsg(ecx, msg, reliable, voice);


		if (msg->GetType() == 14)
			return false;

		if (msg->GetGroup() == 9) // Fix lag when transmitting voice and fakelagging
			voice = true;

		return oSNMEvent(ecx, msg, reliable, voice);
	}
	bool __stdcall hkFireEvent(IGameEvent* pEvent)
	{
		static auto oFireEvent = gameevents_vhook.get_original<bool(__thiscall*)(IGameEventManager2*, IGameEvent* pEvent)>(index::FireEvent);
		const char* szEventName = pEvent->GetName();

		if (!strcmp(szEventName, "server_spawn"))
		{
			const auto net_channel = g_EngineClient->GetNetChannelInfo();
			netchannel_vhook.setup(net_channel);
			if (net_channel != nullptr)
			{
				netchannel_vhook.hook_index(40, hkSendNetMsg);
			}
		}

		if (!strcmp(szEventName, "cs_game_disconnected") || g_Unload)
		{
			if (netchannel_vhook.is_hooked())
			{
				netchannel_vhook.unhook_all();
			}
		}


		if (!strcmp(pEvent->GetName(), "player_death"))
		{
			//C_BasePlayer* hurt = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt("userid")));
			//C_BasePlayer* attacker = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt("attacker")));
			// (g_EngineClient->GetPlayerForUserID(event->GetInt("attacker")) == g_EngineClient->GetLocalPlayer() && g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) != g_EngineClient->GetLocalPlayer())
			auto attacker = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(pEvent->GetInt("attacker")));
			if (attacker && attacker->EntIndex() == g_EngineClient->GetLocalPlayer())
			{
				if (Events::Get().IsKnifeString(pEvent->GetString("weapon")))
				{
					pEvent->SetString("weapon", skins::knifes::UpdateKillIcons());
				}
			}
		}

		return oFireEvent(g_GameEvents, pEvent);
	}
	void hkRecvProxy(const CRecvProxyData* pData, void* entity, void* output)
	{
		static auto original_fn = sequence_vhook->get_original_function();
		const auto local = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
		const auto proxy_data = const_cast<CRecvProxyData*>(pData);
		const auto view_model = static_cast<C_BaseViewModel*>(entity);

		skins::knifes::DoSequenceRemapping(proxy_data, view_model);
		

		original_fn(pData, entity, output);
	}
}
