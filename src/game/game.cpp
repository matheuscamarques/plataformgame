#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>
#include "./game.h"
#include "../core/Log.h"
#include "../core/Time.h"
#include "../support/BodySystem.h"
#include "../support/DwarfAI.h"
#include "../support/World/Generation.h"
#include "../support/World/Stratum.h"
#include "PlayerSprite.h"
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
    game->stratum_ = &game->scheduler_.add<support::StratumManager>();
    game->run_.setStratumManager(game->stratum_);
    game->enemies_ = &game->scheduler_.add<support::EnemySystem>();
    game->scheduler_.add<support::BodySystem>();
    game->particles_ = &game->scheduler_.add<support::ParticleSystem>();
    game->throws_ = &game->scheduler_.add<support::ThrowSystem>();
    game->explodes_ = &game->scheduler_.add<support::ExplosionSystem>();
    game->deaths_ = &game->scheduler_.add<support::DeathSystem>();
    game->melee_ = &game->scheduler_.add<support::MeleeSystem>();
    game->scheduler_.add<support::ContactDamageSystem>();
    game->drops_ = &game->scheduler_.add<support::DropSystem>();
    game->scheduler_.add<support::SpawnSystem>(); // spawn contínuo (sem wiring)
    game->throws_->setExplosionSystem(game->explodes_);
    game->throws_->setParticleSystem(game->particles_);
    game->explodes_->setParticleSystem(game->particles_);
    game->deaths_->setDropSystem(game->drops_);
    game->deaths_->setParticleSystem(game->particles_);
    game->melee_->setParticleSystem(game->particles_);
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
    // Sprites 1x: janela aberta = contexto GL vivo (nunca em teste).
    if (!spritesBuilt_) {
        sprites_ = sprites::build();
        spritesBuilt_ = true;
    }
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

    // Desenha só o visível (+margem), por range de chunks — não pela
    // lista global (chunks modified pinned não encarecem o frame).
    float vx0 = camPos.x - 60.0f, vy0 = camPos.y - 60.0f;
    float vx1 = camPos.x + viewW_ + 60.0f, vy1 = camPos.y + viewH_ + 60.0f;
    // Fundo chapado do estrato do player (1 draw; pop na fronteira
    // marca a transição de propósito).
    {
        const int pty = static_cast<int>(std::floor(player.get()->getY() / BLOCK_SIZE));
        const support::StratumBg bg =
            support::stratumBg(support::stratumAt(pty));
        sf::RectangleShape bgRect(sf::Vector2f(vx1 - vx0, vy1 - vy0));
        bgRect.setPosition(vx0, vy0);
        bgRect.setFillColor(sf::Color(bg.r, bg.g, bg.b));
        window->draw(bgRect);
    }
    getWorld()->forEachEntityInRect(vx0, vy0, vx1, vy1, [&](Entity *entity) {
        entity->draw(window);
    });

    drawPlayerSprite();
    drawEnemiesSprites();

    // Barks com fade 1.5s acima da cabeça (texto; áudio futuro).
    enemies_->forEach([&](support::Enemy &s) {
        if (s.barkTimer <= 0.f || s.currentBark.empty()) return;
        sf::Text t;
        t.setFont(font);
        t.setString(s.currentBark);
        t.setCharacterSize(14);
        const sf::Uint8 a = static_cast<sf::Uint8>(
            255.f * std::max(0.f, std::min(1.f, s.barkTimer / 1.5f)));
        t.setFillColor(sf::Color(255, 240, 200, a));
        t.setOutlineColor(sf::Color(0, 0, 0, a));
        t.setOutlineThickness(1);
        t.setPosition(s.body.getX() - 20.f, s.body.getY() - 24.f);
        window->draw(t);
    });

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

    // Flash do swing: outline da hitbox só na janela Active.
    if (player.get()->meleePhase == MeleePhase::Active) {
        const sf::FloatRect box = player.get()->meleeHitbox();
        sf::RectangleShape r(sf::Vector2f(box.width, box.height));
        r.setPosition(box.left, box.top);
        r.setFillColor(sf::Color::Transparent);
        r.setOutlineColor(sf::Color::Yellow);
        r.setOutlineThickness(1.f);
        window->draw(r);
    }

    drops_->render(*window);

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
        enemies_->forEach([&](support::Enemy &s) { drawParts(s.bodyParts); });
    }

    overlay_.render(*window, font, *getWorld(), *player.get(), objects.size());

    // HUD em espaço de tela (view default): HP, TNT, estrato, morte/pause.
    // Tosco de propósito; HUD bonito é polimento.
    {
        window->setView(window->getDefaultView());
        Player *p = player.get();
        const float ratio = static_cast<float>(p->hp) / static_cast<float>(p->hpMax);

        sf::RectangleShape hpBg(sf::Vector2f(204.f, 20.f));
        hpBg.setPosition(16.f, 16.f);
        hpBg.setFillColor(sf::Color(40, 0, 0));
        window->draw(hpBg);
        sf::RectangleShape hpFg(sf::Vector2f(200.f * ratio, 16.f));
        hpFg.setPosition(18.f, 18.f);
        hpFg.setFillColor(sf::Color(200, 30, 30));
        window->draw(hpFg);

        auto text = [&](const std::string &s, float x, float y, int size = 18) {
            sf::Text t;
            t.setFont(font);
            t.setString(s);
            t.setCharacterSize(size);
            t.setFillColor(sf::Color::White);
            t.setOutlineColor(sf::Color::Black);
            t.setOutlineThickness(1);
            t.setPosition(x, y);
            window->draw(t);
        };
        text("HP " + std::to_string(p->hp) + "/" + std::to_string(p->hpMax), 16.f, 38.f);
        text("TNT:" + std::to_string(p->dynamiteCount) + " J  K melee", 16.f, 62.f);
        const int pty = static_cast<int>(std::floor(p->getY() / BLOCK_SIZE));
        text(std::string(support::stratumName(support::stratumAt(pty)))
             + "  y" + std::to_string(pty), 16.f, 86.f);

        if (run_.isDead()) {
            sf::RectangleShape dim(sf::Vector2f(viewW_, viewH_));
            dim.setFillColor(sf::Color(0, 0, 0, 160));
            window->draw(dim);
            text("VOCE MORREU", viewW_ * 0.5f - 110.f, viewH_ * 0.5f - 40.f, 36);
            text("R para renascer no checkpoint", viewW_ * 0.5f - 170.f, viewH_ * 0.5f + 10.f, 20);
        } else if (run_.isPaused()) {
            text("PAUSADO (ESC)", viewW_ * 0.5f - 110.f, viewH_ * 0.5f - 20.f, 28);
        }
    }

    window->display();
}

void Game::drawPlayerSprite() {
    Player *p = player.get();
    const sf::Texture *tex = game::pickPlayerFrame(
        p->jumping, p->getVx(), p->meleeAnimT > 0.f, p->throwAnimT > 0.f,
        sprites_, p->walkFrame);
    // Escala p/ altura da entidade (50px), aspecto preservado.
    const float s = p->getH() / static_cast<float>(sprites::kPlayerH);
    sf::Sprite spr;
    spr.setTexture(*tex);
    spr.setOrigin(sprites::kPlayerW * 0.5f, static_cast<float>(sprites::kPlayerH));
    spr.setPosition(p->getCenterX(), p->getY() + p->getH());
    spr.setScale(static_cast<float>(p->facing) * s, s);
    window->draw(spr);
}

void Game::drawEnemiesSprites() {
    enemies_->forEach([&](support::Enemy &s) {
        const sf::Texture *tex = &sprites_.slimeIdle;
        float sw = static_cast<float>(sprites::kSlimeW);
        float sh = static_cast<float>(sprites::kSlimeH);
        if (auto *d = dynamic_cast<support::DwarfAI *>(s.ai.get())) {
            sw = static_cast<float>(sprites::kDwarfW);
            sh = static_cast<float>(sprites::kDwarfH);
            switch (d->state()) {
                case support::DwarfState::ThrowWindup:
                case support::DwarfState::ThrowRelease:
                    tex = &sprites_.dwarfThrow; // dinamite visível = telegraph
                    break;
                case support::DwarfState::Melee:
                    tex = &sprites_.dwarfMelee; // picareta em riste
                    break;
                default:
                    if (std::fabs(s.body.getVx()) > 0.5f) {
                        tex = ((tickCount_ / 10) % 2 == 0) ? &sprites_.dwarfWalkA
                                                           : &sprites_.dwarfWalkB;
                    } else {
                        tex = &sprites_.dwarfIdle;
                    }
                    break;
            }
        } else {
            // Squash perseguindo (|vx| alto), idle patrulhando.
            if (std::fabs(s.body.getVx()) > 4.5f) tex = &sprites_.slimeSquash;
        }
        const float sc = s.body.getH() / sh;
        sf::Sprite spr;
        spr.setTexture(*tex);
        spr.setOrigin(sw * 0.5f, sh);
        spr.setPosition(s.body.getCenterX(), s.body.getY() + s.body.getH());
        spr.setScale(static_cast<float>(s.body.facing) * sc, sc);
        window->draw(spr);
    });
}

void Game::tick() {
    tickCount_++;
    Player *p = player.get();
    p->moveRight = input_.held(support::Action::Right);
    p->moveUp    = input_.held(support::Action::Up);
    p->moveDown  = input_.held(support::Action::Down);
    p->moveLeft  = input_.held(support::Action::Left);
    p->runFast   = input_.held(support::Action::RunFast);
    if (p->moveLeft && !p->moveRight) p->facing = -1;
    if (p->moveRight && !p->moveLeft) p->facing = 1;

    // Run gate: morto/pausado congela movimento, mundo e scheduler.
    // RunManager roda sempre (precisa ver o R).
    const bool frozen = run_.isPaused() || run_.isDead();
    if (!frozen) {
        p->tick();

        // S6: J (Action::Light) arremessa dinamite.
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
    }

    // Sistemas (inimigos etc.): scheduler com prioridade declarada.
    // BodySystem reconstrói hitboxes pós-movimento (priority 250).
    // Alvos de explosão: montados aqui (Player + Slimes), lidos pelo
    // ExplosionSystem durante o tick do ThrowSystem.
    std::vector<support::ExplosionTarget> targets;
    targets.push_back({sf::Vector2f(p->getCenterX(), p->getCenterY()),
                       &p->body, nullptr, true, p, nullptr});
    enemies_->forEach([&](support::Enemy &s) {
        if (s.resources.isDead()) return;
        targets.push_back({sf::Vector2f(s.body.getCenterX(), s.body.getCenterY()),
                           &s.bodyParts, &s.resources, false,
                           &s.body, &s.knockbackLock});
    });
    support::GameContext ctx{getWorld(), p, &input_, enemies_,
                             throws_, explodes_, drops_, &targets};
    run_.tick(1.0f / 30.0f, ctx);
    if (!run_.isPaused() && !run_.isDead()) scheduler_.tick(1.0f / 30.0f, ctx);
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
