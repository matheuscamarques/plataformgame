#include <cassert>
#include <cmath>
#include <cstdio>

#include "assets/EquipmentLayout.h"

namespace {
bool near(float a, float b) { return std::fabs(a - b) < 0.01f; }
} // namespace

// Posição de overlay na grade do sprite (sem Body, sem GL).
int main() {
    { // ScaleApplies (sprite 12x20 em (100,200), s=2.5)
        // Elmo em (0,0): canto superior esquerdo do sprite.
        sf::Vector2f p = game::equipSpritePos(100.f, 200.f, 2.5f, 1, 0.f, 0.f, 12);
        assert(near(p.x, 100.f) && near(p.y, 200.f));
        // Peitoral em (0,6): 6 rows * 2.5 = 15px abaixo.
        sf::Vector2f c = game::equipSpritePos(100.f, 200.f, 2.5f, 1, 0.f, 6.f, 12);
        assert(near(c.x, 100.f) && near(c.y, 215.f));
    }
    { // FacingMirrorsX (facing -1: x -> 12 - x - texW)
        // Luva direita em x=8 (w=4): normal 8, espelhado 12-8-4 = 0.
        sf::Vector2f pr = game::equipSpritePos(100.f, 200.f, 2.5f, 1, 8.f, 7.f, 4);
        sf::Vector2f pl = game::equipSpritePos(100.f, 200.f, 2.5f, -1, 8.f, 7.f, 4);
        assert(near(pr.x, 120.f) && near(pl.x, 100.f));
        assert(near(pr.y, pl.y)); // Y não espelha
    }
    { // ScaleDoublesOffset (s=2.5 vs 5.0: mesma grade, dobra mundo)
        sf::Vector2f a = game::equipSpritePos(0.f, 0.f, 2.5f, 1, 0.f, 8.f, 4);
        sf::Vector2f b = game::equipSpritePos(0.f, 0.f, 5.0f, 1, 0.f, 8.f, 4);
        assert(near(a.y, 20.f) && near(b.y, 40.f));
    }

    std::printf("equipment offset test OK\n");
    return 0;
}
