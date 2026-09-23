/**
 * @file src/support/UI/HotbarUI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara barra rápida dos cinco primeiros slots visíveis.
 * @details Define classe com constantes de 5 slots e 48px, handleInput testável headless e render com fonte, incluída por game.h e usada com Inventory do Player.
 */

#pragma once
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "core/Inventory.h"

namespace support {

class InputMap;

// Hotbar 4a: 5 primeiros slots do inventário, sempre visíveis.
// Sem inventário separado (fonte única — Terraria/Minecraft).
// Lógica (slot ativo) testável headless; render precisa de GL.
class HotbarUI {
public:
    static constexpr int   kSlots     = 5;
    static constexpr float kSlotSize  = 48.f;
    static constexpr float kPad       = 4.f;

    // Devolve o novo slot ativo se 1..5 foi apertado, senão `current`.
    int handleInput(const InputMap& input, int current) const;

    void render(sf::RenderTarget& target, const core::Inventory& inv,
                int activeSlot, float screenW, float screenH,
                const sf::Font& font) const;

private:
    sf::Vector2f slotPos(int i, float screenW, float screenH) const {
        const float totalW = kSlots * kSlotSize + (kSlots - 1) * kPad;
        const float x0 = (screenW - totalW) * 0.5f;
        const float y0 = screenH - kSlotSize - 16.f;
        return {x0 + i * (kSlotSize + kPad), y0};
    }
};

} // namespace support
