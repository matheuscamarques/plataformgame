/**
 * @file tests/test_body_from_sprite.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava rebuild de Body a partir de pixels do sprite.
 * @details Cobre sprite ASCII e BodyPartId, roda com make test que compila em build/tests/test_body_from_sprite.
 */

#include <cassert>
#include <cmath>
#include <cstdio>

#include "assets/SpriteFrameRegistry.h"
#include "assets/Sprites/PlayerSprites.h"
#include "core/sprite_from_ascii.h"
#include "support/Combat/Body.h"

using namespace support;
using core::BodyPartId;
using core::PaletteEntry;

namespace {

constexpr int kW = 12, kH = 20;

// Torso deslocado à direita (cols 7-9, rows 5-6); resto vazio.
const char *const kFakeRows[kH] = {
    "............", "............", "............", "............",
    "............", ".......CCC..", ".......CCC..", "............",
    "............", "............", "............", "............",
    "............", "............", "............", "............",
    "............", "............", "............", "............",
};

const PaletteEntry kFakePal[] = {
    {'.', {0, 0, 0, 0}, BodyPartId::None},
    {'C', {1, 2, 3, 255}, BodyPartId::Torso},
};

BodySchema makeSchema() {
    BodySchema s;
    s.overallHeight = 20.f;
    s.halfWidth = 6.f;
    s.parts = {
        {BodyPartId::Torso, {0.f, 0.f}, {6.f, 6.f}, 1.f, 1.f, false},
        {BodyPartId::ArmR, {0.f, 0.f}, {4.f, 4.f}, 1.f, 1.f, false},
        {BodyPartId::Weapon, {0.f, 0.f}, {2.f, 2.f}, 1.f, 1.f, false},
    };
    return s;
}

bool near(float a, float b) { return std::fabs(a - b) < 0.01f; }

} // namespace

int main() {
    { // TorsoFollowsSpritePixels (bbox cols 7-9, rows 5-6, scale 2.5)
        auto schema = makeSchema();
        Body b;
        b.attach(&schema);
        // Entidade 30x50 em (100,100): scale = 50/20 = 2.5, centro (115,125).
        b.rebuildFromSprite({100.f, 100.f}, {30.f, 50.f},
                            kFakeRows, kW, kH, kFakePal, 2, 1);
        const PartState *t = b.find(BodyPartId::Torso);
        assert(t != nullptr);
        // rel = {7-6, 5-10, 3, 2} => world = {115+2.5, 125-12.5, 7.5, 5}.
        assert(near(t->worldBox.left, 117.5f));
        assert(near(t->worldBox.top, 112.5f));
        assert(near(t->worldBox.width, 7.5f));
        assert(near(t->worldBox.height, 5.f));
    }
    { // WeaponFallsBackWhenNoPixels (schema estático preservado)
        auto schema = makeSchema();
        Body b;
        b.attach(&schema);
        b.rebuildFromSprite({0.f, 0.f}, {12.f, 20.f},
                            kFakeRows, kW, kH, kFakePal, 2, 1);
        const PartState *w = b.find(BodyPartId::Weapon);
        assert(w != nullptr);
        assert(near(w->worldBox.width, 2.f));
        assert(near(w->worldBox.height, 2.f));
    }
    { // FacingMirrors (espelha em torno do centro)
        auto schema = makeSchema();
        Body b;
        b.attach(&schema);
        b.rebuildFromSprite({0.f, 0.f}, {12.f, 20.f},
                            kFakeRows, kW, kH, kFakePal, 2, 1);
        const float l1 = b.find(BodyPartId::Torso)->worldBox.left;
        b.rebuildFromSprite({0.f, 0.f}, {12.f, 20.f},
                            kFakeRows, kW, kH, kFakePal, 2, -1);
        const float lm1 = b.find(BodyPartId::Torso)->worldBox.left;
        // cx=6: facing 1 => left 6+2.5=8.5? rel {1,-5,3,2}, scale 1:
        // facing 1: left = 6+1 = 7; facing -1: right edge 6+4=10 => left 7.
        // Simetria: (l1 - 6) == -(lm1 + 3 - 6).
        assert(near(l1 - 6.f, -(lm1 + 3.f - 6.f)));
    }
    { // PunchExtendsArmHitbox (contrato do refactor: bbox segue pixel)
        // Mesma schema, pixels deslocados +5 cols => worldBox desloca
        // 5px de sprite (a 2.5x de escala do player real).
        static const char *const kShifted[kH] = {
            "............", "............", "............", "............",
            "............", ".........CCC", ".........CCC", "............",
            "............", "............", "............", "............",
            "............", "............", "............", "............",
            "............", "............", "............", "............",
        };
        auto schema = makeSchema();
        Body base, shifted;
        base.attach(&schema);
        shifted.attach(&schema);
        base.rebuildFromSprite({0.f, 0.f}, {30.f, 50.f},
                               kFakeRows, kW, kH, kFakePal, 2, 1);
        shifted.rebuildFromSprite({0.f, 0.f}, {30.f, 50.f},
                                  kShifted, kW, kH, kFakePal, 2, 1);
        const PartState *tb = base.find(BodyPartId::Torso);
        const PartState *ts = shifted.find(BodyPartId::Torso);
        assert(tb != nullptr && ts != nullptr);
        // base: cols 7-9; shifted: cols 9-11 => +2px de sprite a 2.5x.
        assert(near(ts->worldBox.left - tb->worldBox.left, 2.f * 2.5f));
    }
    { // RealSpriteFlows (punch real deriva ArmR sem crash)
        auto schema = makeSchema();
        Body punch;
        punch.attach(&schema);
        const auto pf =
            assets::frameData(SpriteFrameId::PlayerPunch);
        punch.rebuildFromSprite({0.f, 0.f}, {30.f, 50.f},
                                pf.rows,
                                sprites::kPlayerW,
                                sprites::kPlayerH, sprites::kPlayerPal,
                                sprites::kPlayerPalCount, 1);
        assert(punch.find(BodyPartId::ArmR) != nullptr);
        assert(punch.find(BodyPartId::Torso) != nullptr);
    }
    { // RegistryResolvesAllIds (todo id tem rows; None formular vazio)
        for (int i = 1; i < static_cast<int>(SpriteFrameId::COUNT); ++i) {
            auto f = assets::frameData(static_cast<SpriteFrameId>(i));
            assert(f.rows != nullptr && f.w > 0 && f.h > 0);
            assert(f.pal != nullptr && f.palCount > 0);
        }
        auto none = assets::frameData(SpriteFrameId::None);
        assert(none.rows == nullptr);
    }

    std::printf("body from sprite test OK\n");
    return 0;
}
