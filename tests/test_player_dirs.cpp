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

    std::printf("player dirs test OK\n");
    return 0;
}
