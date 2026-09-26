/**
 * @file src/support/Enemies/HarpyAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IA da harpia: voo com identidade própria.
 * @details Subclasse mínima de FlyingAI (padrão SkeletonAI): só kind/name.
 */

#pragma once

#include "FlyingAI.h"

namespace support {

class HarpyAI : public FlyingAI {
public:
    HarpyAI() = default;

    const char *name() const override { return "HarpyAI"; }
    core::EntityKind kind() const override {
        return core::EntityKind::Harpy;
    }
};

} // namespace support
