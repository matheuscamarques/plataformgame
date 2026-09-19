#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <vector>

namespace support {

enum class BodyPartId : uint8_t {
    Head, Torso, ArmL, ArmR, LegL, LegR, Weapon,
    COUNT
};

struct PartDef {
    BodyPartId   id;
    sf::Vector2f offset;        // relativo ao centro do corpo
    sf::Vector2f size;          // hitbox da parte
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
};

struct Body {
    const BodySchema* schema = nullptr;
    std::vector<PartState> parts;
    int facing = 1;

    void attach(const BodySchema* s) {
        schema = s;
        parts.resize(s ? s->parts.size() : 0);
    }

    void rebuild(sf::Vector2f topLeftPos, int facing_);
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
