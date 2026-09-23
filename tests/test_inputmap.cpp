/**
 * @file tests/test_inputmap.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava teclas para ações com pressionar e soltar.
 * @details Cobre InputMap e Action, roda com make test que compila em build/tests/test_inputmap.
 */

#include <cassert>
#include <cstdio>
#include "support/Input/InputMap.h"

static sf::Event keyEvent(sf::Event::EventType t, sf::Keyboard::Key k) {
    sf::Event e{};
    e.type = t;
    e.key.code = k;
    return e;
}

int main() {
    using support::Action;
    support::InputMap in;

    // Protocolo: beginFrame (topo) -> handleEvent (poll) -> tick(s) ->
    // onTickEnd -> leitura. beginFrame só expira o latch se houve tick.
    in.beginFrame();
    assert(!in.held(Action::Right) && !in.pressed(Action::Right));

    // seta direita: edge visível até tick + beginFrame
    in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::Right));
    assert(in.held(Action::Right) && in.pressed(Action::Right));
    in.onTickEnd();
    in.beginFrame(); // tick observou: edge expira, held fica
    assert(in.held(Action::Right) && !in.pressed(Action::Right));

    // WASD secundário aciona a mesma ação (frame separado = edge novo)
    in.handleEvent(keyEvent(sf::Event::KeyReleased, sf::Keyboard::Right));
    in.onTickEnd();
    in.beginFrame();
    assert(!in.held(Action::Right));
    in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::D));
    assert(in.held(Action::Right) && in.pressed(Action::Right));
    assert(in.axisX() == 1.f);
    in.onTickEnd();
    in.beginFrame();
    assert(in.held(Action::Right) && !in.pressed(Action::Right));

    // solta: released dispara até o próximo beginFrame
    in.handleEvent(keyEvent(sf::Event::KeyReleased, sf::Keyboard::D));
    assert(!in.held(Action::Right) && in.released(Action::Right));
    in.onTickEnd();
    in.beginFrame();
    assert(!in.released(Action::Right));

    // eixo combinado zera
    in.onTickEnd();
    in.beginFrame();
    in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::Left));
    in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::Right));
    assert(in.axisX() == 0.f);

    // setEnabled(false) zera tudo (debug console)
    in.setEnabled(false);
    assert(!in.held(Action::Left) && !in.pressed(Action::Left) && !in.released(Action::Left));
    assert(in.axisX() == 0.f);
    in.setEnabled(true);

    // rebind limpa estado da ação
    in.onTickEnd();
    in.beginFrame();
    in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::J));
    assert(in.held(Action::Light));
    in.bind(Action::Light, sf::Keyboard::X);
    assert(!in.held(Action::Light));

    // consume mata o edge no mesmo frame (N ticks, 1 disparo)
    in.onTickEnd();
    in.beginFrame();
    in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::T));
    assert(in.pressed(Action::ArrangeAll));
    in.consume(Action::ArrangeAll);
    assert(!in.pressed(Action::ArrangeAll));
    assert(!in.held(Action::ArrangeAll));
    in.onTickEnd();
    in.beginFrame(); // próximo frame sem evento: nada fantasma
    assert(!in.pressed(Action::ArrangeAll));

    // ciclo completo do menu (E abre 1x: aperto -> consume -> release)
    in.onTickEnd();
    in.beginFrame();
    in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::E));
    assert(in.pressed(Action::ToggleInventory));
    in.consume(Action::ToggleInventory);
    assert(!in.pressed(Action::ToggleInventory));
    in.handleEvent(keyEvent(sf::Event::KeyReleased, sf::Keyboard::E));
    in.onTickEnd();
    in.beginFrame();
    assert(!in.held(Action::ToggleInventory));
    in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::E));
    assert(in.pressed(Action::ToggleInventory)); // aperto novo funciona

    // Toque em frame SEM tick sobrevive (60fps vs tick 30Hz: metade dos
    // frames não roda tick; sem latch o toque morria sem ser lido).
    in.onTickEnd();
    in.beginFrame();
    in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::U));
    // frame sem tick: beginFrame preserva, edge continua vivo
    in.beginFrame();
    assert(in.pressed(Action::UseItem));
    in.onTickEnd(); // agora sim houve tick: expira no próximo frame
    in.beginFrame();
    assert(!in.pressed(Action::UseItem));

    std::printf("inputmap test OK\n");
    return 0;
}
