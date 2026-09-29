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
    LeftHand, // segunda arma (dano soma no melee)
    Head,
    Chest,
    Legs,
    Boots,
    Gloves, // luva própria (antes seguia o elmo no Renderer)
    COUNT
};

inline constexpr int kEquipSlotCount = static_cast<int>(EquipSlot::COUNT);

inline const char* equipSlotName(EquipSlot s) {
    static constexpr const char* kNames[] = {
        "", "Right Hand", "Left Hand", "Head",
        "Chest", "Legs", "Boots", "Gloves",
    };
    static_assert(sizeof(kNames) / sizeof(kNames[0]) ==
                      static_cast<std::size_t>(EquipSlot::COUNT),
                  "equipSlotName: tabela fora de sincronia com o enum");
    const int i = static_cast<int>(s);
    if (i < 0 || i >= static_cast<int>(EquipSlot::COUNT)) return "";
    return kNames[i];
}

// Ordem de exibição na aba Equipment (mãos primeiro).
inline EquipSlot equipDisplaySlot(int i) {
    static constexpr EquipSlot kOrder[] = {
        EquipSlot::RightHand, EquipSlot::LeftHand, EquipSlot::Head,
        EquipSlot::Chest, EquipSlot::Legs, EquipSlot::Boots,
        EquipSlot::Gloves};
    if (i < 0 || i >= 7) return EquipSlot::None;
    return kOrder[i];
}

} // namespace core
