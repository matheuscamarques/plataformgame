#pragma once
#include <cstdint>
#include <string>

#include "core/ItemDef.h"

namespace core {

// Instância: o que o player carrega (slot, orb, mão do cursor).
// Leve e copiável; o dado pesado mora no ItemDef (registry).
struct Item {
    std::string defId;
    uint16_t    quantity = 1;

    const ItemDef* def() const {
        return ItemRegistry::instance().find(defId);
    }
    bool isEmpty() const { return defId.empty() || quantity == 0; }
    bool isStackable() const {
        const auto* d = def();
        return d && d->stackMax > 1;
    }
};

} // namespace core
