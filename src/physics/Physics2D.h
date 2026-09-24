/**
 * @file src/physics/Physics2D.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementação 2D do PhysicsBackend sobre World (fundação).
 * @details Physics2D delega queries p/ World::isSolid e resolve AABB por eixo com substeps; único lugar fora de World que conhece colisão de tiles, incluído por sistemas futuros e testes.
 */

#pragma once

#include "physics/PhysicsBackend.h"

namespace support {
class World;
} // namespace support

namespace physics {

// Backend 2D: dono da referência do World (só physics/ conhece World
// p/ colisão — resto do jogo usa a interface). World outlives o backend.
class Physics2D : public PhysicsBackend {
public:
    explicit Physics2D(const support::World &world);

    bool isSolidTile(int tx, int ty) const override;
    MoveResult moveBody(Body &b, float dt, float gravity) override;
    RaycastHit raycast(core::Vec2f a, core::Vec2f b) const override;

private:
    // AABB centrado em c com meio-tamanhos h toca sólido?
    bool overlapsSolid(core::Vec2f c, core::Vec2f h) const;

    const support::World &world_;
};

} // namespace physics
