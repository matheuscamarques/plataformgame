#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../core/System.h"
#include "../entities/entity/entity.hpp"
#include "Behavior.h"
#include "Body.h"
#include "EnemyResources.h"
#include "GameContext.h"
#include "PatienceSystem.h"
#include "../core/Cooldown.h"

namespace support {

class Behavior;

// Enemy = corpo (Entity) + identidade (Behavior via registry).
// Dono: EnemySystem. Física: EnemySystem. Desenho: Game via forEach.
struct Enemy {
    Entity body;
    std::unique_ptr<Behavior> ai;
    EnemyResources resources;
    Body bodyParts;
    bool grounded = false;
    // IA não sobrescreve vel enquanto roda (knockback visível).
    core::Cooldown knockbackLock;
    // Último swing de melee que acertou (1 hit por swing por slime).
    int lastHitSwing = -1;
    // Telegraph da mordida: conta de BITE_WINDUP até 0 SÓ em contato;
    // sem contato recupera (0.5x). Cheio = ocioso (verde).
    float biteWindup = 0.35f;
    // Cooldowns de skills (SkillSystem, opção 3b): morrem com o Enemy.
    std::unordered_map<std::string, core::Cooldown> skillCds;
    // Skills do archetype (Factory copia). Vazio = behavior manual.
    std::vector<std::string> skillIds;
    // Última skill usada (UtilityAI penaliza repetição).
    std::string lastSkillId;
    float lastSkillAge = 999.f;
    // Variante por profundidade (Factory aplica; Elite escala aqui).
    int variantLevel = 1;
    float damageMult = 1.f;
    // Barks (texto com fade; áudio futuro).
    std::string currentBark;
    float barkTimer = 0.f;
    core::Cooldown barkCd{1.5f};
    // Pavio curto + vínculo (DwarfAI tica; eventos entregam).
    PatienceState patience;

    Enemy(Entity b, std::unique_ptr<Behavior> a)
        : body(std::move(b)), ai(std::move(a)) {}
};

// Factory consome ArchetypeRegistry por string: inimigo novo = 1
// REGISTER_ENEMY_ARCHETYPE + 1 Behavior. Zero branch, zero edição aqui.
class Factory {
public:
    // kind desconhecido OU behavior ausente -> nullptr (nunca crash).
    // ctx opcional: com ctx, dispara ai->onSpawn (SpawnSystem passa).
    static std::unique_ptr<Enemy> spawnEnemy(const std::string &kind,
                                             float x, float y,
                                             GameContext *ctx = nullptr);
};

// Opera os slimes: IA + física (gravidade, snap no chão via SpatialHash).
// priority 150 (depois de input, junto da IA).
class EnemySystem : public core::System {
public:
    const char *name() const override { return "EnemySystem"; }
    int priority() const override { return 150; }
    void tick(float dt, GameContext &ctx) override;

    // kind desconhecido = ignorado (Factory retorna null).
    // ctx repassado à Factory p/ onSpawn (SpawnSystem passa o seu).
    void spawn(const std::string &kind, float x, float y,
               GameContext *ctx = nullptr);

    void forEach(const std::function<void(Enemy &)> &fn);
    std::size_t count() const { return slimes_.size(); }

    // Limpa todos (restart da run). Slimes iniciais respawnam pelo caller.
    void clear() { slimes_.clear(); }

    // Remove mortos; onDeath(pos do centro) por removido para juice
    // (partículas/drops no DeathSystem). Erase mora aqui, no dono.
    // Com ctx, dispara ai->onDeath antes do erase (hook opcional).
    void removeDead(const std::function<void(sf::Vector2f)> &onDeath,
                    GameContext *ctx = nullptr);

    // Despawn por distância (economia do SpawnSystem). Remove além do
    // raio (px) do ponto. Retorna quantos removeu.
    std::size_t despawnFar(float x, float y, float radius);

private:
    void physics(Enemy &s, GameContext &ctx);

    std::vector<std::unique_ptr<Enemy>> slimes_;
};

} // namespace support
