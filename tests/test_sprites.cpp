#include <cassert>
#include <cstdio>
#include <cstring>
#include "assets/PlayerSprite.h"
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

    { // PlayerWidths (12x20, 10 frames)
        const char *const *frames[] = {
            kPlayerIdle, kPlayerWalkA, kPlayerWalkB,
            kPlayerJump, kPlayerThrow, kPlayerPunch,
            kPlayerPunchUp, kPlayerPunchDown,
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
        assert(kPlayerPalCount == 10u && kSlimePalCount == 5u && kDwarfPalCount == 11u);
        assert(has(kPlayerIdle, kPlayerH, 'F'));
        assert(has(kPlayerJump, kPlayerH, 'H'));
        assert(has(kPlayerThrow, kPlayerH, 'T'));
        assert(has(kPlayerPunch, kPlayerH, 'H'));
        assert(has(kSlimeIdle, kSlimeH, 'G'));
        assert(has(kDwarfIdle, kDwarfH, 'R'));
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
        assert(kPlayerWalkA[8][1] == 'H');
        assert(kPlayerWalkA[8][10] == '.');
        assert(kPlayerWalkB[8][1] == '.');
        assert(kPlayerWalkB[8][10] == 'H');
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
    { // PunchHasExtendedFist (≥2 'H' nas cols 10-11 em alguma row)
        int maxS = 0;
        for (int y = 0; y < kPlayerH; ++y) {
            int cnt = 0;
            for (int x = 10; x <= 11; ++x) {
                if (kPlayerPunch[y][x] == 'H') ++cnt;
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
    { // PunchUpAlignsLegs (pernas ancoradas; cabeça/torso +2 rows)
        auto firstRowWith = [](const char *const *f, char c) {
            for (int y = 0; y < kPlayerH; ++y) {
                if (std::strchr(f[y], c)) return y;
            }
            return -1;
        };
        assert(firstRowWith(kPlayerPunchUp, 'B')
               == firstRowWith(kPlayerIdle, 'B'));
        assert(firstRowWith(kPlayerPunchUp, 'K')
               == firstRowWith(kPlayerIdle, 'K') + 2);
        assert(firstRowWith(kPlayerPunchUp, 'C')
               == firstRowWith(kPlayerIdle, 'C') + 2);
    }
    { // PunchDownFullyAligned (cabeça/torso/botas nas rows do idle)
        auto firstRowWith = [](const char *const *f, char c) {
            for (int y = 0; y < kPlayerH; ++y) {
                if (std::strchr(f[y], c)) return y;
            }
            return -1;
        };
        assert(firstRowWith(kPlayerPunchDown, 'K')
               == firstRowWith(kPlayerIdle, 'K'));
        assert(firstRowWith(kPlayerPunchDown, 'C')
               == firstRowWith(kPlayerIdle, 'C'));
        assert(firstRowWith(kPlayerPunchDown, 'B')
               == firstRowWith(kPlayerIdle, 'B'));
    }
    { // TextureForFrameMapsEveryId (todo id resolve p/ textura existente)
        SpriteSet sp{};
        const sf::Texture *fake =
            reinterpret_cast<const sf::Texture *>(0x1234);
        assert(game::textureForFrame(support::SpriteFrameId::PlayerIdle, sp)
               == &sp.playerIdle);
        assert(game::textureForFrame(support::SpriteFrameId::DwarfThrow, sp)
               == &sp.dwarfThrow);
        assert(game::textureForFrame(support::SpriteFrameId::SlimeSquash, sp)
               == &sp.slimeSquash);
        assert(game::textureForFrame(support::SpriteFrameId::PlayerPunch, sp, fake)
               == fake); // seam de arma preservado
        assert(game::textureForFrame(support::SpriteFrameId::PlayerPunch, sp)
               == &sp.playerPunch); // fallback soco
        assert(game::textureForFrame(support::SpriteFrameId::PlayerPunchUp,
                                     sp) == &sp.playerPunchUp);
        assert(game::textureForFrame(support::SpriteFrameId::PlayerPunchDown,
                                     sp) == &sp.playerPunchDown);
        assert(game::textureForFrame(support::SpriteFrameId::None, sp)
               == &sp.playerIdle);
    }

    std::printf("sprites test OK\n");
    return 0;
}
