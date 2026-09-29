/**
 * @file src/game/Input.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Processa eventos SFML e atalhos de debug por frame.
 * @details Implementa pollEvents que drena eventos, trata fechar e resize, atualiza InputMap e alterna overlay, inventário e screenshots, chamada por Game run antes do tick.
 */

#include "game.h"

#include "entities/Player/Player.h"

// Input: loop de eventos SFML + mapeamento de edges por frame.
// run() chama 1x por frame, antes dos ticks.

void Game::pollEvents() {
    input_.beginFrame();
    sf::Event event{};
    while (window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window->close();

        if (event.type == sf::Event::Resized) {
            // update the view to the new size of the window
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window->setView(sf::View(visibleArea));
            // Câmera/HUD/bloom usam viewW_/viewH_: acompanha o resize
            // (tela cheia via WM), senão tudo desenha no tamanho do boot.
            viewW_ = static_cast<float>(event.size.width);
            viewH_ = static_cast<float>(event.size.height);
        }

        input_.handleEvent(event);
        // Criação de personagem: texto e Backspace (menu consome o resto).
        if (creationActive_ && !menuDone_) {
            if (event.type == sf::Event::TextEntered) {
                creation_.handleText(event.text.unicode);
            } else if (event.type == sf::Event::KeyPressed &&
                       event.key.code == sf::Keyboard::BackSpace) {
                creation_.handleBackspace();
            }
        }
    }
    if (input_.pressed(support::Action::ToggleDebug)) overlay_.toggle();
    if (input_.pressed(support::Action::ToggleCharView)) charView_ = !charView_;
    if (input_.pressed(support::Action::ScreenshotNow)) {
        screenshots_.setFocus(
            {player->getCenterX(), player->getCenterY()});
        screenshots_.capture("manual");
    }
    if (input_.pressed(support::Action::ToggleAutoMelee))
        screenshots_.setAutoMelee(!screenshots_.autoMelee());
    if (input_.pressed(support::Action::ToggleAutoHurt))
        screenshots_.setAutoHurt(!screenshots_.autoHurt());
    if (input_.pressed(support::Action::ToggleHitboxes))
        overlay_.toggleHitboxes();
    if (input_.pressed(support::Action::ToggleLightMask))
        overlay_.toggleLightMask();
    if (input_.pressed(support::Action::ToggleAi)) overlay_.toggleAi();
    if (input_.pressed(support::Action::ToggleEvents))
        overlay_.toggleEvents();
    if (input_.pressed(support::Action::ToggleWorld))
        overlay_.toggleWorld();
    if (input_.pressed(support::Action::ToggleFileLog)) {
        debugFeed_.setFileEnabled(!debugFeed_.fileEnabled());
        debugFeed_.pushLog(debugFeed_.fileEnabled() ? "filelog on"
                                                    : "filelog off");
    }
    // Debug A/B de playtest (sem re recompilar): F8 cicla o swoosh
    // (ambos → só windup → só impacto), [ cicla override de hitstop
    // (por arma → 3 → 5 → 7 slots). Log no feed (F4) p/ comparar.
    if (input_.pressed(support::Action::SwooshCycle)) {
        input_.consume(support::Action::SwooshCycle);
        swooshMode_ = (swooshMode_ + 1) % 3;
        debugFeed_.pushLog(std::string("swoosh ") +
                           (swooshMode_ == 0
                                ? "windup+impact"
                                : (swooshMode_ == 1 ? "windup" : "impact")));
    }
    if (input_.pressed(support::Action::HitstopCycle)) {
        input_.consume(support::Action::HitstopCycle);
        hitstopOverride_ =
            (hitstopOverride_ == 0)
                ? 3
                : (hitstopOverride_ >= 7 ? 0 : hitstopOverride_ + 2);
        debugFeed_.pushLog(std::string("hitstop ") +
                           (hitstopOverride_ == 0
                                ? "por arma"
                                : std::to_string(hitstopOverride_) +
                                      " slots"));
    }
}
