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
    };
    const Case cases[] = {
        {kPlayerIdleParts, 3},
        {kPlayerWalkAParts, 3},
        {kPlayerWalkBParts, 3},
        {kPlayerJumpParts, 3},
        {kPlayerThrowParts, 3},
        {kPlayerPunchParts, 3},
        {kPlayerPunchUpParts, 3},
        {kPlayerPunchDownParts, 3},
        {kPlayerHurtParts, 3},
        {kPlayerDeathParts, 3},
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
    { // GoldenIdle (compose do idle == snapshot; trava o algoritmo)
        // Snapshot do monolítico deletado na Fase E3: se compose mudar,
        // quebra aqui (as outras poses têm cobertura estrutural acima).
        static const char* const kGoldenIdle[40] = {
            "....KKKK....",
            "....KKKK....",
            "..KKKKKKKK..",
            "..KKKKKKKK..",
            "..KFFFFFFK..",
            "..KFFFFFFK..",
            "..KFEFFEFK..",
            "..KFEFFEFK..",
            "..KFFFFFFK..",
            "..KFFFFFFK..",
            "...FFFFFF...",
            "...FFFFFF...",
            "..CCCCCCCC..",
            "..CCCCCCCC..",
            ".GCCCCCCCCH.",
            ".GCCCCCCCCH.",
            ".GCCCCCCCCH.",
            ".GCCCCCCCCH.",
            ".GCCCCCCCCH.",
            ".GCCCCCCCCH.",
            "..CCCCCCCC..",
            "..CCCCCCCC..",
            "..CCCCCCCC..",
            "..CCCCCCCC..",
            "..CCCCCCCC..",
            "..CCCCCCCC..",
            "...CCCCCC...",
            "...CCCCCC...",
            "...CC..CC...",
            "...CC..CC...",
            "...CC..CC...",
            "...CC..CC...",
            "...LL..BB...",
            "...LL..BB...",
            "...LL..BB...",
            "...LL..BB...",
            "...LL..BB...",
            "...LL..BB...",
            "............",
            "............",
        };
        const std::vector<std::string> got =
            assets::compose(kPlayerIdleParts, 3, 12, 40);
        assert(got.size() == 40u);
        for (int row = 0; row < 40; ++row)
            assert(got[row] == kGoldenIdle[row]);
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
    { // RegistryServesComposed (frameData == compose, sem monolíticos)
        for (int i = 0; i < 10; ++i) {
            const auto id =
                static_cast<support::SpriteFrameId>(static_cast<int>(support::SpriteFrameId::PlayerIdle) + i);
            const auto f = assets::frameData(id);
            assert(f.rows != nullptr && f.w == 12 && f.h == 40);
            const assets::Part* pp = poseParts(static_cast<PlayerPose>(i));
            const std::vector<std::string> composed =
                assets::compose(pp, 3, 12, 40);
            for (int row = 0; row < 40; ++row)
                assert(std::string(f.rows[row]) == composed[row]);
        }
    }

    std::printf("sprite compose test OK\n");
    return 0;
}
