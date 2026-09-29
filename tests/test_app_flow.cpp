/**
 * @file tests/test_app_flow.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless da cola Main -> Creation -> gameplay (sem GL).
 * @details Replica a sequência do App::run sem render/display: Novo no
 * menu abre a criação; Done aplica nome+classe no Player; Voltar
 * retorna ao menu sem tocar no Player. Roda com make test que compila
 * em build/tests/test_app_flow.
 */

#include <cassert>
#include <cstdio>
#include <string>

#include "entities/Player/Player.h"
#include "game/CharacterCreationScreen.h"
#include "game/MainMenuScreen.h"
#include "support/Input/InputMap.h"

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

// Um passo do App: menu consome ação; NewGame abre a criação.
void menuStep(game::menu::MainMenuScreen& menu,
              game::creation::CharacterCreationScreen& creation,
              support::InputMap& in, bool& creationActive) {
    menu.handleInput(in);
    if (menu.consumeAction() == game::menu::Action::NewGame) {
        creation.reset();
        creationActive = true;
        in.clearAll();
    }
}

// Um passo do App: criação consome Done/Back; Done aplica no Player.
void creationStep(game::creation::CharacterCreationScreen& creation,
                  Player& p, support::InputMap& in, bool& creationActive,
                  bool& menuDone) {
    creation.handleInput(in);
    const auto act = creation.consumeAction();
    if (act == game::creation::Action::Done) {
        p.name = creation.name();
        p.applyClass(creation.klass());
        creationActive = false;
        menuDone = true;
        in.clearAll();
    } else if (act == game::creation::Action::Back) {
        creationActive = false;
        in.clearAll();
    }
}

} // namespace

int main() {
    using game::creation::CharacterCreationScreen;
    using game::menu::MainMenuScreen;
    using support::InputMap;

    { // NewOpensCreation (menu Novo -> criação resetada, não gameplay)
        MainMenuScreen menu;
        CharacterCreationScreen creation;
        InputMap in;
        bool creationActive = false;
        bool menuDone = false;
        press(in, sf::Keyboard::Return);
        menuStep(menu, creation, in, creationActive);
        release(in, sf::Keyboard::Return);
        assert(creationActive && !menuDone);
        assert(creation.cursor() == 0 && creation.name().empty());
    }
    { // DoneAppliesPlayer (nome+classe chegam ao Player via applyClass)
        MainMenuScreen menu;
        CharacterCreationScreen creation;
        Player p;
        InputMap in;
        bool creationActive = false;
        bool menuDone = false;
        press(in, sf::Keyboard::Return);
        menuStep(menu, creation, in, creationActive);
        release(in, sf::Keyboard::Return);
        assert(creationActive);
        for (char c : std::string("Ash")) creation.handleText(c);
        // Mago: 3 Downs (Cavaleiro->Bárbaro->Mago), Enter seleciona.
        for (int i = 0; i < 3; ++i) {
            press(in, sf::Keyboard::Down);
            creationStep(creation, p, in, creationActive, menuDone);
            release(in, sf::Keyboard::Down);
        }
        press(in, sf::Keyboard::Return);
        creationStep(creation, p, in, creationActive, menuDone);
        release(in, sf::Keyboard::Return);
        assert(creation.klass() == core::PlayerClass::Mage);
        // COMEÇAR: Enter conclui e aplica.
        press(in, sf::Keyboard::Return);
        creationStep(creation, p, in, creationActive, menuDone);
        release(in, sf::Keyboard::Return);
        assert(!creationActive && menuDone);
        assert(p.name == "Ash");
        assert(p.weaponDef() && p.weaponDef()->id == "wooden_staff");
        assert(p.attuned.size() == 1u);
        assert(p.souls == 0);
    }
    { // BackKeepsPlayer (Voltar não toca no Player nem conclui)
        MainMenuScreen menu;
        CharacterCreationScreen creation;
        Player p;
        InputMap in;
        bool creationActive = false;
        bool menuDone = false;
        press(in, sf::Keyboard::Return);
        menuStep(menu, creation, in, creationActive);
        release(in, sf::Keyboard::Return);
        press(in, sf::Keyboard::Escape);
        creationStep(creation, p, in, creationActive, menuDone);
        release(in, sf::Keyboard::Escape);
        assert(!creationActive && !menuDone);
        assert(p.name.empty()); // Player intacto
        assert(p.attuned.empty());
    }

    std::printf("app flow test OK\n");
    return 0;
}
