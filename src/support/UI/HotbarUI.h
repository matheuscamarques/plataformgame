/**
 * @file src/support/UI/HotbarUI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara barra rápida dos cinco primeiros slots visíveis.
 * @details Define classe com constantes de 5 slots e 48px, handleInput testável headless e render com fonte, incluída por game.h e usada com Inventory do Player.
 */

#pragma once
#include <vector>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "core/Inventory.h"
#include "core/Vec.h"

namespace support {

class InputMap;
} // namespace support

class Player; // global ::Player (só fwd aqui; .cpp inclui)

namespace support {

// Hotbar: só consumíveis ou arremessáveis (poção, bombas), na ordem do
// inventário, compactados nos 5 visíveis. Sem inventário separado
// (fonte única — Terraria/Minecraft).
// Lógica (slot ativo) testável headless; render precisa de GL.
class HotbarUI {
public:
    static constexpr int   kSlots     = 5;
    static constexpr float kSlotSize  = 48.f;
    static constexpr float kPad       = 4.f;

    // Critério da hotbar: Consumable ou throwable (arma/equip nunca entra).
    static bool showsItem(const core::ItemDef* def);

    // Índices reais (no Inventory) dos itens exibidos, em ordem.
    static std::vector<int> filteredSlots(const core::Inventory& inv);

    // Slot real p/ o índice ativo 0..4 (-1 = vazio/fora da lista).
    static int realSlot(const core::Inventory& inv, int active);

    // Devolve o novo slot ativo se 1..5 foi apertado, senão `current`.
    int handleInput(const InputMap& input, int current) const;

    // Cinto DS (substitui a barra Minecraft): 4 boxes com ícones —
    // [Z] mão esq, [X] mão dir, [C] magia sintonizada, [V] item ativo
    // da hotbar (com qtd e n/5). Lógica 1-5 intacta; só o desenho muda.
    void renderBelt(sf::RenderTarget& target, const ::Player& player,
                    int activeSlot, float screenW, float screenH,
                    const sf::Font& font) const;

private:
};

} // namespace support
