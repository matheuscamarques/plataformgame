/**
 * @file tests/test_player_physics.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava physics::step e sua equivalência com Player::tick.
 * @details Cobre pulo exato em 11 ticks, terminal de gravidade, vx run/walk/stop, snapshot de mira, decaimento de cooldowns, determinismo e equivalência wrapper-vs-step campo a campo. Roda com make test em build/tests/test_player_physics.
 */

#include <cassert>
#include <cmath>
#include <cstdio>

#include "entities/Player/Player.h"
#include "physics/PlayerPhysics.hpp"

namespace {

bool feq(float a, float b) { return std::fabs(a - b) < 1e-4f; }

physics::State groundState() {
    physics::State s;
    s.jumping = true; // no chão: pode pular
    return s;
}

} // namespace

int main() {
    using namespace physics;

    { // JumpCompletesIn11Ticks (recharge 25/tick, limite 250: 275 > 250 no 11º)
        State s = groundState();
        Input in;
        in.up = true;
        const float y0 = s.y;
        uint32_t ev = 0;
        for (int i = 0; i < 10; ++i) {
            Output o = step(s, in, kFixedDt);
            s = o.state;
            ev = o.events;
            assert(s.jumping && !(ev & Event::JumpEnded));
            assert(feq(s.jumpingRecharge, 25.f * (i + 1)));
        }
        Output o = step(s, in, kFixedDt);
        s = o.state;
        assert(!s.jumping && (o.events & Event::JumpEnded));
        assert(feq(s.jumpingRecharge, 0.f));
        assert(!s.moveUp); // input consumido no fim do pulo
        // y: -25 x 11 de teleporte; vy 9.8 nos 10 primeiros ticks e
        // 11.8 no 11º (pulo encerra antes da gravidade: 9.8 + 2.0).
        // Mesma ordem de soma do step => bit-idêntico.
        float ey = y0;
        for (int i = 0; i < 10; ++i) ey = ey - 25.f + 9.8f;
        ey = ey - 25.f + 11.8f;
        assert(feq(s.y, ey));
    }

    { // GravityReachesTerminal25 (2/tick a partir de 0: 25 no 13º, trava lá)
        State s; // no ar
        Input in;
        for (int i = 0; i < 12; ++i) s = step(s, in, kFixedDt).state;
        assert(feq(s.vy, 24.f));
        s = step(s, in, kFixedDt).state;
        assert(feq(s.vy, 25.f));
        s = step(s, in, kFixedDt).state;
        assert(feq(s.vy, 25.f));
    }

    { // HorizontalSpeeds (walk 9.8, run +5, solto zera)
        State s = groundState();
        Input in;
        in.right = true;
        assert(feq(step(s, in, kFixedDt).state.vx, 9.8f));
        in.run = true;
        assert(feq(step(s, in, kFixedDt).state.vx, 14.8f));
        in.left = true;
        in.right = false;
        assert(feq(step(s, in, kFixedDt).state.vx, -14.8f));
        in.left = false;
        in.run = false;
        State moving = s;
        moving.vx = 9.8f;
        assert(feq(step(moving, in, kFixedDt).state.vx, 0.f));
    }

    { // AimSnapshotHoldsN (segurar ↑ no ar mantém N por 10 ticks)
        State s; // no ar
        Input in;
        in.up = true;
        for (int i = 0; i < 10; ++i) {
            s = step(s, in, kFixedDt).state;
            assert(s.aim == support::AimDir::N);
        }
    }

    { // WalkAnimAdvancesAt10fps (frame 1 em até 4 ticks; depois reseta ao parar)
        State s = groundState();
        Input in;
        in.right = true;
        int advanceTick = -1;
        for (int i = 0; i < 5; ++i) {
            s = step(s, in, kFixedDt).state;
            if (s.walkFrame == 1) {
                advanceTick = i;
                break;
            }
        }
        assert(advanceTick >= 0 && advanceTick <= 3);
        assert(feq(s.walkTimer, 0.f));
        in.right = false; // parou: volta ao frame 0
        s = step(s, in, kFixedDt).state;
        assert(s.walkFrame == 0 && feq(s.walkTimer, 0.f));
    }

    { // CooldownsDecayAndClamp (0.01 vira 0 exato, nunca negativo)
        State s = groundState();
        s.hurtT = 0.01f;
        s.throwT = 0.5f;
        s.throwAnimT = 0.2f;
        s = step(s, Input{}, kFixedDt).state;
        assert(feq(s.hurtT, 0.f));
        assert(feq(s.throwT, 0.5f - kFixedDt));
        assert(feq(s.throwAnimT, 0.2f - kFixedDt));
    }

    { // Determinism (mesma entrada duas vezes = mesma saída)
        const State s0 = groundState();
        Input in;
        in.up = true;
        in.right = true;
        in.run = true;
        const Output a = step(s0, in, kFixedDt);
        const Output b = step(s0, in, kFixedDt);
        assert(a.events == b.events);
        assert(feq(a.state.x, b.state.x) && feq(a.state.y, b.state.y));
        assert(feq(a.state.vx, b.state.vx) && feq(a.state.vy, b.state.vy));
        assert(a.state.jumping == b.state.jumping);
        assert(feq(a.state.jumpingRecharge, b.state.jumpingRecharge));
        assert(a.state.aim == b.state.aim);
    }

    { // WrapperEquivalence (Player::tick == step campo a campo, 30 ticks)
        Player p;
        p.moveRight = true;
        p.moveUp = true;
        p.jumping = true;
        p.runFast = true;
        p.facing = 1;
        p.hurtIframes.trigger(0.3f);
        p.throwCooldown.trigger();
        p.throwAnimT = 0.2f;
        State s; // espelho: mesmos valores iniciais do Player recém-criado
        s.jumping = true;
        s.hurtT = 0.3f;
        s.throwT = 0.5f;
        s.throwAnimT = 0.2f;
        for (int i = 0; i < 30; ++i) {
            // Espelha o App: flags persistem (held) a menos que o step consuma.
            Input in{p.moveUp, p.moveDown, p.moveLeft, p.moveRight, p.runFast};
            p.tick();
            const Output o = step(s, in, kFixedDt);
            s = o.state;
            assert(feq(p.getX(), s.x) && feq(p.getY(), s.y));
            assert(feq(p.getVx(), s.vx) && feq(p.getVy(), s.vy));
            assert(p.moveUp == s.moveUp && p.moveLeft == s.moveLeft);
            assert(p.moveRight == s.moveRight && p.runFast == s.runFast);
            assert(p.jumping == s.jumping);
            assert(feq(p.jumpingRecharge, s.jumpingRecharge));
            assert(p.aimDir == s.aim);
            assert(p.walkFrame == s.walkFrame && feq(p.walkTimer, s.walkTimer));
            assert(feq(p.throwAnimT, s.throwAnimT));
            assert(feq(p.hurtIframes.remaining(), s.hurtT));
            assert(feq(p.throwCooldown.remaining(), s.throwT));
        }
    }

    std::printf("player physics test OK\n");
    return 0;
}
