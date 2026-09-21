#include "ContactDamageSystem.h"

#include <algorithm>
#include <SFML/Graphics/Rect.hpp>

#include "entities/Player/Player.h"
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
} // namespace

void ContactDamageSystem::tick(float dt, GameContext &ctx) {
    Player *p = ctx.player;
    if (!p || !ctx.enemies) return;

    const sf::FloatRect pb{p->getX(), p->getY(), p->getW(), p->getH()};
    ctx.enemies->forEach([&](Enemy &s) {
        if (s.resources.isDead()) return;
        sf::FloatRect sb{s.body.getX(), s.body.getY(),
                         s.body.getW(), s.body.getH()};
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
