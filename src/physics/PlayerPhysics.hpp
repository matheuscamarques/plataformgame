/**
 * @file src/physics/PlayerPhysics.hpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Física pura do player: único ponto de verdade da simulação de movimento.
 * @details Define State, Input, Events e step() sem I/O, sem SFML e sem estado global. Chamado por Player::tick (solo), futuramente por NetworkWorld (predição) e pelo NIF C++ (autoritativo). Mesma entrada, mesma saída, sempre.
 */

#pragma once

#include "support/Combat/AimDir.h"

namespace support {
class World; // Fase 1 não consulta tiles (colisão segue externa via collide()); declarado p/ o contrato futuro.
}

namespace physics {

// Passo fixo do jogo (espelha Game::tick: 1/30).
inline constexpr float kFixedDt = 1.0f / 30.0f;

// Constantes extraídas de Player::tick (fonte única a partir daqui).
inline constexpr float kGravity = 2.0f;
inline constexpr float kTerminalVelocity = 25.0f;
inline constexpr float kJumpHoldVy = 9.8f;
inline constexpr float kWalkSpeed = 9.8f;
inline constexpr float kRunBonus = 5.0f;
inline constexpr float kWalkFrameTime = 0.10f;
inline constexpr int kWalkFrames = 4;

struct Input {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool run = false;
};

// Estado completo que Player::tick lia/escrevia (mais nada).
struct State {
    float x = 0.f, y = 0.f;
    float vx = 0.f, vy = 0.f;
    bool moveUp = false, moveDown = false, moveLeft = false, moveRight = false;
    bool runFast = false;
    bool jumping = false; // true = no chão (pode pular); false = no ar
    float jumpingRecharge = 0.f;
    bool inWater = false; // step sempre zera na entrada (collide() seta depois)
    int facing = 1;
    support::AimDir aim = support::AimDir::E;
    int walkFrame = 0;
    float walkTimer = 0.f;
    float throwAnimT = 0.f;
    float hurtT = 0.f;  // remaining de hurtIframes
    float throwT = 0.f; // remaining de throwCooldown
};

namespace Event {
inline constexpr uint32_t JumpEnded = 1u << 0; // recharge estourou: jumping=false, moveUp consumido
}

struct Output {
    State state;
    uint32_t events = 0;
};

// Avança a simulação em dt. Pura: sem I/O, sem SFML, sem global.
// Ordem idêntica à de Player::tick (snapshot de mira, teleporte de pulo,
// gravidade, vx, aim, walk anim, timers, cooldowns, integração).
Output step(const State &prev, const Input &in, float dt);
// Contrato futuro (fases de rede/NIF): checagem de chão por tile.
// Fase 1 ignora o mundo — colisão segue externa via collide().
inline Output step(const State &prev, const Input &in, const support::World &, float dt) {
    return step(prev, in, dt);
}

} // namespace physics
