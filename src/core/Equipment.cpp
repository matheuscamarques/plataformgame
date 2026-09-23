/**
 * @file src/core/Equipment.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementação do equipamento com quatro slots fixos do jogador.
 * @details Equipa no slot do def, troca e devolve o antigo e suporta unequip e get, usado por Player e telas de inventário.
 */

#include "core/Equipment.h"

namespace core {

bool Equipment::equip(const Item& item, Item* outOld) {
    if (item.isEmpty()) return false;
    const ItemDef* def = item.def();
    if (!def) return false;
    const int idx = indexOf(def->equipSlot);
    if (idx < 0) return false; // None = não equipável

    if (outOld) *outOld = slots_[idx];
    slots_[idx] = item;
    return true;
}

Item Equipment::unequip(EquipSlot slot) {
    const int idx = indexOf(slot);
    if (idx < 0) return {};
    Item old = slots_[idx];
    slots_[idx] = Item{};
    return old;
}

const Item& Equipment::get(EquipSlot slot) const {
    const int idx = indexOf(slot);
    if (idx < 0) {
        static const Item empty;
        return empty;
    }
    return slots_[idx];
}

bool Equipment::isOccupied(EquipSlot slot) const {
    return !get(slot).isEmpty();
}

bool Equipment::isEmpty() const {
    for (int i = 1; i < kEquipSlotCount; ++i)
        if (!slots_[i].isEmpty()) return false;
    return true;
}

float Equipment::weight() const {
    float total = 0.f;
    for (int i = 1; i < kEquipSlotCount; ++i) {
        const Item& it = slots_[i];
        if (it.isEmpty()) continue;
        if (const ItemDef* def = it.def())
            total += def->weight * it.quantity;
    }
    return total;
}

} // namespace core
