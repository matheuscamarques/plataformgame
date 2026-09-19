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



Game::Game() : Component()
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
     setH(window->getSize().y);
     setW(window->getSize().x);

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
        }
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
    //view->setCenter(player.get()->getX(), player.get()->getY());
    camera.setViewport(this->getW(), this->getH());
    camera.follow(player.get()->getX(), player.get()->getY());
    sf::Vector2f camPos = camera.position();
    auto view = window->getDefaultView();
    view.move(camPos.x, camPos.y);
    //view.zoom(-10.0f);
    window->setView(view);
    auto &objects = getWorld()->getPlatforms();
    // draw total platforms text
    sf::Text totalPlataformsTxt;
    totalPlataformsTxt.setFont(font);
    totalPlataformsTxt.setString("Total Platforms: " + std::to_string(objects.size()));
    totalPlataformsTxt.setCharacterSize(20);
    totalPlataformsTxt.setFillColor(sf::Color::Green);
    totalPlataformsTxt.setPosition(player.get()->getX(), player.get()->getY() - 200);
    totalPlataformsTxt.setOutlineColor(sf::Color::Black);
    totalPlataformsTxt.setOutlineThickness(1);

    window->draw(totalPlataformsTxt);

    // Desenha só o visível (+margem); sem quadtree no caminho.
    float vx0 = camPos.x - 60.0f, vy0 = camPos.y - 60.0f;
    float vx1 = camPos.x + this->getW() + 60.0f, vy1 = camPos.y + this->getH() + 60.0f;
    for(Entity *entity : objects){
        if (entity->getX() + entity->getW() < vx0 || entity->getX() > vx1 ||
            entity->getY() + entity->getH() < vy0 || entity->getY() > vy1) continue;
        entity->draw(window);
    }

    player.get()->draw(window);

    // --- Debug do SpatialHash: query + células + contagens ---
    Player *p = player.get();
    float qx = p->getX() - BLOCK_SIZE;
    float qy = p->getY() - BLOCK_SIZE;
    float qw = p->getW() + BLOCK_SIZE * 2;
    float qh = p->getH() + BLOCK_SIZE * 2;

    sf::RectangleShape queryRect(sf::Vector2f(qw, qh));
    queryRect.setPosition(qx, qy);
    queryRect.setFillColor(sf::Color(0, 255, 0, 30));
    queryRect.setOutlineColor(sf::Color::Green);
    queryRect.setOutlineThickness(1.f);
    window->draw(queryRect);

    const float cs = support::Chunk::HASH_CELL;
    std::vector<std::pair<int,int>> cells;
    getWorld()->debugCells(qx, qy, qw, qh, cells);
    for (auto &cell : cells) {
        float cx = cell.first * cs;
        float cy = cell.second * cs;

        sf::RectangleShape cellRect(sf::Vector2f(cs, cs));
        cellRect.setPosition(cx, cy);
        cellRect.setFillColor(sf::Color::Transparent);
        cellRect.setOutlineColor(sf::Color(64, 128, 255));
        cellRect.setOutlineThickness(1.f);
        window->draw(cellRect);

        sf::Text t;
        t.setFont(font);
        t.setString(std::to_string(getWorld()->debugCellCount(cell.first, cell.second)));
        t.setCharacterSize(12);
        t.setFillColor(sf::Color::Yellow);
        t.setOutlineColor(sf::Color::Black);
        t.setOutlineThickness(1);
        t.setPosition(cx + 4, cy + 4);
        window->draw(t);
    }

    std::vector<Entity*> candidatos;
    getWorld()->query(qx, qy, qw, qh, candidatos);
    totalCandidatesSeen = candidatos.size();

    // draw point
//    sf::CircleShape shape(5.f);
//    shape.setFillColor(sf::Color::Green);
//    shape.setPosition(player.get()->getCenterX(), player.get()->getCenterY());
//    window->draw(shape);

    // draw point
//    sf::CircleShape shape2(5.f);
//    shape2.setFillColor(sf::Color::Green);
//    shape2.setPosition(player.get()->getX(), player.get()->getY());
//    window->draw(shape2);

    // draw text im top player
    sf::Text text;

    text.setString("HASH: " + std::to_string(totalCandidatesSeen));
    text.setCharacterSize(20);
    text.setFont(font);
    text.setFillColor(sf::Color::Green);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(1);
    text.setPosition(player.get()->getX() - player.get()->getW()/2, player.get()->getY() - player.get()->getH()/2);
    window->draw(text);

    // DRAW CIRCLE
//    sf::CircleShape Q1(10);
//    Q1.setFillColor(sf::Color::Green);
//    Q1.setPosition(player.get()->getCenterX() - 50, player.get()->getCenterY() - 50);
//    window->draw(Q1);
//
//    sf::CircleShape Q2(10);
//    Q2.setFillColor(sf::Color::Green);
//    Q2.setPosition(player.get()->getCenterX(), player.get()->getCenterY() - 50);
//    window->draw(Q2);
//
//    sf::CircleShape Q3(10);
//    Q3.setFillColor(sf::Color::Green);
//    Q3.setPosition(player.get()->getCenterX() - 50, player.get()->getCenterY());
//    window->draw(Q3);
//
//    sf::CircleShape Q4(10);
//    Q4.setFillColor(sf::Color::Green);
//    Q4.setPosition(player.get()->getCenterX(), player.get()->getCenterY());
//    window->draw(Q4);

    window->display();
}

void Game::tick() {
    Player *p = player.get();
    p->moveRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    p->moveUp    = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    p->moveDown  = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    p->moveLeft  = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    p->runFast   = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
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
