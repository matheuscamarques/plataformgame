/**
 * @file src/assets/PlayerClips.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Clips de animação do player como dados (Fase C).
 * @details Agrupa por mira igual ao resolvePlayerSprite (Up/N-NE-NW,
 * Down/S-SE-SW, resto Side): o primeiro switch a morrer. Clips de
 * ataque são state-driven (duration 0: avançam via gotoFrame nas
 * transições de updateMelee, relógio único); Hitbox no frame 1
 * (Active). Puro, sem SFML/GL.
 */

#pragma once

#include "support/Combat/AimDir.h"
#include "support/Combat/AnimClip.h"

namespace game {

const support::AnimClip &attackClipSide();
const support::AnimClip &attackClipUp();
const support::AnimClip &attackClipDown();

// Grupo do golpe pela mira congelada (mesma partição do resolve).
const support::AnimClip &attackClipFor(support::AimDir aim);

} // namespace game
