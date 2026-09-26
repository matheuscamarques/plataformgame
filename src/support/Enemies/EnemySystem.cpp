/**
 * @file src/support/Enemies/EnemySystem.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Opera spawn, iteração, física, IA e remoção por distância.
 * @details Implementa spawn, forEach, tick, removeDead e despawnFar com Skills e áudio, chamado pelo loop via GameContext.
 */

#include "EnemySystem.h"

#include <algorithm>

#include "defines.h"
#include "core/Coords.h"
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

void EnemySystem::removeDead(const std::function<void(Enemy&)> &onDeath,
                             GameContext *ctx) {
    for (auto it = slimes_.begin(); it != slimes_.end(); ) {
        if (!(*it)->resources.isDead() && !(*it)->destroyPending) {
            ++it;
            continue;
        }
        if (ctx && (*it)->ai) (*it)->ai->onDeath(**it, *ctx);
        // SFX morte por kind (sem ctx.audio em teste = mudo).
        if (ctx && ctx->audio && (*it)->ai)
            ctx->audio->play(game::keyOf(deathSfxFor((*it)->ai->kind())));
        onDeath(**it); // antes do erase (ref pendurada depois)
        it = slimes_.erase(it);
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
    separate();
}

void EnemySystem::separate() {
    // Inimigos não se atravessam: empurra posicional em pares pelo
    // menor eixo (metade p/ cada). Respeita knockbackLock (impulso),
    // mortos e marcados (removeDead cuida). Física do próximo tick
    // resolve parede (sem grudar em rocha).
    for (std::size_t i = 0; i < slimes_.size(); ++i) {
        for (std::size_t j = i + 1; j < slimes_.size(); ++j) {
            Enemy &a = *slimes_[i];
            Enemy &b = *slimes_[j];
            if (a.resources.isDead() || b.resources.isDead()) continue;
            if (a.destroyPending || b.destroyPending) continue;
            if (a.knockbackLock.running() || b.knockbackLock.running())
                continue;
            const float l = std::max(a.body.getX(), b.body.getX());
            const float r = std::min(a.body.getX() + a.body.getW(),
                                     b.body.getX() + b.body.getW());
            const float t = std::max(a.body.getY(), b.body.getY());
            const float bo = std::min(a.body.getY() + a.body.getH(),
                                      b.body.getY() + b.body.getH());
            const float ox = r - l;
            const float oy = bo - t;
            if (ox <= 0.f || oy <= 0.f) continue;
            if (ox < oy) {
                float s = (a.body.getCenterX() < b.body.getCenterX())
                              ? -1.f
                              : 1.f;
                if (a.body.getCenterX() == b.body.getCenterX()) s = -1.f;
                a.body.setX(a.body.getX() + s * ox * 0.5f);
                b.body.setX(b.body.getX() - s * ox * 0.5f);
            } else {
                float s = (a.body.getCenterY() < b.body.getCenterY())
                              ? -1.f
                              : 1.f;
                if (a.body.getCenterY() == b.body.getCenterY()) s = -1.f;
                a.body.setY(a.body.getY() + s * oy * 0.5f);
                b.body.setY(b.body.getY() - s * oy * 0.5f);
            }
        }
    }
}

void EnemySystem::physics(Enemy &s, GameContext &ctx) {
    Entity &e = s.body;
    // Voadores: integram direto, sem gravidade/chão. Se o centro cai
    // dentro de sólido, sobe (não atravanca em teto de caverna).
    if (s.ai && s.ai->ignoresGravity()) {
        e.setX(e.getX() + e.getVx());
        e.setY(e.getY() + e.getVy());
        s.grounded = false;
        if (ctx.world) {
            const core::TilePos tp = core::worldToTile(
                {e.getCenterX(), e.getCenterY()});
            if (ctx.world->isSolid(tp.x, tp.y)) e.setY(e.getY() - 4.f);
        }
        return;
    }
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
