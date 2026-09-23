/**
 * @file src/core/Equipment.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Cabeçalho do equipamento com quatro slots fixos sem interface gráfica.
 * @details Declara a classe Equipment pura e headless com equip, unequip e forEach, incluído por Player e UI e Item.
 */

#pragma once
#include <array>

#include "core/Item.h"

namespace core {

// Equipamento do player: 4 slots fixos (Dark Souls, sem Tetris).
// Puro (sem SFML, sem Player): headless-safe. O slot natural vem do
// def->equipSlot; equip() troca e devolve o antigo via outOld.
class Equipment {
public:
    // Equipa no slot natural do def. Retorna false se item vazio, sem
    // def, ou não equipável. Slot ocupado: substitui e devolve o antigo.
    bool equip(const Item& item, Item* outOld = nullptr);

    // Remove e devolve o item do slot (vazio se já estava vazio).
    Item unequip(EquipSlot slot);

    const Item& get(EquipSlot slot) const;
    bool        isOccupied(EquipSlot slot) const;
    bool        isEmpty() const;

    template <typename F>
    void forEach(F&& fn) const {
        for (int i = 1; i < kEquipSlotCount; ++i)
            fn(static_cast<EquipSlot>(i), slots_[i]);
    }

private:
    static int indexOf(EquipSlot slot) {
        const int i = static_cast<int>(slot);
        return (i >= 1 && i < kEquipSlotCount) ? i : -1;
    }

    std::array<Item, kEquipSlotCount> slots_{};
};

} // namespace core
