#include <cassert>
#include <cstdio>
#include <vector>
#include <utility>
#include "support/Spatial/spatialhash.h"
int main() {
    support::SpatialHash h(100.0f);
    std::vector<std::pair<int,int>> cells;
    // mesma query 150x150 do render, só muda o alinhamento do player
    h.debugCells(10, 10, 150, 150, cells);
    assert(cells.size() == 4); // 2x2
    std::printf("player em x=10  -> %zu celulas\n", cells.size());
    h.debugCells(60, 10, 150, 150, cells);
    assert(cells.size() == 6); // 3x2
    std::printf("player em x=60  -> %zu celulas\n", cells.size());
    h.debugCells(50, 50, 150, 150, cells);
    assert(cells.size() == 9); // 3x3
    std::printf("player em x=50  -> %zu celulas (emenda de tile)\n", cells.size());
    std::printf("grid alignment OK\n");
    return 0;
}
