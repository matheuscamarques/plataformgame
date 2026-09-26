/**
 * @file src/support/Enemies/SerpentAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IA do serpent: comportamento herdado, identidade própria.
 * @details Subclasse mínima (padrão SkeletonAI): só kind()/name().
 */

#pragma once

#include "DwarfAI.h"

namespace support {

class SerpentAI : public DwarfAI {
public:
    SerpentAI() = default;

    const char *name() const override { return "Serpent"; }
    core::EntityKind kind() const override {
        return core::EntityKind::Serpent;
    }
};

} // namespace support
