/**
 * @file src/entities/Player/Player.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementa lógica do jogador, física, combate e inventário.
 * @details Configura AABB 30x50, gravidade e equipamento inicial de ferro, processa tick, colisão, arremesso, melee, dano e respawn, chamado por Game App e sistemas.
 */

#include "Player.h"
#include <algorithm>
#include <iostream>
#include "defines.h"
#include "physics/PlayerPhysics.hpp"
#include "support/Combat/WeaponRegistry.h"
#include "support/Effects/ThrowSystem.h"

namespace {
// Queda livre: acelera 2px/tick² até 25px/tick (750px/s, ~2.5x os 9.8
// fixos de antes). Terminal < 50px do tile: sem tunelamento.
constexpr float kGravity = 2.0f;
constexpr float kTerminalVelocity = 25.0f;
}
 Player::Player() :
Entity(core::kIdPlayer,0,0,60,100) // AABB 2 blocos (sprite 12x40 a 2.5x)
{
    setFillColor(sf::Color::Red);
    // humanoid(ALTURA, LARGURA): proporcional total — partes, arma e
    // câmera concordam entre si. Visual fino (30px) centrado na caixa.
    static auto schema = support::BodySchema::humanoid(100.f, 60.f);
    body.attach(&schema);
    topUpDynamite();
    topUpStarterKit();
    // Nasce equipado (set de ferro): render idêntico ao Loadout antigo.
    // Direto no equipment (não passa pelo inventário, sem sobra).
    equipment.equip(core::Item{"iron_sword", 1});
    equipment.equip(core::Item{"iron_helm", 1});
    equipment.equip(core::Item{"iron_chest", 1});
    equipment.equip(core::Item{"iron_legs", 1});
    equipment.equip(core::Item{"iron_boots", 1});
    equipment.equip(core::Item{"iron_gloves", 1});
    refreshDerived(); // hpMax/stamina/carga dos attrs base (10)
    stamina = staminaMax;
    //this->setGravity(9.8f);
}

void Player::collide(Entity bloco)
{
    if(
        bloco.getName() == core::kIdWater
    ){
        inWater = true; // nada: silêncio (splash/bolha é fase futura)
        jumping = true;
        return;
    }
    // Lava e deco não colidem como parede (dano vem na Fase C).
    if(
        bloco.getName() == core::kIdLava
    ){
        return;
    }
    // Enemy não empurra o player (dano de contato vem na Fase C).
    if(
        bloco.getName() == core::kIdSlime
    ){
        return;
    }
    if(
        bloco.getName() == core::kIdTreeTrunk || bloco.getName() == core::kIdTreeLeaf
    ){
        return;
    }
    // Correções lidas no rect do BLOCO (top/left/width/height frescos em
    // estáticos). Nunca dims do jogador no lugar do bloco: +getW() na
    // esquerda grudava o player 20px dentro da parede 50px (o "pior na
    // esquerda"). No teto, +getH() coincide em 50px mas é lixo no
    // overload Component (privados nunca setados) — usa bloco.height.
    // Topo dispara quando a base do bloco está no meio do corpo ou acima:
    // janela 0.5h cobre o teleporte do pulo (25px/tick) inteiro — gate
    // 0.25h perdia metade e o player subia pelo teto. Bloco na altura do
    // peito (base abaixo do meio) cai na lateral. Quando o topo dispara,
    // os lados pulam este bloco (evita fling horizontal no bonk fundo).
    bool topHit = false;
    if (getBoundsTop().intersects(bloco)
        && bloco.top + bloco.height <= getY() + getH() * 0.5f) {
        // Teto de verdade (acima do meio): bonk. Bloco na altura do
        // peito cai na correção lateral — sem teleporte p/ baixo.
        setY(bloco.top + bloco.height);
        if (getVy() < 0.f) setVy(0.f); // bonk: teto zera subida (senão gruda)
        topHit = true;
    }

    if (getBoundsBottom().intersects(bloco)) {
        setY(bloco.top - getH());
        moveDown = false;
        jumping = true;
        setVy(0.f); // pouso mata a queda (gravidade reacumula se sair)
    } else if(jumping){
         moveDown = true;
    }

    // Lateral só com penetração >= 6px: roçar lintel/teto com 2px não
    // empurra (passa por baixo); parede de verdade tem penetração funda.
    if (!topHit) {
        const Component sr = getBoundsRight();
        if (sr.intersects(bloco)) {
            const float pen = std::min(sr.top + sr.height, bloco.top + bloco.height)
                            - std::max(sr.top, bloco.top);
            if (pen >= 6.f) setX(bloco.left - getW());
        }
    }

    if (!topHit) {
        const Component sl = getBoundsLeft();
        if (sl.intersects(bloco)) {
            const float pen = std::min(sl.top + sl.height, bloco.top + bloco.height)
                            - std::max(sl.top, bloco.top);
            if (pen >= 6.f) setX(bloco.left + bloco.width);
        }
    }

}

void Player::collide(Component bloco)
{
    // Mesma doutrina do overload Entity: rect do bloco (FloatRect, sempre
    // válido — getX()/getW() do Component são lixo p/ cópias fatiadas).
    // Gate no meio (0.5h) + skip lateral se o topo disparar (idem acima).
    bool topHit = false;
    if (getBoundsTop().intersects(bloco)
        && bloco.top + bloco.height <= getY() + getH() * 0.5f) {
        setY(bloco.top + bloco.height);
        if (getVy() < 0.f) setVy(0.f); // bonk: teto zera subida (senão gruda)
        topHit = true;
    }

    if (getBoundsBottom().intersects(bloco)) {
        setY(bloco.top - getH());
        moveDown = false;
        jumping = true;
        setVy(0.f); // pouso mata a queda (gravidade reacumula se sair)
    } else if(jumping){
         moveDown = true;
    }

    if (!topHit) {
        const Component sr = getBoundsRight();
        if (sr.intersects(bloco)) {
            const float pen = std::min(sr.top + sr.height, bloco.top + bloco.height)
                            - std::max(sr.top, bloco.top);
            if (pen >= 6.f) setX(bloco.left - getW());
        }
    }

    if (!topHit) {
        const Component sl = getBoundsLeft();
        if (sl.intersects(bloco)) {
            const float pen = std::min(sl.top + sl.height, bloco.top + bloco.height)
                            - std::max(sl.top, bloco.top);
            if (pen >= 6.f) setX(bloco.left + bloco.width);
        }
    }

}

void Player::tick() {
    // Wrapper fino sobre physics::step (fonte única do movimento).
    // Comportamento bit-idêntico ao tick antigo: monta State/Input,
    // roda o step puro e escreve de volta (membros + Entity + cooldowns).
    // Sprint drena 10/s e zera corta a corrida; regen com delay 0.8s.
    // Sprint drena 10/s e zera corta a corrida; slow do bleed também
    // corta (micro-stagger comportamental). Custos × staminaCostMult.
    const core::StatusModifiers mods = computeModifiers();
    if (runFast && (moveLeft || moveRight)) {
        stamina = std::max(
            0.f, stamina - (kSprintCost * mods.staminaCostMult) / 30.f);
        staminaDelay.trigger();
    }
    if (runFast && (stamina <= 0.f || bleedSlowTimer > 0.f)) runFast = false;
    physics::State st;
    st.x = getX();
    st.y = getY();
    st.vx = getVx();
    st.vy = getVy();
    st.runFast = runFast;
    st.jumping = jumping;
    st.jumpingRecharge = jumpingRecharge;
    st.inWater = inWater;
    st.facing = facing;
    st.aim = aimDir;
    st.walkFrame = walkFrame;
    st.walkTimer = walkTimer;
    st.throwAnimT = throwAnimT;
    st.hurtT = hurtIframes.remaining();
    st.throwT = throwCooldown.remaining();

    physics::Input in{moveUp, moveDown, moveLeft, moveRight, runFast};
    const physics::Output out =
        physics::step(st, in, physics::kFixedDt);
    const physics::State &s = out.state;

    moveUp = s.moveUp;
    moveDown = s.moveDown;
    moveLeft = s.moveLeft;
    moveRight = s.moveRight;
    runFast = s.runFast;
    jumping = s.jumping;
    jumpingRecharge = s.jumpingRecharge;
    inWater = s.inWater;
    aimDir = s.aim;
    walkFrame = s.walkFrame;
    walkTimer = s.walkTimer;
    throwAnimT = s.throwAnimT;
    if (s.hurtT > 0.f) {
        hurtIframes.trigger(s.hurtT);
    } else {
        hurtIframes.reset();
    }
    if (s.throwT > 0.f) {
        throwCooldown.trigger(s.throwT);
    } else {
        throwCooldown.reset();
    }

    // Integração já feita no step; aqui só o sync do Entity
    // (era Entity::tick sem o x += vx): posição do shape + sensores.
    setX(s.x);
    setY(s.y);
    setVx(s.vx);
    setVy(s.vy);
    this->left = getX();
    this->top = getY();
    this->setPosition(getX(), getY());

    // Regen de estamina: 30/s × mult, só com delay pronto.
    staminaDelay.tick(1.f / 30.0f);
    if (stamina < staminaMax && staminaDelay.ready())
        stamina = std::min(
            staminaMax, stamina + 1.f * computeModifiers().staminaRegenMult);

    // Slow do bleed decai aqui (0.3s de micro-stagger).
    if (bleedSlowTimer > 0.f) bleedSlowTimer -= 1.f / 30.0f;
    if (frostTimer > 0.f) frostTimer -= 1.f / 30.0f; // swing lento expira
    // Buff da arma expira sozinho (timer 0 = permanente até trocar).
    if (weaponBuffTimer > 0.f) {
        weaponBuffTimer -= 1.f / 30.0f;
        if (weaponBuffTimer <= 0.f)
            weaponBuffType = core::DamageType::Physical;
    }

    // HP nunca acima do máximo efetivo (Curse futura reduz).
    if (hp > effectiveHpMax()) hp = effectiveHpMax();

    // Regen de FP: 8/s, sem delay (magia F8).
    if (fp < fpMax) fp = std::min(fpMax, fp + 8.f / 30.0f);

    // Veneno ativo: DoT direto (fura i-frame, pode matar). hp é int:
    // acumula a fração e desconta os inteiros (3/s = 1 a cada 10 ticks).
    if (poisonTimer > 0.f) {
        poisonTimer -= 1.f / 30.0f;
        poisonFrac_ += kPoisonDps / 30.0f;
        const int whole = static_cast<int>(poisonFrac_);
        if (whole > 0) {
            hp -= whole;
            poisonFrac_ -= whole;
            if (hp < 0) hp = 0;
        }
    }
}

void Player::topUpDynamite() {
    for (int missing = 999 - inventory.count("dynamite"); missing > 0;) {
        const int put = std::min(missing, 99);
        const int left = inventory.add(
            core::Item{"dynamite", static_cast<uint16_t>(put)});
        missing -= put - left;
        if (left > 0) break; // cheio: fica com o que coube
    }
}

void Player::topUpStarterKit() {
    // 1 pilha cheia de cada item do registry (ordem de registro;
    // dinamite pula aqui — topUpDynamite dá 999 em 11 pilhas).
    // Generoso como a dinamite: completa o que falta, nunca esvazia.
    for (const std::string& id : core::ItemRegistry::instance().keys()) {
        if (id == "dynamite") continue;
        const core::ItemDef* def =
            core::ItemRegistry::instance().find(id);
        if (!def) continue;
        for (int missing = def->stackMax - inventory.count(id);
             missing > 0;) {
            const int put = std::min(missing, 99);
            const int left = inventory.add(
                core::Item{id, static_cast<uint16_t>(put)});
            missing -= put - left;
            if (left > 0) break; // cheio: fica com o que coube
        }
    }
}

bool Player::tryThrowSlot(support::ThrowSystem &throws, int slot) {
    if (!throwCooldown.ready()) return false;
    if (slot < 0 || slot >= core::Inventory::kCapacity) return false;
    core::Item& item = inventory.slot(slot);
    if (item.isEmpty()) return false;
    const core::ItemDef* def = item.def();
    if (!def || !def->throwable) return false;
    // Mesmo arco da dinamite; stats do def (sem switch por id).
    core::Vec2f vel{220.f * static_cast<float>(facing), -320.f};
    support::Throwable* t = throws.throwItem({getCenterX(), getCenterY()},
                                             vel, def->throwKind);
    if (!t) return false;
    t->fuse        = def->fuse;
    t->radius      = def->blastRadius;
    t->damage      = def->blastDamage;
    t->tilesRadius = def->blastTiles;
    if (item.quantity <= 1) item = core::Item{};
    else --item.quantity;
    throwCooldown.trigger();
    throwAnimT = kThrowAnimDur;
    return true;
}

bool Player::tryUseSlot(int slot) {
    if (slot < 0 || slot >= core::Inventory::kCapacity) return false;
    core::Item& item = inventory.slot(slot);
    if (item.isEmpty()) return false;
    const core::ItemDef* def = item.def();
    if (!def || !def->onUse) return false;
    def->onUse(*this);
    if (item.quantity <= 1) item = core::Item{};
    else --item.quantity;
    return true;
}

bool Player::castAttuned(support::ThrowSystem &throws) {
    if (!throwCooldown.ready()) return false;
    if (attuned.empty()) return false;
    const core::ItemDef* def =
        core::ItemRegistry::instance().find(attuned[0]);
    if (!def || def->type != core::ItemType::Spell) return false;
    const int inte = attrs.get(core::Attr::Intelligence);
    const int fai = attrs.get(core::Attr::Faith);
    if (inte < def->intReq || fai < def->faiReq) return false;
    if (def->spellKind == core::SpellKind::Arrow) {
        if (fp < kArrowCost) return false;
        core::Vec2f vel{500.f * static_cast<float>(facing), -80.f};
        support::Throwable* t = throws.throwItem(
            {getCenterX(), getCenterY()}, vel, support::ThrowKind::Bolt);
        if (!t) return false;
        t->fuse = -1.f; // sem fuse: impacto + expira (igual spit)
        t->damage =
            static_cast<int>(kArrowBase + kArrowBase * core::scaleFactor(inte));
        t->radius = 0.f;
        t->tilesRadius = 0;
        fp -= kArrowCost;
        throwCooldown.trigger();
        throwAnimT = kThrowAnimDur;
        return true;
    }
    if (def->spellKind == core::SpellKind::Heal) {
        if (fp < kHealCost) return false;
        hp = std::min(hpMax, hp + static_cast<int>(kHealBase + fai * 2));
        fp -= kHealCost;
        throwCooldown.trigger();
        return true;
    }
    return false;
}

bool Player::tryThrow(support::ThrowSystem &throws) {    if (!throwCooldown.ready() || inventory.count("dynamite") <= 0)
        return false;
    // Arco fixo na direção do facing; sem mira manual no MVP.
    core::Vec2f vel{220.f * static_cast<float>(facing), -320.f};
    if (!throws.throwItem({getCenterX(), getCenterY()}, vel)) return false;
    inventory.remove("dynamite");
    throwCooldown.trigger();
    throwAnimT = kThrowAnimDur;
    return true;
}

void Player::refreshDerived() {
    hpMax = core::Attributes::maxHP(attrs.get(core::Attr::Vitality));
    staminaMax = static_cast<float>(
        core::Attributes::maxStamina(attrs.get(core::Attr::Endurance)));
    fpMax = 100.f + attrs.get(core::Attr::Attunement) * 10.f;
    resistances_ = computeResistances();
    if (hp > hpMax) hp = hpMax;
    if (stamina > staminaMax) stamina = staminaMax;
    if (fp > fpMax) fp = fpMax;
}

bool Player::attune(const std::string& defId) {
    const core::ItemDef* def = core::ItemRegistry::instance().find(defId);
    if (!def || def->type != core::ItemType::Spell) return false;
    for (const auto& id : attuned)
        if (id == defId) return false; // já sintonizada
    if (static_cast<int>(attuned.size()) >= spellSlots()) return false;
    if (attrs.get(core::Attr::Intelligence) < def->intReq) return false;
    if (attrs.get(core::Attr::Faith) < def->faiReq) return false;
    attuned.push_back(defId);
    return true;
}

bool Player::unattune(const std::string& defId) {
    const auto it = std::remove(attuned.begin(), attuned.end(), defId);
    if (it == attuned.end()) return false;
    attuned.erase(it, attuned.end());
    return true;
}

const core::ItemDef* Player::weaponDef() const {
    const core::Item& w = equipment.get(core::EquipSlot::RightHand);
    return w.isEmpty() ? nullptr : w.def();
}

const core::ItemDef* Player::offHandDef() const {
    const core::Item& w = equipment.get(core::EquipSlot::LeftHand);
    return w.isEmpty() ? nullptr : w.def();
}

void Player::addPoison(float amt) {
    if (poisonTimer > 0.f) return; // ativo: barra não acumula de novo
    poisonBuildup += amt;
    if (poisonBuildup >= statusThreshold()) {
        poisonBuildup = 0.f;
        poisonTimer = kPoisonDur;
    }
}

void Player::addFrost(float amt) {
    if (frostTimer > 0.f) return; // ativo: barra não acumula de novo
    frostBuildup += amt;
    if (frostBuildup >= statusThreshold()) {
        frostBuildup = 0.f;
        frostTimer = kFrostDur;
    }
}

void Player::addBleed(float amt) {
    bleedBuildup += amt;
    if (bleedBuildup >= statusThreshold()) {
        bleedBuildup = 0.f; // burst e reseta (reacumula depois)
        bleedSlowTimer = 0.3f; // micro-slow: corta sprint, pune posição
        hp -= static_cast<int>(hpMax * kBleedPct);
        if (hp < 0) hp = 0;
    }
}

core::StatusModifiers Player::computeModifiers() const {
    core::StatusModifiers mods;
    if (bleedSlowTimer > 0.f) mods.moveSpeedMult = 0.9f;
    if (frostTimer > 0.f) mods.attackSpeedMult = kFrostSlow; // Fase 2
    return mods;
}

int Player::effectiveHpMax() const {
    return static_cast<int>(hpMax * computeModifiers().hpMaxMult);
}

bool Player::hurt(int dmg, core::DamageType type) {
    if (dmg <= 0 || hp <= 0 || !hurtIframes.ready()) return false;
    const int after =
        core::applyResistance(dmg, type, resistances_);
    hp -= static_cast<int>(after * computeModifiers().damageTakenMult);
    if (hp < 0) hp = 0;
    hurtIframes.trigger(0.6f);
    return true;
}

core::Resistances Player::computeResistances() const {
    using core::Attr;
    using core::DamageType;
    core::Resistances r;
    // Mapeamento do plano: END protege físico/frost, VIT o fogo,
    // FTH o lightning. DS1: todo nível de alma protege um pouco
    // (universal = 0.2% por nível além do 1 em tudo).
    const float uni =
        1.f - static_cast<float>(attrs.level() - 1) * 0.002f;
    const float end = static_cast<float>(attrs.get(Attr::Endurance) - 10);
    const float vit = static_cast<float>(attrs.get(Attr::Vitality) - 10);
    const float fth = static_cast<float>(attrs.get(Attr::Faith) - 10);
    r.set(DamageType::Physical, uni - end * 0.005f);
    r.set(DamageType::Frost, uni - end * 0.003f);
    r.set(DamageType::Fire, uni - vit * 0.003f);
    r.set(DamageType::Lightning, uni - fth * 0.004f);
    return r;
}

void Player::respawn(float x, float y) {
    setX(x);
    setY(y);
    setVx(0.f);
    setVy(0.f);
    hp = hpMax;
    hurtIframes.reset();
    throwCooldown.reset();
    staminaDelay.reset();
    curePoison();
    cureBleed();
    cureFrost();
    fp = fpMax; // respawn renova FP (DS)
    topUpDynamite();
    topUpStarterKit();
    refreshDerived();
    stamina = staminaMax; // respawn renova tudo (DS)
    meleePhase = MeleePhase::Idle;
    meleeCombo = 0;
    meleeTimer = 0.f;
    facing = 1;
    jumping = false;
    inWater = false;
    jumpingRecharge = 0.f;
    moveDown = moveUp = moveLeft = moveRight = runFast = false;
}

namespace {
struct MeleeDef {
    float windup, active, recovery;
    int   damage;
    float posture;
    float hx, hy; // tamanho da hitbox
};

// Combo light 3-hit: tempos em segundos (tick fixo 1/30).
constexpr MeleeDef kLight[3] = {
    {0.06f, 0.08f, 0.10f,  8,  5.f, 16.f, 20.f},
    {0.05f, 0.08f, 0.12f, 10,  6.f, 18.f, 20.f},
    {0.10f, 0.10f, 0.22f, 16, 12.f, 22.f, 24.f},
};
} // namespace

namespace {
// Rect local ao centro do player por direção (Y cresce p/ baixo).
// Só vale com arma; soco usa kLight (reto, sem direcionalidade).
struct AimHitboxLocal {
    float cx, cy, w, h;
};
constexpr AimHitboxLocal kAimHitbox[8] = {
    {20.f, 0.f, 20.f, 14.f},   // E
    {14.f, -14.f, 18.f, 14.f}, // NE
    {0.f, -20.f, 14.f, 20.f},  // N
    {-14.f, -14.f, 18.f, 14.f},// NW
    {-20.f, 0.f, 20.f, 14.f},  // W
    {-14.f, 14.f, 18.f, 14.f}, // SW
    {0.f, 20.f, 14.f, 20.f},   // S
    {14.f, 14.f, 18.f, 14.f},  // SE
};
} // namespace

bool Player::startSwing() {
    if (meleePhase == MeleePhase::Idle) {
        meleeCombo = 0;
    } else if (meleePhase == MeleePhase::Recovery) {
        meleeCombo = (meleeCombo + 1) % 3;
    } else {
        return false; // Windup/Active: press ignorado
    }
    if (stamina < kSwingCost * computeModifiers().staminaCostMult)
        return false; // sem fôlego: sem golpe
    stamina -= kSwingCost * computeModifiers().staminaCostMult;
    staminaDelay.trigger();
    meleePhase = MeleePhase::Windup;
    meleeTimer = kLight[meleeCombo].windup;
    meleeSwingId++;
    swingAim = aimDir; // congela direção do próximo golpe
    return true;
}

MeleePhase Player::updateMelee(float dt) {
    if (meleePhase == MeleePhase::Idle) return meleePhase;
    // Frost ativo: swing inteiro corre em câmera lenta (×0.7).
    meleeTimer -= dt * computeModifiers().attackSpeedMult;
    if (meleeTimer > 0.f) return meleePhase;
    const MeleeDef &d = kLight[meleeCombo];
    if (meleePhase == MeleePhase::Windup) {
        meleePhase = MeleePhase::Active;
        meleeTimer = d.active;
    } else if (meleePhase == MeleePhase::Active) {
        meleePhase = MeleePhase::Recovery;
        meleeTimer = d.recovery;
    } else { // Recovery esgotou: volta ao Idle
        meleePhase = MeleePhase::Idle;
        meleeCombo = 0;
    }
    return meleePhase;
}

sf::FloatRect Player::meleeHitbox() {
    if (meleePhase != MeleePhase::Active) return sf::FloatRect{};

    // Com arma: 8 rects por swingAim (snapshot; input não move o golpe).
    // swingAim já é screen-space (tecla esquerda = W = esquerda da tela),
    // então NÃO espelha por facing — o * facing aqui duplicava o espelho
    // e jogava o W para a direita (melee só acertava à direita).
    if (const core::ItemDef* wdef = weaponDef()) {
        const auto &hb = kAimHitbox[static_cast<int>(swingAim)];
        float ws = 1.f, hs = 1.f;
        if (const auto *wd =
                support::WeaponRegistry::instance().find(wdef->id)) {
            ws = wd->spriteW / 16.f;
            hs = wd->spriteH / 8.f;
        }
        // Alcance escala com o corpo (bs=2 a 100px): soco de 1 bloco
        // nunca sairia do corpo de 2 blocos. Soco (abaixo) já é dinâmico.
        const float bs = getH() / 50.f;
        const float w = hb.w * bs * ws, h = hb.h * bs * hs;
        const float cx = getCenterX() + hb.cx * bs;
        const float cy = getCenterY() + hb.cy * bs;
        return sf::FloatRect{cx - w * 0.5f, cy - h * 0.5f, w, h};
    }

    // Fallback: soco. Mantém kLight[] — sem direcionalidade (soco é reto).
    const MeleeDef &d = kLight[meleeCombo];
    const float cx = getCenterX() + static_cast<float>(facing) * (getW() * 0.5f + d.hx * 0.5f);
    const float cy = getCenterY();
    return sf::FloatRect{cx - d.hx * 0.5f, cy - d.hy * 0.5f, d.hx, d.hy};
}

int Player::meleeDamage() const {
    // Acumula em float e trunca UMA vez no fim (bônus < 1 somado ao
    // combo ainda conta; truncar cada parcela zerava STR baixo).
    float dmg = static_cast<float>(kLight[meleeCombo].damage);
    if (const core::ItemDef* off = offHandDef()) dmg += off->damage;
    if (const core::ItemDef* wdef = weaponDef()) {
        // Scaling DS: dano base × Σ letra×fator. Base 10 = zero bônus
        // (seed intacto); soft cap 30. Sem req = metade de tudo.
        dmg += wdef->damage *
            (core::scaleMult(wdef->strScale) *
                 core::scaleFactor(attrs.get(core::Attr::Strength)) +
             core::scaleMult(wdef->dexScale) *
                 core::scaleFactor(attrs.get(core::Attr::Dexterity)) +
             core::scaleMult(wdef->intScale) *
                 core::scaleFactor(attrs.get(core::Attr::Intelligence)) +
             core::scaleMult(wdef->faiScale) *
                 core::scaleFactor(attrs.get(core::Attr::Faith)));
        if (attrs.get(core::Attr::Strength) < wdef->strReq ||
            attrs.get(core::Attr::Dexterity) < wdef->dexReq)
            dmg *= 0.5f;
    }
    return static_cast<int>(dmg);
}

float Player::meleePosture() const { return kLight[meleeCombo].posture; }
