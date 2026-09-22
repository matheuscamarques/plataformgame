#pragma once
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

#include "core/Inventory.h"

namespace support {

class InputMap;

// Grid 8×5 do inventário (fase 4b, teclado puro): setas movem o cursor,
// F pega/solta, E abre/fecha, Esc fecha. Sem mouse (entra com botões
// clicáveis, fase futura). Sem freeze: o jogo segue com o grid aberto.
class InventoryUI {
public:
    static constexpr int   kCols     = 8;
    static constexpr int   kRows     = 5;
    static constexpr int   kSlots    = kCols * kRows; // == Inventory::kCapacity
    static constexpr float kSlotSize = 48.f;
    static constexpr float kPad      = 4.f;

    bool isOpen() const { return open_; }
    void toggle() { open_ = !open_; if (!open_) cancelHeld(); }
    void close() { open_ = false; cancelHeld(); }
    int  cursor() const { return cursor_; }

    // Retorna true se consumiu o input (E/Esc sempre; resto só aberto).
    bool handleInput(const InputMap& input, core::Inventory& inv);

    void render(sf::RenderTarget& target, const core::Inventory& inv,
                const sf::Font& font, float screenW, float screenH) const;

private:
    bool open_     = false;
    int  cursor_   = 0;   // slot selecionado (setas)
    int  heldSlot_ = -1;  // origem do arrasto (-1 = mãos vazias)
    core::Item heldItem_; // item "na mão"

    void cancelHeld() {
        heldSlot_ = -1;
        heldItem_ = core::Item{};
    }

    sf::Vector2f gridOrigin(float screenW, float screenH) const;
    sf::Vector2f slotPos(int index, float screenW, float screenH) const;
    void drawTooltip(sf::RenderTarget& target, const core::ItemDef& def,
                     int quantity, sf::Vector2f anchor, float sw, float sh,
                     const sf::Font& font) const;
};

} // namespace support
