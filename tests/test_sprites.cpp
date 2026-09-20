#include <cassert>
#include <cstdio>
#include <cstring>
#include "assets/PlayerSprite.h"
#include "assets/Sprites.h"
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

    { // PlayerWidths (12x20, 8 frames)
        const char *const *frames[] = {
            kPlayerIdle, kPlayerWalkA, kPlayerWalkB,
            kPlayerJump, kPlayerThrow, kPlayerPunch,
            kPlayerHurt, kPlayerDeath
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
        assert(has(kPlayerPunch, kPlayerH, 'S'));
        assert(has(kSlimeIdle, kSlimeH, 'G'));
        assert(has(kDwarfIdle, kDwarfH, 'H'));
        assert(has(kDwarfThrow, kDwarfH, 'D'));
        assert(has(kDwarfMelee, kDwarfH, 'W'));
    }
    { // OnlyThrowHasTNT (telegraph exclusivo; soco não tem arma)
        assert(has(kPlayerThrow, kPlayerH, 'T'));
        assert(!has(kPlayerIdle, kPlayerH, 'T'));
        assert(!has(kPlayerPunch, kPlayerH, 'T'));
        assert(!has(kPlayerJump, kPlayerH, 'T'));
        assert(!has(kPlayerThrow, kPlayerH, 'W'));
        assert(!has(kPlayerPunch, kPlayerH, 'W'));
    }
    { // WalkFramesDifferInArms (braços em lados opostos)
        assert(kPlayerWalkA[8][1] == 'S');
        assert(kPlayerWalkA[8][10] == '.');
        assert(kPlayerWalkB[8][1] == '.');
        assert(kPlayerWalkB[8][10] == 'S');
    }
    { // PunchBodyAlignsWithIdle (cabeça na mesma row: sem crouch)
        auto firstRowWith = [](const char *const *f, int h, char c) {
            for (int y = 0; y < h; ++y) {
                if (std::strchr(f[y], c)) return y;
            }
            return -1;
        };
        auto firstK = [&](const char *const *f) { return firstRowWith(f, kPlayerH, 'K'); };
        assert(firstK(kPlayerPunch) == firstK(kPlayerIdle));
    }
    { // PunchHasExtendedFist (≥2 'S' nas cols 10-11 em alguma row)
        int maxS = 0;
        for (int y = 0; y < kPlayerH; ++y) {
            int cnt = 0;
            for (int x = 10; x <= 11; ++x) {
                if (kPlayerPunch[y][x] == 'S') ++cnt;
            }
            if (cnt > maxS) maxS = cnt;
        }
        assert(maxS >= 2);
    }
    { // BodiesAligned (cabeça começa na mesma row do idle: sem crouch)
        auto firstRowWith = [](const char *const *f, int h, char c) {
            for (int y = 0; y < h; ++y) {
                if (std::strchr(f[y], c)) return y;
            }
            return -1;
        };
        auto firstK = [&](const char *const *f) { return firstRowWith(f, kPlayerH, 'K'); };
        assert(firstK(kPlayerPunch) == firstK(kPlayerIdle));
        assert(firstK(kPlayerThrow) == firstK(kPlayerIdle));
        auto firstC = [&](const char *const *f) { return firstRowWith(f, kDwarfH, 'C'); };
        assert(firstC(kDwarfThrow) == firstC(kDwarfIdle));
    }
    { // WalkLegsDiffer (B abre as pernas)
        assert(std::strcmp(kPlayerWalkA[15], kPlayerWalkB[15]) != 0);
    }
    { // PickPriority (hurt > melee > throw > jump > walk > idle)
        SpriteSet sp{};
        const sf::Texture *fake =
            reinterpret_cast<const sf::Texture *>(0x1234);
        assert(game::pickPlayerFrame(true, 0.f, true, true, fake, true, sp, 0) == &sp.playerHurt);
        assert(game::pickPlayerFrame(false, 0.f, false, true, fake, true, sp, 0) == fake);
        assert(game::pickPlayerFrame(false, 0.f, false, true, nullptr, true, sp, 0) == &sp.playerPunch);
        assert(game::pickPlayerFrame(true, 0.f, false, false, nullptr, true, sp, 0) == &sp.playerThrow);
        assert(game::pickPlayerFrame(false, 0.f, false, false, nullptr, false, sp, 0) == &sp.playerJump);
        assert(game::pickPlayerFrame(true, 100.f, false, false, nullptr, false, sp, 0) == &sp.playerWalkA);
        assert(game::pickPlayerFrame(true, 100.f, false, false, nullptr, false, sp, 1) == &sp.playerWalkB);
        assert(game::pickPlayerFrame(true, 0.f, false, false, nullptr, false, sp, 0) == &sp.playerIdle);
    }

    std::printf("sprites test OK\n");
    return 0;
}
