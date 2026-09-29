/**
 * @file src/support/Enemies/NamedAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IAs casca-vazia como aliases de template (dedup, item 1).
 * @details Substitui 14 headers idênticos (só kind()/name()): um
 * template parametrizado por base + EntityKind. Strings de name()
 * preservadas byte a byte (test_skeleton trava "SkeletonAI").
 * Registro continua em PackAI.cpp / SkeletonAI.cpp, sem mudanças.
 */

#pragma once

#include "DwarfAI.h"
#include "FlyingAI.h"
#include "SlimeAI.h"

namespace support {

// Nome exibido por kind (SFX, feed, contagem de cap).
template <core::EntityKind K> struct AiName;

#define SUPPORT_AI_NAME(Kind, Str)                \
    template <> struct AiName<core::EntityKind::Kind> { \
        static const char *value() { return Str; }      \
    }

SUPPORT_AI_NAME(Hollow, "HollowAI");
SUPPORT_AI_NAME(Rat, "RatAI");
SUPPORT_AI_NAME(Burst, "BurstAI");
SUPPORT_AI_NAME(Imp, "ImpAI");
SUPPORT_AI_NAME(Elemental, "ElementalAI");
SUPPORT_AI_NAME(Undead, "UndeadAI");
SUPPORT_AI_NAME(Harpy, "HarpyAI");
SUPPORT_AI_NAME(Eye, "DemonEyeAI");
SUPPORT_AI_NAME(Spider, "Spider");
SUPPORT_AI_NAME(Serpent, "Serpent");
SUPPORT_AI_NAME(Golem, "Golem");
SUPPORT_AI_NAME(Blaze, "Blaze");
SUPPORT_AI_NAME(Wraith, "Wraith");
SUPPORT_AI_NAME(Skeleton, "SkeletonAI");

#undef SUPPORT_AI_NAME

// Casca-vazia genérica: herda tudo da base, só identidade própria.
template <typename Base, core::EntityKind K>
class NamedAI : public Base {
public:
    NamedAI() = default;

    const char *name() const override { return AiName<K>::value(); }
    core::EntityKind kind() const override { return K; }
};

using HollowAI = NamedAI<DwarfAI, core::EntityKind::Hollow>;
using RatAI = NamedAI<SlimeAI, core::EntityKind::Rat>;
using BurstAI = NamedAI<DwarfAI, core::EntityKind::Burst>;
using ImpAI = NamedAI<DwarfAI, core::EntityKind::Imp>;
using ElementalAI = NamedAI<DwarfAI, core::EntityKind::Elemental>;
using UndeadAI = NamedAI<DwarfAI, core::EntityKind::Undead>;
using HarpyAI = NamedAI<FlyingAI, core::EntityKind::Harpy>;
using DemonEyeAI = NamedAI<FlyingAI, core::EntityKind::Eye>;
using SpiderAI = NamedAI<DwarfAI, core::EntityKind::Spider>;
using SerpentAI = NamedAI<DwarfAI, core::EntityKind::Serpent>;
using WraithAI = NamedAI<FlyingAI, core::EntityKind::Wraith>;
using GolemAI = NamedAI<DwarfAI, core::EntityKind::Golem>;
using BlazeAI = NamedAI<DwarfAI, core::EntityKind::Blaze>;
using SkeletonAI = NamedAI<DwarfAI, core::EntityKind::Skeleton>;

} // namespace support
