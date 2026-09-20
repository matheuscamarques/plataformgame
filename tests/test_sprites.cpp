#include <cassert>
#include <cstdio>
#include <cstring>
#include "game/Sprites.h"

// ASCII art: widths exatas (linha errada = sprite deslocada) e paletas.
// build() NÃO é chamado (textura exige contexto GL, sem teste headless).
int main() {
    using namespace sprites;

    { // PlayerWidths (12x20 todos os frames)
        const char *const *frames[] = {
            kPlayerIdle, kPlayerWalk0, kPlayerWalk1, kPlayerWalk2,
            kPlayerWalk3, kPlayerJump
        };
        for (auto f : frames) {
            for (int y = 0; y < kPlayerH; ++y) {
                assert(std::strlen(f[y]) == static_cast<std::size_t>(kPlayerW));
            }
        }
    }
    { // SlimeWidths (14x12)
        const char *const *frames[] = {kSlimeIdle, kSlimeSquash};
        for (auto f : frames) {
            for (int y = 0; y < kSlimeH; ++y) {
                assert(std::strlen(f[y]) == static_cast<std::size_t>(kSlimeW));
            }
        }
    }
    { // DwarfWidths (14x18, 5 frames)
        const char *const *frames[] = {
            kDwarfIdle, kDwarfWalkA, kDwarfWalkB, kDwarfThrow, kDwarfMelee
        };
        for (auto f : frames) {
            for (int y = 0; y < kDwarfH; ++y) {
                assert(std::strlen(f[y]) == static_cast<std::size_t>(kDwarfW));
            }
        }
    }
    { // PalettesNonEmpty (toda sprite tem pixel visível, não é vazio)
        assert(kPlayerPalCount == 6u && kSlimePalCount == 5u && kDwarfPalCount == 10u);
        auto hasInk = [](const char *const *f, int h) {
            for (int y = 0; y < h; ++y) {
                for (const char *c = f[y]; *c; ++c) {
                    if (*c != '.') return true;
                }
            }
            return false;
        };
        assert(hasInk(kPlayerIdle, kPlayerH));
        assert(hasInk(kPlayerJump, kPlayerH));
        assert(hasInk(kSlimeIdle, kSlimeH));
        assert(hasInk(kDwarfIdle, kDwarfH));
        assert(hasInk(kDwarfThrow, kDwarfH));
        assert(hasInk(kDwarfMelee, kDwarfH));
    }

    std::printf("sprites test OK\n");
    return 0;
}
