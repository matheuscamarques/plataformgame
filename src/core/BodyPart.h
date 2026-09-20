#pragma once
#include <cstdint>

namespace core {

// Dono do enum: core (nunca o contrário — target test-layers no
// Makefile trava dependência de core para support).
enum class BodyPartId : uint8_t {
    None = 0, // pixel sem dono / parte ausente
    Head,
    Torso,
    ArmL,
    ArmR,
    LegL,
    LegR,
    Weapon,
    COUNT
};

} // namespace core
