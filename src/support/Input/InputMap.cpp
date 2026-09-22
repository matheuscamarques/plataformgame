#include "InputMap.h"

namespace support {

InputMap::InputMap() {
    // Defaults = comportamento atual do jogo (setas + Space),
    // mais WASD/LShift como secundários.
    bind(Action::Left,    sf::Keyboard::Left);
    bind(Action::Right,   sf::Keyboard::Right);
    bind(Action::Up,      sf::Keyboard::Up);
    bind(Action::Down,    sf::Keyboard::Down);
    bind(Action::Jump,    sf::Keyboard::Space);
    bind(Action::Roll,    sf::Keyboard::LShift);
    bind(Action::RunFast, sf::Keyboard::Space);
    bind(Action::Light,   sf::Keyboard::J);
    bind(Action::Heavy,   sf::Keyboard::K);
    bind(Action::Pause,   sf::Keyboard::Escape);
    bind(Action::Restart, sf::Keyboard::R);
    bind(Action::CycleMaterial, sf::Keyboard::M);
    bind(Action::ToggleDebug, sf::Keyboard::F1);
    bind(Action::ToggleCharView, sf::Keyboard::F3);
    bind(Action::ScreenshotNow, sf::Keyboard::F12);
    bind(Action::ToggleAutoMelee, sf::Keyboard::F11);
    bind(Action::ToggleAutoHurt, sf::Keyboard::F10);
    bind(Action::ToggleHitboxes, sf::Keyboard::F2);
    bind(Action::ToggleAi, sf::Keyboard::F5);
    bind(Action::ToggleEvents, sf::Keyboard::F4);
    bind(Action::ToggleWorld, sf::Keyboard::F6);
    bind(Action::ToggleFileLog, sf::Keyboard::F9);
    bind(Action::ToggleLightMask, sf::Keyboard::F7);
    bind(Action::Hotbar1, sf::Keyboard::Num1);
    bind(Action::Hotbar2, sf::Keyboard::Num2);
    bind(Action::Hotbar3, sf::Keyboard::Num3);
    bind(Action::Hotbar4, sf::Keyboard::Num4);
    bind(Action::Hotbar5, sf::Keyboard::Num5);
    bind(Action::ToggleInventory, sf::Keyboard::E);
    bind(Action::Interact, sf::Keyboard::F);
    bind(Action::TabLeft, sf::Keyboard::Q);
    bind(Action::TabRight, sf::Keyboard::R);
    bind(Action::SubTabLeft, sf::Keyboard::A);
    bind(Action::SubTabRight, sf::Keyboard::D);
    bind(Action::FirstSlot, sf::Keyboard::Home);
    bind(Action::LastSlot, sf::Keyboard::End);
    bind(Action::ArrangeAll, sf::Keyboard::T);
    bind(Action::UseItem, sf::Keyboard::U);
    addBind(Action::UseItem, sf::Keyboard::Return);
    addBind(Action::TabRight, sf::Keyboard::Tab); // hábito antigo: Tab avança

    addBind(Action::Left,  sf::Keyboard::A);
    addBind(Action::Right, sf::Keyboard::D);
    addBind(Action::Up,    sf::Keyboard::W);
    addBind(Action::Down,  sf::Keyboard::S);
    addBind(Action::RunFast, sf::Keyboard::LShift);
}

void InputMap::bind(Action a, sf::Keyboard::Key k) {
    if (!indexValid(a)) return;
    keys_[static_cast<std::size_t>(a)].clear();
    keys_[static_cast<std::size_t>(a)].push_back(k);
    clearState(a);
}

void InputMap::addBind(Action a, sf::Keyboard::Key k) {
    if (!indexValid(a)) return;
    auto &v = keys_[static_cast<std::size_t>(a)];
    for (auto old : v)
        if (old == k) return;
    v.push_back(k);
}

void InputMap::clearState(Action a) {
    auto i = static_cast<std::size_t>(a);
    curr_[i] = false;
    prev_[i] = false;
}

static bool anyPressed(const std::vector<sf::Keyboard::Key> &keys) {
    for (auto k : keys)
        if (sf::Keyboard::isKeyPressed(k)) return true;
    return false;
}

void InputMap::handleEvent(const sf::Event& e) {
    if (e.type != sf::Event::KeyPressed && e.type != sf::Event::KeyReleased)
        return;
    bool down = (e.type == sf::Event::KeyPressed);
    for (std::size_t i = 0; i < kCount; ++i) {
        for (auto k : keys_[i]) {
            if (k == e.key.code) {
                curr_[i] = down;
                break;
            }
        }
    }
}

void InputMap::beginFrame() {
    if (pollingMode_) {
        for (std::size_t i = 0; i < kCount; ++i) {
            curr_[i] = anyPressed(keys_[i]);
        }
    }
    prev_ = curr_;
}

bool InputMap::held(Action a) const {
    if (!enabled_ || !indexValid(a)) return false;
    return curr_[static_cast<std::size_t>(a)];
}

bool InputMap::pressed(Action a) const {
    if (!enabled_ || !indexValid(a)) return false;
    auto i = static_cast<std::size_t>(a);
    return curr_[i] && !prev_[i];
}

bool InputMap::released(Action a) const {
    if (!enabled_ || !indexValid(a)) return false;
    auto i = static_cast<std::size_t>(a);
    return !curr_[i] && prev_[i];
}

float InputMap::axisX() const {
    float x = 0.f;
    if (held(Action::Left))  x -= 1.f;
    if (held(Action::Right)) x += 1.f;
    return x;
}

float InputMap::axisY() const {
    float y = 0.f;
    if (held(Action::Up))   y -= 1.f;
    if (held(Action::Down)) y += 1.f;
    return y;
}

} // namespace support
