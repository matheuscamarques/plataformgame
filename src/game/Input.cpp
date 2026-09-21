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
        }

        input_.handleEvent(event);
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
}
