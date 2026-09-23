/**
 * @file tests/test_equipment_offset.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava overlay na grade do sprite com escala.
 * @details Cobre EquipmentLayout sem Body, roda com make test que compila em build/tests/test_equipment_offset.
 */

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
        sf::Vector2f p = game::equipSpritePos(100.f, 200.f, 2.5f, 1, 0.f, 0.f);
        assert(near(p.x, 100.f) && near(p.y, 200.f));
        // Peitoral em (0,6): 6 rows * 2.5 = 15px abaixo.
        sf::Vector2f c = game::equipSpritePos(100.f, 200.f, 2.5f, 1, 0.f, 6.f);
        assert(near(c.x, 100.f) && near(c.y, 215.f));
    }
    { // MirrorLandsOnSameScreenBox (full-width: mesmo box espelhado)
        // Piece full-width (spriteX=0, 12 de largura). Sem mirror ocupa
        // [0,12]. Com facing=-1, o canto DIREITO ancora em 12.
        // Origin (0,0) + scale(-s,s): desenha da posição para a esquerda,
        // então pos.x deve ser 12, não 0.
        auto right = game::equipSpritePos(0.f, 0.f, 1.f, 1, 0.f, 0.f);
        auto left = game::equipSpritePos(0.f, 0.f, 1.f, -1, 0.f, 0.f);
        assert(near(right.x, 0.f));
        assert(near(left.x, 12.f)); // falhava antes do fix (retornava 0)
    }
    { // GloveMirrorsToOtherSide (luva x=8,w=4: [8,12] vira [0,4])
        auto right = game::equipSpritePos(0.f, 0.f, 1.f, 1, 8.f, 0.f);
        auto left = game::equipSpritePos(0.f, 0.f, 1.f, -1, 8.f, 0.f);
        assert(near(right.x, 8.f));
        assert(near(left.x, 4.f)); // [4-4, 4] = [0, 4]
    }
    { // ScaleDoublesOffset (s=2.5 vs 5.0: mesma grade, dobra mundo)
        sf::Vector2f a = game::equipSpritePos(0.f, 0.f, 2.5f, 1, 0.f, 8.f);
        sf::Vector2f b = game::equipSpritePos(0.f, 0.f, 5.0f, 1, 0.f, 8.f);
        assert(near(a.y, 20.f) && near(b.y, 40.f));
    }

    std::printf("equipment offset test OK\n");
    return 0;
}
