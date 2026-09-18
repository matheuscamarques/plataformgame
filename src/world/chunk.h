#pragma once

#include <memory>
#include <vector>

#include "../entities/entity/entity.hpp"

// Um chunk 16x16 tiles em coordenadas de chunk (cx, cy).
// Tile do mundo: tx = cx * W + lx, ty = cy * H + ly (podem ser negativos).
struct Chunk {
    static const int W = 16;
    static const int H = 16;

    int cx = 0;
    int cy = 0;
    // Tipos por tile local [0,W) x [0,H); 0 = vazio.
    std::vector<int> tiles = std::vector<int>(W * H, 0);
    // Entidades visíveis deste chunk (dono).
    std::vector<std::unique_ptr<Entity>> entities;

    int tile(int lx, int ly) const { return tiles[ly * W + lx]; }
};
