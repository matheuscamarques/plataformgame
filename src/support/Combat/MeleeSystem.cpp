/**
 * @file src/support/Combat/MeleeSystem.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Executa swing do player com hit único por swing e dano por parte.
 * @details Implementa tick que lê input Heavy e testa hitbox contra Body parts, chamado pelo loop com EnemySystem e ParticleSystem.
 */

#include "MeleeSystem.h"

#include <SFML/Graphics/Rect.hpp>

#include <algorithm>

#include "entities/Player/Player.h"
#include "game/SoundBank.h"
#include "support/Camera/Camera.h"
#include "support/Combat/Body.h"
#include "support/Debug/DebugFeed.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Input/InputMap.h"
#include "support/Effects/ParticleSystem.h"
#include "core/Vec.h"
#include "core/VecSfml.h"

namespace support {

namespace {
// Nome curto p/ log de debug (feed F4). Espelha kind(), sem string.
const char *enemyKindName(const Enemy &s) {
    if (!s.ai) return "?";
    switch (s.ai->kind()) {
        case core::EntityKind::Dwarf:    return "dwarf";
        case core::EntityKind::Skeleton: return "skeleton";
        case core::EntityKind::Slime:    return "slime";
        default:                         return "?"; // novo kind: adiciona caso
    }
}
const char *partName(BodyPartId id) {
    switch (id) {
        case BodyPartId::Head: return "head";
        case BodyPartId::Torso: return "torso";
        case BodyPartId::ArmL:
        case BodyPartId::ArmR: return "arm";
        case BodyPartId::LegL:
        case BodyPartId::LegR: return "leg";
        case BodyPartId::Weapon: return "weapon";
        default: return "?";
    }
}
} // namespace

void MeleeSystem::tick(float dt, GameContext &ctx) {
    Player *p = ctx.player;
    if (!p || !ctx.enemies) return;

    // K (Heavy) inicia/encadeia. Input pode ser nulo em testes
    // (swing disparado direto por startSwing).
    if (ctx.input && ctx.input->pressed(Action::Heavy)) p->startSwing();

    if (p->updateMelee(dt) != MeleePhase::Active) return;

    const sf::FloatRect box = p->meleeHitbox();
    if (box.width <= 0.f) return;

    const int dmgBase = p->meleeDamage();
    const float postureBase = p->meleePosture();
    ctx.enemies->forEach([&](Enemy &s) {
        if (s.resources.isDead()) return;
        if (s.lastHitSwing == p->meleeSwingId) return; // 1 hit por swing
        // Broadphase: união das partes posicionadas. Braços protuem
        // para fora do AABB do corpo (ArmL x∈[−4,0) relativo); AABB
        // puro os tornaria inalvejáveis — o jogador veria o braço e
        // o golpe passaria batido. Sem partes: AABB (legado/teste).
        const sf::FloatRect sb{s.body.getX(), s.body.getY(),
                               s.body.getW(), s.body.getH()};
        sf::FloatRect broad = sb;
        if (s.bodyParts.schema) {
            bool first = true;
            s.bodyParts.forEach([&](const PartState &st, const PartDef &) {
                if (st.worldBox.width <= 0.f || st.worldBox.height <= 0.f)
                    return;
                if (first) {
                    broad = st.worldBox;
                    first = false;
                    return;
                }
                const float l = std::min(broad.left, st.worldBox.left);
                const float t = std::min(broad.top, st.worldBox.top);
                const float r = std::max(broad.left + broad.width,
                                         st.worldBox.left + st.worldBox.width);
                const float b = std::max(broad.top + broad.height,
                                         st.worldBox.top + st.worldBox.height);
                broad = {l, t, r - l, b - t};
            });
        }
        if (!box.intersects(broad)) return;
        // Narrowphase: maior damageMult entre as partes tocadas
        // (mesma regra da explosão). Sem parte tocada:
        // com schema = whiff; sem schema = AABB 1x (legado).
        const PartDef *best = nullptr;
        sf::FloatRect bestBox = sb;
        float bestDmgMult = 0.f;
        s.bodyParts.forEach([&](const PartState &st, const PartDef &def) {
            if (st.fromSchema) return; // parte oculta: não é alvo
            if (!box.intersects(st.worldBox)) return;
            if (def.damageMult > bestDmgMult) {
                bestDmgMult = def.damageMult;
                best = &def;
                bestBox = st.worldBox;
            }
        });
        float dmgMult = 1.0f;
        float postureMult = 1.0f;
        if (best) {
            dmgMult = best->damageMult;
            postureMult = best->postureMult;
        } else if (s.bodyParts.schema) {
            return; // whiff: dentro do AABB, fora das partes
        }
        const int dmg = static_cast<int>(dmgBase * dmgMult);
        const int applied =
            s.resources.takeDamage(dmg, p->weaponBuffType);
        // SFX hit (só com dano; sem ctx.audio em teste = sem custo).
        if (applied > 0 && ctx.audio) {
            ctx.audio->play(game::keyOf(game::Sfx::MeleeHit), 0.8f);
            if (s.ai && s.ai->kind() == core::EntityKind::Dwarf)
                ctx.audio->play(game::keyOf(game::Sfx::DwarfHurt));
            else
                ctx.audio->play(game::keyOf(game::Sfx::SlimeHurt));
        }
        s.resources.damagePosture(postureBase * postureMult);
        if (applied > 0) {
            if (ctx.camera) ctx.camera->addTrauma(0.15f); // hit conecta
            if (s.ai) s.ai->onTakeHit(s, applied, ctx);
        }
        s.lastHitSwing = p->meleeSwingId;
        if (particles_) {
            // Faísca no centro da parte (feedback anatômico);
            // sem parte, no centro do corpo.
            particles_->spawnHitSpark(
                {bestBox.left + bestBox.width * 0.5f,
                 bestBox.top + bestBox.height * 0.5f});
        }
        // Feed de debug (F2 números, F4 log). Sem ctx.debug, sem custo.
        if (ctx.debug && applied > 0) {
            const core::Vec2f at{bestBox.left + bestBox.width * 0.5f,
                                  bestBox.top + bestBox.height * 0.5f};
            ctx.debug->pushNumber("-" + std::to_string(applied), at);
            ctx.debug->pushLog(std::string("melee ") + enemyKindName(s) +
                               " " + (best ? partName(best->id) : "body") +
                               " -" + std::to_string(applied) + " " +
                               core::damageTypeName(p->weaponBuffType) +
                               " (x" + std::to_string(
                                   s.resources.resistances.get(
                                       p->weaponBuffType)) + ")");
        }
    });
}

} // namespace support
