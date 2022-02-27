#include "./game.h"
#include "../window/window.h"


void Game::main()
{
    new Window(800, 600, "Game", new Game());
}

void Game::setWindow(sf::RenderWindow *cwindow)
{
    window = cwindow;
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
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window->close();
        }

        long long now = sf::Clock().getElapsedTime().asMicroseconds();
        delta += (now - lastTime) / ns;
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
    window->clear();
    //////////// DRAWER
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    window->draw(shape);

    /////////////////////////
    window->display();
}

void Game::tick(){

}


Game::Game() 
{
}