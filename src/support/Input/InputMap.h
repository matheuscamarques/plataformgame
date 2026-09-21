#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <array>
#include <cstdint>
#include <vector>

namespace support {

enum class Action : uint8_t {
    Left, Right, Up, Down,
    Jump, Roll, RunFast,
    Light, Heavy,
    Pause, ToggleDebug, Restart, CycleMaterial, ToggleCharView,
    ScreenshotNow, ToggleAutoMelee, ToggleAutoHurt, // F12, F11, F10
    ToggleHitboxes, ToggleAi, ToggleEvents, ToggleWorld, // F2,F5,F4,F6
    COUNT
};

// InputMap é o único lugar do projeto que toca sf::Keyboard.
// Ninguém mais inclui <SFML/Window/Keyboard.hpp>.
//
// Event-driven: recebe sf::Event no pollEvent, mantém curr_[]/prev_[].
// beginFrame() copia curr_ -> prev_ no topo do tick; pressed/held/released
// derivam dos dois arrays. Cada ação aceita várias teclas (ex.: setas + WASD).
class InputMap {
public:
    InputMap();

    // Binds default. Rebind via bind()/addBind() quando houver menu de opções.
    // bind() troca a lista; addBind() acrescenta (limpa estado da ação).
    void bind(Action a, sf::Keyboard::Key k);
    void addBind(Action a, sf::Keyboard::Key k);

    // Event-driven: chamado dentro do pollEvent do Game.
    void handleEvent(const sf::Event& e);

    // Polling: lê sf::Keyboard::isKeyPressed para cada ação.
    // Use em teste headless ou quando não houver eventos (ex.: primeiro frame).
    // Em produção, prefira handleEvent + beginFrame.
    void setPollingMode(bool enabled) { pollingMode_ = enabled; }

    // Chamado uma vez no topo de cada FRAME, antes do pollEvent.
    // Copia curr_ -> prev_. Se pollingMode, re-lê curr_ do teclado.
    // NÃO chamar por tick: com fixed-step há N ticks por frame e a
    // segunda chamada apagaria o edge antes de ser lido.
    void beginFrame();

    // Consultas.
    bool held    (Action a) const;
    bool pressed (Action a) const; // edge: subiu neste tick
    bool released(Action a) const; // edge: desceu neste tick

    // Consome o edge: pressed() volta a false até o próximo aperto real.
    // Evita duplo-disparo com N ticks por frame (fixed-step): quem consome
    // (ex.: tecla M) chama 1x e os ticks seguintes do frame não repetem.
    void consume(Action a) {
        if (!indexValid(a)) return;
        curr_[static_cast<std::size_t>(a)] = false;
    }

    // Eixos para movimento e IA.
    // Retorna -1, 0 ou 1. Combina Left/Right e Up/Down.
    float axisX() const;
    float axisY() const;

    // Para debug console / UI: captura de teclado.
    // Quando false, held/pressed/released retornam sempre false.
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool enabled() const { return enabled_; }

private:
    static constexpr std::size_t kCount = static_cast<std::size_t>(Action::COUNT);

    std::array<std::vector<sf::Keyboard::Key>, kCount> keys_{};
    std::array<bool, kCount> curr_{};
    std::array<bool, kCount> prev_{};

    bool pollingMode_ = false;
    bool enabled_     = true;

    bool indexValid(Action a) const {
        return static_cast<std::size_t>(a) < kCount;
    }
    void clearState(Action a);
};

} // namespace support
