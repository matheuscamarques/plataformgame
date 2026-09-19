#pragma once

#include <unordered_map>
#include <vector>
#include <cstdint>
#include <utility>
#include <cmath>

class Entity;

namespace support {

// SpatialHash — grade uniforme para consulta espacial.
//
// Uso típico:
//   - Entidades estáticas (plataformas): insert() uma vez na criação.
//   - Entidades móveis (inimigos, projéteis): remove() ANTES de mover,
//     insert() DEPOIS de mover. Ou use update() se preferir.
//
// Consulta:
//   std::vector<Entity*> candidatos;
//   hash.query(x, y, w, h, candidatos);
//
// O vetor de saída é preenchido com entidades únicas (sem duplicatas),
// mesmo que uma entidade ocupe várias células.
//
// NOTA: guarda Entity* (não Handle). O desacoplamento para Handle+AABB
// vem na Fase C, junto com o EntityRegistry.
class SpatialHash {
public:
    explicit SpatialHash(float cellSize);
    ~SpatialHash() = default;

    // Não copiável — o hash é dono dos índices.
    SpatialHash(const SpatialHash&) = delete;
    SpatialHash& operator=(const SpatialHash&) = delete;

    // Insere usando os bounds atuais da entidade.
    void insert(Entity* e);

    // Remove usando os bounds atuais da entidade.
    // IMPORTANTE: para entidades móveis, chame ANTES de mover a entidade.
    void remove(Entity* e);

    // Atalho: remove + insert. Útil quando você sabe que a entidade
    // não mudou de posição, ou quando quer reindexar após mover.
    // Para entidades que se movem, prefira remove() antes e insert() depois.
    void update(Entity* e);

    // Remove todas as entidades de uma vez.
    void clear();

    // Preenche 'out' com todas as entidades que intersectam o retângulo
    // (x, y, w, h). 'out' é limpo antes. Sem duplicatas.
    void query(float x, float y, float w, float h,
               std::vector<Entity*>& out) const;

    // Preenche 'out' com os pares (cx, cy) das células que seriam
    // visitadas por uma query no mesmo retângulo. Só para debug visual.
    void debugCells(float x, float y, float w, float h,
                    std::vector<std::pair<int,int>>& out) const;

    // Quantas entidades há na célula (cx, cy). Para debug visual.
    int getCellCount(int cx, int cy) const;

    float getCellSize() const { return cellSize; }

    // Chave única para (cx, cy), funciona com coordenadas negativas.
    static int64_t key(int cx, int cy);

private:
    float cellSize;
    std::unordered_map<int64_t, std::vector<Entity*>> buckets;

    // Divisão inteira com arredondamento para baixo, correta para negativos.
    static int floorDiv(float v, float d);
};

} // namespace support
