/**
 * @file src/support/Enemies/HollowAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IA do hollowai: mesmo comportamento, identidade própria.
 * @details Subclasse mínima (padrão SkeletonAI): herda tudo, sobrescreve
 * só kind()/name() p/ SFX, feed e contagem de cap.
 */

#pragma once

#include "DwarfAI.h"

namespace support {

class HollowAI : public DwarfAI {
public:
    HollowAI() = default;

    const char *name() const override { return "HollowAI"; }
    core::EntityKind kind() const override {
        return core::EntityKind::Hollow;
    }
};

} // namespace support
