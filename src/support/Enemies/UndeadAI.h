/**
 * @file src/support/Enemies/UndeadAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IA do undeadai: mesmo comportamento, identidade própria.
 * @details Subclasse mínima (padrão SkeletonAI): herda tudo, sobrescreve
 * só kind()/name() p/ SFX, feed e contagem de cap.
 */

#pragma once

#include "DwarfAI.h"

namespace support {

class UndeadAI : public DwarfAI {
public:
    UndeadAI() = default;

    const char *name() const override { return "UndeadAI"; }
    core::EntityKind kind() const override {
        return core::EntityKind::Undead;
    }
};

} // namespace support
