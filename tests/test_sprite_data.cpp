/**
 * @file tests/test_sprite_data.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava core::SpriteData (Fase 3 da fundação).
 * @details Cobre toSpriteData (índices, dims, char fora da paleta) e partAt (parte por pixel, fora da área), roda com make test que compila em build/tests/test_sprite_data.
 */

#include <cassert>
#include <cstdio>

#include "core/SpriteData.h"
#include "core/BodyPart.h"

int main() {
    using core::BodyPartId;

    static const core::SpritePalEntry kPal[] = {
        {'.', 0, BodyPartId::None},
        {'S', core::rgba(200, 195, 170), BodyPartId::Head},
        {'E', core::rgba(10, 10, 15), BodyPartId::Head},
        {'R', core::rgba(210, 205, 185), BodyPartId::Torso},
    };
    static const char *const kRows[] = {
        ".SS.",
        "SERS",
        "ZZZ.", // Z fora da paleta = transparente
    };

    { // ConvertsAsciiToIndices (dims, índices, desconhecido)
        const auto s = core::toSpriteData(kRows, 4, 3, kPal, 4);
        assert(s.width == 4 && s.height == 3);
        assert(s.pal.size() == 4u);
        assert(s.indices.size() == 12u);
        assert(s.indices[0] == 0); // '.'
        assert(s.indices[1] == 1); // 'S'
        assert(s.indices[4] == 1 && s.indices[5] == 2); // 'S','E'
        assert(s.indices[8] == 0); // 'Z' desconhecido = 0
    }
    { // PartAtPreservesNarrowphase (parte por pixel, borda segura)
        const auto s = core::toSpriteData(kRows, 4, 3, kPal, 4);
        assert(core::partAt(s, 1, 0) == BodyPartId::Head);
        assert(core::partAt(s, 1, 1) == BodyPartId::Head); // 'E' = Head
        assert(core::partAt(s, 2, 1) == BodyPartId::Torso); // 'R'
        assert(core::partAt(s, 0, 0) == BodyPartId::None);
        assert(core::partAt(s, 0, 2) == BodyPartId::None); // 'Z' = None
        assert(core::partAt(s, -1, 0) == BodyPartId::None);
        assert(core::partAt(s, 4, 0) == BodyPartId::None);
        assert(core::partAt(s, 0, 3) == BodyPartId::None);
    }

    std::printf("sprite_data test OK\n");
    return 0;
}
