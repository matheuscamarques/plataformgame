#include "chunkmanager.h"

#include <cmath>
#include <cstdlib>

#include "../defines.h"
#include "hash.h"

namespace {
// Linha d'água em tiles do mundo (era `i > m/2` no mapa fixo 50x1000).
const int WATER_ROW = 25;
// Meio da superfície (era `m/2` no mapa fixo).
const int SURFACE_MID = 25;
}

ChunkManager::ChunkManager(uint32_t seed, int radius)
    : seed(seed), radius(radius) {}

int64_t ChunkManager::key(int cx, int cy) {
    return (static_cast<int64_t>(cx) << 32) | static_cast<uint32_t>(cy);
}

int ChunkManager::floorDiv(int a, int b) {
    return a >= 0 ? a / b : -(((-a) + b - 1) / b);
}

int ChunkManager::tileType(int tx, int ty, uint32_t seed) {
    float relief = valueNoise2D(tx * 0.02f, 3.7f, seed);
    int surface = SURFACE_MID - 4 + static_cast<int>(relief * 9.0f);
    if (ty > surface) {
        float pick = rand01(tx, ty, seed ^ 0x9E3779B9u);
        if (pick < 0.15f)      return 1;
        else if (pick < 0.35f) return 2;
        else if (pick < 0.55f) return 3;
        else if (pick < 0.75f) return 4;
        else                   return 5;
    }
    if (ty == surface) return 4;
    float plat = rand01(tx, ty, seed ^ 0x51F37EDu);
    return plat < 0.035f ? 2 : 0;
}

static void paint(Entity *e, int t) {
    if (t == 1)      e->setFillColor(sf::Color(60, 60, 60));
    else if (t == 2) e->setFillColor(sf::Color(146, 90, 43));
    else if (t == 3) e->setFillColor(sf::Color(120, 60, 0));
    else if (t == 4) e->setFillColor(sf::Color(159, 89, 30));
    else if (t == 5) e->setFillColor(sf::Color(150, 75, 0));
}

void ChunkManager::generate(int cx, int cy) {
    auto c = std::make_unique<Chunk>();
    c->cx = cx;
    c->cy = cy;
    for (int ly = 0; ly < Chunk::H; ly++) {
        for (int lx = 0; lx < Chunk::W; lx++) {
            int tx = cx * Chunk::W + lx;
            int ty = cy * Chunk::H + ly;
            int t = tileType(tx, ty, seed);
            c->tiles[ly * Chunk::W + lx] = t;
            if (t == 0) {
                if (ty > WATER_ROW) {
                    auto water = std::make_unique<Entity>(
                        WATER, tx * BLOCK_SIZE, ty * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
                    water->setFillColor(sf::Color(0, 255, 255));
                    c->entities.push_back(std::move(water));
                }
                continue;
            }
            auto platform = std::make_unique<Entity>(
                COLIDE, tx * BLOCK_SIZE, ty * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
            paint(platform.get(), t);
            c->entities.push_back(std::move(platform));
        }
    }
    chunks.emplace(key(cx, cy), std::move(c));
}

void ChunkManager::update(int centerTileX, int centerTileY) {
    int ccx = floorDiv(centerTileX, Chunk::W);
    int ccy = floorDiv(centerTileY, Chunk::H);
    for (int cy = ccy - radius; cy <= ccy + radius; cy++) {
        for (int cx = ccx - radius; cx <= ccx + radius; cx++) {
            if (chunks.find(key(cx, cy)) == chunks.end()) generate(cx, cy);
        }
    }
    for (auto it = chunks.begin(); it != chunks.end();) {
        int cx = static_cast<int>(it->first >> 32);
        int cy = static_cast<int>(it->first & 0xFFFFFFFF);
        if (std::abs(cx - ccx) > radius || std::abs(cy - ccy) > radius)
            it = chunks.erase(it);
        else
            ++it;
    }
}

std::vector<Chunk*> ChunkManager::loaded() {
    std::vector<Chunk*> out;
    out.reserve(chunks.size());
    for (auto &kv : chunks) out.push_back(kv.second.get());
    return out;
}
