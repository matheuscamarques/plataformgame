#pragma once
#include <SFML/Graphics/Color.hpp>
#include <cstdint>

namespace core {

// Material = cor; forma é compartilhada (1 ASCII × N materiais).
// Novo material = 1 entrada aqui; todos os equipamentos ganham variante.
enum class MaterialId : uint8_t {
    Iron,
    Leather,
    Gold,
    Diamond,
    COUNT
};

struct MaterialColors {
    sf::Color main;   // metal claro / couro principal
    sf::Color dark;   // metal escuro / sombra
    sf::Color accent; // detalhe (guarda, emblema, brilho)
};

inline const MaterialColors &materialColors(MaterialId m) {
    static const MaterialColors table[static_cast<int>(MaterialId::COUNT)] = {
        {{190, 190, 200}, {100, 100, 110}, {180, 130, 60}},  // Iron
        {{160, 110, 70}, {100, 70, 45}, {80, 55, 35}},       // Leather
        {{220, 190, 90}, {170, 140, 50}, {255, 230, 130}},   // Gold
        {{180, 220, 240}, {100, 150, 190}, {220, 240, 255}}, // Diamond
    };
    return table[static_cast<int>(m)];
}

inline const char *materialName(MaterialId m) {
    switch (m) {
        case MaterialId::Iron: return "Iron";
        case MaterialId::Leather: return "Leather";
        case MaterialId::Gold: return "Gold";
        case MaterialId::Diamond: return "Diamond";
        default: return "?";
    }
}

} // namespace core
