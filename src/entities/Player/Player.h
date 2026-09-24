/**
 * @file src/entities/Player/Player.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara estado completo do jogador e ações de combate.
 * @details Define classe Player com flags de movimento, corpo, cooldowns, inventário, equipamento, HP, mira e melee, incluída por Game, UI e sistemas de combate.
 */

#pragma once

#include <string>
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include "entities/Entity.hpp"
#include "support/Combat/AimDir.h"
#include "support/Combat/Body.h"
#include "support/Combat/SpriteFrame.h"
#include "core/Attributes.h"
#include "core/Cooldown.h"
#include "core/Equipment.h"
#include "core/Inventory.h"

namespace sf { class Texture; }
namespace support { class ThrowSystem; }

// Fase do swing atual. Idle = sem ataque em curso.
enum class MeleePhase : uint8_t { Idle, Windup, Active, Recovery };

class Player : public Entity
{
    public:

        bool moveDown = false, moveUp = false, moveLeft = false, moveRight = false,runFast = false;
        bool jumping = false;
        bool inWater = false; // collide() seta; tick() reseta (p/ gate de SFX)
        float jumpingRecharge = 0.0f;
        int walkFrame = 0; // 0..3 (sprite walk); 0 parado
        float walkTimer = 0.f;
        float throwAnimT = 0.f; // >0 = frame throw (0.4s)
        // Frame do melee atual (arma equipada). nullptr = soco.
        // Arma futura = 1 linha ao equipar; pick/render não mudam.
        const sf::Texture *meleeTex = nullptr;
        static constexpr float kThrowAnimDur = 0.40f;

        support::Body body; // hitboxes por parte (rebuild via BodySystem)

        // Frame atual (id leve). Game::tick preenche via resolve;
        // BodySystem deriva hitboxes; render mapeia p/ textura.
        support::SpriteFrameId currentFrameId = support::SpriteFrameId::PlayerIdle;

        // S6: verbo de arremesso. Cooldown tickado no Game::tick (1/30 fixo);
        // lógica aqui para ser testável sem Game/janela.
        core::Cooldown throwCooldown{0.5f};
        // Inventário autoritativo (público, mesmo padrão de hp).
        // dynamiteCount morreu aqui: pilha "dynamite" manda no arremesso.
        core::Inventory inventory;
        // Equipamento autoritativo (público, mesmo padrão). Seed de ferro
        // no ctor (nasce equipado); sobrevive à morte como o inventário.
        core::Equipment equipment;

        // Combate: HP + i-frames. Morte/restart ficam para o bloco B.
        int hp = 10000;
        int hpMax = 10000;
        core::Cooldown hurtIframes;

        // Atributos DS (F2+): dirigem hpMax/stamina/carga (refreshDerived).
        core::Attributes attrs;

        // Estamina (F3: teto + regen; F5: consumo em swing/roll/run).
        float stamina = 150.f;
        float staminaMax = 150.f;
        core::Cooldown staminaDelay{0.8f}; // regen só após gastar
        static constexpr float kSwingCost = 20.f;
        static constexpr float kSprintCost = 10.f; // por segundo

        // Carteira de souls (XP coletado). Morte derruba no cadáver
        // (RunManager); R voluntário mantém; respawn não mexe.
        int souls = 0;
        void addSouls(int v) { souls += v; }

        // Magia (F8): FP + magias sintonizadas (cap = spellSlots(ATT)).
        float fp = 200.f;
        float fpMax = 200.f;
        std::vector<std::string> attuned; // defIds, ordem de sintonia
        int spellSlots() const {
            return core::Attributes::spellSlots(
                attrs.get(core::Attr::Attunement));
        }
        // Sintonia: def Spell com req cumprido e espaço livre.
        bool attune(const std::string& defId);
        bool unattune(const std::string& defId);

        // Status effects (F7): acúmulo veneno/sangramento + veneno ativo.
        float poisonBuildup = 0.f;
        float bleedBuildup = 0.f;
        float poisonTimer = 0.f; // >0 = envenenado (DoT correndo)
        float poisonFrac_ = 0.f; // fração de dano acumulada (hp é int)
        static constexpr float kPoisonDps = 3.f;
        static constexpr float kPoisonDur = 8.f;
        static constexpr float kBleedPct = 0.15f; // burst do HP máximo
        static constexpr float kSlimePoison = 25.f; // por mordida
        static constexpr float kDwarfBleed = 30.f;  // por golpe
        float statusThreshold() const {
            return core::Attributes::statusThreshold(
                attrs.get(core::Attr::Resistance),
                attrs.get(core::Attr::Attunement));
        }
        void addPoison(float amt);
        void addBleed(float amt);
        void curePoison() {
            poisonBuildup = 0.f;
            poisonTimer = 0.f;
            poisonFrac_ = 0.f;
        }
        void cureBleed() { bleedBuildup = 0.f; }

        // Arma equipada (def do slot RightHand) ou nullptr = soco.
        // Fonte única p/ render, BodySystem e meleeHitbox.
        const core::ItemDef* weaponDef() const;
        bool hasWeapon() const { return weaponDef() != nullptr; }
        // Segunda arma (LeftHand): dano soma no melee. nullptr = sem.
        const core::ItemDef* offHandDef() const;

        // Carga equipada (mochila não pesa). Pesada = sem correr.
        float equipLoad() const { return equipment.weight(); }
        float maxEquipLoad() const {
            return core::Attributes::maxLoad(
                attrs.get(core::Attr::Endurance));
        }
        bool heavilyLoaded() const {
            return equipLoad() > maxEquipLoad() * 0.5f;
        }

        // Recalcula hpMax/staminaMax/carga dos atributos (ctor, respawn,
        // pós-compra). Não mexe em hp/stamina atuais (só tetos).
        void refreshDerived();

        // Melee light 3-hit. Estado avançado pelo MeleeSystem (tem dt).
        MeleePhase meleePhase = MeleePhase::Idle;
        int meleeCombo = 0;
        float meleeTimer = 0.f;
        int meleeSwingId = 0;

        // Mira em 8 vias (input) + snapshot do swing (hitbox não segue
        // o input no meio do golpe).
        support::AimDir aimDir = support::AimDir::E;
        support::AimDir swingAim = support::AimDir::E;

        // Fonte única do estado de swing. O resolve do sprite, a
        // hitbox e o debug yellow box leem TODOS isto — nunca timer.
        bool inMeleeSwing() const {
            return meleePhase != MeleePhase::Idle;
        }

        // Mira efetiva da arma: fora do swing segue o input (aimDir);
        // no swing congela no snapshot (swingAim). Sem isto, idle após
        // um golpe-W desenharia a arma rotacionada 180° para sempre.
        support::AimDir effectiveAim() const {
            return inMeleeSwing() ? swingAim : aimDir;
        }

        // Seam para parry (sem chamador ainda — CombatSystem consome
        // quando rebate existir). Janela = início do Active.
        bool parryWindowActive() const {
            return meleePhase == MeleePhase::Active
                && meleeTimer > kParryWindowStart;
        }
        static constexpr float kParryWindowStart = 0.06f;

        Player();
        void collide(Entity entity);
        void collide(Component bloco);

        void tick();

        // Tenta arremessar na direção do facing com arco fixo.
        // Retorna false sem efeito se cooldown/inventário/pool bloquearem.
        bool tryThrow(support::ThrowSystem &throws);

        // Joga o item do slot (hotbar ativa): só se for throwable.
        // Consome 1 do slot exato; stats vêm do def (fonte única).
        bool tryThrowSlot(support::ThrowSystem &throws, int slot);

        // Usa o item do slot (hotbar ativa): só se tem onUse (poção).
        // Consome 1. Sem cooldown (igual ao menu Use).
        bool tryUseSlot(int slot);

        // Conjura a 1ª magia sintonizada (G): Arrow vira Bolt, Heal cura.
        // Custa FP + cooldown de arremesso; req INT/FÉ do def.
        static constexpr float kArrowCost = 25.f;
        static constexpr float kHealCost = 40.f;
        static constexpr float kArrowBase = 30.f;
        static constexpr float kHealBase = 50.f;
        bool castAttuned(support::ThrowSystem &throws);

        // Completa a pilha "dynamite" até 999 (legado generoso).
        // Chamado no ctor e no respawn; coleta soma por cima e o
        // inventário sobrevive à morte. Nunca esvazia o resto.
        void topUpDynamite();

        // Kit inicial: 1 pilha cheia de cada item do registry.
        // Mesmo padrão generoso da dinamite (ctor + respawn).
        void topUpStarterKit();

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

        // Hitbox do swing atual (screen-space via swingAim). Só válida
        // em Active; em outras fases retorna rect vazio.
        // Não-const: getters legados do Entity não são const.
        sf::FloatRect meleeHitbox();
        int meleeDamage() const;
        float meleePosture() const;
};