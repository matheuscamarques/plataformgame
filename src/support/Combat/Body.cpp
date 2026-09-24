/**
 * @file src/support/Combat/Body.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementa schemas humanoide e anão e reconstrução das hitboxes por parte.
 * @details Define BodySchema humanoid e dwarf mais Body rebuild e rebuildFromSprite, usado por BodySystem e Factory via BodySchemaRegistry.
 */

#include "Body.h"

#include <cmath>

#include "core/sprite_from_ascii.h"

namespace support {

BodySchema BodySchema::humanoid(float height, float width) {
    BodySchema s;
    s.overallHeight = height;
    s.halfWidth     = width * 0.5f;

    const float hw = width * 0.5f;

    s.parts = {
        // Head — topo, damageMult 2.0
        { BodyPartId::Head,  {0.f, -height * 0.35f}, {width * 0.6f, height * 0.22f}, 2.0f, 1.5f, false },
        // Torso — centro
        { BodyPartId::Torso, {0.f, -height * 0.05f}, {width,        height * 0.32f}, 1.0f, 1.0f, false },
        // Arms
        { BodyPartId::ArmL,  {-hw - 2.f, -height * 0.08f}, {4.f, height * 0.28f}, 0.6f, 0.5f, true },
        { BodyPartId::ArmR,  { hw + 2.f, -height * 0.08f}, {4.f, height * 0.28f}, 0.6f, 0.5f, true },
        // Legs
        { BodyPartId::LegL,  {-hw * 0.5f, height * 0.28f}, {width * 0.4f, height * 0.30f}, 0.7f, 0.8f, true },
        { BodyPartId::LegR,  { hw * 0.5f, height * 0.28f}, {width * 0.4f, height * 0.30f}, 0.7f, 0.8f, true },
        // Weapon — slot zerado; BodySystem preenche por frame quando há
        // arma (computeWeaponBbox no Game). Mult 1.0 para não mudar dano
        // de explosão em quem usa humanoid (slime).
        { BodyPartId::Weapon, {0.f, 0.f}, {0.1f, 0.1f}, 1.0f, 1.0f, false },
    };
    return s;
}

BodySchema BodySchema::dwarf(float height, float width) {
    // Baixo e largo. Braço direito segura picareta (Weapon).
    BodySchema s = humanoid(height, width);
    s.parts.push_back(
        { BodyPartId::Weapon, {width * 0.7f, -height * 0.02f}, {6.f, height * 0.5f}, 1.2f, 1.0f, false }
    );
    return s;
}

void Body::rebuild(core::Vec2f topLeftPos, int facing_) {
    facing = facing_;
    if (!schema) return;

    // Centro do corpo no mundo. topLeftPos é o canto superior esquerdo
    // do AABB (convenção SFML); o centro fica a overallHeight/2 abaixo
    // do topo e a halfWidth da esquerda.
    const core::Vec2f center{
        topLeftPos.x + schema->halfWidth,
        topLeftPos.y + schema->overallHeight * 0.5f
    };

    for (std::size_t i = 0; i < schema->parts.size(); ++i) {
        const PartDef& d = schema->parts[i];
        PartState&     s = parts[i];
        s.id = d.id;
        s.fromSchema = false; // rebuild(): schema É a fonte — não é "ausente"

        const float ox = d.offset.x * static_cast<float>(facing);
        const float oy = d.offset.y;

        s.worldBox = {
            center.x + ox - d.size.x * 0.5f,
            center.y + oy - d.size.y * 0.5f,
            d.size.x,
            d.size.y
        };
    }
}

const PartState* Body::find(BodyPartId id) const {
    for (const auto& p : parts)
        if (p.id == id) return &p;
    return nullptr;
}

void Body::rebuildFromSprite(
    core::Vec2f entityTopLeft, core::Vec2f aabbSize,
    const char* const* rows, int sw, int sh,
    const core::PaletteEntry* pal, std::size_t palN,
    int facing_)
{
    if (!schema || !rows || sw <= 0 || sh <= 0) return;
    facing = facing_;

    // NOTA: entrada não-proporcional é legal (slime 40x30 com sprite
    // 14x12). A derivação usa escala uniforme por `sh`; caixas só saem
    // mais estreitas que o AABB — hitbox = pixels, por design. Um assert
    // de proporcionalidade aqui quebrava o boot (slime), então não há.

    const bool same = (rows == cachedRows)
                   && (sw == cachedW) && (sh == cachedH)
                   && (facing_ == cachedFacing);

    if (!same) {
        cachedRows = rows;
        cachedW = sw;
        cachedH = sh;
        cachedFacing = facing_;
        cachedRelBoxes.assign(schema->parts.size(), sf::FloatRect{});

        for (std::size_t pi = 0; pi < schema->parts.size(); ++pi) {
            const core::BodyPartId target = schema->parts[pi].id;
            int minX = sw, minY = sh, maxX = -1, maxY = -1;

            for (int y = 0; y < sh; ++y) {
                const char* row = rows[y];
                for (int x = 0; x < sw; ++x) {
                    core::BodyPartId cp = core::BodyPartId::None;
                    for (std::size_t k = 0; k < palN; ++k) {
                        if (pal[k].ch == row[x]) { cp = pal[k].part; break; }
                    }
                    if (cp != target) continue;
                    if (x < minX) minX = x;
                    if (y < minY) minY = y;
                    if (x > maxX) maxX = x;
                    if (y > maxY) maxY = y;
                }
            }

            if (maxX < 0) {
                cachedRelBoxes[pi] = {-1e6f, -1e6f, 0.f, 0.f}; // fallback
            } else {
                cachedRelBoxes[pi] = {
                    static_cast<float>(minX) - sw * 0.5f,
                    static_cast<float>(minY) - sh * 0.5f,
                    static_cast<float>(maxX - minX + 1),
                    static_cast<float>(maxY - minY + 1)
                };
            }
        }
    }

    parts.resize(schema->parts.size());
    const float scale = aabbSize.y / static_cast<float>(sh);
    const float cx = entityTopLeft.x + aabbSize.x * 0.5f;
    const float cy = entityTopLeft.y + aabbSize.y * 0.5f;

    for (std::size_t i = 0; i < schema->parts.size(); ++i) {
        const PartDef& def = schema->parts[i];
        const sf::FloatRect& rel = cachedRelBoxes[i];
        PartState& s = parts[i];
        s.id = def.id;

        if (rel.left < -1e5f) {
            // Fallback: schema estático (comportamento antigo).
            s.worldBox = {
                cx + def.offset.x * static_cast<float>(facing) - def.size.x * 0.5f,
                cy + def.offset.y - def.size.y * 0.5f,
                def.size.x, def.size.y
            };
            s.fromSchema = true;
            continue;
        }
        s.fromSchema = false;

        const float rL = rel.left, rR = rel.left + rel.width;
        float wL, wR;
        if (facing >= 0) {
            wL = cx + rL * scale;
            wR = cx + rR * scale;
        } else {
            wL = cx - rR * scale;
            wR = cx - rL * scale;
        }

        s.worldBox = {
            wL,
            cy + rel.top * scale,
            wR - wL,
            rel.height * scale
        };
    }
}

} // namespace support
