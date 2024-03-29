#pragma once

#include "sdk/csgostructs.hpp"
#include "sdk/utils/vfunc_hook.hpp"

#include <d3d9.h>

namespace index
{
	constexpr auto EmitSound1               = 5;
	constexpr auto EmitSound2               = 6;
    constexpr auto EndScene                 = 42;
    constexpr auto Reset                    = 16;
    constexpr auto PaintTraverse            = 41;
    constexpr auto CreateMove               = 22;
    constexpr auto PlaySound                = 82;
    constexpr auto FrameStageNotify         = 37;
    constexpr auto DrawModelExecute         = 21;
    constexpr auto DoPostScreenSpaceEffects = 44;
	constexpr auto SvCheatsGetBool          = 13;
	constexpr auto OverrideView             = 18;
	constexpr auto LockCursor               = 67;
	constexpr auto ShouldDrawFog			= 17;
	constexpr auto FireEvent				= 9;
	constexpr auto SendNetMessage			= 40;
}

namespace Hooks
{
    void Initialize();
    void Shutdown();

    inline vfunc_hook		hlclient_hook;
	inline vfunc_hook		direct3d_hook;
	inline vfunc_hook		vguipanel_hook;
	inline vfunc_hook		vguisurf_hook;
	inline vfunc_hook		mdlrender_hook;
	inline vfunc_hook		viewrender_hook;
	inline vfunc_hook		sound_hook;
	inline vfunc_hook		clientmode_hook;
	inline vfunc_hook		sv_cheats;
	inline vfunc_hook		gameevents_vhook;
	inline vfunc_hook		netchannel_vhook;
	inline RecvPropHook*	sequence_vhook;
	inline void*			ParticleCollectionSimulateAdr;
	inline PVOID			oParticleCollectionSimulate;
	inline bool				b_predicting;


	typedef bool(__thiscall* sendnetmsg_fn)(void*, INetMessage* msg, bool reliable, bool voice);
	inline sendnetmsg_fn original_sendnetmsg = nullptr;
    long __stdcall hkEndScene(IDirect3DDevice9* device);
    long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
    void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket);
	void __fastcall hkCreateMove_Proxy(void* _this, int, int sequence_number, float input_sample_frametime, bool active);
	void __fastcall hkPaintTraverse(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce);
	void __fastcall hkEmitSound1(void* _this, int, IRecipientFilter & filter, int iEntIndex, int iChannel, const char * pSoundEntry, unsigned int nSoundEntryHash, const char * pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector * pOrigin, const Vector * pDirection, void * pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk);
    void __fastcall hkDrawModelExecute(void* _this, int, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld);
    void __stdcall hkFrameStageNotify(ClientFrameStage_t stage);
	void __fastcall hkOverrideView(void* _this, int, CViewSetup * vsView);
	void __fastcall hkLockCursor(void* _this);
    int  __fastcall hkDoPostScreenEffects(void* _this, int, int a1);
	bool __fastcall hkSvCheatsGetBool(PVOID pConVar, void* edx);
	bool __fastcall hkShouldDrawFog(void* ecx, void* edx);
	bool __fastcall hkSendNetMsg(void* ecx, void* edx, INetMessage* msg, bool reliable, bool voice);
	bool __stdcall hkFireEvent(IGameEvent* pEvent);
	void hkRecvProxy(const CRecvProxyData* pData, void* entity, void* output);
	void __fastcall hkParticleCollectionSimulate(CParticleCollection* thisPtr, void* edx);
}
