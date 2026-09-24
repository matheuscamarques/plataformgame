/**
 * @file src/physics/Physics2D.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementa Physics2D: queries no World, AABB por eixo e DDA.
 * @details moveBody integra gravidade com substeps anti-túnel e resolve X depois Y (pouso seta grounded); raycast é DDA em grade com normal da face, usado por Physics2D e testes de fundação.
 */

#include "physics/Physics2D.h"

#include <algorithm>
#include <cmath>

#include "core/Config.h"
#include "world/World.h"

namespace physics {

namespace {

int worldToTile(float w) {
    return static_cast<int>(
        std::floor(w / static_cast<float>(core::kBlockSize)));
}

} // namespace

Physics2D::Physics2D(const support::World &world) : world_(world) {}

bool Physics2D::isSolidTile(int tx, int ty) const {
    return world_.isSolid(tx, ty);
}

bool Physics2D::overlapsSolid(core::Vec2f c, core::Vec2f h) const {
    const int x0 = worldToTile(c.x - h.x);
    const int x1 = worldToTile(c.x + h.x);
    const int y0 = worldToTile(c.y - h.y);
    const int y1 = worldToTile(c.y + h.y);
    for (int ty = y0; ty <= y1; ++ty)
        for (int tx = x0; tx <= x1; ++tx)
            if (world_.isSolid(tx, ty)) return true;
    return false;
}

MoveResult Physics2D::moveBody(Body &b, float dt, float gravity) {
    MoveResult r;
    if (b.isStatic || dt <= 0.f) return r;

    // Repouso estável: parado e com sólido 1px abaixo = continua
    // grounded sem reintegrar (evita hitGround todo frame parado).
    if (b.velocity.x == 0.f && b.velocity.y == 0.f) {
        core::Vec2f probe{b.position.x, b.position.y + 1.f};
        if (overlapsSolid(probe, b.halfExtents)) {
            b.grounded = true;
            return r;
        }
    }

    b.velocity.y += gravity * dt;
    b.grounded = false;

    // Substeps anti-túnel: nenhum passo anda mais que metade do menor
    // semi-eixo (ou meio tile, o que for menor).
    const float dist =
        std::sqrt(b.velocity.x * b.velocity.x + b.velocity.y * b.velocity.y) *
        dt;
    const float minHalf =
        std::min({b.halfExtents.x, b.halfExtents.y,
                  static_cast<float>(core::kBlockSize) * 0.5f});
    const int steps =
        std::max(1, std::min(32, static_cast<int>(std::ceil(dist / std::max(minHalf, 1.f)))));
    const float sdt = dt / static_cast<float>(steps);

    for (int i = 0; i < steps; ++i) {
        // Eixo X primeiro (parede não gruda no chão).
        if (b.velocity.x != 0.f) {
            core::Vec2f next{b.position.x + b.velocity.x * sdt,
                             b.position.y};
            if (overlapsSolid(next, b.halfExtents)) {
                b.velocity.x = 0.f;
                r.hitWall = true;
            } else {
                b.position.x = next.x;
            }
        }
        // Eixo Y depois (pouso/teto).
        if (b.velocity.y != 0.f) {
            core::Vec2f next{b.position.x,
                             b.position.y + b.velocity.y * sdt};
            if (overlapsSolid(next, b.halfExtents)) {
                if (b.velocity.y > 0.f) {
                    b.grounded = true;
                    r.hitGround = true;
                    // Snap de contato: avança de 1px até encostar (repouso
                    // exato, sem afundar no sólido nem flutuar no substep).
                    for (int k = 0; k < 64; ++k) {
                        core::Vec2f down{b.position.x, b.position.y + 1.f};
                        if (overlapsSolid(down, b.halfExtents)) break;
                        b.position.y = down.y;
                    }
                } else {
                    r.hitCeil = true;
                }
                b.velocity.y = 0.f;
            } else {
                b.position.y = next.y;
            }
        } else {
            // Parado no ar sobre sólido = continua grounded (repouso).
            core::Vec2f probe{b.position.x, b.position.y + 1.f};
            if (overlapsSolid(probe, b.halfExtents)) b.grounded = true;
        }
    }
    return r;
}

RaycastHit Physics2D::raycast(core::Vec2f a, core::Vec2f b) const {
    RaycastHit out;
    const float total =
        std::sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
    out.distance = total;
    if (total <= 0.f) return out;

    // DDA de Amanatides & Woo em coords de tile.
    const float dx = (b.x - a.x) / total;
    const float dy = (b.y - a.y) / total;
    int tx = worldToTile(a.x);
    int ty = worldToTile(a.y);
    const int stepX = (dx > 0.f) ? 1 : -1;
    const int stepY = (dy > 0.f) ? 1 : -1;
    const float bs = static_cast<float>(core::kBlockSize);
    const float tDeltaX =
        (dx != 0.f) ? std::fabs(bs / dx) : 1e9f;
    const float tDeltaY =
        (dy != 0.f) ? std::fabs(bs / dy) : 1e9f;
    const float firstX = (dx > 0.f)
        ? ((tx + 1) * bs - a.x) / dx
        : (a.x - tx * bs) / -dx;
    const float firstY = (dy > 0.f)
        ? ((ty + 1) * bs - a.y) / dy
        : (a.y - ty * bs) / -dy;
    float tMaxX = (dx != 0.f) ? firstX : 1e9f;
    float tMaxY = (dy != 0.f) ? firstY : 1e9f;

    float t = 0.f;
    core::Vec2f normal{0.f, 0.f};
    for (int i = 0; i < 512; ++i) {
        if (world_.isSolid(tx, ty)) {
            out.hit = true;
            out.distance = std::min(t, total);
            out.point = {a.x + dx * out.distance, a.y + dy * out.distance};
            out.normal = normal;
            return out;
        }
        if (tMaxX < tMaxY) {
            t = tMaxX;
            tMaxX += tDeltaX;
            tx += stepX;
            normal = {static_cast<float>(-stepX), 0.f};
        } else {
            t = tMaxY;
            tMaxY += tDeltaY;
            ty += stepY;
            normal = {0.f, static_cast<float>(-stepY)};
        }
        if (t > total) break;
    }
    return out;
}

} // namespace physics
