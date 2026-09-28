/**
 * @file src/game/CharacterCreationScreen.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Criação de personagem: nome + classe + COMEÇAR.
 * @details Linhas Nome, 6 classes e COMEÇAR; digitação por TextEntered
 * (só na linha Nome), Backspace apaga, Enter seleciona/confirma.
 * Lógica pura (sem GL, testável headless); preview da classe à direita.
 * Dirigida por InputMap com consume, incluída por App.
 */

#pragma once

#include <string>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "core/PlayerClass.h"
#include "support/Input/InputMap.h"

namespace game {
namespace creation {

// COMEÇAR entrega nome+classe; Voltar retorna ao menu.
enum class Action : uint8_t { None, Done, Back };

class CharacterCreationScreen {
public:
    static constexpr int kNameRow = 0;
    static constexpr int kClassFirst = 1;
    static constexpr int kClassCount =
        static_cast<int>(core::PlayerClass::COUNT);
    static constexpr int kStartRow = kClassFirst + kClassCount;
    static constexpr int kRowCount = kStartRow + 1;
    static constexpr std::size_t kMaxName = 12;

    void reset(); // volta ao inicial (Cavaleiro, nome vazio)

    // Up/Down/UseItem/Pause com consume. Enter na classe seleciona e
    // pula p/ COMEÇAR; Enter no Nome pula p/ COMEÇAR; Enter no
    // COMEÇAR conclui. Esc volta ao menu.
    void handleInput(support::InputMap &in);
    // Texto digitado (TextEntered) e Backspace, via pollEvents.
    void handleText(std::uint32_t unicode);
    void handleBackspace();

    Action consumeAction();

    int cursor() const { return cursor_; }
    const std::string &name() const { return name_; }
    core::PlayerClass klass() const { return selected_; }

    void render(sf::RenderTarget &target, const sf::Font &font, float sw,
                float sh) const;

private:
    int cursor_ = kNameRow; // começa no Nome (digita direto)
    std::string name_;
    core::PlayerClass selected_ = core::PlayerClass::Knight;
    Action pending_ = Action::None;
};

} // namespace creation
} // namespace game
