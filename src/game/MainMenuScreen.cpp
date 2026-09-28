/**
 * @file src/game/MainMenuScreen.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementa navegação e desenho da tela inicial.
 * @details handleInput consome Up/Down/UseItem/Pause; confirmação de
 * Novo/Sair vira Action de disparo único; slots de Load vazios (no-op).
 * Render centralizado por bounds como a vinheta do tarô, chamado por App.
 */

#include "game/MainMenuScreen.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>

#include "support/SfString.h"

namespace game {
namespace menu {

const char *MainMenuScreen::rowLabel(Screen s, int row) {
    if (s == Screen::Main) {
        switch (row) {
            case 0: return "Novo Jogo";
            case 1: return "Carregar Jogo";
            case 2: return "Sair";
            default: return "?";
        }
    }
    if (row >= 0 && row < kSlots) return "Slot: vazio";
    if (row == kSlots) return "Voltar";
    return "?";
}

void MainMenuScreen::handleInput(support::InputMap &in) {
    using support::Action;
    const int n = rowCount();
    if (in.pressed(Action::Up)) {
        in.consume(Action::Up);
        cursor_ = (cursor_ + n - 1) % n;
    }
    if (in.pressed(Action::Down)) {
        in.consume(Action::Down);
        cursor_ = (cursor_ + 1) % n;
    }
    if (in.pressed(Action::UseItem)) {
        in.consume(Action::UseItem);
        if (screen_ == Screen::Main) {
            if (cursor_ == 0) pending_ = game::menu::Action::NewGame;
            else if (cursor_ == 1) {
                screen_ = Screen::Load;
                cursor_ = 0;
            } else if (cursor_ == 2)
                pending_ = game::menu::Action::Quit;
        } else {
            // Load: slots vazios são no-op (SaveSystem vem no #6).
            if (cursor_ == kSlots) {
                screen_ = Screen::Main;
                cursor_ = 1; // volta no Carregar
            }
        }
    }
    if (in.pressed(Action::Pause)) {
        in.consume(Action::Pause);
        if (screen_ == Screen::Load) {
            screen_ = Screen::Main;
            cursor_ = 1;
        }
        // No Main, Esc não faz nada (sair é explícito).
    }
}

Action MainMenuScreen::consumeAction() {
    const Action a = pending_;
    pending_ = Action::None;
    return a;
}

void MainMenuScreen::render(sf::RenderTarget &target, const sf::Font &font,
                            float sw, float sh) const {
    target.setView(sf::View(sf::FloatRect(0.f, 0.f, sw, sh)));
    sf::RectangleShape bg({sw, sh});
    bg.setFillColor(sf::Color(8, 8, 14));
    target.draw(bg);

    auto centerText = [&](const std::string &s, float y, int size,
                          sf::Color c) {
        sf::Text t;
        t.setFont(font);
        t.setString(support::utf8(s));
        t.setCharacterSize(static_cast<unsigned>(size));
        t.setFillColor(c);
        t.setOutlineColor(sf::Color::Black);
        t.setOutlineThickness(1);
        const auto b = t.getLocalBounds();
        t.setPosition(sw * 0.5f - (b.left + b.width * 0.5f), y);
        target.draw(t);
    };
    centerText("WORLD OF RHIZOME", sh * 0.5f - 120.f, 40,
               sf::Color(200, 180, 120));
    if (screen_ == Screen::Load)
        centerText("CARREGAR JOGO", sh * 0.5f - 70.f, 16,
                   sf::Color(150, 150, 150));

    const int n = rowCount();
    for (int i = 0; i < n; ++i) {
        const bool sel = (i == cursor_);
        std::string s = std::string(sel ? "> " : "  ") +
                        rowLabel(screen_, i);
        centerText(s, sh * 0.5f - 30.f + i * 34.f, 22,
                   sel ? sf::Color(255, 220, 100)
                       : sf::Color(170, 170, 180));
    }
    centerText("Setas navegam - Enter confirma - Esc volta",
               sh - 60.f, 13, sf::Color(120, 120, 130));
}

} // namespace menu
} // namespace game
