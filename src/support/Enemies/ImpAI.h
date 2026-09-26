/**
 * @file src/support/Enemies/ImpAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IA do impai: mesmo comportamento, identidade própria.
 * @details Subclasse mínima (padrão SkeletonAI): herda tudo, sobrescreve
 * só kind()/name() p/ SFX, feed e contagem de cap.
 */

#pragma once

#include "DwarfAI.h"

namespace support {

class ImpAI : public DwarfAI {
public:
    ImpAI() = default;

    const char *name() const override { return "ImpAI"; }
    core::EntityKind kind() const override {
        return core::EntityKind::Imp;
    }
};

} // namespace support
