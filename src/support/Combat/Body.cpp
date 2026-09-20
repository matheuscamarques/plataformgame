#include "Body.h"

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

void Body::rebuild(sf::Vector2f topLeftPos, int facing_) {
    facing = facing_;
    if (!schema) return;

    // Centro do corpo no mundo. topLeftPos é o canto superior esquerdo
    // do AABB (convenção SFML); o centro fica a overallHeight/2 abaixo
    // do topo e a halfWidth da esquerda.
    const sf::Vector2f center{
        topLeftPos.x + schema->halfWidth,
        topLeftPos.y + schema->overallHeight * 0.5f
    };

    for (std::size_t i = 0; i < schema->parts.size(); ++i) {
        const PartDef& d = schema->parts[i];
        PartState&     s = parts[i];
        s.id = d.id;

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

} // namespace support
