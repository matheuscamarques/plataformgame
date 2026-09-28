/**
 * @file tests/test_mainmenu.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless da tela inicial (navegação e disparo único).
 * @details Cobre cursor com wrap, Novo (disparo), Carregar (submenu com
 * slots vazios no-op + Voltar), Sair e consumo de edges, roda com make
 * test que compila em build/tests/test_mainmenu.
 */

#include <cassert>
#include <cstdio>

#include "game/MainMenuScreen.h"
#include "support/Input/InputMap.h"

// Espelha test_inventory_ui (InputMap é o único que toca sf::Keyboard).
namespace {
sf::Event keyEvent(sf::Event::EventType t, sf::Keyboard::Key k) {
    sf::Event e{};
    e.type = t;
    e.key.code = k;
    return e;
}

void press(support::InputMap& in, sf::Keyboard::Key k) {
    in.beginFrame();
    in.handleEvent(keyEvent(sf::Event::KeyPressed, k));
}

void release(support::InputMap& in, sf::Keyboard::Key k) {
    in.handleEvent(keyEvent(sf::Event::KeyReleased, k));
    in.onTickEnd();
    in.beginFrame();
}

void tap(support::InputMap& in, sf::Keyboard::Key k) {
    press(in, k);
    release(in, k);
}

} // namespace

int main() {
    using MenuAction = game::menu::Action;
    using Act = support::Action;
    using game::menu::MainMenuScreen;
    using game::menu::Screen;
    using support::InputMap;

    { // NavigateWrap (cursor circula nas 3 linhas)
        MainMenuScreen m;
        InputMap in;
        assert(m.screen() == Screen::Main && m.cursor() == 0);
        assert(m.consumeAction() == MenuAction::None);
        press(in, sf::Keyboard::Up);
        m.handleInput(in);
        release(in, sf::Keyboard::Up);
        assert(m.cursor() == 2); // wrap p/ Sair
        press(in, sf::Keyboard::Down);
        m.handleInput(in);
        release(in, sf::Keyboard::Down);
        assert(m.cursor() == 0);
        press(in, sf::Keyboard::Down);
        m.handleInput(in);
        release(in, sf::Keyboard::Down);
        assert(m.cursor() == 1);
    }
    { // NewGameFiresOnce (Enter dispara Novo 1x)
        MainMenuScreen m;
        InputMap in;
        press(in, sf::Keyboard::Return);
        m.handleInput(in);
        release(in, sf::Keyboard::Return);
        assert(m.consumeAction() == MenuAction::NewGame);
        assert(m.consumeAction() == MenuAction::None); // disparo único
    }
    { // QuitFires (Sair no cursor 2)
        MainMenuScreen m;
        InputMap in;
        press(in, sf::Keyboard::Down);
        m.handleInput(in);
        release(in, sf::Keyboard::Down);
        press(in, sf::Keyboard::Down);
        m.handleInput(in);
        release(in, sf::Keyboard::Down);
        assert(m.cursor() == 2);
        press(in, sf::Keyboard::Return);
        m.handleInput(in);
        release(in, sf::Keyboard::Return);
        assert(m.consumeAction() == MenuAction::Quit);
    }
    { // LoadEmpty (slots vazios no-op; Voltar e Esc voltam)
        MainMenuScreen m;
        InputMap in;
        press(in, sf::Keyboard::Down);
        m.handleInput(in);
        release(in, sf::Keyboard::Down); // Carregar
        press(in, sf::Keyboard::Return);
        m.handleInput(in);
        release(in, sf::Keyboard::Return);
        assert(m.screen() == Screen::Load && m.cursor() == 0);
        assert(m.consumeAction() == MenuAction::None);
        tap(in, sf::Keyboard::Return); // slot vazio: fica
        m.handleInput(in);
        assert(m.screen() == Screen::Load);
        assert(m.consumeAction() == MenuAction::None);
        press(in, sf::Keyboard::Down);
        m.handleInput(in);
        release(in, sf::Keyboard::Down);
        press(in, sf::Keyboard::Down);
        m.handleInput(in);
        release(in, sf::Keyboard::Down);
        press(in, sf::Keyboard::Down);
        m.handleInput(in);
        release(in, sf::Keyboard::Down);
        assert(m.cursor() == 3); // Voltar
        press(in, sf::Keyboard::Return);
        m.handleInput(in);
        release(in, sf::Keyboard::Return);
        assert(m.screen() == Screen::Main && m.cursor() == 1);
        // Esc volta do Load também.
        press(in, sf::Keyboard::Return);
        m.handleInput(in);
        release(in, sf::Keyboard::Return); // entra no Load de novo
        assert(m.screen() == Screen::Load);
        press(in, sf::Keyboard::Escape);
        m.handleInput(in);
        release(in, sf::Keyboard::Escape);
        assert(m.screen() == Screen::Main);
        // Esc no Main não sai nem dispara.
        press(in, sf::Keyboard::Escape);
        m.handleInput(in);
        release(in, sf::Keyboard::Escape);
        assert(m.screen() == Screen::Main);
        assert(m.consumeAction() == MenuAction::None);
    }
    { // EdgesConsumed (navegar não vaza p/ o gameplay)
        MainMenuScreen m;
        InputMap in;
        press(in, sf::Keyboard::Down); // sem soltar: edge vivo
        assert(in.pressed(Act::Down));
        m.handleInput(in);
        assert(!in.pressed(Act::Down)); // menu consumiu
        assert(m.cursor() == 1);
        release(in, sf::Keyboard::Down);
        press(in, sf::Keyboard::Return);
        m.handleInput(in); // entra no Load (consome o Enter)
        assert(!in.pressed(Act::UseItem));
        assert(m.screen() == Screen::Load);
        release(in, sf::Keyboard::Return);
    }

    std::printf("mainmenu test OK\n");
    return 0;
}
