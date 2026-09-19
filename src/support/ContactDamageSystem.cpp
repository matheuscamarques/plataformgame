#include "ContactDamageSystem.h"

#include <algorithm>
#include <SFML/Graphics/Rect.hpp>

#include "../entities/player/player.h"
#include "EnemySystem.h"
#include "GameContext.h"

namespace support {

namespace {
constexpr int kContactDamage = 10;
constexpr float kPushBack = 6.f; // px, direto na posição
} // namespace

void ContactDamageSystem::tick(float /*dt*/, GameContext &ctx) {
    Player *p = ctx.player;
    if (!p || !ctx.enemies) return;

    const sf::FloatRect pb{p->getX(), p->getY(), p->getW(), p->getH()};
    ctx.enemies->forEach([&](Slime &s) {
        if (s.resources.isDead()) return;
        sf::FloatRect sb{s.body.getX(), s.body.getY(),
                         s.body.getW(), s.body.getH()};
        if (!pb.intersects(sb)) return;
        // Separação: o slime é o convidado, sai pela menor ejeção
        // (distância até a borda, não profundidade da interseção).
        // Roda mesmo com player em 0 HP (morte vem no B).
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
        if (!p->hurt(kContactDamage)) return; // i-frame segurou
        // Empurrão posicional: vel do Player legado é sobrescrita
        // todo frame pelos flags de movimento, impulso não persistiria.
        const float away = (p->getCenterX() < s.body.getCenterX()) ? -1.f : 1.f;
        p->setX(p->getX() + away * kPushBack);
    });
}

} // namespace support
