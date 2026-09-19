#pragma once
#include <cstdint>
#include <utility>
#include <vector>

#include "ChunkManager.h"
#include "Tile.h"

class Entity;

namespace support {

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

    // Acesso por coordenada de tile de mundo (pode ser negativa).
    // Não carrega chunk ausente — retorna Air.
    Tile tileAt(int worldTileX, int worldTileY) const;
    bool isSolid(int worldTileX, int worldTileY) const;

    // Quebra um tile: vira Air, marca modified (persiste via LRU),
    // remove a entidade e devolve o tipo anterior em `broken`
    // (para partícula com a cor certa). Bedrock nunca quebra.
    // Chunk ausente -> false.
    bool breakTile(int worldTileX, int worldTileY, Tile *broken = nullptr);

    // Espalha a query (x, y, w, h) pelos hashes dos chunks
    // carregados. 'out' é limpo antes. Sem duplicatas entre chunks.
    void query(float x, float y, float w, float h, std::vector<Entity*> &out);

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
    std::vector<Entity*> activePlatforms_;
    std::vector<Entity*> activeColides_;
};

} // namespace support
