/**
 * @file src/support/Combat/ContactDamageSystem.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Aplica dano de contato com separação, telegraph e empurrão do slime.
 * @details Implementa tick com teste AABB, windup de mordida e pushback, chamado pelo loop após melee usando player e enemies.
 */

#include "ContactDamageSystem.h"

#include <algorithm>
#include <SFML/Graphics/Rect.hpp>

#include "entities/Player/Player.h"
#include "game/SoundBank.h"
#include "support/Debug/DebugFeed.h"
#include "support/Debug/ScreenshotSystem.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"

namespace support {

namespace {
constexpr int kContactDamage = 10;
constexpr float kPushBack = 6.f; // px, direto na posição
constexpr float kBiteWindup = 0.35f; // telegraph da mordida (Enemy::biteWindup)
constexpr float kWindupRecover = 0.5f; // fração que recupera sem contato

// União das partes vivas (pixels, não AABB): mesma fonte do MeleeSystem.
// fromSchema = ausente neste frame (Body.h) → pula; Weapon nunca é
// hurtbox (espada não morde de volta). Vazia (sem boxes: testes, frame
// 1) → false e o chamador cai no AABB legado.
bool aliveUnion(const Body& b, sf::FloatRect& out) {
    bool any = false;
    for (std::size_t i = 0; i < b.parts.size(); ++i) {
        const PartState& st = b.parts[i];
        if (st.fromSchema) continue;
        if (st.id == BodyPartId::Weapon) continue;
        if (st.worldBox.width <= 0.f || st.worldBox.height <= 0.f)
            continue;
        if (!any) {
            out = st.worldBox;
            any = true;
            continue;
        }
        const float l = std::min(out.left, st.worldBox.left);
        const float t = std::min(out.top, st.worldBox.top);
        out = {l, t,
               std::max(out.left + out.width,
                        st.worldBox.left + st.worldBox.width) -
                   l,
               std::max(out.top + out.height,
                        st.worldBox.top + st.worldBox.height) -
                   t};
    }
    return any;
}
} // namespace

void ContactDamageSystem::tick(float dt, GameContext &ctx) {
    Player *p = ctx.player;
    if (!p || !ctx.enemies) return;

    // Hurtbox = união das partes vivas (arte 30px, não AABB 60px:
    // antes o slime mordia 15px antes de encostar). Sem boxes (testes,
    // frame 1), cai no AABB legado. BodySystem (250) roda antes (310).
    const sf::FloatRect pbAABB{p->getX(), p->getY(), p->getW(), p->getH()};
    sf::FloatRect pbUnion{0.f, 0.f, 0.f, 0.f};
    const sf::FloatRect pb =
        aliveUnion(p->body, pbUnion) ? pbUnion : pbAABB;
    ctx.enemies->forEach([&](Enemy &s) {
        if (s.resources.isDead()) return;
        const sf::FloatRect sbAABB{s.body.getX(), s.body.getY(),
                                  s.body.getW(), s.body.getH()};
        sf::FloatRect sbUnion{0.f, 0.f, 0.f, 0.f};
        const sf::FloatRect sb =
            aliveUnion(s.bodyParts, sbUnion) ? sbUnion : sbAABB;
        if (!pb.intersects(sb)) {
            // Sem contato: windup recupera, cor volta ao ocioso.
            if (s.biteWindup < kBiteWindup) {
                s.biteWindup += dt * kWindupRecover;
                if (s.biteWindup >= kBiteWindup) {
                    s.biteWindup = kBiteWindup;
                    s.body.setFillColor(sf::Color(0, 200, 0));
                }
            }
            return;
        }
        // Separação: o slime é o convidado, sai pela menor ejeção
        // (distância até a borda, não profundidade da interseção).
        const float pushLeft  = (sb.left + sb.width) - pb.left;
        const float pushRight = (pb.left + pb.width) - sb.left;
        const float pushUp    = (sb.top + sb.height) - pb.top;
        const float pushDown  = (pb.top + pb.height) - sb.top;
        const float minX = std::min(pushLeft, pushRight);
        const float minY = std::min(pushUp, pushDown);
        if (minX <= minY) {
            const float dir = (pushLeft < pushRight) ? -1.f : 1.f;
            s.body.setX(s.body.getX() + dir * minX);
        } else {
            const float dir = (pushUp < pushDown) ? -1.f : 1.f;
            s.body.setY(s.body.getY() + dir * minY);
        }
        if (p->hp <= 0) return;
        // Telegraph: morde SÓ com windup esgotado (slime fica vermelho).
        s.body.setFillColor(sf::Color(220, 60, 60));
        s.biteWindup -= dt;
        if (s.biteWindup > 0.f) return;
        s.biteWindup = kBiteWindup;
        if (!p->hurt(kContactDamage)) return; // i-frame segurou, tenta de novo
        // Status (F7): slime aplica os dois (veneno forte, sangue fraco);
        // anão só sangra (forte).
        if (s.archetypeId == "slime") {
            p->addPoison(Player::kSlimePoison);
            p->addBleed(Player::kSlimeBleed);
        } else if (s.archetypeId == "dwarf") {
            p->addBleed(Player::kDwarfBleed);
        }
        // SFX mordida (dano aplicado; sem ctx.audio em teste = mudo).
        if (ctx.audio) ctx.audio->play(game::keyOf(game::Sfx::SlimeBite));
        if (ctx.screenshots)
            ctx.screenshots->notifyHurt({p->getCenterX(), p->getCenterY()});
        if (ctx.debug)
            ctx.debug->pushLog("hurt player -" +
                               std::to_string(kContactDamage));
        // Empurrão posicional: vel do Player legado é sobrescrita
        // todo frame pelos flags de movimento, impulso não persistiria.
        const float away = (p->getCenterX() < s.body.getCenterX()) ? -1.f : 1.f;
        p->setX(p->getX() + away * kPushBack);
    });
}

} // namespace support
