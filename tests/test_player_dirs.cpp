/**
 * @file tests/test_player_dirs.cpp
 * @brief Trava a arte direcional do player (Fase D, onda 1).
 * @details Grade full-40 por parte (spans auditados via checkSpan),
 * presença de pixel por parte (hitbox nunca some), S/N simétricos,
 * SE/NE com 3/4 real e frames diferentes do E. Roda com make test.
 */
#include <cassert>
#include <cstdio>
#include <cstring>
#include "assets/PlayerSprite.h"
#include "assets/SpriteComposer.h"
#include "assets/Sprites/PlayerParts.h"
#include "assets/Sprites/PlayerSprites.h"

namespace {

// Canvas 40x40 (auditoria lado a lado): 40 rows de 40 chars, arte
// 12-wide centrada nas cols 14-25; conteúdo mora em [y0,y1], resto
// '.' (linha N == row N do composto).
template <std::size_t N>
void checkSpan(const char* const (&rows)[N], int y0, int y1) {
    assert(N == 40);
    assert(y0 >= 0 && y1 < 40 && y0 <= y1);
    for (int y = 0; y < 40; ++y) {
        assert(std::strlen(rows[y]) == 40);
        if (y < y0 || y > y1)
            for (int x = 0; x < 40; ++x) assert(rows[y][x] == '.');
    }
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
    assert(hasChar(pp[4], "G"));   // braço esq (Tier 1: G lit)
    assert(hasChar(pp[4], "Hh"));  // braço dir (Tier 1: h sombra)
}

bool composedDiffers(const assets::Part* a, const assets::Part* b) {
    const auto ca = assets::compose(a, 5, sprites::kPlayerW, sprites::kPlayerH);
    const auto cb = assets::compose(b, 5, sprites::kPlayerW, sprites::kPlayerH);
    return ca != cb;
}

} // namespace

int main() {
    using sprites::PlayerPose;
    using support::Facing;

    { // IdleDims (spans full-40, 40 chars por row)
        using namespace sprites;
        checkSpan(kPlayerIdleSHead, 0, 11);
        checkSpan(kPlayerIdleSTorso, 12, 27);
        checkSpan(kPlayerIdleSLegs, 28, 33);
        checkSpan(kPlayerIdleSFeet, 34, 39);
        checkSpan(kPlayerIdleSArms, 12, 27);
        checkSpan(kPlayerIdleSEHead, 0, 11);
        checkSpan(kPlayerIdleSETorso, 12, 27);
        checkSpan(kPlayerIdleSELegs, 28, 33);
        checkSpan(kPlayerIdleSEFeet, 34, 39);
        checkSpan(kPlayerIdleSEArms, 12, 27);
        checkSpan(kPlayerIdleNEHead, 0, 11);
        checkSpan(kPlayerIdleNETorso, 12, 27);
        checkSpan(kPlayerIdleNELegs, 28, 33);
        checkSpan(kPlayerIdleNEFeet, 34, 39);
        checkSpan(kPlayerIdleNEArms, 12, 27);
        checkSpan(kPlayerIdleNHead, 0, 11);
        checkSpan(kPlayerIdleNTorso, 12, 27);
        checkSpan(kPlayerIdleNLegs, 28, 33);
        checkSpan(kPlayerIdleNFeet, 34, 39);
        checkSpan(kPlayerIdleNArms, 12, 27);
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
    { // WalkSideViewCycle (sagital: A/C contact, B/D passing)
        using namespace sprites;
        checkSpan(kPlayerWalkALegs, 28, 33);
        checkSpan(kPlayerWalkAFeet, 34, 39);
        checkSpan(kPlayerWalkBLegs, 28, 33);
        checkSpan(kPlayerWalkBFeet, 34, 39);
        checkSpan(kPlayerWalkCLegs, 28, 33);
        checkSpan(kPlayerWalkCFeet, 34, 39);
        checkSpan(kPlayerWalkDLegs, 28, 33);
        checkSpan(kPlayerWalkDFeet, 34, 39);
        // A/C = contact: pernas próprias (sem reuso), pés no chão.
        // 'c' (coxa longe) conta como torso, como o 'C'.
        assert(std::strchr(kPlayerWalkALegs[28], 'c') != nullptr);
        assert(std::strchr(kPlayerWalkCLegs[28], 'c') != nullptr);
        assert(std::strchr(kPlayerWalkAFeet[39], 'L') != nullptr);
        assert(std::strchr(kPlayerWalkAFeet[39], 'b') != nullptr);
        assert(std::strchr(kPlayerWalkCFeet[39], 'L') != nullptr);
        assert(std::strchr(kPlayerWalkCFeet[39], 'b') != nullptr);
        // B/D = passing: pé do balanço some 4 rows (só apoio no chão).
        for (int y = 36; y < 40; ++y) {
            assert(std::strchr(kPlayerWalkBFeet[y], 'L') == nullptr);
            assert(std::strchr(kPlayerWalkBFeet[y], 'b') != nullptr);
            assert(std::strchr(kPlayerWalkDFeet[y], 'L') == nullptr);
            assert(std::strchr(kPlayerWalkDFeet[y], 'b') != nullptr);
        }
        assert(std::strchr(kPlayerWalkBFeet[34], 'L') != nullptr);
        assert(std::strchr(kPlayerWalkDFeet[34], 'L') != nullptr);
        // 4 frames distintos no composto (pernas próprias por frame).
        const assets::Part* a = sprites::posePartsFor(PlayerPose::WalkA, Facing::E);
        const assets::Part* b = sprites::posePartsFor(PlayerPose::WalkB, Facing::E);
        const assets::Part* c = sprites::posePartsFor(PlayerPose::WalkC, Facing::E);
        const assets::Part* d = sprites::posePartsFor(PlayerPose::WalkD, Facing::E);
        assert(composedDiffers(a, b));
        assert(composedDiffers(a, c));
        assert(composedDiffers(a, d));
        assert(composedDiffers(b, c));
        assert(composedDiffers(b, d));
        assert(composedDiffers(c, d));
        // Locomoção nunca usa S/N: artDirFor força E/W (side-view).
        assert(game::artDirFor(PlayerPose::Idle, Facing::S) == Facing::E);
        assert(game::artDirFor(PlayerPose::WalkA, Facing::N) == Facing::E);
        assert(game::artDirFor(PlayerPose::WalkB, Facing::SW) == Facing::W);
        assert(game::artDirFor(PlayerPose::WalkC, Facing::NE) == Facing::E);
        assert(game::artDirFor(PlayerPose::WalkD, Facing::S) == Facing::E);
    }
    { // AirAttackLegs (Punch/Throw no ar: tronco do golpe + tucked Jump)
        using namespace sprites;
        using game::artDirFor;
        const PlayerPose atks[] = {PlayerPose::Punch, PlayerPose::PunchUp,
                                   PlayerPose::PunchDown, PlayerPose::Throw};
        for (PlayerPose a : atks) {
            const assets::Part* ap = sprites::posePartsForAir(a);
            assert(ap != nullptr);
            for (int i = 0; i < 5; ++i) {
                assert(ap[i].w == 40);
                assert(ap[i].offX == 0);
            }
            assert(ap[2].h == 40 && ap[2].offY == 0); // legs full-40
            assert(ap[3].h == 40 && ap[3].offY == 0); // feet full-40
            // Pernas: os mesmos arrays do Jump (tucked, pés em ponta).
            assert(ap[2].rows == kPlayerJumpLegs);
            assert(ap[3].rows == kPlayerJumpFeet);
            // Tronco/cabeça/braços: os mesmos arrays do E de chão.
            const assets::Part* e =
                sprites::posePartsFor(a, Facing::E);
            assert(ap[0].rows == e[0].rows);
            assert(ap[1].rows == e[1].rows);
            assert(ap[4].rows == e[4].rows);
            // E o chão continua plantado (ar ≠ chão em alguma row
            // das pernas).
            bool legsDiffer = false;
            for (int y = 28; y < 40; ++y)
                if (std::string(e[2].rows[y]) != ap[2].rows[y] ||
                    std::string(e[3].rows[y]) != ap[3].rows[y])
                    legsDiffer = true;
            assert(legsDiffer);
            // Direção no ar: side-view; no chão: 8-way.
            assert(artDirFor(a, Facing::S, true) == Facing::E);
            assert(artDirFor(a, Facing::NW, true) == Facing::W);
            assert(artDirFor(a, Facing::S, false) == Facing::S);
        }
        assert(sprites::posePartsForAir(PlayerPose::Jump) == nullptr);
        assert(sprites::posePartsForAir(PlayerPose::Idle) == nullptr);
        assert(sprites::posePartsForAir(PlayerPose::Hurt) == nullptr);
    }
    { // MarchAttackLegs (soco andando: tronco do golpe + pernas)
        using namespace sprites;
        using game::isMarchAttackPose;
        using game::isWalkPose;
        assert(isMarchAttackPose(PlayerPose::Punch));
        assert(isMarchAttackPose(PlayerPose::Throw));
        assert(!isMarchAttackPose(PlayerPose::WalkA));
        assert(!isMarchAttackPose(PlayerPose::Jump));
        assert(isWalkPose(PlayerPose::WalkD));
        assert(!isWalkPose(PlayerPose::Idle));
        const PlayerPose atks[] = {PlayerPose::Punch, PlayerPose::PunchUp,
                                   PlayerPose::PunchDown, PlayerPose::Throw};
        const PlayerPose walks[] = {PlayerPose::WalkA, PlayerPose::WalkB,
                                    PlayerPose::WalkC, PlayerPose::WalkD};
        for (PlayerPose a : atks) {
            for (PlayerPose w : walks) {
                const assets::Part* mp = sprites::posePartsForMarch(a, w);
                assert(mp != nullptr);
                for (int i = 0; i < 5; ++i) {
                    assert(mp[i].w == 40 && mp[i].h == 40);
                    assert(mp[i].offX == 0 && mp[i].offY == 0);
                }
                // Tronco do golpe (E) + pernas da marcha (mesmos arrays).
                const assets::Part* e = sprites::posePartsFor(a, Facing::E);
                const assets::Part* l = sprites::posePartsFor(w, Facing::E);
                assert(mp[0].rows == e[0].rows);
                assert(mp[1].rows == e[1].rows);
                assert(mp[4].rows == e[4].rows);
                assert(mp[2].rows == l[2].rows);
                assert(mp[3].rows == l[3].rows);
            }
            // Marcha lê no composto (troca de perna a cada frame).
            const assets::Part* m0 =
                sprites::posePartsForMarch(a, PlayerPose::WalkA);
            const assets::Part* m1 =
                sprites::posePartsForMarch(a, PlayerPose::WalkB);
            const assets::Part* m2 =
                sprites::posePartsForMarch(a, PlayerPose::WalkC);
            assert(composedDiffers(m0, m1));
            assert(composedDiffers(m0, m2));
            assert(composedDiffers(m1, m2));
        }
        assert(sprites::posePartsForMarch(PlayerPose::Idle,
                                          PlayerPose::WalkA) == nullptr);
        assert(sprites::posePartsForMarch(PlayerPose::Punch,
                                          PlayerPose::Idle) == nullptr);
    }
    { // WalkDims (alturas exatas das 18 novas, 12 chars por row)
        using namespace sprites;
        checkSpan(kPlayerWalkA_S_Arms, 12, 27);
        checkSpan(kPlayerWalkA_N_Arms, 12, 27);
        checkSpan(kPlayerWalkA_SE_Torso, 12, 27);
        checkSpan(kPlayerWalkA_SE_Arms, 12, 27);
        checkSpan(kPlayerWalkA_NE_Torso, 12, 27);
        checkSpan(kPlayerWalkA_NE_Arms, 12, 27);
        checkSpan(kPlayerWalkB_S_Legs, 28, 33);
        checkSpan(kPlayerWalkB_S_Feet, 34, 39);
        checkSpan(kPlayerWalkB_S_Arms, 12, 27);
        checkSpan(kPlayerWalkB_N_Arms, 12, 27);
        checkSpan(kPlayerWalkB_SE_Torso, 12, 27);
        checkSpan(kPlayerWalkB_SE_Legs, 28, 33);
        checkSpan(kPlayerWalkB_SE_Feet, 34, 39);
        checkSpan(kPlayerWalkB_SE_Arms, 12, 27);
        checkSpan(kPlayerWalkB_NE_Torso, 12, 27);
        checkSpan(kPlayerWalkB_NE_Legs, 28, 33);
        checkSpan(kPlayerWalkB_NE_Feet, 34, 39);
        checkSpan(kPlayerWalkB_NE_Arms, 12, 27);
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
        checkSpan(kPlayerPunch_S_Torso, 12, 27);
        checkSpan(kPlayerPunch_S_Arms, 12, 27);
        checkSpan(kPlayerPunch_N_Torso, 12, 27);
        checkSpan(kPlayerPunch_N_Arms, 12, 27);
        checkSpan(kPlayerPunch_SE_Torso, 12, 27);
        checkSpan(kPlayerPunch_SE_Arms, 12, 27);
        checkSpan(kPlayerPunch_NE_Torso, 12, 27);
        checkSpan(kPlayerPunch_NE_Arms, 12, 27);
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
        checkSpan(kPlayerPunchUp_S_Torso, 12, 27);
        checkSpan(kPlayerPunchUp_S_Arms, 12, 27);
        checkSpan(kPlayerPunchUp_N_Torso, 12, 27);
        checkSpan(kPlayerPunchUp_N_Arms, 12, 27);
        checkSpan(kPlayerPunchUp_SE_Torso, 12, 27);
        checkSpan(kPlayerPunchUp_SE_Arms, 12, 27);
        checkSpan(kPlayerPunchUp_NE_Torso, 12, 27);
        checkSpan(kPlayerPunchUp_NE_Arms, 12, 27);
        checkSpan(kPlayerPunchDown_S_Torso, 12, 27);
        checkSpan(kPlayerPunchDown_S_Arms, 12, 27);
        checkSpan(kPlayerPunchDown_N_Torso, 12, 27);
        checkSpan(kPlayerPunchDown_N_Arms, 12, 27);
        checkSpan(kPlayerPunchDown_SE_Torso, 12, 27);
        checkSpan(kPlayerPunchDown_SE_Arms, 12, 27);
        checkSpan(kPlayerPunchDown_NE_Torso, 12, 27);
        checkSpan(kPlayerPunchDown_NE_Arms, 12, 27);
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
    { // HurtJumpDims (6 novas: 4 cabeças + 2 torsos, spans full-40)
        using namespace sprites;
        checkSpan(kPlayerHurt_S_Head, 0, 11);
        checkSpan(kPlayerHurt_N_Head, 0, 11);
        checkSpan(kPlayerHurt_SE_Head, 0, 11);
        checkSpan(kPlayerHurt_SE_Torso, 12, 27);
        checkSpan(kPlayerHurt_NE_Head, 0, 11);
        checkSpan(kPlayerHurt_NE_Torso, 12, 27);
    }
    { // JumpHurtNoBakedArms (G/H fora das 7 cabeças: braço é IK ou
        // overlay; PunchUp tinha os braços erguidos assados aqui e o
        // IK desenhava por cima — 4 braços. Agora moram no overlay.)
        using namespace sprites;
        const char* const* heads[] = {
            kPlayerJumpHead, kPlayerHurtHead, kPlayerHurt_S_Head,
            kPlayerHurt_N_Head, kPlayerHurt_SE_Head, kPlayerHurt_NE_Head,
            kPlayerPunchUpHead,
        };
        for (const char* const* h : heads)
            for (int y = 0; y < 12; ++y) {
                assert(std::strchr(h[y], 'G') == nullptr);
                assert(std::strchr(h[y], 'H') == nullptr);
            }
        // E o overlay do PunchUp cobre os erguidos (preview/hitbox
        // intactos: mesmos pixels, outra parte).
        for (int y = 0; y < 4; ++y) {
            assert(std::strchr(kPlayerPunchUpArms[y], 'G') != nullptr);
            assert(std::strchr(kPlayerPunchUpArms[y], 'H') != nullptr);
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
    { // HurtJumpDirs (arte existe nas 5 dirs p/ ambas; Hurt segue
        // 8-way, Jump é marcha aérea side-view como Idle/Walk)
        using game::artDirFor;
        const PlayerPose poses[] = {PlayerPose::Hurt, PlayerPose::Jump};
        const Facing all4[] = {Facing::S, Facing::SE, Facing::NE,
                               Facing::N};
        const Facing syms[] = {Facing::S, Facing::N};
        for (PlayerPose pose : poses) {
            if (pose == PlayerPose::Hurt) {
                for (Facing f : all4)
                    assert(artDirFor(pose, f) == f);
            } else {
                for (Facing f : all4)
                    assert(artDirFor(pose, f) == Facing::E);
                assert(artDirFor(pose, Facing::W) == Facing::W);
                assert(artDirFor(pose, Facing::NW) == Facing::W);
            }
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
        checkSpan(kPlayerDeath_S_Torso, 12, 27);
        checkSpan(kPlayerDeath_N_Torso, 12, 27);
        checkSpan(kPlayerDeath_SE_Torso, 12, 27);
        checkSpan(kPlayerDeath_NE_Torso, 12, 27);
        checkSpan(kPlayerThrow_S_Head, 0, 11);
        checkSpan(kPlayerThrow_S_Arms, 12, 27);
        checkSpan(kPlayerThrow_N_Head, 0, 11);
        checkSpan(kPlayerThrow_N_Torso, 12, 27);
        checkSpan(kPlayerThrow_N_Arms, 12, 27);
        checkSpan(kPlayerThrow_SE_Head, 0, 11);
        checkSpan(kPlayerThrow_SE_Torso, 12, 27);
        checkSpan(kPlayerThrow_SE_Arms, 12, 27);
        checkSpan(kPlayerThrow_NE_Head, 0, 11);
        checkSpan(kPlayerThrow_NE_Torso, 12, 27);
        checkSpan(kPlayerThrow_NE_Arms, 12, 27);
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
