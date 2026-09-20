#pragma once

#include <cassert>
#include <chrono>
#include <cstdint>
#include <memory>
#include <vector>

#include "../defines.h"
#include "../entities/entity/entity.hpp"
#include "../support/Spatial/spatialhash.h"
#include "Tile.h"

namespace support {

// Um chunk em coordenadas de chunk (cx, cy).
// Tile do mundo: tx = cx * W + lx, ty = cy * H + ly (podem ser negativos).
//
// NOTA DE CONTRATO: W/H = 16 é premissa dos testes (raio 2 => 25 chunks
// carregados, reload idêntico). Mudar exige revalidar test_chunks.
struct Chunk {
    static const int W = 16;
    static const int H = 16;
    // Uma entidade de 50px ocupa 1 célula; célula = 2 tiles.
    static constexpr float HASH_CELL = BLOCK_SIZE * 2.0f;

    int cx = 0;
    int cy = 0;
    // Tipos por tile local [0,W) x [0,H). uint8_t: 256B por chunk
    // em vez de 1KB (32KB em vez de 128KB com 128 chunks).
    std::vector<Tile> tiles = std::vector<Tile>(W * H, Tile::Air);
    // Entidades deste chunk (dono).
    std::vector<std::unique_ptr<Entity>> entities;
    // Índice espacial das entidades (views, sem ownership).
    SpatialHash hash{HASH_CELL};

    Tile tile(int lx, int ly) const {
        assert(lx >= 0 && lx < W && ly >= 0 && ly < H);
        return tiles[ly * W + lx];
    }

    // Escrita pós-geração: marca modified (chunk precisa persistir).
    void setTile(int lx, int ly, Tile v) {
        tiles[ly * W + lx] = v;
        modified_ = true;
    }

    // Escrita da geração: estado inicial, NÃO é modificação.
    void setTileFromGeneration(int lx, int ly, Tile v) {
        tiles[ly * W + lx] = v;
    }

    bool modified() const { return modified_; }

    void touch() { lastAccess_ = std::chrono::steady_clock::now(); }
    std::chrono::steady_clock::time_point lastAccess() const { return lastAccess_; }

    void index(Entity *e) { hash.insert(e); }

    // Remove entidades exatamente neste tile (pós-quebra). Também tira
    // do hash (posição atual — entidade estática não se moveu).
    // removed (opcional): recebe os ponteiros destruídos para o dono
    // expurgar views (World::activePlatforms_/activeColides_).
    // Retorna quantas removeu.
    int removeEntitiesAt(int worldTileX, int worldTileY,
                         std::vector<Entity *> *removed = nullptr) {
        int n = 0;
        for (auto it = entities.begin(); it != entities.end();) {
            Entity *e = it->get();
            const int ex = static_cast<int>(e->getX() / BLOCK_SIZE);
            const int ey = static_cast<int>(e->getY() / BLOCK_SIZE);
            if (ex != worldTileX || ey != worldTileY) {
                ++it;
                continue;
            }
            hash.remove(e);
            if (removed) removed->push_back(e);
            it = entities.erase(it);
            ++n;
        }
        return n;
    }

private:
    bool modified_ = false;
    std::chrono::steady_clock::time_point lastAccess_{};
};

} // namespace support
