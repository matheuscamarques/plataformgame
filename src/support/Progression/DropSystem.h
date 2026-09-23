/**
 * @file src/support/Progression/DropSystem.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara sistema de drops com pools de XP e itens.
 * @details Define structs XPOrb e ItemOrb mais classe System com tick, render, spawn e clear e raios de magnet e coleta, usada pelo EnemySystem, Player e InventoryUI.
 */

#pragma once
#include "core/Inventory.h"
#include "core/Pool.h"
#include "core/System.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <cstddef>
#include <cstdint>

namespace support {

struct GameContext;

struct XPOrb {
    sf::Vector2f pos{0.f, 0.f};
    sf::Vector2f vel{0.f, 0.f};
    int          value = 1;
    float        lifetime = 0.f;
    float        maxLifetime = 12.f;
    bool         active = false;
    bool         magnetized = false;
};

// Orbe de item (fase 2): mesma física/magnet do XP, com payload de
// inventário. pickupDelay impede sugar no frame do spawn; lifetime 60s
// (chão guarda o que não coube). Render: rect na cor da raridade.
struct ItemOrb {
    core::Item   item;
    sf::Vector2f pos{0.f, 0.f};
    sf::Vector2f vel{0.f, 0.f};
    float        lifetime = 0.f;
    float        maxLifetime = 60.f;
    float        pickupDelay = 0.5f;
    bool         active = false;
    bool         magnetized = false;
};

// Orbs de XP: pool próprio, ciclo próprio — sem lógica de gameplay
// no ParticleSystem. Sem colisão com tiles no MVP (orb atravessa
// parede: se o slime morre na beirada, nada fica preso).
class DropSystem : public core::System {
public:
    const char *name() const override { return "DropSystem"; }
    int priority() const override { return 400; }

    void tick(float dt, GameContext &ctx) override;
    void render(sf::RenderTarget &target);

    XPOrb *spawnXP(sf::Vector2f pos, int value = 1);

    // Spawna orbe de item (def precisa existir; senão nullptr, sem custo).
    // Espalha: vel inicial aleatória p/ cima (visual, randRange de efeitos).
    ItemOrb *spawnItem(const std::string& defId, int qty, sf::Vector2f pos);

    std::size_t activeCount() const { return pool_.activeCount(); }
    std::size_t activeItemCount() const { return itemPool_.activeCount(); }
    int totalCollected() const { return collected_; }

    // Limpa orbes ativas (restart). collected_ é estatística da
    // sessão e sobrevive (generoso, modelo C não especifica).
    void clear() { pool_.releaseAll(); itemPool_.releaseAll(); }

private:
    core::Pool<XPOrb> pool_{128};
    core::Pool<ItemOrb> itemPool_{128};
    int collected_ = 0;

    static constexpr float kMagnetRadius  = 64.f;
    static constexpr float kCollectRadius = 12.f;
    static constexpr float kMagnetSpeed   = 280.f;
    static constexpr float kGravity       = 500.f;
};

} // namespace support
