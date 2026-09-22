#pragma once

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <memory>
#include <vector>

#include <SFML/Graphics/Texture.hpp>

#include "defines.h"
#include "entities/Entity.hpp"
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
    static constexpr float HASH_CELL = core::kBlockSize * 2.0f;

    int cx = 0;
    int cy = 0;
    // Tipos por tile local [0,W) x [0,H). uint8_t: 256B por chunk
    // em vez de 1KB (32KB em vez de 128KB com 128 chunks).
    std::vector<Tile> tiles = std::vector<Tile>(W * H, Tile::Air);
    // Luz 0..15 por tile (propagação real; LightPropagator preenche).
    // Sky = sol/lua por cima; Block = fontes (tochas futuras).
    // lightAt = max das duas. Textura só no render (GL); geração e
    // testes tocam só nos grids (headless-safe).
    std::vector<uint8_t> skyLight = std::vector<uint8_t>(W * H, 0);
    std::vector<uint8_t> blockLight = std::vector<uint8_t>(W * H, 0);
    bool lightDirty = true;
    sf::Texture lightmap; // W×H, bilinear; criada sob demanda no render
    // Máscara de visibilidade do último raycast (1 = raio alcançou).
    // Por fonte (raycast sobrescreve); hoje só o player usa.
    std::vector<uint8_t> visibleMask = std::vector<uint8_t>(W * H, 0);
    // Entidades deste chunk (dono).
    std::vector<std::unique_ptr<Entity>> entities;
    // Índice espacial das entidades (views, sem ownership).
    SpatialHash hash{HASH_CELL};

    Tile tile(int lx, int ly) const {
        assert(lx >= 0 && lx < W && ly >= 0 && ly < H);
        return tiles[ly * W + lx];
    }

    inline bool inBounds(int lx, int ly) const {
        return lx >= 0 && ly >= 0 && lx < W && ly < H;
    }

    // Luz combinada 0..15 (max das duas fontes). Fora = 0.
    inline uint8_t lightAt(int lx, int ly) const {
        if (!inBounds(lx, ly)) return 0;
        const int i = ly * W + lx;
        return std::max(skyLight[i], blockLight[i]);
    }

    // Byte 0..255 p/ textura (×17).
    inline uint8_t lightByte(int lx, int ly) const {
        return static_cast<uint8_t>(lightAt(lx, ly) * 17);
    }

    inline bool isVisible(int lx, int ly) const {
        if (!inBounds(lx, ly)) return false;
        return visibleMask[ly * W + lx] != 0;
    }

    inline void clearVisibility() {
        std::fill(visibleMask.begin(), visibleMask.end(), 0);
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
            const int ex = static_cast<int>(e->getX() / core::kBlockSize);
            const int ey = static_cast<int>(e->getY() / core::kBlockSize);
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
