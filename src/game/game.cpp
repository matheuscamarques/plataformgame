#include <iostream>
#include "./game.h"
#include "../window/window.h"



void Game::main()
{
    auto game = new Game();
    auto level = new Level();
    for(auto i =0; i< level->getM();i++){
        for(auto j=0;j<level->getN(); j++){
            auto value = level->map[i][j];
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
    game->setLevel(level);
    new Window(800, 800, "Game", game);
}

void Game::setWindow(sf::RenderWindow *window)
{
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
    long long lastTime = sf::Clock().getElapsedTime().asMicroseconds();
    double amountOfTicks = 30.0;
    double ns = 1000000000 / amountOfTicks;
    double delta = 0;
    long long timer = sf::Clock().getElapsedTime().asMicroseconds();
    int frames = 0;
    int updates = 0;

    while (running && window->isOpen())
    {   
        sf::Event event{};
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window->close();
        }

        long long now = sf::Clock().getElapsedTime().asMicroseconds();
        delta += (double(now - lastTime) / ns);
        lastTime = now;
        while (delta >= 1)
        {
            tick();
            updates++;
            delta--;
        }
        render();
        frames++;

        if (sf::Clock().getElapsedTime().asMicroseconds() - timer > 1000000)
        {
            timer += 1000000;
            //std::cout << "updates: " << updates << " frames: " << frames << std::endl;
            updates = 0;
            frames = 0;
        }
    }
}

void Game::render()
{   
    window->clear(sf::Color::Black);
    //////////// DRAWER
    Level * plevel = getLevel();
    std::vector<Entity> * list = plevel->getPlatforms();
    auto p = list->begin();
    while ( p != this->level->getPlatforms()->end() )
    {
        p->draw(window);
        p->update();
        p++;
    }


    /////////////////////////
    window->display();
}

void Game::tick(){

}


Game::Game() 
{
}

Level* Game::getLevel() {
    return this->level;
}

void Game::setLevel(Level *level) {
    this->level = level;
}
