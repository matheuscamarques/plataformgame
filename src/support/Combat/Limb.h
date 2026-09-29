/**
 * @file src/support/Combat/Limb.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Membro articulado em 2 segmentos + IK (B.1, matemática pura).
 * @details Geometria apenas — arte NÃO entra aqui (o render lê depois).
 * Ângulos em radianos, screen-space Y para baixo: 0 = +X (leste),
 * positivo = horário visual (igual ao sf::Sprite; o render converte
 * rad→deg). facing espelha o espaço do corpo: bendForward é relativo
 * ao corpo ("cotovelo pra frente"), não ao mundo. Sem SFML/GL.
 */

#pragma once

#include "core/Vec.h"

namespace support {

struct LimbSegment {
    core::Vec2f pivot{0.f, 0.f}; // em coords locais do segmento (sprite-px)
    float length = 0.f;          // comprimento em sprite-px
};

struct Limb {
    LimbSegment upper; // ombro → cotovelo
    LimbSegment lower; // cotovelo → mão
    float angleUpper = 0.f; // corrente (radianos, espaço local do corpo)
    float angleLower = 0.f; // relativo ao upper
    core::Vec2f shoulderLocal{0.f, 0.f}; // offset do centro (sprite-px)
};

struct LimbPose {
    core::Vec2f shoulderWorld{0.f, 0.f};
    core::Vec2f elbowWorld{0.f, 0.f};
    core::Vec2f handWorld{0.f, 0.f};
    // Ângulos resolvidos em espaço LOCAL do corpo (sem espelho):
    // o render espelha os pontos, não os ângulos (B.2).
    float angleUpper = 0.f;
    float angleLower = 0.f;
};

// Puro: dado ombro no mundo + alvo da mão, resolve a pose.
// Alvo inalcançável = estica na direção (clampa, sem NaN).
// Alvo = ombro = repouso (tudo coincide, sem divisão por zero).
// facing < 0 espelha o espaço do corpo em X (pontos saem em mundo).
LimbPose solveIK(const Limb &limb, core::Vec2f shoulderWorld,
                 core::Vec2f targetHandWorld, bool bendForward, int facing);

// Cinemática direta dos ângulos correntes (convenção acima).
LimbPose forwardKinematics(const Limb &limb, core::Vec2f shoulderWorld,
                           int facing);

// Escreve os ângulos da pose de volta no membro (B.3 procedural).
inline void applyPose(Limb &limb, const LimbPose &pose) {
    limb.angleUpper = pose.angleUpper;
    limb.angleLower = pose.angleLower;
}

} // namespace support
