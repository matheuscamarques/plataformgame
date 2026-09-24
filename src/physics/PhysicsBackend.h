/**
 * @file src/physics/PhysicsBackend.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Interface abstrata de física 2D (fundação, sem SFML, sem World).
 * @details Declara Body, MoveResult, RaycastHit e PhysicsBackend com queries de tile, move-and-collide AABB e raycast; implementação em Physics2D, incluído por sistemas que precisam colidir sem conhecer World.
 */

#pragma once

#include "core/Vec.h"

namespace physics {

// Corpo dinâmico genérico (posição = centro; halfExtents = metade do AABB).
struct Body {
    core::Vec2f position{0.f, 0.f};
    core::Vec2f velocity{0.f, 0.f};
    core::Vec2f halfExtents{5.f, 5.f};
    bool grounded = false;
    bool isStatic = false;
};

// Resultado de um moveBody (flags p/ IA/animação, sem mudar semântica).
struct MoveResult {
    bool hitGround = false; // pousou neste passo
    bool hitCeil = false;   // bateu a cabeça
    bool hitWall = false;   // travou na lateral
};

// Acerto de raycast em grade de tiles.
struct RaycastHit {
    bool hit = false;
    core::Vec2f point{0.f, 0.f};
    core::Vec2f normal{0.f, 0.f};
    float distance = 0.f; // px da origem até o ponto
};

// Backend de física: queries + integração com colisão + raycast.
// Capacidade NOVA (nenhum sistema legado foi reescrito sobre ela —
// player/inimigos mantêm o resolve próprio e afinado; ThrowSystem usa
// isSolidTile). Futuros sistemas nascem aqui, sem conhecer World.
class PhysicsBackend {
public:
    virtual ~PhysicsBackend() = default;

    // Tile sólido em coords de tile (delega p/ World::isSolid).
    virtual bool isSolidTile(int tx, int ty) const = 0;

    // Integra com gravidade + colisão AABB por eixo (anti-túnel por
    // substeps). Atualiza position/velocity/grounded; retorna flags.
    virtual MoveResult moveBody(Body &b, float dt, float gravity) = 0;

    // Raio em grade (DDA): primeiro tile sólido entre a e b.
    // Sem acerto = hit=false (point/normal zerados, distance = |b-a|).
    virtual RaycastHit raycast(core::Vec2f a, core::Vec2f b) const = 0;
};

} // namespace physics
