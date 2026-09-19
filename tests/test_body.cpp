#include <cassert>
#include <cstdio>
#include "support/Body.h"

// Body por partes: schema, rebuild, facing, multiplicadores.
int main() {
    using namespace support;

    { // HumanoidHasSixParts
        auto s = BodySchema::humanoid();
        assert(s.parts.size() == 6u);
    }
    { // DwarfHasWeaponPart
        auto s = BodySchema::dwarf();
        bool hasWeapon = false;
        for (auto &p : s.parts)
            if (p.id == BodyPartId::Weapon) hasWeapon = true;
        assert(hasWeapon);
    }
    { // RebuildPlacesPartsRelativeToTopLeft
        Body b;
        auto s = BodySchema::humanoid(24.f, 12.f);
        b.attach(&s);
        b.rebuild({100.f, 100.f}, 1);
        const PartState *head = b.find(BodyPartId::Head);
        assert(head != nullptr);
        assert(head->worldBox.top < 100.f + 12.f); // cabeça acima do centro
    }
    { // FacingFlipsArms (ArmL vai para a direita com facing=-1)
        Body b;
        auto s = BodySchema::humanoid();
        b.attach(&s);
        b.rebuild({100.f, 100.f}, 1);
        float armL_facingRight = b.find(BodyPartId::ArmL)->worldBox.left;
        b.rebuild({100.f, 100.f}, -1);
        float armL_facingLeft = b.find(BodyPartId::ArmL)->worldBox.left;
        assert(armL_facingRight < armL_facingLeft);
    }
    { // HeadHasHigherDamageMultiplier
        auto s = BodySchema::humanoid();
        for (auto &p : s.parts) {
            if (p.id == BodyPartId::Head) assert(p.damageMult > 1.5f);
        }
    }
    { // ForEachIteratesAllParts
        Body b;
        auto s = BodySchema::humanoid();
        b.attach(&s);
        b.rebuild({0.f, 0.f}, 1);
        int count = 0;
        b.forEach([&](const PartState &, const PartDef &) { ++count; });
        assert(count == static_cast<int>(s.parts.size()));
    }

    std::printf("body test OK\n");
    return 0;
}
