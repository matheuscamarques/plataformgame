/**
 * @file tests/test_sprites.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava widths, telegraphs e prioridade sem GL.
 * @details Cobre PlayerSprite ASCII, roda com make test que compila em build/tests/test_sprites.
 */

#include <cassert>
#include <cstdio>
#include <cstring>
#include "assets/PlayerSprite.h"
#include "assets/SpriteFrameRegistry.h"
// ASCII art: widths exatas (linha errada = sprite deslocada), telegraphs
// exclusivos e prioridade de pick. build() NÃO é chamado (textura exige
// contexto GL, sem teste headless — compara endereços em SpriteSet vazio).
// Fase E3: monoliticos mortos; rows vêm de frameData (cache composto).
int main() {
    using namespace sprites;
    using support::SpriteFrameId;
    auto prow = [](support::SpriteFrameId id) {
        return assets::frameData(id).rows;
    };
    auto has = [](const char *const *f, int h, char c) {
        for (int y = 0; y < h; ++y) {
            if (std::strchr(f[y], c)) return true;
        }
        return false;
    };

    { // PlayerWidths (12x40, 10 frames)
        using support::SpriteFrameId;
        const support::SpriteFrameId ids[] = {
            SpriteFrameId::PlayerIdle, SpriteFrameId::PlayerWalkA,
            SpriteFrameId::PlayerWalkB, SpriteFrameId::PlayerJump,
            SpriteFrameId::PlayerThrow, SpriteFrameId::PlayerPunch,
            SpriteFrameId::PlayerPunchUp, SpriteFrameId::PlayerPunchDown,
            SpriteFrameId::PlayerHurt, SpriteFrameId::PlayerDeath};
        for (auto id : ids) {
            const char* const* f = prow(id);
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
        using support::SpriteFrameId;
        assert(kPlayerPalCount == 12u && kSlimePalCount == 5u && kDwarfPalCount == 11u);
        assert(has(prow(SpriteFrameId::PlayerIdle), kPlayerH, 'F'));
        assert(has(prow(SpriteFrameId::PlayerJump), kPlayerH, 'H'));
        assert(has(prow(SpriteFrameId::PlayerThrow), kPlayerH, 'T'));
        assert(has(prow(SpriteFrameId::PlayerPunch), kPlayerH, 'H'));
        assert(has(kSlimeIdle, kSlimeH, 'G'));
        assert(has(kDwarfIdle, kDwarfH, 'R'));
        assert(has(kDwarfThrow, kDwarfH, 'D'));
        assert(has(kDwarfMelee, kDwarfH, 'W'));
    }
    { // OnlyThrowHasTNT (telegraph exclusivo; soco não tem arma)
        using support::SpriteFrameId;
        assert(has(prow(SpriteFrameId::PlayerThrow), kPlayerH, 'T'));
        assert(!has(prow(SpriteFrameId::PlayerIdle), kPlayerH, 'T'));
        assert(!has(prow(SpriteFrameId::PlayerPunch), kPlayerH, 'T'));
        assert(!has(prow(SpriteFrameId::PlayerJump), kPlayerH, 'T'));
        assert(!has(prow(SpriteFrameId::PlayerThrow), kPlayerH, 'W'));
        assert(!has(prow(SpriteFrameId::PlayerPunch), kPlayerH, 'W'));
    }
    { // WalkFramesDifferInArms (braços em lados opostos; row 8→16 no 40)
        using support::SpriteFrameId;
        assert(prow(SpriteFrameId::PlayerWalkA)[16][1] == 'G');
        assert(prow(SpriteFrameId::PlayerWalkA)[16][10] == '.');
        assert(prow(SpriteFrameId::PlayerWalkB)[16][1] == '.');
        assert(prow(SpriteFrameId::PlayerWalkB)[16][10] == 'H');
    }
    { // PunchBodyAlignsWithIdle (cabeça na mesma row: sem crouch)
        using support::SpriteFrameId;
        auto firstRowWith = [](const char *const *f, int h, char c) {
            for (int y = 0; y < h; ++y) {
                if (std::strchr(f[y], c)) return y;
            }
            return -1;
        };
        auto firstK = [&](const char *const *f) { return firstRowWith(f, kPlayerH, 'K'); };
        assert(firstK(prow(SpriteFrameId::PlayerPunch)) == firstK(prow(SpriteFrameId::PlayerIdle)));
    }
    { // PunchHasExtendedFist (≥2 'H' nas cols 10-11 em alguma row)
        const char* const* punch = prow(SpriteFrameId::PlayerPunch);
        int maxS = 0;
        for (int y = 0; y < kPlayerH; ++y) {
            int cnt = 0;
            for (int x = 10; x <= 11; ++x) {
                if (punch[y][x] == 'H') ++cnt;
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
        assert(firstK(prow(SpriteFrameId::PlayerPunch)) == firstK(prow(SpriteFrameId::PlayerIdle)));
        assert(firstK(prow(SpriteFrameId::PlayerThrow)) == firstK(prow(SpriteFrameId::PlayerIdle)));
        auto firstC = [&](const char *const *f) { return firstRowWith(f, kDwarfH, 'C'); };
        assert(firstC(kDwarfThrow) == firstC(kDwarfIdle));
    }
    { // WalkLegsDiffer (B abre as pernas)
        assert(std::strcmp(prow(SpriteFrameId::PlayerWalkA)[15], prow(SpriteFrameId::PlayerWalkB)[15]) != 0);
    }
    { // ResolvePriority (hurt > melee > throw > jump > walk > idle)
        using support::SpriteFrameId;
        using support::AimDir;
        assert(game::resolvePlayerSprite(true, 0.f, true, true, AimDir::E,
                                         true, 0) == SpriteFrameId::PlayerHurt);
        assert(game::resolvePlayerSprite(false, 0.f, false, true, AimDir::E,
                                         true, 0) == SpriteFrameId::PlayerPunch);
        assert(game::resolvePlayerSprite(true, 0.f, false, false, AimDir::E,
                                         true, 0) == SpriteFrameId::PlayerThrow);
        assert(game::resolvePlayerSprite(false, 0.f, false, false, AimDir::E,
                                         false, 0) == SpriteFrameId::PlayerJump);
        assert(game::resolvePlayerSprite(true, 100.f, false, false, AimDir::E,
                                         false, 0) == SpriteFrameId::PlayerWalkA);
        assert(game::resolvePlayerSprite(true, 100.f, false, false, AimDir::E,
                                         false, 1) == SpriteFrameId::PlayerWalkB);
        assert(game::resolvePlayerSprite(true, 0.f, false, false, AimDir::E,
                                         false, 0) == SpriteFrameId::PlayerIdle);
    }
    { // ResolveUsesAimDir (corpo segue input: N/NE/NW up, S/SE/SW down)
        using support::SpriteFrameId;
        using support::AimDir;
        auto R = [](AimDir a) {
            return game::resolvePlayerSprite(true, 0.f, false, true, a,
                                             false, 0);
        };
        assert(R(AimDir::E) == SpriteFrameId::PlayerPunch);
        assert(R(AimDir::W) == SpriteFrameId::PlayerPunch);
        assert(R(AimDir::N) == SpriteFrameId::PlayerPunchUp);
        assert(R(AimDir::NE) == SpriteFrameId::PlayerPunchUp);
        assert(R(AimDir::NW) == SpriteFrameId::PlayerPunchUp);
        assert(R(AimDir::S) == SpriteFrameId::PlayerPunchDown);
        assert(R(AimDir::SE) == SpriteFrameId::PlayerPunchDown);
        assert(R(AimDir::SW) == SpriteFrameId::PlayerPunchDown);
    }
    { // PunchUpAlignsLegs (pernas ancoradas; cabeça/torso +4 rows no 40)
        using support::SpriteFrameId;
        auto firstRowWith = [](const char *const *f, char c) {
            for (int y = 0; y < kPlayerH; ++y) {
                if (std::strchr(f[y], c)) return y;
            }
            return -1;
        };
        assert(firstRowWith(prow(SpriteFrameId::PlayerPunchUp), 'B')
               == firstRowWith(prow(SpriteFrameId::PlayerIdle), 'B'));
        assert(firstRowWith(prow(SpriteFrameId::PlayerPunchUp), 'K')
               == firstRowWith(prow(SpriteFrameId::PlayerIdle), 'K') + 4);
        assert(firstRowWith(prow(SpriteFrameId::PlayerPunchUp), 'C')
               == firstRowWith(prow(SpriteFrameId::PlayerIdle), 'C') + 4);
    }
    { // PunchDownFullyAligned (cabeça/torso/botas nas rows do idle)
        using support::SpriteFrameId;
        auto firstRowWith = [](const char *const *f, char c) {
            for (int y = 0; y < kPlayerH; ++y) {
                if (std::strchr(f[y], c)) return y;
            }
            return -1;
        };
        assert(firstRowWith(prow(SpriteFrameId::PlayerPunchDown), 'K')
               == firstRowWith(prow(SpriteFrameId::PlayerIdle), 'K'));
        assert(firstRowWith(prow(SpriteFrameId::PlayerPunchDown), 'C')
               == firstRowWith(prow(SpriteFrameId::PlayerIdle), 'C'));
        assert(firstRowWith(prow(SpriteFrameId::PlayerPunchDown), 'B')
               == firstRowWith(prow(SpriteFrameId::PlayerIdle), 'B'));
    }
    { // TextureForFrameMapsEveryId (inimigos resolvem; default existe)
        SpriteSet sp{};
        assert(game::textureForFrame(support::SpriteFrameId::DwarfThrow, sp)
               == &sp.dwarfThrow);
        assert(game::textureForFrame(support::SpriteFrameId::SlimeSquash, sp)
               == &sp.slimeSquash);
        assert(game::textureForFrame(support::SpriteFrameId::None, sp)
               == &sp.slimeIdle);
    }

    std::printf("sprites test OK\n");
    return 0;
}
