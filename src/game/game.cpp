#include <iostream>
#include <chrono>
#include "./game.h"
#include "../window/window.h"
#include "../entities/player/player.h"
#include "../quadtree/quadtree.h"



Game::Game() : Component()
{
}


void Game::main()
{
    auto game = new Game();
    //game->view = new sf::View(sf::FloatRect(0.f, 0.f, 1000.f, 600.f));
    auto level = new Level(new Quadtree(0.0f, 0.0f, 0.f, 0.f,0,0));
    //  level->quadtree = new Quadtree(0.0f, 0.0f, level->getN()*50, level->getM()*50,0,1);
    auto player = new Player();
    level->setPlayer(player);
    // for(auto i =0; i< level->getM();i++){
    //     for(auto j=0;j<level->getN(); j++){
    //         auto value = level->map[i][j];
    //         std::cout << value << " ";
    //     }
    //     std::cout << std::endl;
    // }
    game->setLevel(level);
    new Window(800, 800, "Game", game);
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
    //view->setCenter(level->getPlayer()->getX(), level->getPlayer()->getY());
    auto view = window->getDefaultView();
    view.move(level->getPlayer()->getX() - this->getW()/2,
              level->getPlayer()->getY() - this->getH()/2);
    //view.zoom(-10.0f);
    window->setView(view);
    std::vector<Entity*> * list = getLevel()->getPlatforms();
    auto p = list->begin();
    while ( p != this->level->getPlatforms()->end() )
    {
        Entity *entity = *p;
        entity->draw(window);
        p++;
    }

    getLevel()->getPlayer()->draw(window);
    getLevel()->quadtree->Draw(window);

    // draw point
    sf::CircleShape shape(5.f);
    shape.setFillColor(sf::Color::Green);
    shape.setPosition(getLevel()->getPlayer()->getCenterX(), getLevel()->getPlayer()->getCenterY());
    window->draw(shape);

    // draw point
    sf::CircleShape shape2(5.f);
    shape2.setFillColor(sf::Color::Green);
    shape2.setPosition(getLevel()->getPlayer()->getX(), getLevel()->getPlayer()->getY());
    window->draw(shape2);

    window->display();
}

void Game::tick() {
    //std::cout << "tick" << std::endl;
    getLevel()->getPlayer()->moveRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    getLevel()->getPlayer()->moveUp    = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    getLevel()->getPlayer()->moveDown  = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    getLevel()->getPlayer()->moveLeft  = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    getLevel()->getPlayer()->runFast   = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

    getLevel()->getPlayer()->tick();


    vector<Entity*> * objects = getLevel()->getColidePlatforms();
    getLevel()->quadtree = new Quadtree(
            getLevel()->getPlayer()->getX() - getLevel()->getPlayer()->getW()*2 + getLevel()->getPlayer()->getW()/2 ,
            getLevel()->getPlayer()->getY() - getLevel()->getPlayer()->getH()*2 + getLevel()->getPlayer()->getH()/2,
            getLevel()->getPlayer()->getW()*4,
            getLevel()->getPlayer()->getH()*4,
            0,
            2);

    for(auto i = objects->begin(); i != objects->end(); i++){
        Entity *entity = *i;
        entity->tick();
        getLevel()->quadtree->AddObject( entity );
    }

    vector<Entity*> returnObjects = getLevel()->quadtree->GetObjectsAt( getLevel()->getPlayer()->getCenterX(), getLevel()->getPlayer()->getCenterY() );
    cout << returnObjects.size() << endl;

    for(auto i = returnObjects.begin(); i != returnObjects.end(); i++){
        Entity *entity = *i;
        auto color = entity->getFillColor();

        if(getLevel()->getPlayer()->isColide(*entity)){
            getLevel()->getPlayer()->collide(*entity);
            entity->setFillColor(sf::Color::Red);
        }
        //entity->setFillColor(color);
        entity->tick();
    }

    returnObjects = getLevel()->quadtree->GetObjectsAt( getLevel()->getPlayer()->getX(), getLevel()->getPlayer()->getY() );
    cout << returnObjects.size() << endl;

    for(auto i = returnObjects.begin(); i != returnObjects.end(); i++){
        Entity *entity = *i;
        auto color = entity->getFillColor();

        if(getLevel()->getPlayer()->isColide(*entity)){
            getLevel()->getPlayer()->collide(*entity);
            entity->setFillColor(sf::Color::Red);
        }
        //entity->setFillColor(color);
        entity->tick();
    }

    getLevel()->quadtree->Clear();
}

Level* Game::getLevel() {
    return this->level;
}

void Game::setLevel(Level *level) {
    this->level = level;
}
