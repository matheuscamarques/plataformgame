/**
 * @file src/support/Spatial/spatialhash.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementa grade uniforme para inserir, remover e consultar entidades.
 * @details Define insert, remove, update e query com chave int64 de célula, usado por física e colisão via Entity bounds.
 */

#include "spatialhash.h"
#include "entities/Entity.hpp"

namespace support {

SpatialHash::SpatialHash(float cellSize)
    : cellSize(cellSize)
{
}

int SpatialHash::floorDiv(float v, float d) {
    return static_cast<int>(std::floor(v / d));
}

int64_t SpatialHash::key(int cx, int cy) {
    // Combina dois int32 numa única chave int64.
    // O cast para uint32_t garante que cy negativo não invada os bits de cx.
    return (static_cast<int64_t>(cx) << 32) | static_cast<int64_t>(static_cast<uint32_t>(cy));
}

void SpatialHash::insert(Entity* e) {
    int cx0 = floorDiv(e->getX(), cellSize);
    int cy0 = floorDiv(e->getY(), cellSize);
    int cx1 = floorDiv(e->getX() + e->getW(), cellSize);
    int cy1 = floorDiv(e->getY() + e->getH(), cellSize);

    for (int cy = cy0; cy <= cy1; ++cy) {
        for (int cx = cx0; cx <= cx1; ++cx) {
            buckets[key(cx, cy)].push_back(e);
        }
    }
}

void SpatialHash::remove(Entity* e) {
    int cx0 = floorDiv(e->getX(), cellSize);
    int cy0 = floorDiv(e->getY(), cellSize);
    int cx1 = floorDiv(e->getX() + e->getW(), cellSize);
    int cy1 = floorDiv(e->getY() + e->getH(), cellSize);

    for (int cy = cy0; cy <= cy1; ++cy) {
        for (int cx = cx0; cx <= cx1; ++cx) {
            auto it = buckets.find(key(cx, cy));
            if (it == buckets.end()) continue;

            auto& v = it->second;
            for (auto vit = v.begin(); vit != v.end(); ++vit) {
                if (*vit == e) {
                    v.erase(vit);
                    break;
                }
            }
            if (v.empty()) {
                buckets.erase(it);
            }
        }
    }
}

void SpatialHash::update(Entity* e) {
    remove(e);
    insert(e);
}

void SpatialHash::clear() {
    buckets.clear();
}

void SpatialHash::query(float x, float y, float w, float h,
                        std::vector<Entity*>& out) const
{
    out.clear();

    int cx0 = floorDiv(x, cellSize);
    int cy0 = floorDiv(y, cellSize);
    int cx1 = floorDiv(x + w, cellSize);
    int cy1 = floorDiv(y + h, cellSize);

    for (int cy = cy0; cy <= cy1; ++cy) {
        for (int cx = cx0; cx <= cx1; ++cx) {
            auto it = buckets.find(key(cx, cy));
            if (it == buckets.end()) continue;

            for (Entity* e : it->second) {
                // Deduplica: uma entidade pode ocupar várias células.
                bool dup = false;
                for (Entity* o : out) {
                    if (o == e) { dup = true; break; }
                }
                if (!dup) out.push_back(e);
            }
        }
    }
}

void SpatialHash::debugCells(float x, float y, float w, float h,
                             std::vector<std::pair<int,int>>& out) const
{
    out.clear();

    int cx0 = floorDiv(x, cellSize);
    int cy0 = floorDiv(y, cellSize);
    int cx1 = floorDiv(x + w, cellSize);
    int cy1 = floorDiv(y + h, cellSize);

    for (int cy = cy0; cy <= cy1; ++cy) {
        for (int cx = cx0; cx <= cx1; ++cx) {
            out.push_back({cx, cy});
        }
    }
}

int SpatialHash::getCellCount(int cx, int cy) const {
    auto it = buckets.find(key(cx, cy));
    if (it == buckets.end()) return 0;
    return static_cast<int>(it->second.size());
}

} // namespace support
