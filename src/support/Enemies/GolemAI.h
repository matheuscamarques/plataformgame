/**
 * @file src/support/Enemies/GolemAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IA do golem: comportamento herdado, identidade própria.
 * @details Subclasse mínima (padrão SkeletonAI): só kind()/name().
 */

#pragma once

#include "DwarfAI.h"

namespace support {

class GolemAI : public DwarfAI {
public:
    GolemAI() = default;

    const char *name() const override { return "Golem"; }
    core::EntityKind kind() const override {
        return core::EntityKind::Golem;
    }
};

} // namespace support
