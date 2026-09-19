#include "ContactDamageSystem.h"

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
    if (!p || !ctx.enemies || p->hp <= 0) return;

    const sf::FloatRect pb{p->getX(), p->getY(), p->getW(), p->getH()};
    ctx.enemies->forEach([&](Slime &s) {
        if (s.resources.isDead()) return;
        const sf::FloatRect sb{s.body.getX(), s.body.getY(),
                               s.body.getW(), s.body.getH()};
        if (!pb.intersects(sb)) return;
        if (!p->hurt(kContactDamage)) return; // i-frame segurou
        // Empurrão posicional: vel do Player legado é sobrescrita
        // todo frame pelos flags de movimento, impulso não persistiria.
        const float away = (p->getCenterX() < s.body.getCenterX()) ? -1.f : 1.f;
        p->setX(p->getX() + away * kPushBack);
    });
}

} // namespace support
