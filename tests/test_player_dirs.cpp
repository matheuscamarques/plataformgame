/**
 * @file tests/test_player_dirs.cpp
 * @brief Trava a arte direcional do player (Fase D, onda 1).
 * @details Dims exatas por parte (12/16/6/6/16, rows de 12 chars),
 * presença de pixel por parte (hitbox nunca some), S/N simétricos,
 * SE/NE com 3/4 real e frames diferentes do E. Roda com make test.
 */
#include <cassert>
#include <cstdio>
#include <cstring>
#include "assets/PlayerSprite.h"
#include "assets/SpriteComposer.h"
#include "assets/Sprites/PlayerParts.h"

namespace {

template <std::size_t N>
void checkRows(const char* const (&rows)[N], int expectH) {
    assert(N == static_cast<std::size_t>(expectH));
    for (const char* r : rows) assert(std::strlen(r) == 12);
}

bool hasChar(const assets::Part& p, const char* chars) {
    for (int y = 0; y < p.h; ++y)
        for (int x = 0; x < p.w; ++x)
            if (std::strchr(chars, p.rows[y][x])) return true;
    return false;
}

// Simetria módulo-espelho: pares esq/dir (G/H braços, L/B pernas)
// valem como iguais — vista simétrica de verdade.
bool symEq(char a, char b) {
    if (a == b) return true;
    if ((a == 'G' && b == 'H') || (a == 'H' && b == 'G')) return true;
    if ((a == 'L' && b == 'B') || (a == 'B' && b == 'L')) return true;
    return false;
}

bool rowSym(const char* r) {
    const std::size_t n = std::strlen(r);
    for (std::size_t i = 0; i < n / 2; ++i)
        if (!symEq(r[i], r[n - 1 - i])) return false;
    return true;
}

bool partSym(const assets::Part& p) {
    for (int y = 0; y < p.h; ++y)
        if (!rowSym(p.rows[y])) return false;
    return true;
}

// Presença no frame composto. arms=nullptr pula o teste de braços
// (Jump/Hurt: braços são IK procedural, não pixels na arte).
void checkComposed(const assets::Part* pp, const char* head,
                   const char* torso, const char* legs,
                   const char* arms = nullptr,
                   const char* weapon = nullptr) {
    bool h = false, t = false, l = false, a = (arms == nullptr), w = true;
    for (int i = 0; i < 5; ++i) {
        if (hasChar(pp[i], head)) h = true;
        if (hasChar(pp[i], torso)) t = true;
        if (hasChar(pp[i], legs)) l = true;
        if (arms && hasChar(pp[i], arms)) a = true;
    }
    if (weapon) {
        w = false;
        for (int i = 0; i < 5; ++i)
            if (hasChar(pp[i], weapon)) w = true;
    }
    assert(h && t && l && a && w);
}

// Presença por parte (hitbox e anchors nunca somem por direção).
void checkPresence(const assets::Part* pp) {
    assert(hasChar(pp[0], "FE"));  // cabeça (F ou E)
    assert(hasChar(pp[1], "C"));   // torso
    assert(hasChar(pp[2], "CLB")); // pernas
    assert(hasChar(pp[3], "LB"));  // pés
    assert(hasChar(pp[4], "G"));   // braço esq
    assert(hasChar(pp[4], "H"));   // braço dir
}

bool composedDiffers(const assets::Part* a, const assets::Part* b) {
    const auto ca = assets::compose(a, 5, 12, 40);
    const auto cb = assets::compose(b, 5, 12, 40);
    return ca != cb;
}

} // namespace

int main() {
    using sprites::PlayerPose;
    using support::Facing;

    { // IdleDims (alturas exatas por parte, 12 chars por row)
        using namespace sprites;
        checkRows(kPlayerIdleSHead, 12);
        checkRows(kPlayerIdleSTorso, 16);
        checkRows(kPlayerIdleSLegs, 6);
        checkRows(kPlayerIdleSFeet, 6);
        checkRows(kPlayerIdleSArms, 16);
        checkRows(kPlayerIdleSEHead, 12);
        checkRows(kPlayerIdleSETorso, 16);
        checkRows(kPlayerIdleSELegs, 6);
        checkRows(kPlayerIdleSEFeet, 6);
        checkRows(kPlayerIdleSEArms, 16);
        checkRows(kPlayerIdleNEHead, 12);
        checkRows(kPlayerIdleNETorso, 16);
        checkRows(kPlayerIdleNELegs, 6);
        checkRows(kPlayerIdleNEFeet, 6);
        checkRows(kPlayerIdleNEArms, 16);
        checkRows(kPlayerIdleNHead, 12);
        checkRows(kPlayerIdleNTorso, 16);
        checkRows(kPlayerIdleNLegs, 6);
        checkRows(kPlayerIdleNFeet, 6);
        checkRows(kPlayerIdleNArms, 16);
    }
    { // IdlePresence (toda direção mantém as 6 partes vivas)
        checkPresence(sprites::posePartsFor(PlayerPose::Idle, Facing::S));
        checkPresence(sprites::posePartsFor(PlayerPose::Idle, Facing::SE));
        checkPresence(sprites::posePartsFor(PlayerPose::Idle, Facing::NE));
        checkPresence(sprites::posePartsFor(PlayerPose::Idle, Facing::N));
        checkPresence(sprites::posePartsFor(PlayerPose::Idle, Facing::E));
    }
    { // SymmViews (S e N: todas as rows simétricas módulo-espelho)
        const assets::Part* s =
            sprites::posePartsFor(PlayerPose::Idle, Facing::S);
        const assets::Part* n =
            sprites::posePartsFor(PlayerPose::Idle, Facing::N);
        for (int i = 0; i < 5; ++i) {
            assert(partSym(s[i]));
            assert(partSym(n[i]));
        }
    }
    { // ThreeQuarterViews (SE/NE: ao menos 1 row assimétrica de verdade)
        const assets::Part* se =
            sprites::posePartsFor(PlayerPose::Idle, Facing::SE);
        const assets::Part* ne =
            sprites::posePartsFor(PlayerPose::Idle, Facing::NE);
        bool seAsym = false, neAsym = false;
        for (int i = 0; i < 5; ++i) {
            if (!partSym(se[i])) seAsym = true;
            if (!partSym(ne[i])) neAsym = true;
        }
        assert(seAsym && neAsym);
    }
    { // IdleDiffersFromE (5 dirs = 5 silhuetas no frame composto)
        const assets::Part* e =
            sprites::posePartsFor(PlayerPose::Idle, Facing::E);
        assert(composedDiffers(
            e, sprites::posePartsFor(PlayerPose::Idle, Facing::S)));
        assert(composedDiffers(
            e, sprites::posePartsFor(PlayerPose::Idle, Facing::SE)));
        assert(composedDiffers(
            e, sprites::posePartsFor(PlayerPose::Idle, Facing::NE)));
        assert(composedDiffers(
            e, sprites::posePartsFor(PlayerPose::Idle, Facing::N)));
    }
    { // WalkDims (alturas exatas das 18 novas, 12 chars por row)
        using namespace sprites;
        checkRows(kPlayerWalkA_S_Arms, 16);
        checkRows(kPlayerWalkA_N_Arms, 16);
        checkRows(kPlayerWalkA_SE_Torso, 16);
        checkRows(kPlayerWalkA_SE_Arms, 16);
        checkRows(kPlayerWalkA_NE_Torso, 16);
        checkRows(kPlayerWalkA_NE_Arms, 16);
        checkRows(kPlayerWalkB_S_Legs, 6);
        checkRows(kPlayerWalkB_S_Feet, 6);
        checkRows(kPlayerWalkB_S_Arms, 16);
        checkRows(kPlayerWalkB_N_Arms, 16);
        checkRows(kPlayerWalkB_SE_Torso, 16);
        checkRows(kPlayerWalkB_SE_Legs, 6);
        checkRows(kPlayerWalkB_SE_Feet, 6);
        checkRows(kPlayerWalkB_SE_Arms, 16);
        checkRows(kPlayerWalkB_NE_Torso, 16);
        checkRows(kPlayerWalkB_NE_Legs, 6);
        checkRows(kPlayerWalkB_NE_Feet, 6);
        checkRows(kPlayerWalkB_NE_Arms, 16);
    }
    { // WalkPresenceSymDiff (vivas + S/N simétricas + 3/4 + ≠E)
        const PlayerPose poses[] = {PlayerPose::WalkA, PlayerPose::WalkB,
                                    PlayerPose::WalkC, PlayerPose::WalkD};
        const Facing syms[] = {Facing::S, Facing::N};
        const Facing asyms[] = {Facing::SE, Facing::NE};
        const Facing all4[] = {Facing::S, Facing::SE, Facing::NE,
                               Facing::N};
        for (PlayerPose pose : poses) {
            for (Facing f : all4)
                checkPresence(sprites::posePartsFor(pose, f));
            for (Facing f : syms) {
                const assets::Part* pp = sprites::posePartsFor(pose, f);
                for (int i = 0; i < 5; ++i) assert(partSym(pp[i]));
            }
            bool asymFound = false;
            for (Facing f : asyms) {
                const assets::Part* pp = sprites::posePartsFor(pose, f);
                for (int i = 0; i < 5; ++i)
                    if (!partSym(pp[i])) asymFound = true;
            }
            assert(asymFound);
            const assets::Part* e = sprites::posePartsFor(pose, Facing::E);
            for (Facing f : all4)
                assert(composedDiffers(e, sprites::posePartsFor(pose, f)));
        }
    }
    { // PunchDims (8 novas: torso+braço por direção, 16 rows)
        using namespace sprites;
        checkRows(kPlayerPunch_S_Torso, 16);
        checkRows(kPlayerPunch_S_Arms, 16);
        checkRows(kPlayerPunch_N_Torso, 16);
        checkRows(kPlayerPunch_N_Arms, 16);
        checkRows(kPlayerPunch_SE_Torso, 16);
        checkRows(kPlayerPunch_SE_Arms, 16);
        checkRows(kPlayerPunch_NE_Torso, 16);
        checkRows(kPlayerPunch_NE_Arms, 16);
    }
    { // PunchPresenceDiff (vivas + ≠E; soco é assimétrico por natureza)
        using game::artDirFor;
        assert(artDirFor(PlayerPose::Punch, Facing::S) == Facing::S);
        assert(artDirFor(PlayerPose::Punch, Facing::SE) == Facing::SE);
        assert(artDirFor(PlayerPose::Punch, Facing::NE) == Facing::NE);
        assert(artDirFor(PlayerPose::Punch, Facing::N) == Facing::N);
        assert(artDirFor(PlayerPose::Punch, Facing::W) == Facing::E);
        const Facing all4[] = {Facing::S, Facing::SE, Facing::NE,
                               Facing::N};
        const assets::Part* e =
            sprites::posePartsFor(PlayerPose::Punch, Facing::E);
        for (Facing f : all4) {
            checkPresence(sprites::posePartsFor(PlayerPose::Punch, f));
            assert(composedDiffers(
                e, sprites::posePartsFor(PlayerPose::Punch, f)));
        }
    }
    { // PunchUpDownDims (16 novas: torso+braço por direção)
        using namespace sprites;
        checkRows(kPlayerPunchUp_S_Torso, 16);
        checkRows(kPlayerPunchUp_S_Arms, 16);
        checkRows(kPlayerPunchUp_N_Torso, 16);
        checkRows(kPlayerPunchUp_N_Arms, 16);
        checkRows(kPlayerPunchUp_SE_Torso, 16);
        checkRows(kPlayerPunchUp_SE_Arms, 16);
        checkRows(kPlayerPunchUp_NE_Torso, 16);
        checkRows(kPlayerPunchUp_NE_Arms, 16);
        checkRows(kPlayerPunchDown_S_Torso, 16);
        checkRows(kPlayerPunchDown_S_Arms, 16);
        checkRows(kPlayerPunchDown_N_Torso, 16);
        checkRows(kPlayerPunchDown_N_Arms, 16);
        checkRows(kPlayerPunchDown_SE_Torso, 16);
        checkRows(kPlayerPunchDown_SE_Arms, 16);
        checkRows(kPlayerPunchDown_NE_Torso, 16);
        checkRows(kPlayerPunchDown_NE_Arms, 16);
    }
    { // PunchUpDownPresenceDiff (vivas + ≠E nas 8 combinações)
        using game::artDirFor;
        const PlayerPose poses[] = {PlayerPose::PunchUp,
                                    PlayerPose::PunchDown};
        const Facing all4[] = {Facing::S, Facing::SE, Facing::NE,
                               Facing::N};
        for (PlayerPose pose : poses) {
            for (Facing f : all4)
                assert(artDirFor(pose, f) == f);
            assert(artDirFor(pose, Facing::W) == Facing::E);
            const assets::Part* e = sprites::posePartsFor(pose, Facing::E);
            for (Facing f : all4) {
                checkPresence(sprites::posePartsFor(pose, f));
                assert(composedDiffers(
                    e, sprites::posePartsFor(pose, f)));
            }
        }
    }
    { // HurtJumpDims (6 novas: 4 cabeças + 2 torsos, 12/16 rows)
        using namespace sprites;
        checkRows(kPlayerHurt_S_Head, 12);
        checkRows(kPlayerHurt_N_Head, 12);
        checkRows(kPlayerHurt_SE_Head, 12);
        checkRows(kPlayerHurt_SE_Torso, 16);
        checkRows(kPlayerHurt_NE_Head, 12);
        checkRows(kPlayerHurt_NE_Torso, 16);
    }
    { // JumpHurtNoBakedArms (G/H fora das 6 cabeças: braço é IK)
        using namespace sprites;
        const char* const* heads[] = {
            kPlayerJumpHead, kPlayerHurtHead, kPlayerHurt_S_Head,
            kPlayerHurt_N_Head, kPlayerHurt_SE_Head, kPlayerHurt_NE_Head,
        };
        for (const char* const* h : heads)
            for (int y = 0; y < 12; ++y) {
                assert(std::strchr(h[y], 'G') == nullptr);
                assert(std::strchr(h[y], 'H') == nullptr);
            }
    }
    { // TorsosNoBakedArms (12 poses × 5 dirs: torso é só corpo/roupa)
        const Facing dirs[] = {Facing::E, Facing::S, Facing::SE,
                               Facing::NE, Facing::N};
        for (int i = 0; i < sprites::kPlayerPoseCount; ++i) {
            const auto pose = static_cast<PlayerPose>(i);
            for (Facing f : dirs) {
                const assets::Part* pp =
                    sprites::posePartsFor(pose, f);
                const assets::Part& torso = pp[1];
                for (int y = 0; y < torso.h; ++y) {
                    assert(std::strchr(torso.rows[y], 'G') == nullptr);
                    assert(std::strchr(torso.rows[y], 'H') == nullptr);
                }
            }
        }
    }
    { // HurtJumpDirs (vivas no composto + ≠E; S/N simétricas)
        using game::artDirFor;
        const PlayerPose poses[] = {PlayerPose::Hurt, PlayerPose::Jump};
        const Facing all4[] = {Facing::S, Facing::SE, Facing::NE,
                               Facing::N};
        const Facing syms[] = {Facing::S, Facing::N};
        for (PlayerPose pose : poses) {
            for (Facing f : all4)
                assert(artDirFor(pose, f) == f);
            const assets::Part* e = sprites::posePartsFor(pose, Facing::E);
            for (Facing f : all4) {
                const assets::Part* pp = sprites::posePartsFor(pose, f);
                checkComposed(pp, "FE", "C", "LB"); // sem GH: braço é IK
                assert(composedDiffers(e, pp));
            }
            for (Facing f : syms) {
                const assets::Part* pp = sprites::posePartsFor(pose, f);
                for (int i = 0; i < 5; ++i) assert(partSym(pp[i]));
            }
        }
    }
    { // DeathThrowDims (4 torsos + 4 cabeças + 4 torsos + 4 braços)
        using namespace sprites;
        checkRows(kPlayerDeath_S_Torso, 16);
        checkRows(kPlayerDeath_N_Torso, 16);
        checkRows(kPlayerDeath_SE_Torso, 16);
        checkRows(kPlayerDeath_NE_Torso, 16);
        checkRows(kPlayerThrow_S_Head, 12);
        checkRows(kPlayerThrow_S_Arms, 16);
        checkRows(kPlayerThrow_N_Head, 12);
        checkRows(kPlayerThrow_N_Torso, 16);
        checkRows(kPlayerThrow_N_Arms, 16);
        checkRows(kPlayerThrow_SE_Head, 12);
        checkRows(kPlayerThrow_SE_Torso, 16);
        checkRows(kPlayerThrow_SE_Arms, 16);
        checkRows(kPlayerThrow_NE_Head, 12);
        checkRows(kPlayerThrow_NE_Torso, 16);
        checkRows(kPlayerThrow_NE_Arms, 16);
    }
    { // DeathThrowDirs (vivas no composto + ≠E; Death S/N simétricas)
        using game::artDirFor;
        const Facing all4[] = {Facing::S, Facing::SE, Facing::NE,
                               Facing::N};
        for (Facing f : all4) {
            assert(artDirFor(PlayerPose::Death, f) == f);
            assert(artDirFor(PlayerPose::Throw, f) == f);
        }
        const assets::Part* de =
            sprites::posePartsFor(PlayerPose::Death, Facing::E);
        const assets::Part* te =
            sprites::posePartsFor(PlayerPose::Throw, Facing::E);
        for (Facing f : all4) {
            const assets::Part* pd =
                sprites::posePartsFor(PlayerPose::Death, f);
            checkComposed(pd, "FE", "C", "LB", "GH");
            assert(composedDiffers(de, pd));
            const assets::Part* pt =
                sprites::posePartsFor(PlayerPose::Throw, f);
            checkComposed(pt, "FE", "C", "LB", "GH", "Tt");
            assert(composedDiffers(te, pt));
        }
        for (Facing f : {Facing::S, Facing::N}) {
            const assets::Part* pd =
                sprites::posePartsFor(PlayerPose::Death, f);
            for (int i = 0; i < 5; ++i) assert(partSym(pd[i]));
        }
    }

    std::printf("player dirs test OK\n");
    return 0;
}
