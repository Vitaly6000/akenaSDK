#include "chams.hpp"
#include <fstream>

#include "../sdk/csgostructs.hpp"
#include "../configs/options.hpp"
#include "../hooks/hooks.hpp"
#include "../helpers/input.hpp"

Chams::Chams() {
	materialRegular = g_MatSystem->FindMaterial("debug/debugambientcube");
	materialFlat = g_MatSystem->FindMaterial("debug/debugdrawflat");

	std::ofstream("csgo\\materials\\material_glow.vmt") << R"#("VertexLitGeneric" 
{
 "$additive"               "1"
 "$envmap"                 "models/effects/cube_white"
 "$envmaptint"             "[1 1 1]"
 "$envmapfresnel"          "1"
 "$envmapfresnelminmaxexp" "[0 1 2]"
 "$ignorez"                "0"
}
)#";
	materialGlow = g_MatSystem->FindMaterial("material_glow");

	std::ofstream("csgo\\materials\\material_eso_glow.vmt") << R"#("VertexLitGeneric" {
 
	"$additive"               "1"
	"$envmap"                 "models/effects/cube_white"
	"$envmaptint"             "[1 1 1]"
	"$envmapfresnel"          "1"
	"$envmapfresnelminmaxexp" "[0 1 2]"
	"$alpha"				  "0.8"
})#";
	materialEsoGlow = g_MatSystem->FindMaterial("material_eso_glow");
	materialGlowArmsrace = g_MatSystem->FindMaterial("dev/glow_armsrace.vmt");
}

Chams::~Chams() { 
	std::remove("csgo\\materials\\material_glow.vmt");
	std::remove("csgo\\materials\\material_eso_glow.vmt");
}

void Chams::OverrideMaterial(bool ignoreZ, bool wireframe, int type, const Color& rgba) {
	bool bFound = false;
	IMaterial* material = change_material(type);
	IMaterialVar* pMatVar = material->FindVar("$envmaptint", &bFound);

	material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
	material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignoreZ);

	if (bFound) (*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pMatVar + 0x2c))((uintptr_t)pMatVar, rgba.r() / 255.f, rgba.g() / 255.f, rgba.b() / 255.f);

	material->ColorModulate(rgba.r() / 255.0f, rgba.g() / 255.0f, rgba.b() / 255.0f);
	material->AlphaModulate(rgba.a() / 255.0f);
	material->IncrementReferenceCount();
	g_MdlRender->ForcedMaterialOverride(material);
}

void Chams::OnDrawModelExecute(IMatRenderContext* ctx,const DrawModelState_t& state,const ModelRenderInfo_t& info,matrix3x4_t* matrix) {
	const auto mdl = info.pModel;
	bool is_arm = strstr(mdl->szName, "arms") != nullptr;
	bool is_sleeve = strstr(mdl->szName, "sleeve") != nullptr;
	bool is_weapon = strstr(mdl->szName, "models/weapons/v_") != nullptr;

	if (is_sleeve && g_Options.chams_sleeve_enabled) {
		OverrideMaterial(false, false, g_Options.chams_material_sleeve, Color(g_Options.color_chams_sleeve));
	}
	else if (is_arm && g_Options.chams_arms_enabled) {
		OverrideMaterial(false, false, g_Options.chams_material_arms, Color(g_Options.color_chams_arms));
	}
	else if (is_weapon && g_Options.chams_weapon_enabled && !is_arm) {
		OverrideMaterial(false, false, g_Options.chams_material_weapon, Color(g_Options.color_chams_weapon));
	}
	else if (g_Options.chams_player_enabled) {
		C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(info.entity_index);
		if (!entity || entity->IsDormant() || !entity->IsPlayer()) return;

		const auto enemy = entity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
		if (!enemy && g_Options.chams_player_enemies_only) return;

		auto is_local = entity->EntIndex() == g_LocalPlayer->EntIndex();
		if (is_local) return;

		const auto clr_front = enemy ? Color(g_Options.color_chams_player_enemy_visible) : Color(g_Options.color_chams_player_ally_visible);
		const auto clr_back = enemy ? Color(g_Options.color_chams_player_enemy_occluded) : Color(g_Options.color_chams_player_ally_occluded);

		if (g_Options.chams_player_ignorez) {
			OverrideMaterial(true, g_Options.chams_player_wireframe, g_Options.chams_material, clr_back);
			Hooks::dme::o_dme(g_MdlRender, 0, ctx, state, info, matrix);
			OverrideMaterial(false, g_Options.chams_player_wireframe, g_Options.chams_material, clr_front);
		}
		else OverrideMaterial(false, g_Options.chams_player_wireframe, g_Options.chams_material, clr_front);
	}
}
