/**
 * @file src/core/Inventory.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementação do inventário com empilhamento e remoção atômica.
 * @details Adiciona completando pilhas e ocupando vazios e remove tudo ou nada, usado por Player, loot e comércio.
 */

#include "core/Inventory.h"

#include <algorithm>

namespace core {

int Inventory::add(const Item& item) {
    if (item.isEmpty() || item.quantity == 0) return 0;

    const ItemDef* def = item.def();
    if (!def) return item.quantity; // def desconhecido: recusa tudo

    int remaining = item.quantity;
    const int stackMax = def->stackMax;

    // FASE 1: completa stacks existentes do mesmo tipo (não fragmenta).
    if (stackMax > 1) {
        for (auto& slot : slots_) {
            if (remaining <= 0) break;
            if (slot.defId != item.defId) continue;
            if (slot.quantity >= stackMax) continue;
            const int transfer = std::min(stackMax - slot.quantity, remaining);
            slot.quantity = static_cast<uint16_t>(slot.quantity + transfer);
            remaining -= transfer;
        }
    }

    // FASE 2: ocupa slots vazios (novas stacks).
    for (auto& slot : slots_) {
        if (remaining <= 0) break;
        if (!slot.isEmpty()) continue;
        const int transfer = std::min(stackMax, remaining);
        slot = item;
        slot.quantity = static_cast<uint16_t>(transfer);
        remaining -= transfer;
    }

    return remaining; // FASE 3: sobra (inventário cheio)
}

bool Inventory::remove(const std::string& defId, int quantity) {
    if (quantity <= 0) return true;
    if (count(defId) < quantity) return false; // atômico: nada sai

    int remaining = quantity;
    for (auto& slot : slots_) {
        if (remaining <= 0) break;
        if (slot.defId != defId) continue;
        const int take = std::min<int>(slot.quantity, remaining);
        slot.quantity = static_cast<uint16_t>(slot.quantity - take);
        remaining -= take;
        if (slot.quantity == 0) slot = Item{};
    }
    return true;
}

int Inventory::count(const std::string& defId) const {
    int total = 0;
    for (const auto& slot : slots_)
        if (slot.defId == defId) total += slot.quantity;
    return total;
}

int Inventory::usedSlots() const {
    int n = 0;
    for (const auto& slot : slots_)
        if (!slot.isEmpty()) ++n;
    return n;
}

void Inventory::swap(int a, int b) {
    if (a == b) return;
    if (a < 0 || a >= kCapacity || b < 0 || b >= kCapacity) return;
    std::swap(slots_[a], slots_[b]);
}

void Inventory::sort() {
    // Bolha estável sobre tipo+id; vazios afundam (trocas só se fazem
    // sentido: vazio nunca sobe). N=40, O(n²) irrelevante.
    for (int i = 0; i < kCapacity; ++i) {
        for (int j = i + 1; j < kCapacity; ++j) {
            const Item& a = slots_[i];
            const Item& b = slots_[j];
            if (a.isEmpty() && b.isEmpty()) continue;
            if (!a.isEmpty() && b.isEmpty()) continue; // cheio fica
            if (a.isEmpty() && !b.isEmpty()) {
                std::swap(slots_[i], slots_[j]);
                continue;
            }
            const ItemDef* da = a.def();
            const ItemDef* db = b.def();
            const int ta = da ? static_cast<int>(da->type) : 999;
            const int tb = db ? static_cast<int>(db->type) : 999;
            if (ta == tb ? a.defId > b.defId : ta > tb)
                std::swap(slots_[i], slots_[j]);
        }
    }
}

} // namespace core
