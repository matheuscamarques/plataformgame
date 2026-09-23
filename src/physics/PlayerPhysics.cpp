/**
 * @file src/physics/PlayerPhysics.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementação do step puro de movimento do player.
 * @details Réplica exata da ordem de Player::tick: snapshot de mira, teleporte de pulo com recharge, gravidade com terminal, vx por direção, aim, walk anim, throwAnimT, cooldowns e integração. Qualquer divergência aqui quebra o solo — trava em test_player_physics.
 */

#include "physics/PlayerPhysics.hpp"

#include "core/Config.h"

namespace physics {

Output step(const State &prev, const Input &in, float dt) {
    State s = prev;
    uint32_t events = 0;

    s.inWater = false; // reset; collide() seta de novo se houver água
    // Snapshot do input p/ mira: o pulo consome moveUp abaixo; sem a
    // cópia, segurar ↑ no ar perde o N após ~5 ticks de subida.
    const bool aimUp = in.up;
    const bool aimDown = in.down;
    const bool aimLeft = in.left;
    const bool aimRight = in.right;
    s.moveUp = in.up;
    s.moveDown = in.down;
    s.moveLeft = in.left;
    s.moveRight = in.right;
    s.runFast = in.run;

    if (s.moveUp && s.jumping) {
        s.y -= core::kBlockSize * 1.0f / 2;

        s.jumpingRecharge += core::kBlockSize * 1.0f / 2;
        if (s.jumpingRecharge > core::kBlockSize * 5) {
            s.jumping = false;
            s.jumpingRecharge = 0.0f;
            s.moveUp = false;
            events |= Event::JumpEnded;
        }
    }

    const float vxRunSpeed = s.runFast ? kRunBonus : 0.0f;

    // Gravidade com arrasto: acelera até a velocidade terminal.
    // No pulo (teleporte) mantém 9.8 (pulo idêntico ao antigo); fora
    // dele, acumula. Pouso zera no collide(); knockback p/ cima faz
    // arco (soma e cai).
    if (s.moveUp && s.jumping) {
        s.vy = kJumpHoldVy;
    } else {
        float vy = s.vy + kGravity;
        if (vy > kTerminalVelocity) vy = kTerminalVelocity;
        s.vy = vy;
    }

    if (s.moveLeft) {
        s.vx = -kWalkSpeed - vxRunSpeed;
    }
    if (s.moveRight) s.vx = kWalkSpeed + vxRunSpeed;

    if (!s.moveLeft && !s.moveRight) {
        s.vx = 0.0f;
    }

    // Mira segue o input todo tick; o swing congela a sua (snapshot).
    s.aim = support::resolveAim(aimUp, aimDown, aimLeft, aimRight, s.facing);

    // Walk anim (10fps, só no chão): parado volta ao frame 0.
    // jumping=true = no chão (pode pular); false = no ar.
    if ((s.moveLeft || s.moveRight) && s.jumping) {
        s.walkTimer += dt;
        if (s.walkTimer >= kWalkFrameTime) {
            s.walkTimer = 0.f;
            s.walkFrame = (s.walkFrame + 1) % kWalkFrames;
        }
    } else {
        s.walkFrame = 0;
        s.walkTimer = 0.f;
    }

    // Timer de ataque (frame telegraph).
    if (s.throwAnimT > 0.f) s.throwAnimT -= dt;

    // Cooldowns, tickados aqui (1 só lugar). Mesma regra de Cooldown::tick:
    // nunca negativam. Sem isto, i-frames nunca expiram e o sprite
    // trava em kPlayerHurt (pick tem hurt como 1ª prioridade).
    if (s.hurtT > 0.f) {
        s.hurtT -= dt;
        if (s.hurtT < 0.f) s.hurtT = 0.f;
    }
    if (s.throwT > 0.f) {
        s.throwT -= dt;
        if (s.throwT < 0.f) s.throwT = 0.f;
    }

    // Integração (era Entity::tick): x += vx, y += vy.
    s.x += s.vx;
    s.y += s.vy;

    return {s, events};
}

} // namespace physics
