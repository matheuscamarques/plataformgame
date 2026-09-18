#include <iostream>
#include <chrono>
#include <cmath>
#include "./game.h"
#include "../window/window.h"
#include "../entities/player/player.h"
#include "../quadtree/quadtree.h"

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
    auto level = std::make_unique<Level>(WORLD_SEED);
    game->player = std::make_unique<Player>();
    game->setLevel(std::move(level));

    if (!game->font.loadFromFile("./arial.ttf"))
    {
        throw std::runtime_error("Could not load font");
    }

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
    // System nano time
    auto lastTime = std::chrono::high_resolution_clock::now();
    float amountOfTicks = 30.0;
    float ns = 1000000000 / amountOfTicks;
    float delta = 0;
    auto timer = std::chrono::high_resolution_clock::now();
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
        auto now = std::chrono::high_resolution_clock::now();
        delta += std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastTime).count();
        lastTime = now;
        while (delta >= ns)
        {
            tick();
            updates++;
            delta -= ns;
        }
        render();
        frames++;

        if(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - timer).count() > 1)
        {
            timer = std::chrono::high_resolution_clock::now();
            //std::cout << "updates: " << updates << " frames: " << frames << std::endl;
            updates = 0;
            frames = 0;
        }

    }
}

void Game::render()
{

    window->clear(sf::Color(135, 206, 235));
    //view->setCenter(player.get()->getX(), player.get()->getY());
    auto view = window->getDefaultView();
    view.move(player.get()->getX() - this->getW()/2,
              player.get()->getY() - this->getH()/2);
    //view.zoom(-10.0f);
    window->setView(view);
    getLevel()->quadtree->Draw(window);
    auto &objects = getLevel()->getPlatforms();
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

    getLevel()->quadtree = std::make_unique<Quadtree>(
            player.get()->getX() - this->getW()/2 ,
            player.get()->getY() - this->getH()/2,
            this->getW(),
            this->getH(),
            0,1);
    level->quadtree->SetFont(this->font);

    for(Entity *entity : objects){
        entity->tick();
        getLevel()->quadtree->AddObject( entity );
    }

    vector<Entity*> returnObjectsQ1 = getLevel()->quadtree->GetObjectsAt( player.get()->getCenterX() - 50, player.get()->getCenterY() -50 );
    vector<Entity*> returnObjectsQ2 = getLevel()->quadtree->GetObjectsAt( player.get()->getCenterX(), player.get()->getCenterY() - 50);
    vector<Entity*> returnObjectsQ3 = getLevel()->quadtree->GetObjectsAt( player.get()->getCenterX() - 50, player.get()->getCenterY() );
    vector<Entity*> returnObjectsQ4 = getLevel()->quadtree->GetObjectsAt( player.get()->getCenterX(), player.get()->getCenterY() );

    vector<Entity*> returnObjects = vector<Entity*>();
    returnObjects.insert(returnObjects.end(), returnObjectsQ1.begin(), returnObjectsQ1.end());
    returnObjects.insert(returnObjects.end(), returnObjectsQ2.begin(), returnObjectsQ2.end());
    returnObjects.insert(returnObjects.end(), returnObjectsQ3.begin(), returnObjectsQ3.end());
    returnObjects.insert(returnObjects.end(), returnObjectsQ4.begin(), returnObjectsQ4.end());


    totalQuadtreeSee =  returnObjects.size();
    for(auto i = returnObjects.begin(); i != returnObjects.end(); i++){
        Entity *entity = *i;
        entity->draw(window);
    }

    getLevel()->quadtree->Clear();

    player.get()->draw(window);
    getLevel()->quadtree->Draw(window);

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

    text.setString("QTREE: " + std::to_string(totalQuadtreeSee));
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
    //std::cout << "tick" << std::endl;
    player.get()->moveRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    player.get()->moveUp    = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    player.get()->moveDown  = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    player.get()->moveLeft  = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    player.get()->runFast   = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    player.get()->tick();

    // Mundo infinito: carrega/descarrega chunks em torno do tile do player.
    int playerTileX = static_cast<int>(std::floor(player.get()->getX() / BLOCK_SIZE));
    int playerTileY = static_cast<int>(std::floor(player.get()->getY() / BLOCK_SIZE));
    getLevel()->update(playerTileX, playerTileY);


    auto &objects = getLevel()->getColidePlatforms();
    getLevel()->quadtree = std::make_unique<Quadtree>(
            player.get()->getX() - player.get()->getW()/2 - player.get()->getW()/2 ,
            player.get()->getY() - player.get()->getH()/2 - player.get()->getH()/2,
            player.get()->getW() * 4,
            player.get()->getH() * 4,
            0,2);
    level->quadtree->SetFont(this->font);

    for(auto i = objects.begin(); i != objects.end(); i++){
        Entity *entity = *i;
        entity->tick();
        getLevel()->quadtree->AddObject( entity );
    }

    vector<Entity*> returnObjects = getLevel()->quadtree->GetObjectsAt( player.get()->getCenterX(), player.get()->getCenterY() );

    //totalQuadtreeSee =  returnObjects.size();
    for(auto i = returnObjects.begin(); i != returnObjects.end(); i++){
        Entity *entity = *i;
        auto color = entity->getFillColor();

        if(player.get()->isColide(*entity)){
            player.get()->collide(*entity);
        }
        //entity->setFillColor(color);
        entity->tick();
        //entity->setFillColor(sf::Color::Red);

    }

    getLevel()->quadtree->Clear();
}

Level* Game::getLevel() {
    return this->level.get();
}

void Game::setLevel(std::unique_ptr<Level> level) {
    this->level = std::move(level);
}

Player* Game::getPlayer() {
    return this->player.get();
}
