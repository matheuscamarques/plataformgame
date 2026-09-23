/**
 * @file tests/test_equipment.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava larguras de botas e luvas nos sprites.
 * @details Cobre EquipSprites com widths, roda com make test que compila em build/tests/test_equipment.
 */

#include <cassert>
#include <cstdio>
#include <cstring>

#include "assets/Sprites/EquipSprites.h"
#include "support/Combat/Body.h"

using namespace support;

int main() {
    { // BootsAndGlovesWidths
        for (int y = 0; y < sprites::kBootsH; ++y) {
            assert(std::strlen(sprites::kIronBootsIdle[y]) ==
                   static_cast<std::size_t>(sprites::kBootsW));
        }
        for (int y = 0; y < sprites::kGloveH; ++y) {
            assert(std::strlen(sprites::kIronGlovesIdle[y]) ==
                   static_cast<std::size_t>(sprites::kGloveW));
        }
    }
    { // AnchorFollowsRebuild (peça anda com a parte: mesmo delta)
        BodySchema s = BodySchema::humanoid(50.f, 50.f);
        Body b;
        b.attach(&s);
        b.rebuild({100.f, 100.f}, 1);
        const PartState *h1 = b.find(BodyPartId::Head);
        assert(h1 != nullptr);
        const float x1 = h1->worldBox.left;
        b.rebuild({200.f, 100.f}, 1);
        const PartState *h2 = b.find(BodyPartId::Head);
        assert(h2 != nullptr);
        assert(h2->worldBox.left - x1 > 99.f &&
               h2->worldBox.left - x1 < 101.f);
    }

    std::printf("equipment test OK\n");
    return 0;
}
