#include <cassert>
#include <cstdio>
#include "entities/Entity.hpp"
#include "entities/Player/Player.h"
#include "defines.h"

// Lava não colide como parede (igual água). Dano vem na Fase C.
int main() {
    Player p;
    p.setX(100.0f);
    p.setY(100.0f);

    Entity lava(LAVA, 100, 100, 50, 50);
    float x0 = p.getX(), y0 = p.getY();
    if (p.isColide(lava)) p.collide(lava);
    assert(p.getX() == x0 && p.getY() == y0);

    // controle: bloco sólido no mesmo lugar DESLOCA o player
    Entity rock(COLIDE, 100, 100, 50, 50);
    bool moved = false;
    float bx = p.getX(), by = p.getY();
    if (p.isColide(rock)) { p.collide(rock); moved = true; }
    assert(moved && (p.getX() != bx || p.getY() != by));

    std::printf("lava noclip test OK\n");
    return 0;
}
