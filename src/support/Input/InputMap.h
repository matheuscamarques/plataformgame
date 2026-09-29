/**
 * @file src/support/Input/InputMap.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara ações lógicas e interface de estado do teclado.
 * @details Define enum Action com movimento, combate, pause, debug, hotbar e inventário mais classe InputMap com bind e consulta, incluída por Game, Player e UIs.
 */

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
    Pause, ToggleDebug, Restart, ToggleCharView,
    ScreenshotNow, ToggleAutoMelee, ToggleAutoHurt, // F12, F11, F10
    ToggleHitboxes, ToggleAi, ToggleEvents, ToggleWorld, // F2,F5,F4,F6
    ToggleFileLog, // F9: log de eventos em logs/debug.log
    ToggleLightMask, // F7: máscara do raycast de luz
    SwooshCycle,     // F8: alterna swoosh windup/impacto (debug A/B)
    HitstopCycle,    // [: alterna override de hitstop (debug A/B)
    Hotbar1, Hotbar2, Hotbar3, Hotbar4, Hotbar5, // fase 4a: 1-5
    ToggleInventory, // fase 4b: E abre/fecha grid
    Interact,        // F: abre/executa menu de ação (com grid aberto)
    TabLeft,         // Q: tab principal anterior
    TabRight,        // Tab: próxima tab principal
    SubTabLeft,      // A: sub-tab anterior (só em Inventory)
    SubTabRight,     // D: próxima sub-tab
    FirstSlot,       // Home: primeiro slot
    LastSlot,        // End: último slot
    Cast,            // G: conjura a 1ª magia sintonizada (F8)
    ArrangeAll,      // T: reordena tudo (atalho, também no menu)
    UseItem,         // U/Enter: usa o selecionado (atalho, também no menu)
    CycleLeftHand,   // Z: troca arma da mão esquerda (fora de menu)
    CycleRightHand,  // X: troca arma da mão direita (fora de menu)
    CycleSpell,      // C: próxima magia sintonizada (fora de menu)
    CycleItem,       // V: próximo slot da hotbar (fora de menu)
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
    // Latch: se nenhum tick rodou desde o último beginFrame (frame com
    // 0 ticks — comum a 60fps com tick 30Hz), o edge é preservado p/ o
    // próximo frame em vez de morrer sem ser lido. onTickEnd() marca.
    void beginFrame();

    // Game::tick chama no fim de cada tick: houve observação dos edges.
    // Sem isso, beginFrame preserva o latch (toque em frame sem tick
    // nunca se perde).
    void onTickEnd() { ticksRan_ = true; }

    // Consultas.
    bool held    (Action a) const;
    bool pressed (Action a) const; // latch: até consume ou frame pós-tick
    bool released(Action a) const; // edge: desceu neste tick

    // Consome o edge: pressed() volta a false até o próximo aperto real.
    // Evita duplo-disparo com N ticks por frame (fixed-step): quem consome
    // chama 1x e os ticks seguintes do frame não repetem.
    void consume(Action a) {
        if (!indexValid(a)) return;
        curr_[static_cast<std::size_t>(a)] = false;
        latch_[static_cast<std::size_t>(a)] = false;
    }

    // Tecla física (criação de personagem): navega por setas/Enter/Esc
    // sem passar pelos binds — letras digitadas (WASD...) nunca movem
    // o cursor. Mesmo latch de pressed(), mesmo beginFrame/onTickEnd.
    bool pressedKey(sf::Keyboard::Key k) const;
    void consumeKey(sf::Keyboard::Key k);

    // Zera tudo (transições splash/menu/criação/gameplay): nenhum edge
    // velho vaza (Enter do splash não confirma o menu, 'E' digitado
    // não abre o inventário no spawn, etc).
    void clearAll();

    // Zera só os edges (latches), preservando held: a criação chama no
    // fim de todo frame — nenhuma tecla digitada vaza p/ o gameplay
    // (nem p/ o próximo consumidor), e segurar tecla continua valendo.
    void clearEdges();

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
    static constexpr std::size_t kKeys =
        static_cast<std::size_t>(sf::Keyboard::KeyCount);

    static std::size_t keyIndex(sf::Keyboard::Key k) {
        return static_cast<std::size_t>(k);
    }
    bool keyValid(sf::Keyboard::Key k) const {
        return keyIndex(k) < kKeys;
    }

    std::array<std::vector<sf::Keyboard::Key>, kCount> keys_{};
    std::array<bool, kCount> curr_{};
    std::array<bool, kCount> prev_{};
    std::array<bool, kCount> latch_{}; // edge pegajoso até tick/consume
    std::array<bool, kKeys> rawCurr_{};
    std::array<bool, kKeys> rawPrev_{};
    std::array<bool, kKeys> rawLatch_{}; // física: mesma regra do latch_

    bool pollingMode_ = false;
    bool enabled_     = true;
    bool ticksRan_    = false; // algum tick rodou desde o beginFrame

    bool indexValid(Action a) const {
        return static_cast<std::size_t>(a) < kCount;
    }
    void clearState(Action a);
};

} // namespace support
