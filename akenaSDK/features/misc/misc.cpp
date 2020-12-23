#include "misc.h"

c_misc misc;

void c_misc::CreateMove(CUserCmd* cmd) {
    RankReveal(cmd);
    if (g_LocalPlayer->IsAlive()) {
        Bunnyhop(cmd);
        JumpBug(cmd);
        EdgeBug(cmd);
        EdgeJump(cmd);
    }
}

void c_misc::Bunnyhop(CUserCmd* cmd) {
    if (!g_Options.misc_bhop && jumpbugging)
        return;

    static bool jumped_last_tick = false, should_fake_jump;

    if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->m_fFlags() & FL_INWATER)
        return;

    if (!jumped_last_tick && should_fake_jump) {
        should_fake_jump = false;
        cmd->buttons |= IN_JUMP;
    }
    else if (cmd->buttons & IN_JUMP) {
        if (g_LocalPlayer->m_fFlags() & FL_ONGROUND) {
            jumped_last_tick = true;
            should_fake_jump = true;
        }
        else {
            cmd->buttons &= ~IN_JUMP;
            jumped_last_tick = false;
        }
    }
    else {
        jumped_last_tick = false;
        should_fake_jump = false;
    }
}

void c_misc::RankReveal(CUserCmd* cmd){
    if (g_Options.misc_showranks && cmd->buttons & IN_SCORE)
        g_CHLClient->DispatchUserMessage(CS_UM_ServerRankRevealAll, 0, 0, nullptr);
}

void c_misc::NullStrafe(CUserCmd* cmd) {
    if (!g_Options.misc_null_strafe && !g_Options.misc_bhop)
        return;

    if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->m_fFlags() & FL_INWATER)
        return;

    if (cmd->mousedx > 0 && cmd->buttons & IN_MOVERIGHT && cmd->buttons & IN_MOVELEFT)
        cmd->sidemove = -450.f;
    else if (cmd->mousedx < 0 && cmd->buttons & IN_MOVELEFT && cmd->buttons & IN_MOVERIGHT)
        cmd->sidemove = 450.f;
}

void c_misc::JumpBug(CUserCmd* cmd) {
    jumpbugging = false;
    if (!g_Options.misc_jump_bug && InputSys::Get().WasKeyPressed(g_Options.misc_jump_bug_key))
        return;
    jumpbugging = true;

    if (g_LocalPlayer->m_fFlags() & FL_ONGROUND) {
        cmd->buttons &= ~IN_JUMP;
        if(!(g_LocalPlayer->m_fFlags() & FL_ONGROUND))
            cmd->buttons |= IN_DUCK;
    }
}

void c_misc::EdgeBug(CUserCmd* cmd) {
    //to-do
}

void c_misc::EdgeJump(CUserCmd* cmd) {
    //to-do
}