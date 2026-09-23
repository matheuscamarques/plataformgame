/**
 * @file src/support/UI/InventoryUI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara UI de inventário estilo Dark Souls via teclado.
 * @details Define máquina Closed, Browse, ActionMenu e ConfirmDrop com abas, cursor, filtros e ações Use, Equip e Drop, incluída por game.h e dirigida por InputMap.
 */

#pragma once
#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

#include "core/Inventory.h"
#include "core/Equipment.h"

namespace support {

class InputMap;
class DropSystem;

} // namespace support

class Player; // global; completo só no .cpp

namespace support {

// UI de inventário estilo Dark Souls (teclado puro, sem mouse).
// Máquina de estados: Closed -> Browse <-> ActionMenu -> ConfirmDrop.
// Sem freeze: o jogo segue com o menu aberto (decisão); por isso a UI
// consome todos os edges enquanto aberta (App faz o consume).
//
// Dependências injetadas (nullable; null = ação correspondente no-op):
// inv_ (obrigatório p/ quase tudo), equipment_ (Equip/Unequip + aba),
// player_ (Use/Drop), drops_ (Drop).
class InventoryUI {
public:
    enum class MainTab : uint8_t { Inventory, Equipment, COUNT };
    enum class SubTab : uint8_t {
        All, Materials, Consumables, Weapons, Armor, Keys, COUNT
    };
    enum class UIState : uint8_t { Closed, Browse, ActionMenu, ConfirmDrop };
    // Ações do menu (lista dinâmica por item; Cancel = Esc).
    enum class MenuAction : uint8_t { Use, Equip, Unequip, Drop, Arrange, COUNT };

    static constexpr int   kCols     = 8;
    static constexpr int   kRows     = 5;
    static constexpr int   kSlots    = kCols * kRows; // == Inventory::kCapacity
    static constexpr float kSlotSize = 56.f;
    static constexpr float kPad      = 4.f;
    static constexpr float kEquipSlotSize = 96.f;
    static constexpr float kEquipPad      = 24.f;

    static const char* mainTabName(MainTab t);
    static const char* subTabName(SubTab t);
    static const char* menuActionName(MenuAction a);
    static bool matchesSubTab(const core::ItemDef* def, SubTab sub);

    // Dependências (injetadas pelo Game a cada tick).
    void setInventory(core::Inventory* inv)   { inv_ = inv; }
    void setEquipment(core::Equipment* eq)    { equipment_ = eq; }
    void setPlayer(::Player* p)               { player_ = p; }
    void setDrops(DropSystem* d)              { drops_ = d; }

    // Estado. open() reseta p/ Browse/Inventory/All/cursor 0.
    bool isOpen() const { return state_ != UIState::Closed; }
    UIState state() const { return state_; }
    void open();
    void close();
    void toggle();

    // Input. Retorna true se consumiu (sempre, enquanto aberto).
    bool handleInput(const InputMap& input);

    void render(sf::RenderTarget& target, const sf::Font& font,
                float screenW, float screenH);

    // Observabilidade (testes).
    int cursor() const { return cursor_; }
    void setCursor(int i) {
        cursor_ = std::clamp(i, 0, kSlots - 1);
        snapCursor();
    }
    MainTab mainTab() const { return mainTab_; }
    void setMainTab(MainTab t) {
        mainTab_ = t;
        cursor_ = 0;
        equipCursor_ = 0;
        snapCursor();
    }
    SubTab subTab() const { return subTab_; }
    void setSubTab(SubTab s) {
        subTab_ = s;
        cursor_ = 0;
        snapCursor();
    }
    int equipCursor() const { return equipCursor_; }
    int actionCursor() const { return actionCursor_; }

    // Slots não-vazios que casam com a sub-tab (ordem de slot).
    std::vector<int> filteredSlots() const;
    // Ações disponíveis p/ o item atual (vazio = menu nem abre).
    std::vector<MenuAction> menuActions() const;

    // Item sob o cursor (grid ou slot de equipamento), ou nullptr.
    const core::Item* selectedItem() const;
    // "DMG: 18 (atual 12, +6)" — só p/ arma/armadura com referência.
    struct StatCompare {
        bool show = false; // tem com o que comparar
        int equipped = 0;  // valor atual no Equipment
        int diff = 0;      // selecionado - atual
    };
    StatCompare compareStats() const;
    // "Soltar 10x Pedra?" p/ o ConfirmDrop (fallback genérico sem item).
    std::string confirmText() const;

    // Executa ação do menu sobre o item atual. Retorna se algo aconteceu
    // (false = bloqueado: sem deps, chave, grid cheio). Guarda feedback
    // p/ o rodapé (última ação).
    bool executeAction(MenuAction action);
    const std::string& feedback() const { return feedback_; }

private:
    core::Inventory* inv_        = nullptr;
    core::Equipment* equipment_  = nullptr;
    ::Player*        player_     = nullptr;
    DropSystem*      drops_      = nullptr;

    UIState state_     = UIState::Closed;
    MainTab mainTab_   = MainTab::Inventory;
    SubTab  subTab_    = SubTab::All;
    int     cursor_      = 0; // slot do grid (índice; hotbar lê índice)
    int     equipCursor_ = 0; // 0..3 (RightHand, Head, Chest, Legs)
    int     actionCursor_ = 0; // índice em menuActions()
    std::string feedback_;     // última ação (rodapé; limpa ao abrir)

    void handleBrowse(const InputMap& input);
    void handleActionMenu(const InputMap& input);
    void handleConfirmDrop(const InputMap& input);
    void openActionMenu();

    bool slotMatches(int index) const; // ocupado + casa com a sub-tab
    int firstValid() const; // 1º slot navegável (-1 = nenhum)
    int lastValid() const;  // último navegável (-1 = nenhum)
    void snapCursor();      // cursor p/ o 1º válido (ou fica)
    void stepCursor(int delta); // pula inválidos (1 volta)
    void cycleMainTab(int delta);      // troca + cursor 0
    void cycleSubTab(int delta);       // troca + cursor 0

    sf::Vector2f gridOrigin(float sw, float sh) const;
    sf::Vector2f slotPos(int index, float sw, float sh) const;
    sf::Vector2f detailOrigin(float sw, float sh) const;
    static constexpr float kDetailW = 270.f;

    void renderMainTabs(sf::RenderTarget& t, float sw, const sf::Font& f) const;
    void renderSubTabs(sf::RenderTarget& t, float sw, const sf::Font& f) const;
    void renderGrid(sf::RenderTarget& t, float sw, float sh,
                    const sf::Font& f) const;
    void renderEquipTab(sf::RenderTarget& t, float sw, float sh,
                        const sf::Font& f) const;
    void renderDetailPanel(sf::RenderTarget& t, float sw, float sh,
                           const sf::Font& f) const;
    void renderActionMenu(sf::RenderTarget& t, float sw, float sh,
                          const sf::Font& f) const;
    void renderConfirmDrop(sf::RenderTarget& t, float sw, float sh,
                           const sf::Font& f) const;
    void renderFooter(sf::RenderTarget& t, float sw, float sh,
                      const sf::Font& f) const;
};

} // namespace support
