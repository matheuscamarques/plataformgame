/**
 * @file tests/test_creation.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless da criação de personagem (nome + classe).
 * @details Cobre navegação, digitação com teto, backspace, seleção de
 * classe, COMEÇAR com payload e Voltar, roda com make test que compila
 * em build/tests/test_creation.
 */

#include <cassert>
#include <cstdio>

#include "game/CharacterCreationScreen.h"
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

void nav(support::InputMap& in, game::creation::CharacterCreationScreen& s,
         sf::Keyboard::Key k) {
    press(in, k);
    s.handleInput(in);
    release(in, k);
}

} // namespace

int main() {
    using MenuAct = game::creation::Action;
    using Act = support::Action;
    using game::creation::CharacterCreationScreen;
    using support::InputMap;
    constexpr int kStart =
        CharacterCreationScreen::kStartRow;

    { // TypingEditsName (só na linha Nome, teto 12, backspace)
        CharacterCreationScreen s;
        InputMap in;
        assert(s.cursor() == 0 && s.name().empty());
        s.handleText('G');
        s.handleText('u');
        s.handleText('t');
        s.handleText('s');
        assert(s.name() == "Guts");
        s.handleText(7); // controle: ignora
        s.handleText(200); // fora do ASCII: ignora
        assert(s.name() == "Guts");
        for (int i = 0; i < 20; ++i) s.handleText('x');
        assert(s.name().size() == CharacterCreationScreen::kMaxName);
        s.handleBackspace();
        assert(s.name().size() ==
               CharacterCreationScreen::kMaxName - 1);
        // Fora do Nome: digitar puxa o foco e edita.
        nav(in, s, sf::Keyboard::Down); // classe 0
        assert(s.cursor() == 1);
        s.handleText('Z');
        assert(s.cursor() == 0);
        assert(s.name().back() == 'Z');
        s.handleBackspace();
        assert(s.name().back() != 'Z');
    }
    { // PreviewFollowsSelection (boneco veste a selecionada)
        CharacterCreationScreen s;
        InputMap in;
        assert(s.preview().weaponDef() &&
               s.preview().weaponDef()->id == "iron_sword"); // Knight
        nav(in, s, sf::Keyboard::Down);
        nav(in, s, sf::Keyboard::Down);
        nav(in, s, sf::Keyboard::Down); // Mago
        nav(in, s, sf::Keyboard::Return); // seleciona
        assert(s.klass() == core::PlayerClass::Mage);
        assert(s.preview().weaponDef() &&
               s.preview().weaponDef()->id == "wooden_staff");
        assert(s.preview().attuned.size() == 1u);
        s.reset();
        assert(s.preview().weaponDef()->id == "iron_sword");
    }
    { // SelectClassJumpsToStart (Enter na classe marca e avança)
        CharacterCreationScreen s;
        InputMap in;
        assert(s.klass() == core::PlayerClass::Knight); // default
        nav(in, s, sf::Keyboard::Down);
        nav(in, s, sf::Keyboard::Down);
        nav(in, s, sf::Keyboard::Down); // Mago (linha 3)
        assert(s.cursor() == 3);
        nav(in, s, sf::Keyboard::Return);
        assert(s.klass() == core::PlayerClass::Mage);
        assert(s.cursor() == kStart);
        assert(s.consumeAction() == MenuAct::None); // só marca
    }
    { // StartDeliversPayload (COMEÇAR entrega nome+classe 1x)
        CharacterCreationScreen s;
        InputMap in;
        s.handleText('A');
        for (int i = 0; i < kStart; ++i) nav(in, s, sf::Keyboard::Down);
        assert(s.cursor() == kStart);
        nav(in, s, sf::Keyboard::Return);
        assert(s.consumeAction() == MenuAct::Done);
        assert(s.consumeAction() == MenuAct::None);
        assert(s.name() == "A");
        assert(s.klass() == core::PlayerClass::Knight);
    }
    { // LettersDontNavigate (WASD digitam, setas navegam)
        CharacterCreationScreen s;
        InputMap in;
        // 's' digita (não desce), 'w' digita (não sobe).
        press(in, sf::Keyboard::S);
        s.handleInput(in);
        release(in, sf::Keyboard::S);
        s.handleText('s');
        press(in, sf::Keyboard::W);
        s.handleInput(in);
        release(in, sf::Keyboard::W);
        s.handleText('w');
        assert(s.cursor() == 0);
        assert(s.name() == "sw");
        // 'u' digita (não confirma): sem disparo, segue no Nome.
        press(in, sf::Keyboard::U);
        s.handleInput(in);
        release(in, sf::Keyboard::U);
        s.handleText('u');
        assert(s.cursor() == 0);
        assert(s.consumeAction() == MenuAct::None);
        assert(s.name() == "swu");
        // Seta física ainda navega.
        press(in, sf::Keyboard::Down);
        s.handleInput(in);
        release(in, sf::Keyboard::Down);
        assert(s.cursor() == 1);
    }
    { // BackToMenu (Esc volta sem concluir)
        CharacterCreationScreen s;
        InputMap in;
        nav(in, s, sf::Keyboard::Escape);
        assert(s.consumeAction() == MenuAct::Back);
        assert(s.consumeAction() == MenuAct::None);
    }
    { // ResetRestores (reentrada limpa)
        CharacterCreationScreen s;
        InputMap in;
        s.handleText('Z');
        nav(in, s, sf::Keyboard::Down);
        nav(in, s, sf::Keyboard::Return);
        s.reset();
        assert(s.name().empty() && s.cursor() == 0);
        assert(s.klass() == core::PlayerClass::Knight);
        assert(s.consumeAction() == MenuAct::None);
        (void)in;
    }

    std::printf("creation test OK\n");
    return 0;
}
