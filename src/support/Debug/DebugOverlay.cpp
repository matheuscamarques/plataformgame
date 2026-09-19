#include "DebugOverlay.h"

#include "../../defines.h"
#include "../../entities/player/player.h"
#include "../World/World.h"
#include "../World/Chunk.h"

namespace support {

void DebugOverlay::render(sf::RenderWindow &window, const sf::Font &font,
                          World &world, ::Player &player,
                          std::size_t totalPlatforms) {
    if (!visible_) return;

    float qx = player.getX() - BLOCK_SIZE;
    float qy = player.getY() - BLOCK_SIZE;
    float qw = player.getW() + BLOCK_SIZE * 2;
    float qh = player.getH() + BLOCK_SIZE * 2;

    sf::RectangleShape queryRect(sf::Vector2f(qw, qh));
    queryRect.setPosition(qx, qy);
    queryRect.setFillColor(sf::Color(0, 255, 0, 30));
    queryRect.setOutlineColor(sf::Color::Green);
    queryRect.setOutlineThickness(1.f);
    window.draw(queryRect);

    const float cs = Chunk::HASH_CELL;
    std::vector<std::pair<int,int>> cells;
    world.debugCells(qx, qy, qw, qh, cells);
    for (auto &cell : cells) {
        float cx = cell.first * cs;
        float cy = cell.second * cs;

        sf::RectangleShape cellRect(sf::Vector2f(cs, cs));
        cellRect.setPosition(cx, cy);
        cellRect.setFillColor(sf::Color::Transparent);
        cellRect.setOutlineColor(sf::Color(64, 128, 255));
        cellRect.setOutlineThickness(1.f);
        window.draw(cellRect);

        sf::Text t;
        t.setFont(font);
        t.setString(std::to_string(world.debugCellCount(cell.first, cell.second)));
        t.setCharacterSize(12);
        t.setFillColor(sf::Color::Yellow);
        t.setOutlineColor(sf::Color::Black);
        t.setOutlineThickness(1);
        t.setPosition(cx + 4, cy + 4);
        window.draw(t);
    }

    std::vector<Entity*> candidatos;
    world.query(qx, qy, qw, qh, candidatos);

    sf::Text totalPlataformsTxt;
    totalPlataformsTxt.setFont(font);
    totalPlataformsTxt.setString("Total Platforms: " + std::to_string(totalPlatforms));
    totalPlataformsTxt.setCharacterSize(20);
    totalPlataformsTxt.setFillColor(sf::Color::Green);
    totalPlataformsTxt.setPosition(player.getX(), player.getY() - 200);
    totalPlataformsTxt.setOutlineColor(sf::Color::Black);
    totalPlataformsTxt.setOutlineThickness(1);
    window.draw(totalPlataformsTxt);

    sf::Text text;
    text.setString("HASH: " + std::to_string(candidatos.size())
        + " TNT: " + std::to_string(player.dynamiteCount) + " (J)");
    text.setCharacterSize(20);
    text.setFont(font);
    text.setFillColor(sf::Color::Green);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(1);
    text.setPosition(player.getX() - player.getW() / 2, player.getY() - player.getH() / 2);
    window.draw(text);
}

} // namespace support
