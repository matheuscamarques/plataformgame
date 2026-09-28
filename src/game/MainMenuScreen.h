/**
 * @file src/game/MainMenuScreen.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Tela inicial: Novo Jogo, Carregar Jogo, Sair.
 * @details Máquina Main <-> Load com cursor e ação de disparo único.
 * Lógica pura (sem GL, testável headless); render SFML direto como o
 * HUD. Slots de save sempre vazios até o SaveSystem (#6). Dirigida por
 * InputMap com consume (Up/Down/UseItem/Pause), incluída por App.
 */

#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "support/Input/InputMap.h"

namespace game {
namespace menu {

// Novo Jogo entra no gameplay (classe TEMP até a criação, #3).
// Sair fecha a janela. Carregar sem save é no-op.
enum class Action : uint8_t { None, NewGame, Quit };

enum class Screen : uint8_t { Main, Load };

class MainMenuScreen {
public:
    static constexpr int kMainRows = 3; // Novo, Carregar, Sair
    static constexpr int kSlots = 3;
    static constexpr int kLoadRows = kSlots + 1; // + Voltar

    static const char *rowLabel(Screen s, int row);

    // Up/Down movem, UseItem (U/Enter) confirma, Pause (Esc) volta.
    // Consome os edges lidos (sem vazar p/ o gameplay).
    void handleInput(support::InputMap &in);

    // Disparo único: None até a próxima confirmação de Novo/Sair.
    Action consumeAction();

    Screen screen() const { return screen_; }
    int cursor() const { return cursor_; }
    int rowCount() const {
        return screen_ == Screen::Main ? kMainRows : kLoadRows;
    }

    void render(sf::RenderTarget &target, const sf::Font &font, float sw,
                float sh) const;

private:
    Screen screen_ = Screen::Main;
    int cursor_ = 0;
    Action pending_ = Action::None;
};

} // namespace menu
} // namespace game
