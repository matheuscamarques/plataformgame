#pragma once
#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>

#include "defines.h"
#include "entities/Entity.hpp"
#include "ChunkManager.h"
#include "Tile.h"

namespace support {

class ChunkLoader;

// Fachada pública do mundo. Features NUNCA acessam Chunk ou ChunkManager
// diretamente — só a World.
//
// NOTA DE ACOPLAMENTO: World acumula tiles + entidades (estáticas e
// dinâmicas via listas ativas). Isso rompe a fronteira "Support tiles-only"
// de propósito: no estágio atual, qualquer jogo 2D tem estáticos +
// dinâmicos e separar agora seria especulação. Se um dia houver necessidade
// de separar (mundo procedural em background, física desacoplada), a
// divisão natural é `World` (tiles) + `EntityLayer` (dinâmicos). Não fazer
// agora.
class World {
public:
    explicit World(uint32_t seed);

    // Sem cópia — World é dono do estado de chunks.
    World(const World&) = delete;
    World& operator=(const World&) = delete;

    // Streaming: carrega/descarrega chunks em torno do tile do player e
    // reconstrói as listas ativas. Chamar uma vez por tick.
    void update(int playerTileX, int playerTileY);

    // Camada 6 (opt-in, default off): com loader, update() pede faltantes
    // ao worker e adota até 2 prontos/frame; sem loader, 100% síncrono.
    void setChunkLoader(ChunkLoader* loader) { loader_ = loader; }

    // Construção nua p/ o worker (repasse ao ChunkManager; thread-safe).
    std::unique_ptr<Chunk> buildBareChunk(int cx, int cy);

    // Acesso por coordenada de tile de mundo (pode ser negativa).
    // Não carrega chunk ausente — retorna Air.
    Tile tileAt(int worldTileX, int worldTileY) const;
    bool isSolid(int worldTileX, int worldTileY) const;

    // Altura da superfície (world px) na coordenada X — p/ céu/luz.
    // Vem da geração (não dos chunks): vale mesmo descarregado.
    float surfaceYAt(float worldX) const;

    // Quebra um tile: vira Air, marca modified (persiste via LRU),
    // remove a entidade e devolve o tipo anterior em `broken`
    // (para partícula com a cor certa). Bedrock nunca quebra.
    // Chunk ausente -> false.
    bool breakTile(int worldTileX, int worldTileY, Tile *broken = nullptr);

    // Espalha a query (x, y, w, h) pelos hashes dos chunks
    // carregados. 'out' é limpo antes. Sem duplicatas entre chunks.
    void query(float x, float y, float w, float h, std::vector<Entity*> &out);

    // Culling de render: itera SÓ as entidades dos chunks que tocam o
    // rect (x0,y0,x1,y1), com teste de sobreposição por entidade.
    // O(n_visível), não O(carregado): chunks modified pinned pelo LRU
    // não encarecem o frame. Chunks ausentes são pulados (sem load).
    template <typename F>
    void forEachEntityInRect(float x0, float y0, float x1, float y1, F &&fn) {
        const float cw = static_cast<float>(Chunk::W) * core::kBlockSize;
        const float ch = static_cast<float>(Chunk::H) * core::kBlockSize;
        const int cx0 = static_cast<int>(std::floor(x0 / cw));
        const int cx1 = static_cast<int>(std::floor(x1 / cw));
        const int cy0 = static_cast<int>(std::floor(y0 / ch));
        const int cy1 = static_cast<int>(std::floor(y1 / ch));
        for (int cy = cy0; cy <= cy1; ++cy) {
            for (int cx = cx0; cx <= cx1; ++cx) {
                Chunk *c = chunks_.find(cx, cy);
                if (!c) continue;
                for (auto &slot : c->entities) {
                    Entity *e = slot.get();
                    if (e->getX() + e->getW() < x0 || e->getX() > x1 ||
                        e->getY() + e->getH() < y0 || e->getY() > y1)
                        continue;
                    fn(e);
                }
            }
        }
    }

    // Busca chunk carregado por coords de chunk (nullptr se ausente).
    Chunk *findChunk(int cx, int cy) { return chunks_.find(cx, cy); }

    // Itera chunks carregados que tocam o rect (p/ sprites do lightmap).
    template <typename F>
    void forEachChunkInRect(float x0, float y0, float x1, float y1, F &&fn) {
        const float cw = static_cast<float>(Chunk::W) * core::kBlockSize;
        const float ch = static_cast<float>(Chunk::H) * core::kBlockSize;
        for (Chunk *c : chunks_.loaded()) {
            const float cx0 = c->cx * cw;
            const float cy0 = c->cy * ch;
            if (cx0 + cw < x0 || cx0 > x1 || cy0 + ch < y0 || cy0 > y1)
                continue;
            fn(c);
        }
    }

    // Debug visual do SpatialHash (grade em coords de mundo).
    void debugCells(float x, float y, float w, float h,
                    std::vector<std::pair<int,int>> &out);
    int debugCellCount(int cx, int cy);

        // Views não-owning para as entidades dos chunks ativos.
        // Const-ref: dá pra mutar as entidades, não o vetor.
        const std::vector<Entity*> & getPlatforms();
        const std::vector<Entity*> & getColidePlatforms();

    // Debug / métricas.
    std::size_t loadedChunkCount() const { return chunks_.loadedCount(); }
    std::size_t modifiedChunkCount() const { return chunks_.modifiedCount(); }
    uint32_t getSeed() const { return chunks_.getSeed(); }

private:
    ChunkManager chunks_;
    ChunkLoader* loader_ = nullptr; // opt-in camada 6; não é dono
    std::vector<Entity*> activePlatforms_;
    std::vector<Entity*> activeColides_;
};

} // namespace support
