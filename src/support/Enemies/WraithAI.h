/**
 * @file src/support/Enemies/WraithAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IA do wraith: comportamento herdado, identidade própria.
 * @details Subclasse mínima (padrão SkeletonAI): só kind()/name().
 */

#pragma once

#include "FlyingAI.h"

namespace support {

class WraithAI : public FlyingAI {
public:
    WraithAI() = default;

    const char *name() const override { return "Wraith"; }
    core::EntityKind kind() const override {
        return core::EntityKind::Wraith;
    }
};

} // namespace support
