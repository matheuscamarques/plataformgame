#include "EnemySystem.h"

#include <algorithm>

#include "../defines.h"
#include "World/World.h"

namespace support {

void EnemySystem::spawn(const std::string &kind, float x, float y) {
    auto s = Factory::spawnEnemy(kind, x, y);
    if (s) slimes_.push_back(std::move(s));
}

void EnemySystem::forEach(const std::function<void(Slime &)> &fn) {
    for (auto &s : slimes_) fn(*s);
}

void EnemySystem::tick(float dt, GameContext &ctx) {
    for (auto &s : slimes_) {
        if (s->ai) s->ai->onTick(s->body, dt, ctx);
        physics(*s, ctx);
    }
}

void EnemySystem::physics(Slime &s, GameContext &ctx) {
    Entity &e = s.body;
    e.setVy(std::min(e.getVy() + 9.8f, 20.0f));
    e.setX(e.getX() + e.getVx());
    e.setY(e.getY() + e.getVy());
    s.grounded = false;

    if (!ctx.world) return;
    std::vector<Entity *> around;
    ctx.world->query(e.getX() - 2.0f, e.getY() - 2.0f,
                     e.getW() + 4.0f, e.getH() + 4.0f, around);
    for (Entity *c : around) {
        if (c->getName() != COLIDE) continue;
        if (e.getBoundsBottom().intersects(*c)) {
            e.setY(c->getY() - e.getH());
            e.setVy(0.0f);
            s.grounded = true;
        }
    }
    if (s.ai) s.ai->setGrounded(s.grounded);
}

} // namespace support
