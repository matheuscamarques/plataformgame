/**
 * @file src/support/Combat/WeaponDef.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define forma, pivot e offsets da arma para swing e hitbox.
 * @details Expõe struct WeaponDef com sprite, origem e handOffset, consumido por BodySystem e render via WeaponRegistry.
 */

#pragma once
#include <string>

namespace support {

// Forma + dimensões do swing de uma arma. Sprite-px, escalado em runtime.
// Dano/timing por arma ficam vazios até o commit de WeaponDef-v2.
struct WeaponDef {
    std::string id;

    // Dimensões do sprite de swing (retângulo em sprite-px).
    int spriteW = 16;
    int spriteH = 8;

    // Pivot do swing dentro do sprite (guarda da espada).
    float originX = 5.f;
    float originY = 5.f;

    // Offset da mão no mundo (sprite-px), escalado por s no BodySystem.
    float handOffsetX = 4.f;
    float handOffsetY = 8.f;

    // Texturas por fase (draw): true = idle/windup/swing, false = só
    // idle em toda fase. Nome da textura continua manual no draw
    // (sem texture registry ainda) — este bool ao menos tira o if
    // por id do render.
    bool hasSwingPhases = true;
};

} // namespace support
