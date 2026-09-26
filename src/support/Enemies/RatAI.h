/**
 * @file src/support/Enemies/RatAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IA do ratai: mesmo comportamento, identidade própria.
 * @details Subclasse mínima (padrão SkeletonAI): herda tudo, sobrescreve
 * só kind()/name() p/ SFX, feed e contagem de cap.
 */

#pragma once

#include "SlimeAI.h"

namespace support {

class RatAI : public SlimeAI {
public:
    RatAI() = default;

    const char *name() const override { return "RatAI"; }
    core::EntityKind kind() const override {
        return core::EntityKind::Rat;
    }
};

} // namespace support
