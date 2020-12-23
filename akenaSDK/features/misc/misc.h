#pragma once

#include "../../sdk/math/Vector.hpp"
#include "../../sdk/csgostructs.hpp"
#include "../../helpers/math.hpp"
#include "../../configs/options.hpp"
#include "../../render/render.hpp"
#include "../../helpers/input.hpp"

class C_BasePlayer;
class CUserCmd;

class c_misc {
public:
	void CreateMove(CUserCmd* cmd);
	void NullStrafe(CUserCmd* cmd);
private:
	void Bunnyhop(CUserCmd* cmd);
	void RankReveal(CUserCmd* cmd);
	void JumpBug(CUserCmd* cmd);
	void EdgeBug(CUserCmd* cmd);
	void EdgeJump(CUserCmd* cmd);

	bool jumpbugging = false;
};

extern c_misc misc;