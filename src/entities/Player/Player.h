/**
 * @file src/entities/Player/Player.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara estado completo do jogador e ações de combate.
 * @details Define classe Player com flags de movimento, corpo, cooldowns, inventário, equipamento, HP, mira e melee, incluída por Game, UI e sistemas de combate.
 */

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include "entities/Entity.hpp"
#include "support/Combat/AimDir.h"
#include "support/Combat/AnimPlayer.h"
#include "support/Combat/Facing.h"
#include "support/Combat/Limb.h"
#include "support/Combat/SweepArc.h"
#include "support/Combat/Body.h"
#include "support/Combat/SpriteFrame.h"
#include "core/Attributes.h"
#include "core/PlayerClass.h"
#include "core/Resistances.h"
#include "core/StatusModifiers.h"
#include "core/TarotCard.h"
#include "core/Cooldown.h"
#include "core/Equipment.h"
#include "core/Vec.h"
#include "physics/PlayerPhysics.hpp"
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
        int hp = 100;
        int hpMax = 100;
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

        // Nome do personagem (criação; save futuro). Vazio = sem nome.
        std::string name;

        // Magia (F8): FP + magias sintonizadas (cap = spellSlots(ATT)).
        float fp = 200.f;
        float fpMax = 200.f;
        std::vector<std::string> attuned; // defIds, ordem de sintonia
        int spellSlots() const {
            return core::Attributes::spellSlots(
                       attrs.get(core::Attr::Attunement)) +
                   tarotFx.spellSlots; // Mago: +1 a cada 2 cópias
        }
        // Sintonia: def Spell com req cumprido e espaço livre.
        bool attune(const std::string& defId);
        bool unattune(const std::string& defId);

        // Status effects (F7): acúmulo veneno/sangramento + veneno ativo.
        float poisonBuildup = 0.f;
        float bleedBuildup = 0.f;
        float poisonTimer = 0.f; // >0 = envenenado (DoT correndo)
        float poisonFrac_ = 0.f; // fração de dano acumulada (hp é int)
        float bleedSlowTimer = 0.f; // >0 = micro-slow pós-burst (0.3s)
        // Frost (Fase 2 elementais): buildup espelha poison; ativo =
        // swing lento via attackSpeedMult (sem dano direto).
        float frostBuildup = 0.f;
        float frostTimer = 0.f; // >0 = congelando (swing ×0.7)

        // Pipeline comportamental (review F7+): agrega status ativos.
        // Poison fiel: identidade (só dano, já aplicado). Bleed: slow.
        core::StatusModifiers computeModifiers() const;
        // HP máximo efetivo (hpMax × curse futura). Tick clampa hp.
        int effectiveHpMax() const;
        static constexpr float kPoisonDps = 3.f;
        static constexpr float kPoisonDur = 8.f;
        static constexpr float kBleedPct = 0.15f; // burst do HP máximo
        static constexpr float kFrostDur = 6.f; // janela de swing lento
        static constexpr float kFrostSlow = 0.7f; // attackSpeedMult ativo
        static constexpr float kSlimePoison = 25.f; // por mordida
        static constexpr float kSlimeBleed = 15.f;  // slime aplica os dois
        static constexpr float kDwarfBleed = 30.f;  // por golpe
        float statusThreshold() const {
            return core::Attributes::statusThreshold(
                       attrs.get(core::Attr::Resistance),
                       attrs.get(core::Attr::Attunement)) *
                   tarotFx.statusResistMult;
        }
        void addPoison(float amt);
        void addBleed(float amt);
        void addFrost(float amt);
        void curePoison() {
            poisonBuildup = 0.f;
            poisonTimer = 0.f;
            poisonFrac_ = 0.f;
        }
        void cureBleed() {
            bleedBuildup = 0.f;
            bleedSlowTimer = 0.f;
        }
        void cureFrost() {
            frostBuildup = 0.f;
            frostTimer = 0.f;
        }

        // Arma equipada (def do slot RightHand) ou nullptr = soco.
        // Fonte única p/ render, BodySystem e sweepArc.
        const core::ItemDef* weaponDef() const;
        bool hasWeapon() const { return weaponDef() != nullptr; }
        // Segunda arma (LeftHand): dano soma no melee. nullptr = sem.
        const core::ItemDef* offHandDef() const;

        // Carga equipada (mochila não pesa). Cartas do fado NÃO pesam:
        // 1000 cartas não encostam na carga; só debuffs (lentos, fracos)
        // cobram. Pesada = sem correr.
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

        // Corpo em 8 vias (híbrido Fase A): fora do swing segue o
        // movimento (parado mantém); no swing congela no snapshot do
        // golpe (telegraph, até Idle). Render ainda lê facing (±1,
        // derivado) — sem mudança visual até a Fase D.
        support::Facing facing8 = support::Facing::E;
        void setFacing8(support::Facing f) {
            facing8 = f;
            facing = support::facingSign(f);
        }

        // Fonte única do estado de swing. O resolve do sprite, a
        // hitbox e o debug yellow box leem TODOS isto — nunca timer.
        bool inMeleeSwing() const {
            return meleePhase != MeleePhase::Idle;
        }

        // Player do clip de ataque (Fase C): startSwing toca o clip do
        // grupo da mira; updateMelee avança o frame nas transições
        // (relógio único = meleeTimer). MeleeSystem lê liveEvents().
        support::AnimPlayer anim;

        // Braços articulados (B.2): comprimentos canônicos em sprite-rows
        // (3+3, medidos na arte atual); Renderer calibra o alcance por
        // frame contra o box legado (B.3 dirige os alvos).
        support::Limb limbR_, limbL_;
        // Alvos procedurais da mão em mundo (B.3): por fase do swing
        // (windup recuo / active estendida / recovery retorno) ou
        // repouso+senoide fora dele. Renderer consome; B.4 a hitbox.
        // live=false (preview/respawn) = Renderer usa o box legado.
        core::Vec2f targetHandR_{0.f, 0.f}, targetHandL_{0.f, 0.f};
        bool handTargetsLive_ = false;
        // Poses resolvidas (B.4, mundo, como desenhadas): fonte única
        // para drawLimb, weaponHand e computeWeaponBbox. Sem lag de
        // fase além do já existente (transição corre no MeleeSystem).
        support::LimbPose poseR_, poseL_;
        void updateLimbs();
        // Mão da arma em mundo (B.4, espelho exato draw↔bbox): pose IK
        // quando live; box do ArmR senão; {0,0} sem fonte alguma.
        core::Vec2f weaponHand() const {
            if (handTargetsLive_) return poseR_.handWorld;
            const support::PartState* arm =
                body.find(support::BodyPartId::ArmR);
            if (!arm) return {0.f, 0.f};
            return {arm->worldBox.left + arm->worldBox.width * 0.5f,
                    arm->worldBox.top + arm->worldBox.height};
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

        // Conjura a 1ª magia sintonizada (G): Arrow vira Bolt, Heal cura,
        // Fire vira bola de fogo, FrostWeapon buffa a arma. FP + cooldown.
        static constexpr float kArrowCost = 25.f;
        static constexpr float kHealCost = 40.f;
        static constexpr float kArrowBase = 30.f;
        static constexpr float kHealBase = 50.f;
        static constexpr float kFireCost = 35.f;
        static constexpr float kFireBase = 40.f;
        static constexpr float kFireRadius = 60.f; // área generosa
        static constexpr float kFrostWeaponCost = 30.f;
        static constexpr float kFrostWeaponDur = 30.f;
        bool castAttuned(support::ThrowSystem &throws);

        // Completa a pilha "dynamite" até 999 (legado generoso).
        // Chamado no ctor e no respawn; coleta soma por cima e o
        // inventário sobrevive à morte. Nunca esvazia o resto.
        void topUpDynamite();

        // Kit inicial: 1 pilha cheia de cada item do registry.
        // Mesmo padrão generoso da dinamite (ctor + respawn).
        void topUpStarterKit();

        // Criação de personagem (menu futuro): zera tudo e aplica a
        // classe (attrs, equipamento, itens, magias). Seed do ctor
        // continua até o menu existir (TEMP-SeedForMenu).
        void applyClass(core::PlayerClass klass);

        // Decomposição do dano p/ UI (mesma matemática do combate).
        struct MeleeBreakdown {
            int base = 0;
            float strBonus = 0.f;
            float dexBonus = 0.f;
            float intBonus = 0.f;
            float faiBonus = 0.f;
            bool halvedByReq = false;
            int total = 0;
        };
        MeleeBreakdown meleeDamageBreakdown() const;
        // Variante com contexto do alvo (MeleeSystem): fração de HP do
        // alvo (Justiça cheia/ferida) + alone (Eremita).
        MeleeBreakdown meleeDamageBreakdownVs(float targetHpFrac,
                                              bool alone) const;
        // Rolagem DS (LShift): rajada 0.4s com i-frames 0.35s, custo
        // 25 stamina, só no chão e fora do golpe Active. Fat roll
        // (carga pesada) rola sem i-frames. Direção: input ou facing.
        float rollTimer = 0.f; // >0 = rolando
        int rollDir = 1;
        core::Cooldown rollIframes{physics::kRollIframes};
        bool rolling() const { return rollTimer > 0.f; }
        bool startRoll();
        // Troca rápida DS (Z/X/C/V, sem pausar): cicla listas da
        // mochila + equipado. Sem overlay (lista completa é o menu).
        // Gates: morto e golpe Active barram (stagger futuro barra aqui).
        bool canQuickSwap() const {
            return hp > 0 && meleePhase != MeleePhase::Active;
        }
        // Próxima arma da mão (dir/esq): devolve a atual e equipa a
        // próxima da mochila (ordem de slot). <2 armas = false.
        // outName recebe o nome da arma que entrou (toast do HUD).
        bool cycleHand(core::EquipSlot hand, std::string *outName = nullptr);
        // Próxima magia: roda attuned (0 vai p/ o fim). <2 = false.
        bool cycleSpell(std::string *outName = nullptr);
        // Tarô (fado forçado, 78 arcanos): cartas permanentes na run,
        // trade-off linear por cópia (Diabo ×2 = 1.5× dano e recebido).
        // Sem pickup, sem escolha, sem descarte: aplica direto na morte
        // (DeathSystem) e só zera morrendo (respawn) ou apagando tudo
        // (deleteCharacter). Peso do destino amaldiçoa por faixa.
        std::unordered_map<core::TarotArcana, int,
                           core::TarotRegistry::ArcanaHash>
            tarotCards;
        core::TarotEffect tarotFx; // agregado recomputado no addCard
        void addTarotCard(core::TarotArcana a);
        void recomputeTarot();
        // Contadores derivados (recompute): cartas e peso do destino.
        int totalTarotCards_ = 0;
        int tarotWeight_ = 0;
        int totalTarotCards() const { return totalTarotCards_; }
        int tarotWeight() const { return tarotWeight_; }
        // Maldição do peso (applyTarotCurse no tick): mults extras sobre
        // o agregado; DoT nunca mata (trava em 1). Só peso >100 cobra.
        float curseHpMaxMult_ = 1.f;
        float curseMoveMult_ = 1.f;
        float curseDefMult_ = 1.f;
        float curseDotT_ = 0.f;
        float cursePoisonT_ = 0.f;
        void applyTarotCurse(float dt);
        // Morte (killStack): stacks com janela de 30s, somem no respawn.
        int killStacks_ = 0;
        float killTimer_ = 0.f;
        void addKillStack();
        // Julgamento: quantos revives já gastou nesta run.
        int tarotRevivesUsed_ = 0;
        int tarotRevivesUsed() const { return tarotRevivesUsed_; }
        int killStacks() const { return killStacks_; }
        // Maldição do peso em palavras (puro, p/ UI): espelha os
        // limiares de applyTarotCurse sem os timers.
        struct CurseInfo {
            bool cursed = false;
            float hpMaxMult = 1.f;
            float moveMult = 1.f;
            float defMult = 1.f;
            bool dot20 = false;   // -1 vida / 20s
            bool dot15 = false;   // -2 vidas / 15s
            bool poison = false;  // veneno permanente
        };
        CurseInfo tarotCurse() const;
        // Enforcado: bônus plano de dano por 10s após apanhar.
        float convBonus_ = 0.f;
        float convTimer_ = 0.f;
        // Vinheta do fado: estado p/ o Renderer desenhar 1.5s sem pausar.
        // recent_ = anel das últimas 3 cartas (HUD compacto, fade 30s).
        bool tarotRevealActive_ = false;
        core::TarotArcana tarotRevealArcana_ = core::TarotArcana::Fool;
        float tarotRevealAge_ = 0.f;
        static constexpr float kTarotRevealLife = 1.5f;
        core::TarotArcana tarotRecent_[3] = {};
        int tarotRecentCount_ = 0;
        float tarotRecentAge_ = 999.f;
        void showTarotReveal(core::TarotArcana a);
        bool tarotRevealActive() const { return tarotRevealActive_; }
        core::TarotArcana tarotRevealArcana() const {
            return tarotRevealArcana_;
        }
        float tarotRevealAge() const { return tarotRevealAge_; }
        // Anel das últimas 3 (HUD): índice = (total-1-i) % 3, i=0 última.
        const core::TarotArcana *tarotRecent() const { return tarotRecent_; }
        int tarotRecentTotal() const { return tarotRecentCount_; }
        float tarotRecentAge() const { return tarotRecentAge_; }
        void tickTarot(float dt);
        // Apaga o personagem inteiro (única saída do fado): attrs,
        // tarô, inventário, equipamento, souls. Sem meio-termo.
        void deleteCharacter();
        // Dano com contexto do alvo (MeleeSystem): fração de HP do
        // alvo (Justiça cheia/ferida) + alone (Eremita).
        // meleeDamage() = sem contexto (UI/teste).
        int meleeDamageVs(float targetHpFrac, bool alone) const;
        // Multiplicador efetivo de dano recebido (taken ÷ defesa ÷ curse).
        float takenMult() const;
        // MeleeSystem passa o tipo; default físico = comportamento atual.
        // Timer: 0 = permanente até trocar (frost_weapon seta 30s).
        core::DamageType weaponBuffType = core::DamageType::Physical;
        float weaponBuffTimer = 0.f;

        // Dano com gate de i-frame (0.6s). Retorna se aplicou.
        // hp trava em 0; morte/restart vêm no bloco B.
        // Tipo filtra pela resistência antes dos i-frames.
        bool hurt(int dmg,
                  core::DamageType type = core::DamageType::Physical);

        // Resistências derivadas (Fase 1 elementais): END/VIT/FTH +
        // defesa universal por nível (DS1: todo nível protege um pouco).
        core::Resistances resistances_;
        core::Resistances computeResistances() const;

        // Reset completo para respawn (RunManager): HP, pos, vel,
        // cooldowns, melee idle, inventário. Facing vira direita.
        void respawn(float x, float y);

        // Inicia swing (Idle→combo 0) ou encadeia (Recovery→próximo).
        // Retorna false se já está em Windup/Active.
        bool startSwing();

        // Avança timers; retorna a fase atual.
        MeleePhase updateMelee(float dt);

        // Arco de varredura ao vivo (Fase E, cone+anel da mão): válido
        // só no Active com pose (vazio senão). MeleeSystem consome.
        // Não-const: getters legados do Entity não são const.
        support::SweepArc sweepArc();
        // Arco previsto p/ a mira (debug F2 laranja): mesma geometria
        // do ao vivo, sem exigir swing (vazio sem boxes).
        support::SweepArc predictedSweep(support::AimDir aim);
        int meleeDamage() const;
        float meleePosture() const;
};