/**
 * @file src/core/Item.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Instância leve de item que aponta para a definição compartilhada.
 * @details Guarda defId e quantidade com acesso via def e testes de vazio, usado por Inventory, Equipment e cursor.
 */

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
