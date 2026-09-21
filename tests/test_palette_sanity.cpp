#include <cassert>
#include <cstdio>
#include <cstring>
#include <set>

#include "assets/Sprites/PlayerSprites.h"
#include "assets/Sprites/EnemySprites.h"

// Sanity de palette: 1 char = 1 parte (S ambíguo morreu aqui).
// Mão (H) e rosto (F) nunca se intersectam no mesmo frame.
namespace {

struct Bbox {
    int minY, maxY;
    bool valid;
};

Bbox bboxForChar(const char *const *rows, int h, char ch) {
    int minY = h, maxY = -1;
    for (int y = 0; y < h; ++y) {
        if (std::strchr(rows[y], ch)) {
            if (y < minY) minY = y;
            if (y > maxY) maxY = y;
        }
    }
    return {minY, maxY, maxY >= 0};
}

// Faixas de rows não se intersectam (braço erguido acima do rosto
// é OK; o bug era mão E rosto na MESMA faixa).
bool separated(Bbox a, Bbox b) {
    if (!a.valid || !b.valid) return true;
    return a.maxY < b.minY || b.maxY < a.minY;
}

bool noDuplicateChars(const core::PaletteEntry *pal, std::size_t n) {
    std::set<char> seen;
    for (std::size_t i = 0; i < n; ++i) {
        if (!seen.insert(pal[i].ch).second) return false;
    }
    return true;
}

} // namespace

int main() {
    using namespace sprites;

    { // NoDuplicateChars (1 char = 1 parte, sem ambiguidade)
        assert(noDuplicateChars(kPlayerPal, kPlayerPalCount));
        assert(noDuplicateChars(kDwarfPal, kDwarfPalCount));
        assert(noDuplicateChars(kSlimePal, kSlimePalCount));
    }
    { // PunchHandNotOverFace (o bug original: H e F separados)
        auto hand = bboxForChar(kPlayerPunch, kPlayerH, 'H');
        auto face = bboxForChar(kPlayerPunch, kPlayerH, 'F');
        assert(hand.valid && face.valid);
        assert(separated(hand, face));
    }
    { // AllPlayerFramesHandFaceSeparatedX (H/G nunca dentro do span-X de F)
        // Refinado p/ PunchUp: braços laterais compartilham rows com o
        // rosto (H/G rows 0-6, F 4-6) mas nunca as colunas (H/G 1/10, F 3-8).
        // Faixa-Y rejeitaria; disjunção-X por row pega o bug real
        // (mão-sobre-rosto = H/G dentro de [fMin,fMax]).
        const char *const *frames[] = {
            kPlayerIdle, kPlayerWalkA, kPlayerWalkB, kPlayerJump,
            kPlayerThrow, kPlayerPunch, kPlayerPunchUp, kPlayerPunchDown,
            kPlayerHurt, kPlayerDeath,
        };
        for (auto *f : frames) {
            for (int y = 0; y < kPlayerH; ++y) {
                int fMin = kPlayerW, fMax = -1;
                for (int x = 0; x < kPlayerW; ++x)
                    if (f[y][x] == 'F') {
                        if (x < fMin) fMin = x;
                        if (x > fMax) fMax = x;
                    }
                if (fMax < 0) continue; // sem F na row
                for (int x = 0; x < kPlayerW; ++x) {
                    char c = f[y][x];
                    bool isHand = (c == 'H' || c == 'G');
                    assert(!(isHand && x >= fMin && x <= fMax));
                }
            }
        }
    }
    { // LeftAndRightHandsHaveDistinctChars
        bool hasH = false, hasG = false;
        for (int y = 0; y < kPlayerH; ++y) {
            for (int x = 0; x < kPlayerW; ++x) {
                if (kPlayerIdle[y][x] == 'H') hasH = true;
                if (kPlayerIdle[y][x] == 'G') hasG = true;
            }
        }
        assert(hasH);
        assert(hasG);
    }
    { // LegLeftRightHaveDistinctChars
        bool hasB = false, hasL = false;
        for (int y = 0; y < kPlayerH; ++y) {
            for (int x = 0; x < kPlayerW; ++x) {
                if (kPlayerIdle[y][x] == 'B') hasB = true;
                if (kPlayerIdle[y][x] == 'L') hasL = true;
            }
        }
        assert(hasB);
        assert(hasL);
    }
    { // DwarfHandFaceBarba (H mão, F rosto, R barba presentes)
        const char *const *frames[] = {
            kDwarfIdle, kDwarfWalkA, kDwarfWalkB, kDwarfThrow, kDwarfMelee,
        };
        for (auto *f : frames) {
            auto hand = bboxForChar(f, kDwarfH, 'H');
            auto face = bboxForChar(f, kDwarfH, 'F');
            assert(separated(hand, face));
            assert(bboxForChar(f, kDwarfH, 'R').valid); // barba sempre
        }
    }

    std::printf("palette sanity test OK\n");
    return 0;
}
