/**
 * @file src/support/Enemies/SkeletonAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IA do esqueleto: mesmos estados do anão, identidade própria.
 * @details Subclasse de DwarfAI sem lógica nova (herda patrulha, melee e
 * recuo); sobrescreve só kind()/name() para SFX, feed e contagem de cap.
 * Sem dynamite: o arquétipo lista só skill melee, então o UtilityAI
 * nunca escolhe throw (alcance mora na skill, não na IA).
 */

#pragma once

#include "DwarfAI.h"

namespace support {

// Esqueleto (reflexo do player): reusa a máquina do DwarfAI.
//Melee-only por dados (skills do arquétipo), não por branch aqui.
class SkeletonAI : public DwarfAI {
public:
    SkeletonAI() = default;

    const char *name() const override { return "SkeletonAI"; }
    core::EntityKind kind() const override {
        return core::EntityKind::Skeleton;
    }
};

} // namespace support
