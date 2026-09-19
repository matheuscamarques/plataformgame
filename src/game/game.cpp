#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>
#include "./game.h"
#include "../core/Log.h"
#include "../core/Time.h"
#include "../support/BodySystem.h"
#include "../support/World/Generation.h"
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
    // Spawn no flanco de montanha mais próximo (terra garantida):
    // cair do céu no meio do nada não mostra o jogo.
    int spawnTx = support::findSpawnTileX(0, WORLD_SEED);
    game->player->setX(spawnTx * BLOCK_SIZE);
    game->setWorld(std::move(world));

    if (!game->font.loadFromFile("./arial.ttf"))
    {
        throw std::runtime_error("Could not load font");
    }
    LOG_INFO("Game", "boot ok, seed=" << WORLD_SEED << " spawnTx=" << spawnTx);

    // Sistemas via scheduler; 2 slimes perto do spawn (determinístico).
    game->enemies_ = &game->scheduler_.add<support::EnemySystem>();
    game->scheduler_.add<support::BodySystem>();
    game->particles_ = &game->scheduler_.add<support::ParticleSystem>();
    game->throws_ = &game->scheduler_.add<support::ThrowSystem>();
    game->explodes_ = &game->scheduler_.add<support::ExplosionSystem>();
    game->throws_->setExplosionSystem(game->explodes_);
    game->throws_->setParticleSystem(game->particles_);
    game->explodes_->setParticleSystem(game->particles_);
    game->enemies_->spawn("slime", (spawnTx - 6) * BLOCK_SIZE, 0.0f);
    game->enemies_->spawn("slime", (spawnTx + 6) * BLOCK_SIZE, 0.0f);

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

    enemies_->forEach([&](support::Slime &s) { s.body.draw(window); });

    // Throwables visíveis: círculo com cor pelo fuse (verde→vermelho).
    throws_->forEachActive([&](const support::Throwable &t) {
        sf::CircleShape c(3.f);
        c.setOrigin(3.f, 3.f);
        c.setPosition(t.pos);
        if (t.kind == support::ThrowKind::Dynamite) {
            float r = std::clamp(t.fuse / 1.0f, 0.f, 1.f); // 1 cheio → 0 explodindo
            c.setFillColor(sf::Color(
                static_cast<sf::Uint8>(255 - 155 * r),
                static_cast<sf::Uint8>(80 + 120 * r),
                60));
        } else {
            c.setFillColor(sf::Color(200, 180, 60));
        }
        window->draw(c);
    });

    particles_->render(*window);

    // Debug draw das hitboxes por parte (só com overlay ligado).
    if (overlay_.visible()) {
        auto drawParts = [&](const support::Body &b) {
            b.forEach([&](const support::PartState &st, const support::PartDef &) {
                sf::RectangleShape r(sf::Vector2f(st.worldBox.width, st.worldBox.height));
                r.setPosition(st.worldBox.left, st.worldBox.top);
                r.setFillColor(sf::Color::Transparent);
                r.setOutlineColor(sf::Color::Magenta);
                r.setOutlineThickness(1.f);
                window->draw(r);
            });
        };
        drawParts(player.get()->body);
        enemies_->forEach([&](support::Slime &s) { drawParts(s.bodyParts); });
    }

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
    if (p->moveLeft && !p->moveRight) p->facing = -1;
    if (p->moveRight && !p->moveLeft) p->facing = 1;
    p->tick();

    // S6: J (Action::Light, sem uso até aqui) arremessa dinamite.
    // Cooldown cobre o edge por frame: pressed fica alto em todos os
    // ticks do frame, o 2º tick já encontra cooldown rodando.
    p->throwCooldown.tick(1.0f / 30.0f);
    if (input_.pressed(support::Action::Light)) p->tryThrow(*throws_);

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

    // Sistemas (inimigos etc.): scheduler com prioridade declarada.
    // BodySystem reconstrói hitboxes pós-movimento (priority 250).
    // Alvos de explosão: montados aqui (Player + Slimes), lidos pelo
    // ExplosionSystem durante o tick do ThrowSystem.
    std::vector<support::ExplosionTarget> targets;
    targets.push_back({sf::Vector2f(p->getCenterX(), p->getCenterY()),
                       &p->body, nullptr, true});
    enemies_->forEach([&](support::Slime &s) {
        if (s.resources.isDead()) return;
        targets.push_back({sf::Vector2f(s.body.getCenterX(), s.body.getCenterY()),
                           &s.bodyParts, &s.resources, false});
    });
    support::GameContext ctx{getWorld(), p, &input_, enemies_,
                             throws_, explodes_, &targets};
    scheduler_.tick(1.0f / 30.0f, ctx);
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
