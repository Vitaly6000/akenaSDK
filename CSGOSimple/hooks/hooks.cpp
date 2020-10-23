#include "hooks.hpp"
#include <intrin.h>  

#include "../render/render.hpp"
#include "../menu/menu.hpp"
#include "../configs/options.hpp"
#include "../helpers/input.hpp"
#include "../helpers/utils.hpp"
#include "../features/bhop.hpp"
#include "../features/chams.hpp"
#include "../features/visuals.hpp"
#include "../features/glow.hpp"

#pragma comment(lib, "minhook.lib")
#pragma intrinsic(_ReturnAddress)  

namespace Hooks {
	void Initialize() {
		hlclient_hook.setup(g_CHLClient);
		direct3d_hook.setup(g_D3DDevice9);
		vguipanel_hook.setup(g_VGuiPanel);
		vguisurf_hook.setup(g_VGuiSurface);
		sound_hook.setup(g_EngineSound);
		mdlrender_hook.setup(g_MdlRender);
		clientmode_hook.setup(g_ClientMode);
		ConVar* sv_cheats_con = g_CVar->FindVar("sv_cheats");
		sv_cheats.setup(sv_cheats_con);

		const auto gv_endscene = reinterpret_cast<void*>(get_virtual(g_D3DDevice9,  index::EndScene));
		const auto gv_reset	   = reinterpret_cast<void*>(get_virtual(g_D3DDevice9,  index::Reset));
		const auto gv_fsn	   = reinterpret_cast<void*>(get_virtual(g_CHLClient,   index::FrameStageNotify));
		const auto gv_crmove   = reinterpret_cast<void*>(get_virtual(g_ClientMode,  index::CreateMove));
		const auto gv_painttr  = reinterpret_cast<void*>(get_virtual(g_VGuiPanel,   index::PaintTraverse));
		const auto gv_sound	   = reinterpret_cast<void*>(get_virtual(g_EngineSound, index::EmitSound1));
		const auto gv_lcursor  = reinterpret_cast<void*>(get_virtual(g_VGuiSurface, index::LockCursor));
		const auto gv_dme	   = reinterpret_cast<void*>(get_virtual(g_MdlRender,   index::DrawModelExecute));
		const auto gv_pse	   = reinterpret_cast<void*>(get_virtual(g_ClientMode,  index::DoPostScreenSpaceEffects));
		const auto gv_ovview   = reinterpret_cast<void*>(get_virtual(g_ClientMode,  index::OverrideView));;
		const auto gv_cheats   = reinterpret_cast<void*>(get_virtual(sv_cheats_con, index::SvCheatsGetBool));

		if (MH_Initialize() != MH_OK)
			throw std::runtime_error("failed to initialize hooks.");

		MH_CreateHook(gv_endscene, &end_scene::hook,	 reinterpret_cast<void**>(&end_scene::o_end_scene));
		MH_CreateHook(gv_reset,	   &reset::hook,		 reinterpret_cast<void**>(&reset::o_reset));
		MH_CreateHook(gv_fsn,      &fsn::hook,			 reinterpret_cast<void**>(&fsn::o_fsn));
		MH_CreateHook(gv_crmove,   &creatmove::hook,	 reinterpret_cast<void**>(&creatmove::o_crmove));
		MH_CreateHook(gv_painttr,  &pant_traverse::hook, reinterpret_cast<void**>(&pant_traverse::o_paint));
		MH_CreateHook(gv_sound,    &emit_sound::hook,	 reinterpret_cast<void**>(&emit_sound::o_sound));
		MH_CreateHook(gv_lcursor,  &lock_cursour::hook,  reinterpret_cast<void**>(&lock_cursour::o_lock));
		MH_CreateHook(gv_dme,      &dme::hook,			 reinterpret_cast<void**>(&dme::o_dme));
		MH_CreateHook(gv_pse,      &dpse::hook,			 reinterpret_cast<void**>(&dpse::o_effects));
		MH_CreateHook(gv_ovview,   &override_view::hook, reinterpret_cast<void**>(&override_view::o_ovview));
		MH_CreateHook(gv_cheats,   &cheats::hook,		 reinterpret_cast<void**>(&cheats::o_cheats));

		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
			throw std::runtime_error("failed to enable hooks.");
	}
	//--------------------------------------------------------------------------------
	void Shutdown() {
		hlclient_hook.unhook_all();
		direct3d_hook.unhook_all();
		vguipanel_hook.unhook_all();
		vguisurf_hook.unhook_all();
		mdlrender_hook.unhook_all();
		clientmode_hook.unhook_all();
		sound_hook.unhook_all();
		sv_cheats.unhook_all();

		MH_Uninitialize();
		MH_DisableHook(MH_ALL_HOOKS);

		Glow::Get().Shutdown();
	}
	//--------------------------------------------------------------------------------
	long __stdcall end_scene::hook(IDirect3DDevice9* device) {
		static auto viewmodel_fov = g_CVar->FindVar("viewmodel_fov");
		viewmodel_fov->m_fnChangeCallbacks.m_Size = 0;
		viewmodel_fov->SetValue(g_Options.viewmodel_fov);

		DWORD colorwrite, srgbwrite;
		IDirect3DVertexDeclaration9* vert_dec = nullptr;
		IDirect3DVertexShader9* vert_shader = nullptr;
		DWORD dwOld_D3DRS_COLORWRITEENABLE = NULL;
		device->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		device->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

		device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		//removes the source engine color correction
		device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		device->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
		device->GetVertexDeclaration(&vert_dec);
		device->GetVertexShader(&vert_shader);
		device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
		device->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		device->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		device->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		device->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

		
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();


		auto esp_drawlist = Render::Get().RenderScene();

		Menu::Get().Render();
	

		ImGui::Render(esp_drawlist);

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		device->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		device->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
		device->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
		device->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
		device->SetVertexDeclaration(vert_dec);
		device->SetVertexShader(vert_shader);

		return o_end_scene(device);
	}
	//--------------------------------------------------------------------------------
	long __stdcall reset::hook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* presentation_parameters) {
		Menu::Get().OnDeviceLost();

		auto hr = o_reset(device, presentation_parameters);

		if (hr >= 0) Menu::Get().OnDeviceReset();

		return hr;
	}
	//--------------------------------------------------------------------------------
	bool __stdcall creatmove::hook(float input_sample_frametime, CUserCmd* cmd) {
		o_crmove(input_sample_frametime, cmd);

		if (!cmd || !cmd->command_number)
			return false;
		
		if (Menu::Get().IsVisible())
			cmd->buttons &= ~IN_ATTACK;

		if (g_Options.misc_bhop)
			BunnyHop::OnCreateMove(cmd);

		// https://github.com/spirthack/CSGOSimple/issues/69
		if (g_Options.misc_showranks && cmd->buttons & IN_SCORE) // rank revealer will work even after unhooking, idk how to "hide" ranks  again
			g_CHLClient->DispatchUserMessage(CS_UM_ServerRankRevealAll, 0, 0, nullptr);

		return false;
	}
	//--------------------------------------------------------------------------------
	void __fastcall pant_traverse::hook(void* _this, int edx, vgui::VPANEL panel, bool force_repaint, bool allow_force) {
		static auto panelId = vgui::VPANEL{ 0 };
		o_paint(g_VGuiPanel, edx, panel, force_repaint, allow_force);

		if (!panelId) {
			const auto panelName = g_VGuiPanel->GetName(panel);
			if (!strcmp(panelName, "FocusOverlayPanel")) {
				panelId = panel;
			}
		}
		else if (panelId == panel) 
		{
			//Ignore 50% cuz it called very often
			static bool bSkip = false;
			bSkip = !bSkip;

			if (bSkip)
				return;

			Render::Get().BeginScene();
		}
	}
	//--------------------------------------------------------------------------------
	void __fastcall emit_sound::hook(void* _this, int edx, IRecipientFilter& filter, int ent_index, int channel, const char* sound_entry, unsigned int sound_entry_hash, const char* sample, float volume, int seed, float attenuation, int flags, int pitch, const Vector* origin, const Vector* direction, void* utl_vec_origins, bool update_positions, float sound_time, int speaker_entity, int unk) {
		if (!strcmp(sound_entry, "UIPanorama.popup_accept_match_beep")) {
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
		o_sound(g_EngineSound, edx, filter, ent_index, channel, sound_entry, sound_entry_hash, sample, volume, seed, attenuation, flags, pitch, origin, direction, utl_vec_origins, update_positions, sound_time, speaker_entity, unk);
	}
	//--------------------------------------------------------------------------------
	int __fastcall dpse::hook(void* _this, int edx, int a1) {
		if (g_LocalPlayer && g_Options.glow_enabled)
			Glow::Get().Run(); 

		return o_effects(g_ClientMode, edx, a1);
	}
	//--------------------------------------------------------------------------------
	void __fastcall fsn::hook(void* _this, int edx, ClientFrameStage_t stage) {
		// may be u will use it lol
		o_fsn(g_CHLClient, edx, stage);
	}
	//--------------------------------------------------------------------------------
	void __fastcall override_view::hook(void* _this, int edx, CViewSetup* vs_view) {
		if (g_EngineClient->IsInGame() && vs_view)
			Visuals::Get().ThirdPerson();
		
		o_ovview(g_ClientMode, edx, vs_view);
	}
	//--------------------------------------------------------------------------------
	void __fastcall lock_cursour::hook(void* _this) {
		if (Menu::Get().IsVisible()) {
			g_VGuiSurface->UnlockCursor();
			g_InputSystem->ResetInputState();
			return;
		}
		o_lock(g_VGuiSurface);
	}
	//--------------------------------------------------------------------------------
	void __fastcall dme::hook(void* _this, int edx, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* custom_bone_to_world) {
		if (g_MdlRender->IsForcedMaterialOverride())
			return o_dme(_this, edx, ctx, state, info, custom_bone_to_world);

		Chams::Get().OnDrawModelExecute(ctx, state, info, custom_bone_to_world);
		o_dme(_this, edx, ctx, state, info, custom_bone_to_world);
		g_MdlRender->ForcedMaterialOverride(nullptr);
	}
	//--------------------------------------------------------------------------------
	bool __fastcall cheats::hook(PVOID convar, void* edx) {
		static auto dwCAM_Think = Utils::PatternScan(GetModuleHandleW(L"client.dll"), "85 C0 75 30 38 86");
		static auto ofunc = sv_cheats.get_original<bool(__thiscall *)(PVOID)>(13);
		if (!ofunc) return false;
		if (reinterpret_cast<DWORD>(_ReturnAddress()) == reinterpret_cast<DWORD>(dwCAM_Think)) return true;
		return o_cheats(convar, edx);
	}
}
