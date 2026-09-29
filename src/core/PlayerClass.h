/**
 * @file src/core/PlayerClass.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Classes iniciais estilo DS (Criação de personagem, passo 1).
 * @details Enum PlayerClass + ClassDef puro (atributos, equipamento, itens,
 * magias). Sem escudo/adaga no registry: Cavaleiro vai sem escudo e Ladrão
 * de espada leve. Puro core (sem Player, sem SFML), testável headless.
 */

#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "core/Attributes.h"

namespace core {

// 6 classes (DS): distribuição difere, total similar (~80-84).
enum class PlayerClass : uint8_t {
    Knight,    // espada + peitoral, equilibrado
    Barbarian, // machado, STR/VIT
    Mage,      // cajado + soul_arrow, INT/ATT
    Cleric,    // sino + heal_light, FÉ/ATT
    Thief,     // espada leve + dinamites, DEX/RES
    Deprived,  // tudo 10, 1 poção
    COUNT
};

inline const char *className(PlayerClass c) {
    static constexpr const char *kNames[] = {
        "Cavaleiro", "Bárbaro", "Mago", "Clérigo", "Ladrão", "Desprovido",
    };
    static_assert(sizeof(kNames) / sizeof(kNames[0]) ==
                      static_cast<std::size_t>(PlayerClass::COUNT),
                  "className: tabela fora de sincronia com o enum");
    const int i = static_cast<int>(c);
    if (i < 0 || i >= static_cast<int>(PlayerClass::COUNT)) return "?";
    return kNames[i];
}

struct ClassDef {
    PlayerClass klass = PlayerClass::Deprived;
    const char *flavor = "";
    // Ordem: VIT, ATT, END, STR, DEX, RES, INT, FÉ (ordem do enum Attr).
    int base[static_cast<int>(Attr::COUNT)] = {};
    std::vector<std::string> equipment; // ItemDef ids (defSlot manda)
    std::vector<std::pair<std::string, int>> items; // (id, qtd)
    std::vector<std::string> spells; // defIds p/ attune (se couber)
};

inline ClassDef classDef(PlayerClass c) {
    ClassDef d;
    d.klass = c;
    switch (c) {
        case PlayerClass::Knight:
            d.flavor = "Equilibrado: espada, peitoral e poções.";
            d.base[static_cast<int>(Attr::Vitality)] = 12;
            d.base[static_cast<int>(Attr::Attunement)] = 10;
            d.base[static_cast<int>(Attr::Endurance)] = 12;
            d.base[static_cast<int>(Attr::Strength)] = 12;
            d.base[static_cast<int>(Attr::Dexterity)] = 10;
            d.base[static_cast<int>(Attr::Resistance)] = 10;
            d.base[static_cast<int>(Attr::Intelligence)] = 8;
            d.base[static_cast<int>(Attr::Faith)] = 10;
            d.equipment = {"iron_sword", "iron_chest"};
            d.items = {{"potion", 3}};
            break;
        case PlayerClass::Barbarian:
            d.flavor = "Bruto: machado e vida, pouca sutileza.";
            d.base[static_cast<int>(Attr::Vitality)] = 14;
            d.base[static_cast<int>(Attr::Attunement)] = 8;
            d.base[static_cast<int>(Attr::Endurance)] = 12;
            d.base[static_cast<int>(Attr::Strength)] = 14;
            d.base[static_cast<int>(Attr::Dexterity)] = 8;
            d.base[static_cast<int>(Attr::Resistance)] = 10;
            d.base[static_cast<int>(Attr::Intelligence)] = 8;
            d.base[static_cast<int>(Attr::Faith)] = 8;
            d.equipment = {"iron_axe"};
            d.items = {{"potion", 2}};
            break;
        case PlayerClass::Mage:
            d.flavor = "Sábio: cajado e flecha da alma.";
            d.base[static_cast<int>(Attr::Vitality)] = 10;
            d.base[static_cast<int>(Attr::Attunement)] = 12;
            d.base[static_cast<int>(Attr::Endurance)] = 10;
            d.base[static_cast<int>(Attr::Strength)] = 8;
            d.base[static_cast<int>(Attr::Dexterity)] = 10;
            d.base[static_cast<int>(Attr::Resistance)] = 10;
            d.base[static_cast<int>(Attr::Intelligence)] = 14;
            d.base[static_cast<int>(Attr::Faith)] = 10;
            d.equipment = {"wooden_staff"};
            d.spells = {"soul_arrow"};
            break;
        case PlayerClass::Cleric:
            d.flavor = "Devoto: sino e luz curativa.";
            d.base[static_cast<int>(Attr::Vitality)] = 11;
            d.base[static_cast<int>(Attr::Attunement)] = 12;
            d.base[static_cast<int>(Attr::Endurance)] = 10;
            d.base[static_cast<int>(Attr::Strength)] = 10;
            d.base[static_cast<int>(Attr::Dexterity)] = 8;
            d.base[static_cast<int>(Attr::Resistance)] = 10;
            d.base[static_cast<int>(Attr::Intelligence)] = 8;
            d.base[static_cast<int>(Attr::Faith)] = 14;
            d.equipment = {"priest_bell"};
            d.spells = {"heal_light"};
            break;
        case PlayerClass::Thief:
            d.flavor = "Sorrateiro: lâmina leve e pólvora.";
            d.base[static_cast<int>(Attr::Vitality)] = 10;
            d.base[static_cast<int>(Attr::Attunement)] = 10;
            d.base[static_cast<int>(Attr::Endurance)] = 12;
            d.base[static_cast<int>(Attr::Strength)] = 8;
            d.base[static_cast<int>(Attr::Dexterity)] = 14;
            d.base[static_cast<int>(Attr::Resistance)] = 12;
            d.base[static_cast<int>(Attr::Intelligence)] = 10;
            d.base[static_cast<int>(Attr::Faith)] = 8;
            d.equipment = {"leather_sword"};
            d.items = {{"dynamite", 5}};
            break;
        case PlayerClass::Deprived:
        default:
            d.klass = PlayerClass::Deprived;
            d.flavor = "Nada além de uma poção e vontade.";
            for (int i = 0; i < static_cast<int>(Attr::COUNT); ++i)
                d.base[i] = 10;
            d.items = {{"potion", 1}};
            break;
    }
    return d;
}

} // namespace core
