/**
 * @file src/core/EquipSlot.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Enum de slots de equipamento com nomes para interface.
 * @details Define EquipSlot com mão direita, cabeça, peito e pernas, usado por ItemDef e Equipment para quebrar ciclo de includes.
 */

#pragma once
#include <cstdint>

namespace core {

// Slot de equipamento. None = não equipável (a peça nem entra no menu
// Equip). Definido aqui (e não em Equipment.h) porque ItemDef precisa
// dele e Equipment precisa de Item — header próprio quebra o ciclo.
enum class EquipSlot : uint8_t {
    None,
    RightHand,
    Head,
    Chest,
    Legs,
    COUNT
};

inline constexpr int kEquipSlotCount = static_cast<int>(EquipSlot::COUNT);

inline const char* equipSlotName(EquipSlot s) {
    switch (s) {
        case EquipSlot::RightHand: return "Right Hand";
        case EquipSlot::Head:      return "Head";
        case EquipSlot::Chest:     return "Chest";
        case EquipSlot::Legs:      return "Legs";
        default:                   return "";
    }
}

} // namespace core
