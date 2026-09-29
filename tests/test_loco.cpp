/**
 * @file tests/test_loco.cpp
 * @brief Teste headless da locomoção data-driven (fecha a Fase C).
 * @details Seleção marcha/idle, avanço no relógio fixo, freeze no
 * swing e Step nos contacts. Roda com make test.
 */
#include <cassert>
#include <cstdio>
#include "entities/Player/Player.h"
#include "support/Combat/AnimClip.h"

int main() {
    using support::SpriteFrameId;
    constexpr uint32_t kStep = support::AnimEvent::Step;

    { // IdleParado (sem input: clip idle, sem Step, sem fim)
        Player p;
        p.tick();
        assert(p.loco.currentFrame() == SpriteFrameId::PlayerIdle);
        assert(p.loco.consumeEvents() == 0);
        p.tick();
        assert(p.loco.currentFrame() == SpriteFrameId::PlayerIdle);
    }
    { // MarchaAvança (0.10s/frame: 4 ticks chegam no B com Step)
        Player p;
        p.moveRight = true;
        p.jumping = true; // chão: marcha anda
        p.tick();
        assert(p.loco.currentFrame() == SpriteFrameId::PlayerWalkA);
        p.tick();
        p.tick();
        p.tick(); // 0.133s: contact B
        assert(p.loco.currentFrame() == SpriteFrameId::PlayerWalkB);
        assert(p.loco.consumeEvents() == kStep);
        for (int i = 0; i < 6; ++i) p.tick(); // 0.333s: contact D
        assert(p.loco.currentFrame() == SpriteFrameId::PlayerWalkD);
        assert(p.loco.consumeEvents() == kStep);
    }
    { // SwingCongela (marcha em B + 5 ticks de swing: fica no B)
        Player p;
        p.moveRight = true;
        p.jumping = true;
        p.tick();
        p.tick();
        p.tick();
        p.tick();
        assert(p.loco.currentFrame() == SpriteFrameId::PlayerWalkB);
        assert(p.loco.consumeEvents() == kStep); // drena
        assert(p.startSwing());
        for (int i = 0; i < 5; ++i) p.tick();
        assert(p.inMeleeSwing());
        assert(p.loco.currentFrame() == SpriteFrameId::PlayerWalkB);
        assert(p.loco.consumeEvents() == 0); // sem Step no golpe
    }
    { // VoltaAIdle (parou: troca de clip, recomeça do zero)
        Player p;
        p.moveRight = true;
        p.jumping = true;
        p.tick();
        p.tick();
        p.tick();
        p.tick();
        assert(p.loco.currentFrame() == SpriteFrameId::PlayerWalkB);
        p.moveRight = false;
        p.tick();
        assert(p.loco.currentFrame() == SpriteFrameId::PlayerIdle);
    }

    std::printf("loco test OK\n");
    return 0;
}
