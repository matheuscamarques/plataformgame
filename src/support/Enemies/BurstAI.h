/**
 * @file src/support/Enemies/BurstAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IA do burstai: mesmo comportamento, identidade própria.
 * @details Subclasse mínima (padrão SkeletonAI): herda tudo, sobrescreve
 * só kind()/name() p/ SFX, feed e contagem de cap.
 */

#pragma once

#include "DwarfAI.h"

namespace support {

class BurstAI : public DwarfAI {
public:
    BurstAI() = default;

    const char *name() const override { return "BurstAI"; }
    core::EntityKind kind() const override {
        return core::EntityKind::Burst;
    }
};

} // namespace support
