#pragma once
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>

#include "core/Inventory.h"

namespace support {

class InputMap;

} // namespace support

class Player; // global (entities/Player/Player.h); completo só no .cpp

namespace support {

// Grid 8×5 do inventário (fase 4b, teclado puro): setas movem o cursor,
// F pega/solta, E abre/fecha, Esc fecha. Sem mouse (entra com botões
// clicáveis, fase futura). Sem freeze: o jogo segue com o grid aberto.
//
// Fase 4c (tabs): Q/Tab ciclam abas por ItemType; setas pulam slots que
// não casam (vazios sempre casam, p/ permitir soltar em qualquer aba).
// Fase 4d (painel): detalhe fixo à direita com dano/defesa quando >0.
// Fase 4e (intents): R descarta (Key/Quest bloqueiam), T reordena via
// Inventory::sort(), U/Enter usa consomível ou equipa arma/armadura.
// R com o grid aberto consome o edge p/ o RunManager não restartar;
// o App faz o consume + spawna o orbe a partir do pendingDrop().
class InventoryUI {
public:
    static constexpr int   kCols     = 8;
    static constexpr int   kRows     = 5;
    static constexpr int   kSlots    = kCols * kRows; // == Inventory::kCapacity
    static constexpr float kSlotSize = 48.f;
    static constexpr float kPad      = 4.f;

    // Abas: All + 1 por ItemType (Key cobre Key e Quest).
    enum Tab : int {
        TabAll = 0,
        TabMaterial,
        TabConsumable,
        TabWeapon,
        TabArmor,
        TabKey,
        TabCount,
    };
    static const char* tabName(int tab);

    bool isOpen() const { return open_; }
    void toggle() { open_ = !open_; if (!open_) cancelHeld(); }
    void close() { open_ = false; cancelHeld(); }
    int  cursor() const { return cursor_; }
    int  tab() const { return tab_; }

    // Filtro da aba: vazio casa sempre; Key cobre Key+Quest.
    static bool matchesTab(const core::ItemDef* def, int tab);
    bool matchesSlot(const core::Inventory& inv, int index) const;

    // Drop pendente p/ o App spawnar o orbe (inv já mutado aqui).
    struct PendingDrop {
        std::string defId;
        int qty = 0;
    };
    bool hasPendingDrop() const { return pendingDrop_.qty > 0; }
    PendingDrop takePendingDrop() {
        PendingDrop out = pendingDrop_;
        pendingDrop_ = PendingDrop{};
        return out;
    }

    // Retorna true se consumiu o input (E/Esc sempre; resto só aberto).
    // R/Q/Tab/T/U consomem mesmo em no-op, p/ nada vazar p/ o jogo.
    // player == nullptr: Use vira no-op consumido (teste sem Player).
    bool handleInput(const InputMap& input, core::Inventory& inv,
                     ::Player* player = nullptr);

    void render(sf::RenderTarget& target, const core::Inventory& inv,
                const sf::Font& font, float screenW, float screenH) const;

private:
    bool open_     = false;
    int  cursor_   = 0;   // slot selecionado (setas)
    int  tab_      = TabAll;
    int  heldSlot_ = -1;  // origem do arrasto (-1 = mãos vazias)
    core::Item heldItem_; // item "na mão"
    PendingDrop pendingDrop_;

    void cancelHeld() {
        heldSlot_ = -1;
        heldItem_ = core::Item{};
        pendingDrop_ = PendingDrop{};
    }

    // Anda o cursor pulando slots que não casam com a aba (até 1 volta).
    void stepCursor(int delta, const core::Inventory& inv);
    void clampCursorToTab(const core::Inventory& inv);

    sf::Vector2f gridOrigin(float screenW, float screenH) const;
    sf::Vector2f slotPos(int index, float screenW, float screenH) const;
    void drawTabBar(sf::RenderTarget& target, const sf::Font& font,
                    float sw, float sh) const;
    void drawDetailPanel(sf::RenderTarget& target, const core::Inventory& inv,
                         const sf::Font& font, float sw, float sh) const;
};

} // namespace support
