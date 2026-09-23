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
Entity(core::kIdPlayer,0,0,30,50) // AABB derivado do sprite 12x20 a 2.5x
{
    setFillColor(sf::Color::Red);
    // humanoid(ALTURA, LARGURA): AABB 30x50. Já foi (30, 50) invertido —
    // hw=25 punha o ArmL 12px fora do AABB (caixa magenta flutuante).
    static auto schema = support::BodySchema::humanoid(50.f, 30.f);
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
    sf::Vector2f vel{220.f * static_cast<float>(facing), -320.f};
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

bool Player::tryThrow(support::ThrowSystem &throws) {    if (!throwCooldown.ready() || inventory.count("dynamite") <= 0)
        return false;
    // Arco fixo na direção do facing; sem mira manual no MVP.
    sf::Vector2f vel{220.f * static_cast<float>(facing), -320.f};
    if (!throws.throwItem({getCenterX(), getCenterY()}, vel)) return false;
    inventory.remove("dynamite");
    throwCooldown.trigger();
    throwAnimT = kThrowAnimDur;
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

bool Player::hurt(int dmg) {
    if (dmg <= 0 || hp <= 0 || !hurtIframes.ready()) return false;
    hp -= dmg;
    if (hp < 0) hp = 0;
    hurtIframes.trigger(0.6f);
    return true;
}

void Player::respawn(float x, float y) {
    setX(x);
    setY(y);
    setVx(0.f);
    setVy(0.f);
    hp = hpMax;
    hurtIframes.reset();
    throwCooldown.reset();
    topUpDynamite();
    topUpStarterKit();
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
    meleePhase = MeleePhase::Windup;
    meleeTimer = kLight[meleeCombo].windup;
    meleeSwingId++;
    swingAim = aimDir; // congela direção do próximo golpe
    return true;
}

MeleePhase Player::updateMelee(float dt) {
    if (meleePhase == MeleePhase::Idle) return meleePhase;
    meleeTimer -= dt;
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
        const float w = hb.w * ws, h = hb.h * hs;
        const float cx = getCenterX() + hb.cx;
        const float cy = getCenterY() + hb.cy;
        return sf::FloatRect{cx - w * 0.5f, cy - h * 0.5f, w, h};
    }

    // Fallback: soco. Mantém kLight[] — sem direcionalidade (soco é reto).
    const MeleeDef &d = kLight[meleeCombo];
    const float cx = getCenterX() + static_cast<float>(facing) * (getW() * 0.5f + d.hx * 0.5f);
    const float cy = getCenterY();
    return sf::FloatRect{cx - d.hx * 0.5f, cy - d.hy * 0.5f, d.hx, d.hy};
}

int Player::meleeDamage() const {
    int dmg = kLight[meleeCombo].damage;
    if (const core::ItemDef* off = offHandDef()) dmg += off->damage;
    return dmg;
}

float Player::meleePosture() const { return kLight[meleeCombo].posture; }
