#pragma once

#include <SFML/Graphics/Rect.hpp>
#include "../entity/entity.hpp"
#include "../../support/Body.h"
#include "../../core/Cooldown.h"

namespace support { class ThrowSystem; }

// Fase do swing atual. Idle = sem ataque em curso.
enum class MeleePhase : uint8_t { Idle, Windup, Active, Recovery };

class Player : public Entity
{
    public:

        bool moveDown = false, moveUp = false, moveLeft = false, moveRight = false,runFast = false;
        bool jumping = false;
        float jumpingRecharge = 0.0f;
        int walkFrame = 0; // 0..3 (sprite walk); 0 parado
        float walkTimer = 0.f;
        float meleeAnimT = 0.f; // >0 = frame melee (0.3s)
        float throwAnimT = 0.f; // >0 = frame throw (0.4s)
        static constexpr float kMeleeAnimDur = 0.30f;
        static constexpr float kThrowAnimDur = 0.40f;

        support::Body body; // hitboxes por parte (rebuild via BodySystem)

        // S6: verbo de arremesso. Cooldown tickado no Game::tick (1/30 fixo);
        // lógica aqui para ser testável sem Game/janela.
        core::Cooldown throwCooldown{0.5f};
        int dynamiteCount = 999;

        // Combate: HP + i-frames. Morte/restart ficam para o bloco B.
        int hp = 100;
        int hpMax = 100;
        core::Cooldown hurtIframes;

        // Melee light 3-hit. Estado avançado pelo MeleeSystem (tem dt).
        MeleePhase meleePhase = MeleePhase::Idle;
        int meleeCombo = 0;
        float meleeTimer = 0.f;
        int meleeSwingId = 0;

        Player();
        void collide(Entity entity);
        void collide(Component bloco);
        void tick();

        // Tenta arremessar na direção do facing com arco fixo.
        // Retorna false sem efeito se cooldown/inventário/pool bloquearem.
        bool tryThrow(support::ThrowSystem &throws);

        // Dano com gate de i-frame (0.6s). Retorna se aplicou.
        // hp trava em 0; morte/restart vêm no bloco B.
        bool hurt(int dmg);

        // Reset completo para respawn (RunManager): HP, pos, vel,
        // cooldowns, melee idle, inventário. Facing vira direita.
        void respawn(float x, float y);

        // Inicia swing (Idle→combo 0) ou encadeia (Recovery→próximo).
        // Retorna false se já está em Windup/Active.
        bool startSwing();

        // Avança timers; retorna a fase atual.
        MeleePhase updateMelee(float dt);

        // Hitbox do swing atual (à frente, lado do facing). Só válida
        // em Active; em outras fases retorna rect vazio.
        // Não-const: getters legados do Entity não são const.
        sf::FloatRect meleeHitbox();
        int meleeDamage() const;
        float meleePosture() const;
};