/**
 * @file src/support/Combat/Body.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara PartDef, BodySchema, PartState e Body para combate por partes.
 * @details Define dados de hitbox com multiplicadores mais attach, rebuild e find, incluído por BodySystem, Melee e Explosion.
 */

#pragma once

#include "core/Vec.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "core/BodyPart.h"

namespace core {
struct PaletteEntry;
}

namespace support {

using core::BodyPartId;

struct PartDef {
    BodyPartId   id;
    core::Vec2f  offset;        // relativo ao centro do corpo
    core::Vec2f  size;          // hitbox da parte
    float        damageMult  = 1.0f;
    float        postureMult = 1.0f;
    bool         breakable   = false;
};

struct BodySchema {
    std::vector<PartDef> parts;
    float                overallHeight = 24.f;
    float                halfWidth     = 6.f;

    // Schema pronto para humanoide. Player e inimigos herdam e ajustam.
    static BodySchema humanoid(float height = 24.f, float width = 12.f);
    // Baixo e largo, com Weapon (picareta/arco). Para o anão (futuro).
    static BodySchema dwarf(float height = 18.f, float width = 14.f);
};

// Estado de runtime: rect global da parte neste frame.
// Sem heap por frame: vector alocado uma vez em attach().
struct PartState {
    BodyPartId    id = BodyPartId::Torso;
    sf::FloatRect worldBox;
    // fromSchema = true significa: corpo sprite-driven (rebuildFromSprite)
    // mas a parte NÃO tem pixels no sprite deste frame.
    // Semântica: parte inexistente neste frame. Consumidores DEVEM pular:
    //   - Renderer (drawParts, drawPlayerEquipment): não desenhar
    //   - MeleeSystem, ExplosionSystem: não considerar alvo
    // Body::rebuild() (sem sprite: testes, corpos estáticos) sempre deixa
    // false — ali o schema É a fonte válida, não "ausência".
    // NOTA: worldBox NÃO é zerado no fallback (mantém o valor do schema:
    // computeWeaponBbox/drawPlayerWeapon leem ArmR incondicionalmente).
    bool fromSchema = false;
};

struct Body {
    const BodySchema* schema = nullptr;
    std::vector<PartState> parts;
    int facing = 1;

    // Cache da derivação por sprite: só re-varre se frame/facing mudar.
    const char* const* cachedRows = nullptr;
    int cachedW = 0, cachedH = 0, cachedFacing = 0;
    std::vector<sf::FloatRect> cachedRelBoxes; // em coords de sprite

    void attach(const BodySchema* s) {
        schema = s;
        parts.resize(s ? s->parts.size() : 0);
    }

    void rebuild(core::Vec2f topLeftPos, int facing_);

    // Hitbox = bbox dos pixels de cada parte no sprite atual.
    // Sem pixels p/ parte => fallback no schema (comportamento antigo).
    void rebuildFromSprite(
        core::Vec2f entityTopLeft, core::Vec2f aabbSize,
        const char* const* rows, int spriteW, int spriteH,
        const core::PaletteEntry* pal, std::size_t palCount,
        int facing_);
    const PartState* find(BodyPartId id) const;

    // Itera (estado, definição) em lockstep.
    template <typename F>
    void forEach(F&& fn) const {
        if (!schema) return;
        for (std::size_t i = 0; i < parts.size(); ++i)
            fn(parts[i], schema->parts[i]);
    }
};

} // namespace support
