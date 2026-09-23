/**
 * @file tests/test_spatialhash.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava insert, query e remoção com negativas.
 * @details Cobre SpatialHash e Entity, roda com make test que compila em build/tests/test_spatialhash.
 */

#include <cassert>
#include <cstdio>
#include <vector>
#include "support/Spatial/spatialhash.h"
#include "entities/Entity.hpp"

int main() {
    support::SpatialHash h(100.0f);

    Entity a(1, 10, 10, 50, 50);       // células (0,0)-(1,1)
    Entity b(2, 500, 500, 50, 50);     // longe
    Entity c(3, -150, -150, 50, 50);   // negativas (-2,-2)-(-1,-1)

    h.insert(&a);
    h.insert(&b);
    h.insert(&c);

    std::vector<Entity*> out;
    h.query(0, 0, 200, 200, out);      // pega só 'a'
    assert(out.size() == 1 && out[0] == &a);

    h.query(-200, -200, 100, 100, out); // pega só 'c'
    assert(out.size() == 1 && out[0] == &c);

    h.query(-1000, -1000, 5000, 5000, out); // todos, sem duplicatas
    assert(out.size() == 3);

    // remove: some das queries
    h.remove(&a);
    h.query(0, 0, 200, 200, out);
    assert(out.empty());
    assert(h.getCellCount(0, 0) == 0);

    // update reinsere
    h.update(&a);
    h.query(0, 0, 200, 200, out);
    assert(out.size() == 1 && out[0] == &a);

    // clear
    h.clear();
    h.query(-1000, -1000, 5000, 5000, out);
    assert(out.empty());

    // debugCells: query 100x100 em célula 100 -> 2x2 a 3x3 células
    std::vector<std::pair<int,int>> cells;
    h.debugCells(0, 0, 100, 100, cells);
    assert(cells.size() == 4); // (0..1, 0..1)
    h.debugCells(0, 0, 150, 150, cells);
    assert(cells.size() == 4); // (0..1, 0..1): borda em 150/100 -> 0,1

    // key única incl. negativos
    assert(support::SpatialHash::key(1, 2) != support::SpatialHash::key(1, -2));
    assert(support::SpatialHash::key(-1, -2) != support::SpatialHash::key(-2, -1));

    // entidade exatamente na borda da célula aparece dos dois lados sem dup
    Entity d(4, 100, 0, 50, 50); // começa na borda x=100
    h.insert(&d);
    h.query(0, 0, 100, 100, out);
    assert(out.size() == 1 && out[0] == &d);
    assert(h.getCellCount(1, 0) == 1);

    std::printf("spatialhash test OK\n");
    return 0;
}
