/**
 * @file src/support/Combat/AnimClip.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Formato de dados de animação: sequência de keyframes + eventos.
 * @details Fase C do plano de profundidade: descreve animações como dados
 * (não como switch em resolve). Keyframe com duration <= 0 é
 * state-driven (avança via AnimPlayer::gotoFrame, p/ o melee que tem
 * relógio próprio em updateMelee); duration > 0 é time-driven (tick
 * com dt, p/ locomotion em loop). Puro, sem SFML/GL.
 */

#pragma once

#include <cstdint>

#include "support/Combat/SpriteFrame.h"

namespace support {

// Eventos disparados por keyframe (bitmask). Hitbox é nível (vale
// enquanto o frame está ativo); o resto é borda (dispara ao entrar).
namespace AnimEvent {
[[maybe_unused]] inline constexpr uint32_t Hitbox = 1u << 0; // janela de dano viva
[[maybe_unused]] inline constexpr uint32_t Sfx = 1u << 1;    // quem consome: Fase F
[[maybe_unused]] inline constexpr uint32_t Shake = 1u << 2;  // quem consome: Fase F
[[maybe_unused]] inline constexpr uint32_t Step = 1u << 3;   // contact frame: poeira no pé
[[maybe_unused]] inline constexpr uint32_t kLevelMask = Hitbox; // bits que valem por nível
} // namespace AnimEvent

struct AnimKeyframe {
    SpriteFrameId frame = SpriteFrameId::None;
    float duration = 0.f; // <=0 = state-driven (só gotoFrame avança)
    uint32_t events = 0;
};

struct AnimClip {
    const char *name = "?";
    const AnimKeyframe *frames = nullptr;
    int count = 0;
    bool loop = false;
};

} // namespace support
