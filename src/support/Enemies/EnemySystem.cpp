#include "EnemySystem.h"

#include <algorithm>

#include "defines.h"
#include "game/SoundBank.h"
#include "support/Skills/SkillSystem.h"
#include "world/World.h"

namespace support {

void EnemySystem::spawn(const std::string &kind, float x, float y,
                        GameContext *ctx) {
    auto s = Factory::spawnEnemy(kind, x, y, ctx);
    if (s) slimes_.push_back(std::move(s));
}

void EnemySystem::forEach(const std::function<void(Enemy &)> &fn) {
    for (auto &s : slimes_) fn(*s);
}

void EnemySystem::removeDead(const std::function<void(sf::Vector2f)> &onDeath,
                             GameContext *ctx) {
    for (auto it = slimes_.begin(); it != slimes_.end(); ) {
        if (!(*it)->resources.isDead() && !(*it)->destroyPending) {
            ++it;
            continue;
        }
        if (ctx && (*it)->ai) (*it)->ai->onDeath(**it, *ctx);
        // SFX morte por kind (sem ctx.audio em teste = mudo).
        if (ctx && ctx->audio && (*it)->ai)
            ctx->audio->play(game::keyOf(
                (*it)->ai->kind() == core::EntityKind::Dwarf
                    ? game::Sfx::DwarfDeath : game::Sfx::SlimeDeath));
        sf::Vector2f pos{(*it)->body.getCenterX(), (*it)->body.getCenterY()};
        it = slimes_.erase(it);
        onDeath(pos);
    }
}

std::size_t EnemySystem::despawnFar(float x, float y, float radius) {
    std::size_t n = 0;
    const float r2 = radius * radius;
    for (auto it = slimes_.begin(); it != slimes_.end(); ) {
        const float dx = (*it)->body.getCenterX() - x;
        const float dy = (*it)->body.getCenterY() - y;
        if (dx * dx + dy * dy <= r2) { ++it; continue; }
        it = slimes_.erase(it);
        ++n;
    }
    return n;
}

void EnemySystem::tick(float dt, GameContext &ctx) {
    for (auto &s : slimes_) {
        // Recursos primeiro: o behavior já vê regen do frame e pode canPay.
        s->resources.tick(dt);
        s->knockbackLock.tick(dt);
        // Cooldowns de skill antes do behavior (ele vê o estado atualizado).
        SkillSystem::tick(*s, dt);
        // Lock rodando: física integra o impulso, IA não toca em vel.
        if (s->knockbackLock.running()) {
            physics(*s, ctx);
            continue;
        }
        if (s->ai) s->ai->onTick(*s, dt, ctx);
        physics(*s, ctx);
    }
}

void EnemySystem::physics(Enemy &s, GameContext &ctx) {
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
        if (c->getName() != core::kIdColide) continue;
        if (e.getBoundsBottom().intersects(*c)) {
            e.setY(c->getY() - e.getH());
            e.setVy(0.0f);
            s.grounded = true;
        }
    }
    if (s.ai) s.ai->setGrounded(s.grounded);
}

} // namespace support
