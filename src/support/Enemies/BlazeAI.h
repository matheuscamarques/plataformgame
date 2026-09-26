/**
 * @file src/support/Enemies/BlazeAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IA do blaze: comportamento herdado, identidade própria.
 * @details Subclasse mínima (padrão SkeletonAI): só kind()/name().
 */

#pragma once

#include "DwarfAI.h"

namespace support {

class BlazeAI : public DwarfAI {
public:
    BlazeAI() = default;

    const char *name() const override { return "Blaze"; }
    core::EntityKind kind() const override {
        return core::EntityKind::Blaze;
    }
};

} // namespace support
