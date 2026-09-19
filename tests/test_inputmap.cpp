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

    // Protocolo: beginFrame (topo) -> handleEvent (poll) -> leitura.
    in.beginFrame();
    assert(!in.held(Action::Right) && !in.pressed(Action::Right));

    // seta direita: edge visível até o próximo beginFrame
    in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::Right));
    assert(in.held(Action::Right) && in.pressed(Action::Right));
    in.beginFrame(); // próximo frame, sem evento novo: held sim, pressed não
    assert(in.held(Action::Right) && !in.pressed(Action::Right));

    // WASD secundário aciona a mesma ação (frame separado = edge novo)
    in.handleEvent(keyEvent(sf::Event::KeyReleased, sf::Keyboard::Right));
    in.beginFrame();
    assert(!in.held(Action::Right));
    in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::D));
    assert(in.held(Action::Right) && in.pressed(Action::Right));
    assert(in.axisX() == 1.f);
    in.beginFrame();
    assert(in.held(Action::Right) && !in.pressed(Action::Right));

    // solta: released dispara até o próximo beginFrame
    in.handleEvent(keyEvent(sf::Event::KeyReleased, sf::Keyboard::D));
    assert(!in.held(Action::Right) && in.released(Action::Right));
    in.beginFrame();
    assert(!in.released(Action::Right));

    // eixo combinado zera
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
    in.beginFrame();
    in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::J));
    assert(in.held(Action::Light));
    in.bind(Action::Light, sf::Keyboard::X);
    assert(!in.held(Action::Light));

    std::printf("inputmap test OK\n");
    return 0;
}
