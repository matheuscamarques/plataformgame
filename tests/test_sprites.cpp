#include <cassert>
#include <cstdio>
#include <cstring>
#include "game/PlayerSprite.h"
#include "game/Sprites.h"

// ASCII art: widths exatas (linha errada = sprite deslocada), telegraphs
// exclusivos e prioridade de pick. build() NÃO é chamado (textura exige
// contexto GL, sem teste headless — compara endereços em SpriteSet vazio).
int main() {
    using namespace sprites;
    auto has = [](const char *const *f, int h, char c) {
        for (int y = 0; y < h; ++y) {
            if (std::strchr(f[y], c)) return true;
        }
        return false;
    };

    { // PlayerWidths (12x20, 6 frames)
        const char *const *frames[] = {
            kPlayerIdle, kPlayerWalkA, kPlayerWalkB,
            kPlayerJump, kPlayerThrow, kPlayerMelee
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
        assert(kPlayerPalCount == 9u && kSlimePalCount == 5u && kDwarfPalCount == 10u);
        assert(has(kPlayerIdle, kPlayerH, 'S'));
        assert(has(kPlayerJump, kPlayerH, 'S'));
        assert(has(kPlayerThrow, kPlayerH, 'T'));
        assert(has(kPlayerMelee, kPlayerH, 'W'));
        assert(has(kSlimeIdle, kSlimeH, 'G'));
        assert(has(kDwarfIdle, kDwarfH, 'H'));
        assert(has(kDwarfThrow, kDwarfH, 'D'));
        assert(has(kDwarfMelee, kDwarfH, 'W'));
    }
    { // OnlyThrowHasTNT + OnlyMeleeHasSword (telegraphs exclusivos)
        assert(has(kPlayerThrow, kPlayerH, 'T'));
        assert(!has(kPlayerIdle, kPlayerH, 'T'));
        assert(!has(kPlayerMelee, kPlayerH, 'T'));
        assert(!has(kPlayerJump, kPlayerH, 'T'));
        assert(has(kPlayerMelee, kPlayerH, 'W'));
        assert(!has(kPlayerThrow, kPlayerH, 'W'));
        assert(!has(kPlayerIdle, kPlayerH, 'W'));
        assert(!has(kPlayerJump, kPlayerH, 'W'));
    }
    { // WalkFramesDifferInArms (braços em lados opostos)
        assert(kPlayerWalkA[8][1] == 'S');
        assert(kPlayerWalkA[8][10] == '.');
        assert(kPlayerWalkB[8][1] == '.');
        assert(kPlayerWalkB[8][10] == 'S');
    }
    { // PickPriority (melee > throw > jump > walk > idle)
        SpriteSet sp{};
        assert(game::pickPlayerFrame(true, 0.f, true, true, sp, 0) == &sp.playerMelee);
        assert(game::pickPlayerFrame(false, 0.f, false, true, sp, 0) == &sp.playerThrow);
        assert(game::pickPlayerFrame(false, 0.f, false, false, sp, 0) == &sp.playerJump);
        assert(game::pickPlayerFrame(true, 100.f, false, false, sp, 0) == &sp.playerWalkA);
        assert(game::pickPlayerFrame(true, 100.f, false, false, sp, 1) == &sp.playerWalkB);
        assert(game::pickPlayerFrame(true, 0.f, false, false, sp, 0) == &sp.playerIdle);
    }

    std::printf("sprites test OK\n");
    return 0;
}
