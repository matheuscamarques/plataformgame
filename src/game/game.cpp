#include <iostream>
#include <chrono>
#include <cmath>
#include "./game.h"
#include "../core/Log.h"
#include "../core/Time.h"
#include "../window/window.h"
#include "../entities/player/player.h"

namespace {
constexpr uint32_t WORLD_SEED = 1337u;
}



Game::Game()
{
}


void Game::main()
{
    auto game = std::make_unique<Game>();
    //game->view = new sf::View(sf::FloatRect(0.f, 0.f, 1000.f, 600.f));
    auto world = std::make_unique<support::World>(WORLD_SEED);
    game->player = std::make_unique<Player>();
    game->setWorld(std::move(world));

    if (!game->font.loadFromFile("./arial.ttf"))
    {
        throw std::runtime_error("Could not load font");
    }
    LOG_INFO("Game", "boot ok, seed=" << WORLD_SEED);

    // add border font

    Window window(800, 800, "Game", game.get());
}

void Game::setWindow(sf::RenderWindow *window)
{
     viewW_ = static_cast<float>(window->getSize().x);
     viewH_ = static_cast<float>(window->getSize().y);

    //window->setView(*this->view);
    this->window = window;
}

void Game::start()
{
    if (running)
    {
        return;
    }
    running = true;
    //m_thread.launch();
}

void Game::run()
{
    // Preserva os 30 TPS do loop original (Time default é 1/60).
    core::Time::setFixedStep(1.0f / 30.0f);
    float lastStat = 0.0f;
    int frames = 0;
    int updates = 0;

    while (running && window->isOpen())
    {
        // Ordem: beginFrame ANTES do pollEvent, senão o edge morre
        // antes do primeiro tick ler (e há N ticks por frame).
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
        core::Time::beginFrame();
        int ticks = core::Time::consumeTicks();
        for (int i = 0; i < ticks; i++)
        {
            tick();
            updates++;
        }
        render();
        frames++;

        if (core::Time::elapsed() - lastStat >= 1.0f)
        {
            lastStat = core::Time::elapsed();
            updates = 0;
            frames = 0;
        }

    }
}

void Game::render()
{

    window->clear(sf::Color(135, 206, 235));
    camera.setViewport(viewW_, viewH_);
    camera.follow(player.get()->getX(), player.get()->getY());
    sf::Vector2f camPos = camera.position();
    auto view = window->getDefaultView();
    view.move(camPos.x, camPos.y);
    window->setView(view);
    auto &objects = getWorld()->getPlatforms();

    // Desenha só o visível (+margem); sem quadtree no caminho.
    float vx0 = camPos.x - 60.0f, vy0 = camPos.y - 60.0f;
    float vx1 = camPos.x + viewW_ + 60.0f, vy1 = camPos.y + viewH_ + 60.0f;
    for(Entity *entity : objects){
        if (entity->getX() + entity->getW() < vx0 || entity->getX() > vx1 ||
            entity->getY() + entity->getH() < vy0 || entity->getY() > vy1) continue;
        entity->draw(window);
    }

    player.get()->draw(window);

    overlay_.render(*window, font, *getWorld(), *player.get(), objects.size());

    window->display();
}

void Game::tick() {
    Player *p = player.get();
    p->moveRight = input_.held(support::Action::Right);
    p->moveUp    = input_.held(support::Action::Up);
    p->moveDown  = input_.held(support::Action::Down);
    p->moveLeft  = input_.held(support::Action::Left);
    p->runFast   = input_.held(support::Action::RunFast);
    p->tick();

    // Mundo infinito: carrega/descarrega chunks em torno do tile do player.
    int playerTileX = static_cast<int>(std::floor(p->getX() / BLOCK_SIZE));
    int playerTileY = static_cast<int>(std::floor(p->getY() / BLOCK_SIZE));
    getWorld()->update(playerTileX, playerTileY);

    // Consulta o hash ao redor do player (1 tile de margem).
    // Inclui água de propósito: Player::collide usa WATER para natação.
    std::vector<Entity*> candidatos;
    getWorld()->query(
        p->getX() - BLOCK_SIZE,
        p->getY() - BLOCK_SIZE,
        p->getW() + BLOCK_SIZE * 2,
        p->getH() + BLOCK_SIZE * 2,
        candidatos);

    for (Entity *e : candidatos) {
        if (p->isColide(*e)) {
            p->collide(*e);
        }
    }
}

support::World* Game::getWorld() {
    return this->world.get();
}

void Game::setWorld(std::unique_ptr<support::World> world) {
    this->world = std::move(world);
}

Player* Game::getPlayer() {
    return this->player.get();
}
