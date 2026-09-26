/**
 * @file src/support/Enemies/SpiderAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IA do spider: comportamento herdado, identidade própria.
 * @details Subclasse mínima (padrão SkeletonAI): só kind()/name().
 */

#pragma once

#include "DwarfAI.h"

namespace support {

class SpiderAI : public DwarfAI {
public:
    SpiderAI() = default;

    const char *name() const override { return "Spider"; }
    core::EntityKind kind() const override {
        return core::EntityKind::Spider;
    }
};

} // namespace support
