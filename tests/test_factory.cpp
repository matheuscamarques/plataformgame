/**
 * @file tests/test_factory.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava slime 40x30 verde e kind inválido nulo.
 * @details Cobre Factory e nome slime, roda com make test que compila em build/tests/test_factory.
 */

#include <cassert>
#include <cmath>
#include <cstdio>
#include "entities/Player/Player.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "defines.h"

// Factory cria slime 40x30 verde nomeado; kind inválido dá null.
int main() {
    using namespace support;

    auto s = Factory::spawnEnemy("slime", 100.0f, 0.0f);
    assert(s != nullptr);
    assert(s->body.getName() == core::kIdSlime);
    assert(std::fabs(s->body.getW() - 40.0f) < 0.001f);
    assert(std::fabs(s->body.getH() - 30.0f) < 0.001f);
    assert(std::fabs(s->body.getX() - 100.0f) < 0.001f);
    assert(s->ai != nullptr);

    // S2: recursos default de trash.
    assert(s->resources.isTrash == true);
    assert(s->resources.hp == 40 && s->resources.hpMax == 40);
    assert(std::fabs(s->resources.posture - 20.0f) < 0.001f);

    auto none = Factory::spawnEnemy("dragon", 0.0f, 0.0f);
    assert(none == nullptr);

    // Tarô híbrido: Diabo engorda o HP dos nascidos (sem mundo = sem
    // elite, só o mult). Mesmas coords = mesma variante/equip.
    {
        Player bare;
        GameContext ctxB{};
        ctxB.player = &bare;
        auto eBare = Factory::spawnEnemy("slime", 100.0f, 0.0f, &ctxB);
        Player devil;
        devil.addTarotCard(core::TarotArcana::Devil);
        GameContext ctxD{};
        ctxD.player = &devil;
        auto eDevil = Factory::spawnEnemy("slime", 100.0f, 0.0f, &ctxD);
        assert(eBare && eDevil);
        assert(eDevil->resources.hpMax > eBare->resources.hpMax);
        assert(eDevil->resources.hp == eDevil->resources.hpMax);
    }

    std::printf("factory test OK\n");
    return 0;
}
