/**
 * @file tests/test_deferred.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava morte adiada com erase só em removeDead.
 * @details Cobre EnemySystem e iteração segura, roda com make test que compila em build/tests/test_deferred.
 */

#include <cassert>
#include <cstdio>

#include "support/Enemies/EnemySystem.h"

// Morte adiada (camada 7): marcar durante iteração é seguro; o erase
// mora no removeDead (dono), no fim do frame. Headless (sem GL).
int main() {
    using namespace support;

    { // MarkDuranteForEach (2 vivos -> marca 1 no loop -> sweep leva 1)
        EnemySystem enemies;
        enemies.spawn("slime", 0.f, 0.f);
        enemies.spawn("slime", 100.f, 0.f);
        assert(enemies.count() == 2);
        int visited = 0;
        enemies.forEach([&](Enemy& e) {
            ++visited;
            if (visited == 1) enemies.markForDestroy(e); // seguro: sem erase
        });
        assert(visited == 2); // loop completou (sem invalidação)
        assert(enemies.count() == 2); // nada removido ainda
        int deaths = 0;
        enemies.removeDead([&](sf::Vector2f) { ++deaths; }, nullptr);
        assert(deaths == 1);
        assert(enemies.count() == 1);
    }
    { // MortoPorHpContinuaValendo (flag OU hp<=0)
        EnemySystem enemies;
        enemies.spawn("slime", 0.f, 0.f);
        enemies.forEach([](Enemy& e) { e.resources.hp = 0; });
        int deaths = 0;
        enemies.removeDead([&](sf::Vector2f) { ++deaths; }, nullptr);
        assert(deaths == 1 && enemies.count() == 0);
    }
    { // VivoNaoMarcadoSobrevive (sweep sem efeito colateral)
        EnemySystem enemies;
        enemies.spawn("slime", 0.f, 0.f);
        int deaths = 0;
        enemies.removeDead([&](sf::Vector2f) { ++deaths; }, nullptr);
        assert(deaths == 0 && enemies.count() == 1);
    }

    std::printf("deferred test OK\n");
    return 0;
}
