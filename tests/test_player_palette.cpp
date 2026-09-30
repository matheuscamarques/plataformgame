/**
 * @file tests/test_player_palette.cpp
 * @brief Trava invariantes I1-I5 do manual de PlayerParts.h.
 * @details I1 (40 rows) via static_assert por array (gerado);
 * I2/I3/I4 em runtime sobre todos os 134 arrays; I5 sobre todos
 * os Parts de posePartsFor/posePartsForAir. Arte nova entra aqui
 * regenerando a lista (mesmo script que gerou).
 */
#include <cassert>
#include <cstdio>
#include <cstring>
#include <set>
#include <string>
#include "assets/SpriteComposer.h"
#include "assets/Sprites/PlayerParts.h"
#include "assets/Sprites/PlayerPalette.h"
#include "assets/Sprites/PlayerSprites.h"

namespace {
struct Span { int lo, hi; };
Span spanFor(const char* name) {
    std::string n(name);
    if (n == "kPlayerJumpArms" || n == "kPlayerHurtArms" ||
        n == "kPlayerPunchUpArms")
        return {0, 13}; // exceção Arms-em-y=0 (manual)
    if (n.size() >= 4 && n.compare(n.size() - 4, 4, "Head") == 0)
        return {0, 11};
    if (n.size() >= 5 && n.compare(n.size() - 5, 5, "Torso") == 0)
        return {12, 27};
    if (n.size() >= 4 && n.compare(n.size() - 4, 4, "Legs") == 0)
        return {28, 33};
    if (n.size() >= 4 && n.compare(n.size() - 4, 4, "Feet") == 0)
        return {34, 39};
    if (n.size() >= 4 && n.compare(n.size() - 4, 4, "Arms") == 0)
        return {12, 27};
    return {-1, -1}; // sem kind: falha abaixo
}
} // namespace

// I1: 40 rows por array (tempo de compilação).
static_assert(sizeof(sprites::kPlayerIdleHead) / sizeof(sprites::kPlayerIdleHead[0]) == 40, "kPlayerIdleHead");
static_assert(sizeof(sprites::kPlayerIdleTorso) / sizeof(sprites::kPlayerIdleTorso[0]) == 40, "kPlayerIdleTorso");
static_assert(sizeof(sprites::kPlayerIdleLegs) / sizeof(sprites::kPlayerIdleLegs[0]) == 40, "kPlayerIdleLegs");
static_assert(sizeof(sprites::kPlayerIdleFeet) / sizeof(sprites::kPlayerIdleFeet[0]) == 40, "kPlayerIdleFeet");
static_assert(sizeof(sprites::kPlayerIdleArms) / sizeof(sprites::kPlayerIdleArms[0]) == 40, "kPlayerIdleArms");
static_assert(sizeof(sprites::kPlayerIdleSHead) / sizeof(sprites::kPlayerIdleSHead[0]) == 40, "kPlayerIdleSHead");
static_assert(sizeof(sprites::kPlayerIdleSTorso) / sizeof(sprites::kPlayerIdleSTorso[0]) == 40, "kPlayerIdleSTorso");
static_assert(sizeof(sprites::kPlayerIdleSLegs) / sizeof(sprites::kPlayerIdleSLegs[0]) == 40, "kPlayerIdleSLegs");
static_assert(sizeof(sprites::kPlayerIdleSFeet) / sizeof(sprites::kPlayerIdleSFeet[0]) == 40, "kPlayerIdleSFeet");
static_assert(sizeof(sprites::kPlayerIdleSArms) / sizeof(sprites::kPlayerIdleSArms[0]) == 40, "kPlayerIdleSArms");
static_assert(sizeof(sprites::kPlayerIdleNHead) / sizeof(sprites::kPlayerIdleNHead[0]) == 40, "kPlayerIdleNHead");
static_assert(sizeof(sprites::kPlayerIdleNTorso) / sizeof(sprites::kPlayerIdleNTorso[0]) == 40, "kPlayerIdleNTorso");
static_assert(sizeof(sprites::kPlayerIdleNLegs) / sizeof(sprites::kPlayerIdleNLegs[0]) == 40, "kPlayerIdleNLegs");
static_assert(sizeof(sprites::kPlayerIdleNFeet) / sizeof(sprites::kPlayerIdleNFeet[0]) == 40, "kPlayerIdleNFeet");
static_assert(sizeof(sprites::kPlayerIdleNArms) / sizeof(sprites::kPlayerIdleNArms[0]) == 40, "kPlayerIdleNArms");
static_assert(sizeof(sprites::kPlayerIdleSEHead) / sizeof(sprites::kPlayerIdleSEHead[0]) == 40, "kPlayerIdleSEHead");
static_assert(sizeof(sprites::kPlayerIdleSETorso) / sizeof(sprites::kPlayerIdleSETorso[0]) == 40, "kPlayerIdleSETorso");
static_assert(sizeof(sprites::kPlayerIdleSELegs) / sizeof(sprites::kPlayerIdleSELegs[0]) == 40, "kPlayerIdleSELegs");
static_assert(sizeof(sprites::kPlayerIdleSEFeet) / sizeof(sprites::kPlayerIdleSEFeet[0]) == 40, "kPlayerIdleSEFeet");
static_assert(sizeof(sprites::kPlayerIdleSEArms) / sizeof(sprites::kPlayerIdleSEArms[0]) == 40, "kPlayerIdleSEArms");
static_assert(sizeof(sprites::kPlayerIdleNEHead) / sizeof(sprites::kPlayerIdleNEHead[0]) == 40, "kPlayerIdleNEHead");
static_assert(sizeof(sprites::kPlayerIdleNETorso) / sizeof(sprites::kPlayerIdleNETorso[0]) == 40, "kPlayerIdleNETorso");
static_assert(sizeof(sprites::kPlayerIdleNELegs) / sizeof(sprites::kPlayerIdleNELegs[0]) == 40, "kPlayerIdleNELegs");
static_assert(sizeof(sprites::kPlayerIdleNEFeet) / sizeof(sprites::kPlayerIdleNEFeet[0]) == 40, "kPlayerIdleNEFeet");
static_assert(sizeof(sprites::kPlayerIdleNEArms) / sizeof(sprites::kPlayerIdleNEArms[0]) == 40, "kPlayerIdleNEArms");
static_assert(sizeof(sprites::kPlayerWalkA_S_Arms) / sizeof(sprites::kPlayerWalkA_S_Arms[0]) == 40, "kPlayerWalkA_S_Arms");
static_assert(sizeof(sprites::kPlayerWalkA_N_Arms) / sizeof(sprites::kPlayerWalkA_N_Arms[0]) == 40, "kPlayerWalkA_N_Arms");
static_assert(sizeof(sprites::kPlayerWalkA_SE_Torso) / sizeof(sprites::kPlayerWalkA_SE_Torso[0]) == 40, "kPlayerWalkA_SE_Torso");
static_assert(sizeof(sprites::kPlayerWalkA_SE_Arms) / sizeof(sprites::kPlayerWalkA_SE_Arms[0]) == 40, "kPlayerWalkA_SE_Arms");
static_assert(sizeof(sprites::kPlayerWalkA_NE_Torso) / sizeof(sprites::kPlayerWalkA_NE_Torso[0]) == 40, "kPlayerWalkA_NE_Torso");
static_assert(sizeof(sprites::kPlayerWalkA_NE_Arms) / sizeof(sprites::kPlayerWalkA_NE_Arms[0]) == 40, "kPlayerWalkA_NE_Arms");
static_assert(sizeof(sprites::kPlayerWalkB_S_Legs) / sizeof(sprites::kPlayerWalkB_S_Legs[0]) == 40, "kPlayerWalkB_S_Legs");
static_assert(sizeof(sprites::kPlayerWalkB_S_Feet) / sizeof(sprites::kPlayerWalkB_S_Feet[0]) == 40, "kPlayerWalkB_S_Feet");
static_assert(sizeof(sprites::kPlayerWalkB_S_Arms) / sizeof(sprites::kPlayerWalkB_S_Arms[0]) == 40, "kPlayerWalkB_S_Arms");
static_assert(sizeof(sprites::kPlayerWalkB_N_Arms) / sizeof(sprites::kPlayerWalkB_N_Arms[0]) == 40, "kPlayerWalkB_N_Arms");
static_assert(sizeof(sprites::kPlayerWalkB_SE_Torso) / sizeof(sprites::kPlayerWalkB_SE_Torso[0]) == 40, "kPlayerWalkB_SE_Torso");
static_assert(sizeof(sprites::kPlayerWalkB_SE_Legs) / sizeof(sprites::kPlayerWalkB_SE_Legs[0]) == 40, "kPlayerWalkB_SE_Legs");
static_assert(sizeof(sprites::kPlayerWalkB_SE_Feet) / sizeof(sprites::kPlayerWalkB_SE_Feet[0]) == 40, "kPlayerWalkB_SE_Feet");
static_assert(sizeof(sprites::kPlayerWalkB_SE_Arms) / sizeof(sprites::kPlayerWalkB_SE_Arms[0]) == 40, "kPlayerWalkB_SE_Arms");
static_assert(sizeof(sprites::kPlayerWalkB_NE_Torso) / sizeof(sprites::kPlayerWalkB_NE_Torso[0]) == 40, "kPlayerWalkB_NE_Torso");
static_assert(sizeof(sprites::kPlayerWalkB_NE_Legs) / sizeof(sprites::kPlayerWalkB_NE_Legs[0]) == 40, "kPlayerWalkB_NE_Legs");
static_assert(sizeof(sprites::kPlayerWalkB_NE_Feet) / sizeof(sprites::kPlayerWalkB_NE_Feet[0]) == 40, "kPlayerWalkB_NE_Feet");
static_assert(sizeof(sprites::kPlayerWalkB_NE_Arms) / sizeof(sprites::kPlayerWalkB_NE_Arms[0]) == 40, "kPlayerWalkB_NE_Arms");
static_assert(sizeof(sprites::kPlayerPunch_S_Torso) / sizeof(sprites::kPlayerPunch_S_Torso[0]) == 40, "kPlayerPunch_S_Torso");
static_assert(sizeof(sprites::kPlayerPunch_S_Arms) / sizeof(sprites::kPlayerPunch_S_Arms[0]) == 40, "kPlayerPunch_S_Arms");
static_assert(sizeof(sprites::kPlayerPunch_N_Torso) / sizeof(sprites::kPlayerPunch_N_Torso[0]) == 40, "kPlayerPunch_N_Torso");
static_assert(sizeof(sprites::kPlayerPunch_N_Arms) / sizeof(sprites::kPlayerPunch_N_Arms[0]) == 40, "kPlayerPunch_N_Arms");
static_assert(sizeof(sprites::kPlayerPunch_SE_Torso) / sizeof(sprites::kPlayerPunch_SE_Torso[0]) == 40, "kPlayerPunch_SE_Torso");
static_assert(sizeof(sprites::kPlayerPunch_SE_Arms) / sizeof(sprites::kPlayerPunch_SE_Arms[0]) == 40, "kPlayerPunch_SE_Arms");
static_assert(sizeof(sprites::kPlayerPunch_NE_Torso) / sizeof(sprites::kPlayerPunch_NE_Torso[0]) == 40, "kPlayerPunch_NE_Torso");
static_assert(sizeof(sprites::kPlayerPunch_NE_Arms) / sizeof(sprites::kPlayerPunch_NE_Arms[0]) == 40, "kPlayerPunch_NE_Arms");
static_assert(sizeof(sprites::kPlayerPunchDown_S_Torso) / sizeof(sprites::kPlayerPunchDown_S_Torso[0]) == 40, "kPlayerPunchDown_S_Torso");
static_assert(sizeof(sprites::kPlayerPunchDown_S_Arms) / sizeof(sprites::kPlayerPunchDown_S_Arms[0]) == 40, "kPlayerPunchDown_S_Arms");
static_assert(sizeof(sprites::kPlayerPunchDown_N_Torso) / sizeof(sprites::kPlayerPunchDown_N_Torso[0]) == 40, "kPlayerPunchDown_N_Torso");
static_assert(sizeof(sprites::kPlayerPunchDown_N_Arms) / sizeof(sprites::kPlayerPunchDown_N_Arms[0]) == 40, "kPlayerPunchDown_N_Arms");
static_assert(sizeof(sprites::kPlayerPunchDown_SE_Torso) / sizeof(sprites::kPlayerPunchDown_SE_Torso[0]) == 40, "kPlayerPunchDown_SE_Torso");
static_assert(sizeof(sprites::kPlayerPunchDown_SE_Arms) / sizeof(sprites::kPlayerPunchDown_SE_Arms[0]) == 40, "kPlayerPunchDown_SE_Arms");
static_assert(sizeof(sprites::kPlayerPunchDown_NE_Torso) / sizeof(sprites::kPlayerPunchDown_NE_Torso[0]) == 40, "kPlayerPunchDown_NE_Torso");
static_assert(sizeof(sprites::kPlayerPunchDown_NE_Arms) / sizeof(sprites::kPlayerPunchDown_NE_Arms[0]) == 40, "kPlayerPunchDown_NE_Arms");
static_assert(sizeof(sprites::kPlayerWalkAHead) / sizeof(sprites::kPlayerWalkAHead[0]) == 40, "kPlayerWalkAHead");
static_assert(sizeof(sprites::kPlayerWalkATorso) / sizeof(sprites::kPlayerWalkATorso[0]) == 40, "kPlayerWalkATorso");
static_assert(sizeof(sprites::kPlayerWalkALegs) / sizeof(sprites::kPlayerWalkALegs[0]) == 40, "kPlayerWalkALegs");
static_assert(sizeof(sprites::kPlayerWalkAFeet) / sizeof(sprites::kPlayerWalkAFeet[0]) == 40, "kPlayerWalkAFeet");
static_assert(sizeof(sprites::kPlayerWalkCFeet) / sizeof(sprites::kPlayerWalkCFeet[0]) == 40, "kPlayerWalkCFeet");
static_assert(sizeof(sprites::kPlayerWalkAArms) / sizeof(sprites::kPlayerWalkAArms[0]) == 40, "kPlayerWalkAArms");
static_assert(sizeof(sprites::kPlayerWalkBHead) / sizeof(sprites::kPlayerWalkBHead[0]) == 40, "kPlayerWalkBHead");
static_assert(sizeof(sprites::kPlayerWalkBTorso) / sizeof(sprites::kPlayerWalkBTorso[0]) == 40, "kPlayerWalkBTorso");
static_assert(sizeof(sprites::kPlayerWalkBLegs) / sizeof(sprites::kPlayerWalkBLegs[0]) == 40, "kPlayerWalkBLegs");
static_assert(sizeof(sprites::kPlayerWalkBFeet) / sizeof(sprites::kPlayerWalkBFeet[0]) == 40, "kPlayerWalkBFeet");
static_assert(sizeof(sprites::kPlayerWalkBArms) / sizeof(sprites::kPlayerWalkBArms[0]) == 40, "kPlayerWalkBArms");
static_assert(sizeof(sprites::kPlayerJumpHead) / sizeof(sprites::kPlayerJumpHead[0]) == 40, "kPlayerJumpHead");
static_assert(sizeof(sprites::kPlayerJumpTorso) / sizeof(sprites::kPlayerJumpTorso[0]) == 40, "kPlayerJumpTorso");
static_assert(sizeof(sprites::kPlayerJumpLegs) / sizeof(sprites::kPlayerJumpLegs[0]) == 40, "kPlayerJumpLegs");
static_assert(sizeof(sprites::kPlayerJumpFeet) / sizeof(sprites::kPlayerJumpFeet[0]) == 40, "kPlayerJumpFeet");
static_assert(sizeof(sprites::kPlayerJumpArms) / sizeof(sprites::kPlayerJumpArms[0]) == 40, "kPlayerJumpArms");
static_assert(sizeof(sprites::kPlayerThrowHead) / sizeof(sprites::kPlayerThrowHead[0]) == 40, "kPlayerThrowHead");
static_assert(sizeof(sprites::kPlayerThrowTorso) / sizeof(sprites::kPlayerThrowTorso[0]) == 40, "kPlayerThrowTorso");
static_assert(sizeof(sprites::kPlayerThrowLegs) / sizeof(sprites::kPlayerThrowLegs[0]) == 40, "kPlayerThrowLegs");
static_assert(sizeof(sprites::kPlayerThrowFeet) / sizeof(sprites::kPlayerThrowFeet[0]) == 40, "kPlayerThrowFeet");
static_assert(sizeof(sprites::kPlayerThrowArms) / sizeof(sprites::kPlayerThrowArms[0]) == 40, "kPlayerThrowArms");
static_assert(sizeof(sprites::kPlayerPunchHead) / sizeof(sprites::kPlayerPunchHead[0]) == 40, "kPlayerPunchHead");
static_assert(sizeof(sprites::kPlayerPunchTorso) / sizeof(sprites::kPlayerPunchTorso[0]) == 40, "kPlayerPunchTorso");
static_assert(sizeof(sprites::kPlayerPunchLegs) / sizeof(sprites::kPlayerPunchLegs[0]) == 40, "kPlayerPunchLegs");
static_assert(sizeof(sprites::kPlayerPunchFeet) / sizeof(sprites::kPlayerPunchFeet[0]) == 40, "kPlayerPunchFeet");
static_assert(sizeof(sprites::kPlayerPunchArms) / sizeof(sprites::kPlayerPunchArms[0]) == 40, "kPlayerPunchArms");
static_assert(sizeof(sprites::kPlayerPunchUpHead) / sizeof(sprites::kPlayerPunchUpHead[0]) == 40, "kPlayerPunchUpHead");
static_assert(sizeof(sprites::kPlayerPunchUpTorso) / sizeof(sprites::kPlayerPunchUpTorso[0]) == 40, "kPlayerPunchUpTorso");
static_assert(sizeof(sprites::kPlayerPunchUpLegs) / sizeof(sprites::kPlayerPunchUpLegs[0]) == 40, "kPlayerPunchUpLegs");
static_assert(sizeof(sprites::kPlayerPunchUpFeet) / sizeof(sprites::kPlayerPunchUpFeet[0]) == 40, "kPlayerPunchUpFeet");
static_assert(sizeof(sprites::kPlayerPunchUpArms) / sizeof(sprites::kPlayerPunchUpArms[0]) == 40, "kPlayerPunchUpArms");
static_assert(sizeof(sprites::kPlayerPunchUp_S_Torso) / sizeof(sprites::kPlayerPunchUp_S_Torso[0]) == 40, "kPlayerPunchUp_S_Torso");
static_assert(sizeof(sprites::kPlayerPunchUp_S_Arms) / sizeof(sprites::kPlayerPunchUp_S_Arms[0]) == 40, "kPlayerPunchUp_S_Arms");
static_assert(sizeof(sprites::kPlayerPunchUp_N_Torso) / sizeof(sprites::kPlayerPunchUp_N_Torso[0]) == 40, "kPlayerPunchUp_N_Torso");
static_assert(sizeof(sprites::kPlayerPunchUp_N_Arms) / sizeof(sprites::kPlayerPunchUp_N_Arms[0]) == 40, "kPlayerPunchUp_N_Arms");
static_assert(sizeof(sprites::kPlayerPunchUp_SE_Torso) / sizeof(sprites::kPlayerPunchUp_SE_Torso[0]) == 40, "kPlayerPunchUp_SE_Torso");
static_assert(sizeof(sprites::kPlayerPunchUp_SE_Arms) / sizeof(sprites::kPlayerPunchUp_SE_Arms[0]) == 40, "kPlayerPunchUp_SE_Arms");
static_assert(sizeof(sprites::kPlayerPunchUp_NE_Torso) / sizeof(sprites::kPlayerPunchUp_NE_Torso[0]) == 40, "kPlayerPunchUp_NE_Torso");
static_assert(sizeof(sprites::kPlayerPunchUp_NE_Arms) / sizeof(sprites::kPlayerPunchUp_NE_Arms[0]) == 40, "kPlayerPunchUp_NE_Arms");
static_assert(sizeof(sprites::kPlayerPunchDownHead) / sizeof(sprites::kPlayerPunchDownHead[0]) == 40, "kPlayerPunchDownHead");
static_assert(sizeof(sprites::kPlayerPunchDownTorso) / sizeof(sprites::kPlayerPunchDownTorso[0]) == 40, "kPlayerPunchDownTorso");
static_assert(sizeof(sprites::kPlayerPunchDownLegs) / sizeof(sprites::kPlayerPunchDownLegs[0]) == 40, "kPlayerPunchDownLegs");
static_assert(sizeof(sprites::kPlayerPunchDownFeet) / sizeof(sprites::kPlayerPunchDownFeet[0]) == 40, "kPlayerPunchDownFeet");
static_assert(sizeof(sprites::kPlayerPunchDownArms) / sizeof(sprites::kPlayerPunchDownArms[0]) == 40, "kPlayerPunchDownArms");
static_assert(sizeof(sprites::kPlayerHurtHead) / sizeof(sprites::kPlayerHurtHead[0]) == 40, "kPlayerHurtHead");
static_assert(sizeof(sprites::kPlayerHurtTorso) / sizeof(sprites::kPlayerHurtTorso[0]) == 40, "kPlayerHurtTorso");
static_assert(sizeof(sprites::kPlayerHurtLegs) / sizeof(sprites::kPlayerHurtLegs[0]) == 40, "kPlayerHurtLegs");
static_assert(sizeof(sprites::kPlayerHurtFeet) / sizeof(sprites::kPlayerHurtFeet[0]) == 40, "kPlayerHurtFeet");
static_assert(sizeof(sprites::kPlayerHurtArms) / sizeof(sprites::kPlayerHurtArms[0]) == 40, "kPlayerHurtArms");
static_assert(sizeof(sprites::kPlayerHurt_S_Head) / sizeof(sprites::kPlayerHurt_S_Head[0]) == 40, "kPlayerHurt_S_Head");
static_assert(sizeof(sprites::kPlayerHurt_N_Head) / sizeof(sprites::kPlayerHurt_N_Head[0]) == 40, "kPlayerHurt_N_Head");
static_assert(sizeof(sprites::kPlayerHurt_SE_Head) / sizeof(sprites::kPlayerHurt_SE_Head[0]) == 40, "kPlayerHurt_SE_Head");
static_assert(sizeof(sprites::kPlayerHurt_SE_Torso) / sizeof(sprites::kPlayerHurt_SE_Torso[0]) == 40, "kPlayerHurt_SE_Torso");
static_assert(sizeof(sprites::kPlayerHurt_NE_Head) / sizeof(sprites::kPlayerHurt_NE_Head[0]) == 40, "kPlayerHurt_NE_Head");
static_assert(sizeof(sprites::kPlayerHurt_NE_Torso) / sizeof(sprites::kPlayerHurt_NE_Torso[0]) == 40, "kPlayerHurt_NE_Torso");
static_assert(sizeof(sprites::kPlayerDeathHead) / sizeof(sprites::kPlayerDeathHead[0]) == 40, "kPlayerDeathHead");
static_assert(sizeof(sprites::kPlayerDeathTorso) / sizeof(sprites::kPlayerDeathTorso[0]) == 40, "kPlayerDeathTorso");
static_assert(sizeof(sprites::kPlayerDeathLegs) / sizeof(sprites::kPlayerDeathLegs[0]) == 40, "kPlayerDeathLegs");
static_assert(sizeof(sprites::kPlayerDeathFeet) / sizeof(sprites::kPlayerDeathFeet[0]) == 40, "kPlayerDeathFeet");
static_assert(sizeof(sprites::kPlayerDeathArms) / sizeof(sprites::kPlayerDeathArms[0]) == 40, "kPlayerDeathArms");
static_assert(sizeof(sprites::kPlayerDeath_S_Torso) / sizeof(sprites::kPlayerDeath_S_Torso[0]) == 40, "kPlayerDeath_S_Torso");
static_assert(sizeof(sprites::kPlayerDeath_N_Torso) / sizeof(sprites::kPlayerDeath_N_Torso[0]) == 40, "kPlayerDeath_N_Torso");
static_assert(sizeof(sprites::kPlayerDeath_SE_Torso) / sizeof(sprites::kPlayerDeath_SE_Torso[0]) == 40, "kPlayerDeath_SE_Torso");
static_assert(sizeof(sprites::kPlayerDeath_NE_Torso) / sizeof(sprites::kPlayerDeath_NE_Torso[0]) == 40, "kPlayerDeath_NE_Torso");
static_assert(sizeof(sprites::kPlayerThrow_S_Head) / sizeof(sprites::kPlayerThrow_S_Head[0]) == 40, "kPlayerThrow_S_Head");
static_assert(sizeof(sprites::kPlayerThrow_S_Arms) / sizeof(sprites::kPlayerThrow_S_Arms[0]) == 40, "kPlayerThrow_S_Arms");
static_assert(sizeof(sprites::kPlayerThrow_N_Head) / sizeof(sprites::kPlayerThrow_N_Head[0]) == 40, "kPlayerThrow_N_Head");
static_assert(sizeof(sprites::kPlayerThrow_N_Torso) / sizeof(sprites::kPlayerThrow_N_Torso[0]) == 40, "kPlayerThrow_N_Torso");
static_assert(sizeof(sprites::kPlayerThrow_N_Arms) / sizeof(sprites::kPlayerThrow_N_Arms[0]) == 40, "kPlayerThrow_N_Arms");
static_assert(sizeof(sprites::kPlayerThrow_SE_Head) / sizeof(sprites::kPlayerThrow_SE_Head[0]) == 40, "kPlayerThrow_SE_Head");
static_assert(sizeof(sprites::kPlayerThrow_SE_Torso) / sizeof(sprites::kPlayerThrow_SE_Torso[0]) == 40, "kPlayerThrow_SE_Torso");
static_assert(sizeof(sprites::kPlayerThrow_SE_Arms) / sizeof(sprites::kPlayerThrow_SE_Arms[0]) == 40, "kPlayerThrow_SE_Arms");
static_assert(sizeof(sprites::kPlayerThrow_NE_Head) / sizeof(sprites::kPlayerThrow_NE_Head[0]) == 40, "kPlayerThrow_NE_Head");
static_assert(sizeof(sprites::kPlayerThrow_NE_Torso) / sizeof(sprites::kPlayerThrow_NE_Torso[0]) == 40, "kPlayerThrow_NE_Torso");
static_assert(sizeof(sprites::kPlayerThrow_NE_Arms) / sizeof(sprites::kPlayerThrow_NE_Arms[0]) == 40, "kPlayerThrow_NE_Arms");

int main() {
    using namespace sprites;
    static const char* const* kArrays[] = {
        kPlayerIdleHead,
        kPlayerIdleTorso,
        kPlayerIdleLegs,
        kPlayerIdleFeet,
        kPlayerIdleArms,
        kPlayerIdleSHead,
        kPlayerIdleSTorso,
        kPlayerIdleSLegs,
        kPlayerIdleSFeet,
        kPlayerIdleSArms,
        kPlayerIdleNHead,
        kPlayerIdleNTorso,
        kPlayerIdleNLegs,
        kPlayerIdleNFeet,
        kPlayerIdleNArms,
        kPlayerIdleSEHead,
        kPlayerIdleSETorso,
        kPlayerIdleSELegs,
        kPlayerIdleSEFeet,
        kPlayerIdleSEArms,
        kPlayerIdleNEHead,
        kPlayerIdleNETorso,
        kPlayerIdleNELegs,
        kPlayerIdleNEFeet,
        kPlayerIdleNEArms,
        kPlayerWalkA_S_Arms,
        kPlayerWalkA_N_Arms,
        kPlayerWalkA_SE_Torso,
        kPlayerWalkA_SE_Arms,
        kPlayerWalkA_NE_Torso,
        kPlayerWalkA_NE_Arms,
        kPlayerWalkB_S_Legs,
        kPlayerWalkB_S_Feet,
        kPlayerWalkB_S_Arms,
        kPlayerWalkB_N_Arms,
        kPlayerWalkB_SE_Torso,
        kPlayerWalkB_SE_Legs,
        kPlayerWalkB_SE_Feet,
        kPlayerWalkB_SE_Arms,
        kPlayerWalkB_NE_Torso,
        kPlayerWalkB_NE_Legs,
        kPlayerWalkB_NE_Feet,
        kPlayerWalkB_NE_Arms,
        kPlayerPunch_S_Torso,
        kPlayerPunch_S_Arms,
        kPlayerPunch_N_Torso,
        kPlayerPunch_N_Arms,
        kPlayerPunch_SE_Torso,
        kPlayerPunch_SE_Arms,
        kPlayerPunch_NE_Torso,
        kPlayerPunch_NE_Arms,
        kPlayerPunchDown_S_Torso,
        kPlayerPunchDown_S_Arms,
        kPlayerPunchDown_N_Torso,
        kPlayerPunchDown_N_Arms,
        kPlayerPunchDown_SE_Torso,
        kPlayerPunchDown_SE_Arms,
        kPlayerPunchDown_NE_Torso,
        kPlayerPunchDown_NE_Arms,
        kPlayerWalkAHead,
        kPlayerWalkATorso,
        kPlayerWalkALegs,
        kPlayerWalkAFeet,
        kPlayerWalkCFeet,
        kPlayerWalkAArms,
        kPlayerWalkBHead,
        kPlayerWalkBTorso,
        kPlayerWalkBLegs,
        kPlayerWalkBFeet,
        kPlayerWalkBArms,
        kPlayerJumpHead,
        kPlayerJumpTorso,
        kPlayerJumpLegs,
        kPlayerJumpFeet,
        kPlayerJumpArms,
        kPlayerThrowHead,
        kPlayerThrowTorso,
        kPlayerThrowLegs,
        kPlayerThrowFeet,
        kPlayerThrowArms,
        kPlayerPunchHead,
        kPlayerPunchTorso,
        kPlayerPunchLegs,
        kPlayerPunchFeet,
        kPlayerPunchArms,
        kPlayerPunchUpHead,
        kPlayerPunchUpTorso,
        kPlayerPunchUpLegs,
        kPlayerPunchUpFeet,
        kPlayerPunchUpArms,
        kPlayerPunchUp_S_Torso,
        kPlayerPunchUp_S_Arms,
        kPlayerPunchUp_N_Torso,
        kPlayerPunchUp_N_Arms,
        kPlayerPunchUp_SE_Torso,
        kPlayerPunchUp_SE_Arms,
        kPlayerPunchUp_NE_Torso,
        kPlayerPunchUp_NE_Arms,
        kPlayerPunchDownHead,
        kPlayerPunchDownTorso,
        kPlayerPunchDownLegs,
        kPlayerPunchDownFeet,
        kPlayerPunchDownArms,
        kPlayerHurtHead,
        kPlayerHurtTorso,
        kPlayerHurtLegs,
        kPlayerHurtFeet,
        kPlayerHurtArms,
        kPlayerHurt_S_Head,
        kPlayerHurt_N_Head,
        kPlayerHurt_SE_Head,
        kPlayerHurt_SE_Torso,
        kPlayerHurt_NE_Head,
        kPlayerHurt_NE_Torso,
        kPlayerDeathHead,
        kPlayerDeathTorso,
        kPlayerDeathLegs,
        kPlayerDeathFeet,
        kPlayerDeathArms,
        kPlayerDeath_S_Torso,
        kPlayerDeath_N_Torso,
        kPlayerDeath_SE_Torso,
        kPlayerDeath_NE_Torso,
        kPlayerThrow_S_Head,
        kPlayerThrow_S_Arms,
        kPlayerThrow_N_Head,
        kPlayerThrow_N_Torso,
        kPlayerThrow_N_Arms,
        kPlayerThrow_SE_Head,
        kPlayerThrow_SE_Torso,
        kPlayerThrow_SE_Arms,
        kPlayerThrow_NE_Head,
        kPlayerThrow_NE_Torso,
        kPlayerThrow_NE_Arms,
    };
    static const char* kNames[] = {
        "kPlayerIdleHead",
        "kPlayerIdleTorso",
        "kPlayerIdleLegs",
        "kPlayerIdleFeet",
        "kPlayerIdleArms",
        "kPlayerIdleSHead",
        "kPlayerIdleSTorso",
        "kPlayerIdleSLegs",
        "kPlayerIdleSFeet",
        "kPlayerIdleSArms",
        "kPlayerIdleNHead",
        "kPlayerIdleNTorso",
        "kPlayerIdleNLegs",
        "kPlayerIdleNFeet",
        "kPlayerIdleNArms",
        "kPlayerIdleSEHead",
        "kPlayerIdleSETorso",
        "kPlayerIdleSELegs",
        "kPlayerIdleSEFeet",
        "kPlayerIdleSEArms",
        "kPlayerIdleNEHead",
        "kPlayerIdleNETorso",
        "kPlayerIdleNELegs",
        "kPlayerIdleNEFeet",
        "kPlayerIdleNEArms",
        "kPlayerWalkA_S_Arms",
        "kPlayerWalkA_N_Arms",
        "kPlayerWalkA_SE_Torso",
        "kPlayerWalkA_SE_Arms",
        "kPlayerWalkA_NE_Torso",
        "kPlayerWalkA_NE_Arms",
        "kPlayerWalkB_S_Legs",
        "kPlayerWalkB_S_Feet",
        "kPlayerWalkB_S_Arms",
        "kPlayerWalkB_N_Arms",
        "kPlayerWalkB_SE_Torso",
        "kPlayerWalkB_SE_Legs",
        "kPlayerWalkB_SE_Feet",
        "kPlayerWalkB_SE_Arms",
        "kPlayerWalkB_NE_Torso",
        "kPlayerWalkB_NE_Legs",
        "kPlayerWalkB_NE_Feet",
        "kPlayerWalkB_NE_Arms",
        "kPlayerPunch_S_Torso",
        "kPlayerPunch_S_Arms",
        "kPlayerPunch_N_Torso",
        "kPlayerPunch_N_Arms",
        "kPlayerPunch_SE_Torso",
        "kPlayerPunch_SE_Arms",
        "kPlayerPunch_NE_Torso",
        "kPlayerPunch_NE_Arms",
        "kPlayerPunchDown_S_Torso",
        "kPlayerPunchDown_S_Arms",
        "kPlayerPunchDown_N_Torso",
        "kPlayerPunchDown_N_Arms",
        "kPlayerPunchDown_SE_Torso",
        "kPlayerPunchDown_SE_Arms",
        "kPlayerPunchDown_NE_Torso",
        "kPlayerPunchDown_NE_Arms",
        "kPlayerWalkAHead",
        "kPlayerWalkATorso",
        "kPlayerWalkALegs",
        "kPlayerWalkAFeet",
        "kPlayerWalkCFeet",
        "kPlayerWalkAArms",
        "kPlayerWalkBHead",
        "kPlayerWalkBTorso",
        "kPlayerWalkBLegs",
        "kPlayerWalkBFeet",
        "kPlayerWalkBArms",
        "kPlayerJumpHead",
        "kPlayerJumpTorso",
        "kPlayerJumpLegs",
        "kPlayerJumpFeet",
        "kPlayerJumpArms",
        "kPlayerThrowHead",
        "kPlayerThrowTorso",
        "kPlayerThrowLegs",
        "kPlayerThrowFeet",
        "kPlayerThrowArms",
        "kPlayerPunchHead",
        "kPlayerPunchTorso",
        "kPlayerPunchLegs",
        "kPlayerPunchFeet",
        "kPlayerPunchArms",
        "kPlayerPunchUpHead",
        "kPlayerPunchUpTorso",
        "kPlayerPunchUpLegs",
        "kPlayerPunchUpFeet",
        "kPlayerPunchUpArms",
        "kPlayerPunchUp_S_Torso",
        "kPlayerPunchUp_S_Arms",
        "kPlayerPunchUp_N_Torso",
        "kPlayerPunchUp_N_Arms",
        "kPlayerPunchUp_SE_Torso",
        "kPlayerPunchUp_SE_Arms",
        "kPlayerPunchUp_NE_Torso",
        "kPlayerPunchUp_NE_Arms",
        "kPlayerPunchDownHead",
        "kPlayerPunchDownTorso",
        "kPlayerPunchDownLegs",
        "kPlayerPunchDownFeet",
        "kPlayerPunchDownArms",
        "kPlayerHurtHead",
        "kPlayerHurtTorso",
        "kPlayerHurtLegs",
        "kPlayerHurtFeet",
        "kPlayerHurtArms",
        "kPlayerHurt_S_Head",
        "kPlayerHurt_N_Head",
        "kPlayerHurt_SE_Head",
        "kPlayerHurt_SE_Torso",
        "kPlayerHurt_NE_Head",
        "kPlayerHurt_NE_Torso",
        "kPlayerDeathHead",
        "kPlayerDeathTorso",
        "kPlayerDeathLegs",
        "kPlayerDeathFeet",
        "kPlayerDeathArms",
        "kPlayerDeath_S_Torso",
        "kPlayerDeath_N_Torso",
        "kPlayerDeath_SE_Torso",
        "kPlayerDeath_NE_Torso",
        "kPlayerThrow_S_Head",
        "kPlayerThrow_S_Arms",
        "kPlayerThrow_N_Head",
        "kPlayerThrow_N_Torso",
        "kPlayerThrow_N_Arms",
        "kPlayerThrow_SE_Head",
        "kPlayerThrow_SE_Torso",
        "kPlayerThrow_SE_Arms",
        "kPlayerThrow_NE_Head",
        "kPlayerThrow_NE_Torso",
        "kPlayerThrow_NE_Arms",
    };
    constexpr int N = static_cast<int>(sizeof(kArrays) / sizeof(kArrays[0]));
    assert(N == 134);
    std::set<char> pal;
    for (std::size_t i = 0; i < kPlayerPaletteCount; ++i)
        pal.insert(kPlayerPalette[i].ch);
    for (int i = 0; i < N; ++i) {
        Span sp = spanFor(kNames[i]);
        assert(sp.lo >= 0); // I4: kind conhecido
        int lo = 40, hi = -1;
        for (int y = 0; y < 40; ++y) {
            assert(std::strlen(kArrays[i][y]) == 40); // I2
            assert(kArrays[i][y][0] != 0); // nunca string vazia
            for (int x = 0; x < 40; ++x) {
                const char c = kArrays[i][y][x];
                if (c == '.') continue;
                assert(pal.count(c) && "I3: char sem entrada na paleta");
                if (y < lo) lo = y;
                if (y > hi) hi = y;
            }
        }
        if (hi >= 0) { // I4: conteúdo dentro do span (vazio passa)
            assert(lo >= sp.lo && hi <= sp.hi);
        }
    }
    for (int pi = 0; pi < kPlayerPoseCount; ++pi) { // I5
        auto pose = static_cast<PlayerPose>(pi);
        for (int di = 0; di < kArtDirCount; ++di) {
            const assets::Part* pp =
                posePartsFor(pose, artDirForIndex(di));
            for (int k = 0; k < 5; ++k) {
                assert(pp[k].w == 40 && pp[k].h == 40);
                assert(pp[k].offX == 0 && pp[k].offY == 0);
            }
        }
        if (const assets::Part* ap = posePartsForAir(pose))
            for (int k = 0; k < 5; ++k) {
                assert(ap[k].w == 40 && ap[k].h == 40);
                assert(ap[k].offX == 0 && ap[k].offY == 0);
            }
    }
    std::printf("player palette test OK\n");
    return 0;
}
