/**
 * @file src/support/Enemies/DemonEyeAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IA do olho demoníaco: voo com identidade própria.
 * @details Subclasse mínima de FlyingAI (padrão SkeletonAI): só kind/name.
 */

#pragma once

#include "FlyingAI.h"

namespace support {

class DemonEyeAI : public FlyingAI {
public:
    DemonEyeAI() = default;

    const char *name() const override { return "DemonEyeAI"; }
    core::EntityKind kind() const override {
        return core::EntityKind::Eye;
    }
};

} // namespace support
