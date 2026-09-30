/**
 * @file tests/test_sprite_compose.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava composição == monolítico (Fase B).
 * @details Cobre os 10 frames do player: compor as partes tem que dar
 * byte a byte o frame monolítico. Trava o canvas 40x40 (40x40@0,0).
 */

#include <cassert>
#include <cstdio>
#include <cstring>
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
        {kPlayerIdleParts, 4},
        {kPlayerWalkAParts, 4},
        {kPlayerWalkBParts, 4},
        {kPlayerJumpParts, 4},
        {kPlayerThrowParts, 4},
        {kPlayerPunchParts, 4},
        {kPlayerPunchUpParts, 4},
        {kPlayerPunchDownParts, 4},
        {kPlayerHurtParts, 4},
        {kPlayerDeathParts, 4},
    };

    { // FullFrameParts (canvas 40x40: toda parte cobre o frame
        // inteiro; conteúdo 12-wide centrado nas cols 14-25, resto é
        // '.' — linha N de qualquer array == linha N do composto)
        for (const auto& c : cases) {
            for (std::size_t i = 0; i < c.count; ++i) {
                assert(c.parts[i].w == 40 && c.parts[i].h == 40);
                assert(c.parts[i].offX == 0 && c.parts[i].offY == 0);
                assert(c.parts[i].rows != nullptr);
                for (int y = 0; y < 40; ++y)
                    assert(std::strlen(c.parts[i].rows[y]) == 40);
            }
        }
    }
    { // GoldenIdle (compose do idle == snapshot; trava o algoritmo)
        // Snapshot REGERADO na unificação (rosto E detalhado + cintura
        // + postura S): se compose mudar, quebra aqui (as outras poses
        // têm cobertura estrutural acima).
        static const char* const kGoldenIdle[40] = {
            "..................KKKK..................",
            ".................KKKKKK.................",
            "................KKKKKKKK................",
            "................KFFFFFFK................",
            "................KFEFFEFK................",
            "................KFEFFEFK................",
            "................KFFFFFFK................",
            "................KFFFFFEK................",
            "................KFFFFFFK................",
            ".................FFFFFF.................",
            ".................FFFFFF.................",
            "..................FFFF..................",
            "................CCCCCCCC................",
            "................CCCCCCCC................",
            "...............GCCCCCCCCH...............",
            "...............GCCCCCCCCH...............",
            "...............GCCCCCCCCH...............",
            "...............GCCCCCCCCH...............",
            "...............GCCCCCCCCH...............",
            "...............GCCCCCCCCH...............",
            "................CCCCCCCC................",
            "................CCCCCCCC................",
            "................CCKKKKCC................",
            "................CCKKKKCC................",
            "................CCCCCCCC................",
            "................CCCCCCCC................",
            ".................CCCCCC.................",
            ".................CCCCCC.................",
            "................CC....CC................",
            "................CC....CC................",
            "................CC....CC................",
            "................CC....CC................",
            "................LL....BB................",
            "................LL....BB................",
            "................LL....BB................",
            "................LL....BB................",
            "...............LL......BB...............",
            "...............LL......BB...............",
            "..............LL........BB..............",
            "..............LL........BB..............",
        };
        const std::vector<std::string> got =
            assets::compose(kPlayerIdleParts, 5, kPlayerW, kPlayerH);
        assert(got.size() == 40u);
        for (int row = 0; row < 40; ++row)
            assert(got[row] == kGoldenIdle[row]);
    }
    { // ClipOutOfBounds (parte fora do buffer: recorta sem crash)
        // Head full-40 pendurada em offY 36: rows 36-39 ← conteúdo 0-3.
        const assets::Part hang[] = {{kPlayerIdleHead, kPlayerW, kPlayerH, 0, 36}};
        const std::vector<std::string> got = assets::compose(hang, 1, kPlayerW, kPlayerH);
        assert(got.size() == 40u);
        for (int row = 0; row < 36; ++row) assert(got[row] == "........................................");
        assert(got[36] == kPlayerIdleHead[0]);
    }
    { // PosesCoverAllFrames (12 poses × 5 partes 12x40@0,0)
        assert(kPlayerPoseCount == 12);
        for (int i = 0; i < kPlayerPoseCount; ++i) {
            const assets::Part* pp =
                poseParts(static_cast<PlayerPose>(i));
            assert(pp != nullptr);
            for (int k = 0; k < 5; ++k) {
                assert(pp[k].w == 40 && pp[k].h == 40);
                assert(pp[k].offX == 0 && pp[k].offY == 0);
            }
            assert(pp[0].rows && pp[1].rows && pp[2].rows && pp[3].rows &&
                   pp[4].rows);
        }
        // Fora da faixa: fallback idle (nunca nullptr).
        assert(poseParts(static_cast<PlayerPose>(99)) == kPlayerIdleParts);
    }
    { // ArmsOverlayAddsOnly (overlay: só G/H, só sobre '.' do composto-4)
        for (int i = 0; i < kPlayerPoseCount; ++i) {
            const assets::Part* pp =
                poseParts(static_cast<PlayerPose>(i));
            const std::vector<std::string> base =
                assets::compose(pp, 4, kPlayerW, kPlayerH);
            const assets::Part& ov = pp[4];
            for (int y = 0; y < ov.h; ++y)
                for (int x = 0; x < ov.w; ++x) {
                    const char a = ov.rows[y][x];
                    if (a == '.') continue;
                    // Overlay só carrega braço (Tier 1: sombra g/h conta).
                    assert(a == 'G' || a == 'H' || a == 'g' || a == 'h');
                    assert(base[ov.offY + y][ov.offX + x] == '.');
                }
        }
    }
    { // RegistryServesComposed (frameData == compose, sem monolíticos)
        for (int i = 0; i < 10; ++i) {
            const auto id =
                static_cast<support::SpriteFrameId>(static_cast<int>(support::SpriteFrameId::PlayerIdle) + i);
            const auto f = assets::frameData(id);
            assert(f.rows != nullptr && f.w == 40 && f.h == 40);
            const assets::Part* pp = poseParts(static_cast<PlayerPose>(i));
            const std::vector<std::string> composed =
                assets::compose(pp, 5, kPlayerW, kPlayerH);
            for (int row = 0; row < 40; ++row)
                assert(std::string(f.rows[row]) == composed[row]);
        }
        // WalkC/D moram no fim do enum (sem deslocar ids): checagem direta.
        const support::SpriteFrameId extraIds[] = {
            support::SpriteFrameId::PlayerWalkC,
            support::SpriteFrameId::PlayerWalkD};
        const PlayerPose extraPoses[] = {PlayerPose::WalkC,
                                         PlayerPose::WalkD};
        for (int k = 0; k < 2; ++k) {
            const auto f = assets::frameData(extraIds[k]);
            assert(f.rows != nullptr && f.w == 40 && f.h == 40);
            const assets::Part* pp = poseParts(extraPoses[k]);
            const std::vector<std::string> composed =
                assets::compose(pp, 5, kPlayerW, kPlayerH);
            for (int row = 0; row < 40; ++row)
                assert(std::string(f.rows[row]) == composed[row]);
        }
    }

    std::printf("sprite compose test OK\n");
    return 0;
}
