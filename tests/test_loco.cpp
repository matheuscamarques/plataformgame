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
    { // MarchaAvança (contact A entra com Step; B sem; C com Step)
        Player p;
        p.moveRight = true;
        p.jumping = true; // chão: marcha anda
        p.tick();
        assert(p.loco.currentFrame() == SpriteFrameId::PlayerWalkA);
        assert(p.loco.consumeEvents() == kStep); // poeira do contact A
        p.tick();
        p.tick();
        p.tick(); // 0.133s: passing B, sem poeira
        assert(p.loco.currentFrame() == SpriteFrameId::PlayerWalkB);
        assert(p.loco.consumeEvents() == 0);
        for (int i = 0; i < 3; ++i) p.tick(); // 0.233s: contact C
        assert(p.loco.currentFrame() == SpriteFrameId::PlayerWalkC);
        assert(p.loco.consumeEvents() == kStep);
    }
    { // MarchaSegueNoGolpe (marcha em B + swing: pernas não travam)
        Player p;
        p.moveRight = true;
        p.jumping = true;
        p.tick();
        assert(p.loco.consumeEvents() == kStep); // drena o Step do A
        p.tick();
        p.tick();
        p.tick();
        assert(p.loco.currentFrame() == SpriteFrameId::PlayerWalkB);
        assert(p.loco.consumeEvents() == 0); // passing B: nada a drenar
        assert(p.startSwing());
        for (int i = 0; i < 5; ++i) p.tick();
        assert(p.inMeleeSwing());
        // O clip avançou (B→C→D...) em vez de congelar: poeira do C sai.
        assert(p.loco.currentFrame() == SpriteFrameId::PlayerWalkD);
        assert(p.loco.consumeEvents() == kStep);
    }
    { // GolpeParadoPlanta (sem marcha no swing: volta ao idle)
        Player p;
        p.moveRight = true;
        p.jumping = true;
        p.tick();
        assert(p.startSwing());
        p.moveRight = false; // parou no meio do golpe
        p.tick();
        assert(p.loco.currentFrame() == SpriteFrameId::PlayerIdle);
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
