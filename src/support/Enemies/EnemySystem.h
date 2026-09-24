/**
 * @file src/support/Enemies/EnemySystem.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara Enemy, Factory e o sistema dono de corpos e IAs.
 * @details Define struct Enemy com Body e Behavior mais Factory spawnEnemy e EnemySystem prioridade 150, usado por Game e Spawns.
 */

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/System.h"
#include "entities/Entity.hpp"
#include "Behavior.h"
#include "support/Combat/Body.h"
#include "support/Combat/SpriteFrame.h"
#include "EnemyResources.h"
#include "support/GameContext.h"
#include "support/Progression/PatienceSystem.h"
#include "core/Cooldown.h"

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
    // Id do archetype que gerou ("slime", "dwarf"...): dirige drops.
    // Preenchido pela Factory; vazio = sem drops (só XP).
    std::string archetypeId;
    // Morte adiada: marcado durante iteração (forEach/tick), varrido no
    // removeDead. Nunca erase no meio do loop (camada 7).
    bool destroyPending = false;
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
    // Frame atual (id leve). Game::tick preenche; BodySystem deriva
    // hitboxes; render mapeia p/ textura. None = fallback schema.
    SpriteFrameId currentFrameId = SpriteFrameId::None;

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

    // Empurra pares sobrepostos p/ fora (meio a meio, menor eixo).
    // Headless-testável; tick chama no fim (após física de todos).
    void separate();

    // kind desconhecido = ignorado (Factory retorna null).
    // ctx repassado à Factory p/ onSpawn (SpawnSystem passa o seu).
    void spawn(const std::string &kind, float x, float y,
               GameContext *ctx = nullptr);

    void forEach(const std::function<void(Enemy &)> &fn);
    std::size_t count() const { return slimes_.size(); }

    // Limpa todos (restart da run). Slimes iniciais respawnam pelo caller.
    void clear() { slimes_.clear(); }

    // Remove mortos; onDeath(inimigo) por removido para juice
    // (partículas/drops/XP no DeathSystem). Erase mora aqui, no dono.
    // Com ctx, dispara ai->onDeath antes do erase (hook opcional).
    // Também varre destroyPending (markForDestroy): morte marcada
    // durante iteração cai aqui no fim do frame, nunca no meio do loop.
    void removeDead(const std::function<void(Enemy&)> &onDeath,
                    GameContext *ctx = nullptr);

    // Marca p/ destruição adiada (seguro dentro de forEach/tick).
    // O erase acontece no próximo removeDead.
    void markForDestroy(Enemy& e) { e.destroyPending = true; }

    // Despawn por distância (economia do SpawnSystem). Remove além do
    // raio (px) do ponto. Retorna quantos removeu.
    std::size_t despawnFar(float x, float y, float radius);

private:
    void physics(Enemy &s, GameContext &ctx);

    std::vector<std::unique_ptr<Enemy>> slimes_;
};

} // namespace support
