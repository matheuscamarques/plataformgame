/**
 * @file tests/test_sprite_compose.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava composição == monolítico (Fase B).
 * @details Cobre os 10 frames do player: compor as 3 partes tem que dar
 * byte a byte o frame monolítico. Trava os limites (12+16+12=40).
 */

#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

#include "assets/SpriteComposer.h"
#include "assets/SpriteFrameRegistry.h"
#include "assets/Sprites/PlayerParts.h"
#include "assets/Sprites/PlayerSprites.h"

int main() {
    using namespace sprites;

    struct Case {
        const assets::Part* parts;
        std::size_t count;
        const char* const* ref;
    };
    const Case cases[] = {
        {kPlayerIdleParts, 3, kPlayerIdle},
        {kPlayerWalkAParts, 3, kPlayerWalkA},
        {kPlayerWalkBParts, 3, kPlayerWalkB},
        {kPlayerJumpParts, 3, kPlayerJump},
        {kPlayerThrowParts, 3, kPlayerThrow},
        {kPlayerPunchParts, 3, kPlayerPunch},
        {kPlayerPunchUpParts, 3, kPlayerPunchUp},
        {kPlayerPunchDownParts, 3, kPlayerPunchDown},
        {kPlayerHurtParts, 3, kPlayerHurt},
        {kPlayerDeathParts, 3, kPlayerDeath},
    };

    { // PartitionCoversFrame (12+16+12=40, largura 12, offsets empilham)
        for (const auto& c : cases) {
            int total = 0;
            int y = 0;
            for (std::size_t i = 0; i < c.count; ++i) {
                assert(c.parts[i].w == 12);
                assert(c.parts[i].offX == 0);
                assert(c.parts[i].offY == y); // sem buraco nem sobreposição
                y += c.parts[i].h;
                total += c.parts[i].h;
            }
            assert(total == 40 && y == 40);
        }
    }
    { // ComposeEqualsMonolithic (byte a byte, 10 frames)
        for (const auto& c : cases) {
            const std::vector<std::string> got =
                assets::compose(c.parts, c.count, 12, 40);
            assert(got.size() == 40u);
            for (int row = 0; row < 40; ++row) {
                assert(got[row] == c.ref[row]);
            }
        }
    }
    { // ClipOutOfBounds (parte fora do buffer: recorta sem crash)
        const assets::Part hang[] = {{kPlayerIdleHead, 12, 12, 0, 36}};
        const std::vector<std::string> got = assets::compose(hang, 1, 12, 40);
        assert(got.size() == 40u);
        for (int row = 0; row < 36; ++row) assert(got[row] == "............");
        assert(got[36] == kPlayerIdleHead[0]);
    }
    { // PosesCoverAllFrames (10 poses × 3 partes com dims certas)
        assert(kPlayerPoseCount == 10);
        for (int i = 0; i < kPlayerPoseCount; ++i) {
            const assets::Part* pp =
                poseParts(static_cast<PlayerPose>(i));
            assert(pp != nullptr);
            assert(pp[0].w == 12 && pp[0].h == 12); // head
            assert(pp[1].w == 12 && pp[1].h == 16); // torso
            assert(pp[2].w == 12 && pp[2].h == 12); // legs
            assert(pp[0].rows && pp[1].rows && pp[2].rows);
        }
        // Fora da faixa: fallback idle (nunca nullptr).
        assert(poseParts(static_cast<PlayerPose>(99)) == kPlayerIdleParts);
    }
    { // RegistryServesComposed (frameData == monolítico, pré-Fase E3)
        const struct {
            support::SpriteFrameId id;
            const char* const* ref;
        } cases[] = {
            {support::SpriteFrameId::PlayerIdle, kPlayerIdle},
            {support::SpriteFrameId::PlayerWalkA, kPlayerWalkA},
            {support::SpriteFrameId::PlayerWalkB, kPlayerWalkB},
            {support::SpriteFrameId::PlayerJump, kPlayerJump},
            {support::SpriteFrameId::PlayerThrow, kPlayerThrow},
            {support::SpriteFrameId::PlayerPunch, kPlayerPunch},
            {support::SpriteFrameId::PlayerPunchUp, kPlayerPunchUp},
            {support::SpriteFrameId::PlayerPunchDown, kPlayerPunchDown},
            {support::SpriteFrameId::PlayerHurt, kPlayerHurt},
            {support::SpriteFrameId::PlayerDeath, kPlayerDeath},
        };
        for (const auto& c : cases) {
            const auto f = assets::frameData(c.id);
            assert(f.rows != nullptr && f.w == 12 && f.h == 40);
            for (int row = 0; row < 40; ++row)
                assert(std::string(f.rows[row]) == c.ref[row]);
        }
    }

    std::printf("sprite compose test OK\n");
    return 0;
}
