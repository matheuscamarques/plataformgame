/**
 * @file src/support/Enemies/ElementalAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IA do elementalai: mesmo comportamento, identidade própria.
 * @details Subclasse mínima (padrão SkeletonAI): herda tudo, sobrescreve
 * só kind()/name() p/ SFX, feed e contagem de cap.
 */

#pragma once

#include "DwarfAI.h"

namespace support {

class ElementalAI : public DwarfAI {
public:
    ElementalAI() = default;

    const char *name() const override { return "ElementalAI"; }
    core::EntityKind kind() const override {
        return core::EntityKind::Elemental;
    }
};

} // namespace support
