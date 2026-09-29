/**
 * @file src/entities/Player/Player.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementa lógica do jogador, física, combate e inventário.
 * @details Configura AABB 30x50, gravidade e equipamento inicial de ferro, processa tick, colisão, arremesso, melee, dano e respawn, chamado por Game App e sistemas.
 */

#include "Player.h"
#include <algorithm>
#include <iostream>
#include "assets/PlayerClips.h"
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
    // Braços canônicos: 3 rows ombro→cotovelo + 3 cotovelo→mão (arte).
    limbR_.upper.length = limbR_.lower.length = 3.f;
    limbL_.upper.length = limbL_.lower.length = 3.f;
    // Sem seed: nasce Desprovido (base 10, 1 poção, sem souls);
    // menu de criação aplica a classe via applyClass (#3).
    // Kit generoso e set de ferro morreram com o seed.
    applyClass(core::PlayerClass::Deprived);
    stamina = staminaMax;
    fp = fpMax;
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
        const bool wasAirborne = !jumping; // borda de descida: pousou
        setY(bloco.top - getH());
        moveDown = false;
        jumping = true;
        setVy(0.f); // pouso mata a queda (gravidade reacumula se sair)
        if (wasAirborne) landAnimT = kLandAnimDur; // squash + poeira
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
        const bool wasAirborne = !jumping; // borda de descida: pousou
        setY(bloco.top - getH());
        moveDown = false;
        jumping = true;
        setVy(0.f); // pouso mata a queda (gravidade reacumula se sair)
        if (wasAirborne) landAnimT = kLandAnimDur; // squash + poeira
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
    st.rollT = rollTimer;
    st.rollDir = rollDir;

    physics::Input in{moveUp, moveDown, moveLeft, moveRight, runFast,
                      computeModifiers().moveSpeedMult};
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
    if (s.rollT > 0.f) {
        rollTimer = s.rollT;
    } else {
        rollTimer = 0.f;
    }
    rollIframes.tick(physics::kFixedDt);

    // Integração já feita no step; aqui só o sync do Entity
    // (era Entity::tick sem o x += vx): posição do shape + sensores.
    setX(s.x);
    setY(s.y);
    setVx(s.vx);
    setVy(s.vy);
    this->left = getX();
    this->top = getY();
    this->setPosition(getX(), getY());

    // Corpo híbrido (Fase A): no swing vira para o snapshot do golpe;
    // andando segue a velocidade (eixo dominante); parado mantém.
    if (inMeleeSwing()) {
        setFacing8(swingAim);
    } else if (std::fabs(getVx()) > 1.f || std::fabs(getVy()) > 1.f) {
        setFacing8(support::facingFromVelocity(getVx(), getVy(), facing8));
    }
    // Mãos procedurais fora do swing (no swing, updateMelee dirige).
    // Locomoção primeiro: a senoide dos braços lê o índice do clip.
    if (!inMeleeSwing()) {
        const bool marching = (moveLeft || moveRight) && jumping;
        loco.play(marching ? game::walkClip() : game::idleClip());
        loco.tick(physics::kFixedDt);
        updateLimbs();
    }

    // Regen de estamina: 30/s × mult, só com delay pronto.
    staminaDelay.tick(1.f / 30.0f);
    if (stamina < staminaMax && staminaDelay.ready())
        stamina = std::min(
            staminaMax, stamina + 1.f * computeModifiers().staminaRegenMult);

    // Slow do bleed decai aqui (0.3s de micro-stagger).
    if (bleedSlowTimer > 0.f) bleedSlowTimer -= 1.f / 30.0f;
    if (frostTimer > 0.f) frostTimer -= 1.f / 30.0f; // swing lento expira
    // Squash de pouso decai sozinho (0.12s; Renderer lê o restante).
    if (landAnimT > 0.f) {
        landAnimT -= 1.f / 30.0f;
        if (landAnimT < 0.f) landAnimT = 0.f;
    }
    // Buff da arma expira sozinho (timer 0 = permanente até trocar).
    if (weaponBuffTimer > 0.f) {
        weaponBuffTimer -= 1.f / 30.0f;
        if (weaponBuffTimer <= 0.f)
            weaponBuffType = core::DamageType::Physical;
    }

    // HP nunca acima do máximo efetivo (tarô + maldição reduzem).
    if (hp > effectiveHpMax()) hp = effectiveHpMax();

    // Regen de FP: 8/s × tarô, sem delay (magia F8).
    if (fp < fpMax)
        fp = std::min(fpMax, fp + 8.f / 30.0f * tarotFx.fpRegenMult);

    // Tarô: vinheta, killstacks, conversão e maldição do peso.
    tickTarot(physics::kFixedDt);

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

void Player::applyClass(core::PlayerClass klass) {
    // Personagem novo: zera bens, carteira e magias do seed/kit.
    // Tarô começa vazio (morre-se sem fado); deleteCharacter noutro.
    const core::ClassDef def = core::classDef(klass);
    for (int i = 0; i < core::kAttrCount; ++i) {
        attrs.set(static_cast<core::Attr>(i),
                  def.base[i] <= 0 ? 10 : def.base[i]);
    }
    equipment = core::Equipment{};
    inventory = core::Inventory{};
    attuned.clear();
    souls = 0;
    for (const auto& id : def.equipment) {
        if (core::ItemRegistry::instance().find(id))
            equipment.equip(core::Item{id, 1});
    }
    for (const auto& [id, qty] : def.items) {
        if (!core::ItemRegistry::instance().find(id)) continue;
        int left = qty;
        while (left > 0) {
            const int put = std::min(left, 99);
            if (inventory.add(core::Item{
                    id, static_cast<uint16_t>(put)}) > 0)
                break; // cheio: fica com o que coube
            left -= put;
        }
    }
    for (const auto& id : def.spells) attune(id); // cabe ou fica p/ depois
    refreshDerived();
    hp = hpMax;
    stamina = staminaMax;
    fp = fpMax;
    curePoison();
    cureBleed();
    cureFrost();
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
    // Catalisador DS: staff p/ magias, sino p/ milagres — EQUIPADO
    // em qualquer mão (mochila não conta).
    if (def->reqCatalyst != core::CatalystKind::None) {
        bool has = false;
        for (auto slot :
             {core::EquipSlot::RightHand, core::EquipSlot::LeftHand}) {
            const core::Item &it = equipment.get(slot);
            if (it.isEmpty()) continue;
            if (const core::ItemDef *d = it.def()) {
                if (d->providesCatalyst == def->reqCatalyst) {
                    has = true;
                    break;
                }
            }
        }
        if (!has) return false;
    }
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
        hp = std::min(effectiveHpMax(),
                      hp + static_cast<int>((kHealBase + fai * 2) *
                                            tarotFx.healingReceivedMult));
        fp -= kHealCost;
        throwCooldown.trigger();
        return true;
    }
    if (def->spellKind == core::SpellKind::Fire) {
        // Bola de Fogo (Fase 3): linear rápida + explosão Fire no
        // impacto (raio 40, sem quebrar tiles). Escala com INT.
        if (fp < kFireCost) return false;
        core::Vec2f vel{450.f * static_cast<float>(facing), -60.f};
        support::Throwable* t = throws.throwItem(
            {getCenterX(), getCenterY()}, vel, support::ThrowKind::Fireball);
        if (!t) return false;
        t->fuse = -1.f; // impacto dispara a explosão (igual Bolt)
        t->damage =
            static_cast<int>(kFireBase + kFireBase * core::scaleFactor(inte));
        t->damageType = core::DamageType::Fire;
        t->radius = kFireRadius;
        t->tilesRadius = 0; // fogo não quebra rocha
        fp -= kFireCost;
        throwCooldown.trigger();
        throwAnimT = kThrowAnimDur;
        return true;
    }
    if (def->spellKind == core::SpellKind::FrostWeapon) {
        // Arma Gélida (Fase 3): melee vira Frost (Temperança encurta).
        if (fp < kFrostWeaponCost) return false;
        weaponBuffType = core::DamageType::Frost;
        weaponBuffTimer = kFrostWeaponDur * tarotFx.buffDurationMult;
        fp -= kFrostWeaponCost;
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
    // Mundo: atributos contam +10% nos vitais (VIT→HP, END→fôlego).
    const int vit =
        static_cast<int>(attrs.get(core::Attr::Vitality) * tarotFx.attrMult);
    const int end =
        static_cast<int>(attrs.get(core::Attr::Endurance) * tarotFx.attrMult);
    hpMax = core::Attributes::maxHP(vit);
    staminaMax = static_cast<float>(core::Attributes::maxStamina(end));
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

bool Player::startRoll() {
    if (hp <= 0 || rollTimer > 0.f) return false;
    if (!jumping) return false; // só no chão (jumping=true = chão)
    if (meleePhase == MeleePhase::Active) return false;
    if (stamina < physics::kRollCost * computeModifiers().staminaCostMult)
        return false; // sem fôlego: sem rolagem
    stamina -= physics::kRollCost * computeModifiers().staminaCostMult;
    staminaDelay.trigger();
    rollDir = moveLeft ? -1 : (moveRight ? 1 : facing);
    setFacing8(rollDir >= 0 ? support::Facing::E : support::Facing::W);
    rollTimer = physics::kRollDur;
    // Fat roll (carga pesada) não dá i-frames (DS).
    if (!heavilyLoaded()) rollIframes.trigger();
    return true;
}

bool Player::cycleHand(core::EquipSlot hand, std::string *outName) {
    if (!canQuickSwap()) return false;
    if (hand != core::EquipSlot::RightHand &&
        hand != core::EquipSlot::LeftHand)
        return false;
    // Candidatas: equipada atual + armas da mochila, em ordem do
    // registry (canônica; Arrange do inventário não muda o ciclo).
    std::vector<std::string> ids;
    const core::Item &cur = equipment.get(hand);
    if (!cur.isEmpty()) ids.push_back(cur.defId);
    for (const auto &id : core::ItemRegistry::instance().keys()) {
        if (!cur.isEmpty() && id == cur.defId) continue;
        if (inventory.count(id) <= 0) continue;
        const core::ItemDef *d = core::ItemRegistry::instance().find(id);
        if (!d || d->type != core::ItemType::Weapon) continue;
        ids.push_back(id);
    }
    if (ids.size() < 2) return false; // nada p/ trocar
    // Mão equipada: atual é ids[0], próxima é ids[1]. Mão vazia:
    // ids[0] é da mochila, equipa direto.
    const std::string &nextId = cur.isEmpty() ? ids[0] : ids[1];
    if (!cur.isEmpty()) {
        core::Item back = equipment.unequip(hand);
        if (!back.isEmpty()) inventory.add(back);
    }
    if (!inventory.remove(nextId, 1)) return false;
    if (!equipment.equipTo(hand, core::Item{nextId, 1})) {
        inventory.add(core::Item{nextId, 1}); // reverte: devolve
        return false;
    }
    if (outName) {
        if (const core::ItemDef *d =
                core::ItemRegistry::instance().find(nextId))
            *outName = d->name;
    }
    refreshDerived(); // peso da arma conta na carga
    return true;
}

bool Player::cycleSpell(std::string *outName) {
    if (!canQuickSwap()) return false;
    if (attuned.size() < 2) return false;
    std::rotate(attuned.begin(), attuned.begin() + 1, attuned.end());
    if (outName) {
        if (const core::ItemDef *d =
                core::ItemRegistry::instance().find(attuned[0]))
            *outName = d->name;
    }
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

    // Atributos: Destreza aumenta velocidade de caminhada
    int dex = attrs.get(core::Attr::Dexterity);
    float dexBonus = 1.0f + std::max(0, dex - 10) * 0.01f; // +1% por ponto acima de 10
    mods.moveSpeedMult *= dexBonus;

    // Atributos: Resistência reduz custo de stamina em ataques
    int res = attrs.get(core::Attr::Resistance);
    float resBonus = 1.0f - std::max(0, res - 10) * 0.005f; // -0.5% por ponto acima de 10
    if (resBonus < 0.5f) resBonus = 0.5f; // limite inferior
    mods.staminaCostMult *= resBonus;

    // Tarô: multiplica sobre os status + maldição do peso.
    mods.moveSpeedMult *= tarotFx.moveSpeedMult * curseMoveMult_;
    mods.damageTakenMult *= tarotFx.damageTakenMult;
    mods.attackSpeedMult *= tarotFx.attackSpeedMult;
    mods.staminaRegenMult *= tarotFx.staminaRegenMult;
    mods.staminaCostMult *= tarotFx.staminaCostMult;
    mods.hpMaxMult *= tarotFx.hpMaxMult * curseHpMaxMult_;
    return mods;
}

float Player::takenMult() const {
    // Recebido = taken ÷ defesa ÷ maldição (defesa <1 = apanha mais).
    // Condicionais (9 de Paus, Carro) vivem no hurt(), não aqui.
    const float def = tarotFx.defenseMult * curseDefMult_;
    if (def <= 0.f) return computeModifiers().damageTakenMult;
    return computeModifiers().damageTakenMult / def;
}

void Player::showTarotReveal(core::TarotArcana a) {
    tarotRevealActive_ = true;
    tarotRevealArcana_ = a;
    tarotRevealAge_ = 0.f;
}

Player::CurseInfo Player::tarotCurse() const {
    CurseInfo c;
    const int w = tarotWeight_;
    if (w <= 100) return c;
    c.cursed = true;
    c.hpMaxMult = 0.95f;
    if (w > 150) c.moveMult = 0.90f;
    if (w > 200) c.defMult = 0.95f;
    if (w > 300) c.dot20 = true;
    if (w > 400) c.dot15 = true;
    if (w > 500) c.poison = true;
    return c;
}

void Player::addKillStack() {
    if (tarotFx.killStackMax <= 0) return;
    if (killStacks_ < tarotFx.killStackMax) ++killStacks_;
    killTimer_ = 30.f;
}

void Player::tickTarot(float dt) {
    // Vinheta do fado: 1.5s, some sozinha (jogo não pausa).
    if (tarotRevealActive_) {
        tarotRevealAge_ += dt;
        if (tarotRevealAge_ >= kTarotRevealLife) tarotRevealActive_ = false;
    }
    if (tarotRecentAge_ < 30.f) tarotRecentAge_ += dt;
    // Morte: janela de 30s sem matar zera os stacks.
    if (killTimer_ > 0.f) {
        killTimer_ -= dt;
        if (killTimer_ <= 0.f) {
            killTimer_ = 0.f;
            killStacks_ = 0;
        }
    }
    // Enforcado: bônus convertido expira em 10s.
    if (convTimer_ > 0.f) {
        convTimer_ -= dt;
        if (convTimer_ <= 0.f) {
            convTimer_ = 0.f;
            convBonus_ = 0.f;
        }
    }
    applyTarotCurse(dt);
}

void Player::applyTarotCurse(float dt) {
    // Peso do destino por faixa (só acima de 100 cobra). Mults
    // recompostos do zero a cada tick; DoT trava HP em 1 (nunca mata).
    const int w = tarotWeight_;
    curseHpMaxMult_ = 1.f;
    curseMoveMult_ = 1.f;
    curseDefMult_ = 1.f;
    if (w > 100) curseHpMaxMult_ *= 0.95f;
    if (w > 150) curseMoveMult_ *= 0.90f;
    if (w > 200) curseDefMult_ *= 0.95f;
    if (w > 300) {
        curseDotT_ += dt;
        if (curseDotT_ >= 20.f) {
            curseDotT_ = 0.f;
            hp = std::max(1, hp - 1);
        }
    } else {
        curseDotT_ = 0.f;
    }
    if (w > 400) {
        curseDotT_ += dt;
        if (curseDotT_ >= 15.f) {
            curseDotT_ = 0.f;
            hp = std::max(1, hp - 2);
        }
    }
    if (w > 500) {
        cursePoisonT_ += dt;
        if (cursePoisonT_ >= 1.f) {
            cursePoisonT_ -= 1.f;
            hp = std::max(1, hp - 1);
        }
    } else {
        cursePoisonT_ = 0.f;
    }
    if (hp > effectiveHpMax()) hp = effectiveHpMax();
}

void Player::deleteCharacter() {
    // Única saída do fado: apaga TUDO (nível, cartas, bens, souls).
    attrs = core::Attributes{};
    tarotCards.clear();
    recomputeTarot();
    tarotRevivesUsed_ = 0;
    killStacks_ = 0;
    killTimer_ = 0.f;
    convBonus_ = 0.f;
    convTimer_ = 0.f;
    tarotRevealActive_ = false;
    tarotRecentCount_ = 0;
    tarotRecentAge_ = 999.f;
    inventory = core::Inventory{};
    equipment = core::Equipment{};
    attuned.clear();
    souls = 0;
    refreshDerived();
    hp = hpMax;
    stamina = staminaMax;
    fp = fpMax;
    curePoison();
    cureBleed();
    cureFrost();
}

void Player::addTarotCard(core::TarotArcana a) {
    if (!core::TarotRegistry::instance().has(a)) return;
    tarotCards[a] += 1;
    // Anel das últimas 3 (HUD compacto, fade 30s no tickTarot).
    tarotRecent_[tarotRecentCount_ % 3] = a;
    ++tarotRecentCount_;
    tarotRecentAge_ = 0.f;
    recomputeTarot();
}

void Player::recomputeTarot() {
    tarotFx = core::TarotEffect{};
    totalTarotCards_ = 0;
    tarotWeight_ = 0;
    const auto addMult = [](float base, int n) {
        return 1.f + (base - 1.f) * static_cast<float>(n);
    };
    for (const auto &[arcana, count] : tarotCards) {
        const core::TarotCardDef *def =
            core::TarotRegistry::instance().find(arcana);
        if (!def) continue;
        const auto &e = def->effect;
        tarotFx.damageMult *= addMult(e.damageMult, count);
        tarotFx.magicDamageMult *= addMult(e.magicDamageMult, count);
        tarotFx.physicalDamageMult *= addMult(e.physicalDamageMult, count);
        tarotFx.critDamageMult *= addMult(e.critDamageMult, count);
        tarotFx.attackSpeedMult *= addMult(e.attackSpeedMult, count);
        tarotFx.hpMaxMult *= addMult(e.hpMaxMult, count);
        tarotFx.defenseMult *= addMult(e.defenseMult, count);
        tarotFx.postureMaxMult *= addMult(e.postureMaxMult, count);
        tarotFx.statusResistMult *= addMult(e.statusResistMult, count);
        tarotFx.healingReceivedMult *=
            addMult(e.healingReceivedMult, count);
        tarotFx.moveSpeedMult *= addMult(e.moveSpeedMult, count);
        tarotFx.staminaRegenMult *= addMult(e.staminaRegenMult, count);
        tarotFx.staminaCostMult *= addMult(e.staminaCostMult, count);
        tarotFx.fpRegenMult *= addMult(e.fpRegenMult, count);
        tarotFx.soulsGainMult *= addMult(e.soulsGainMult, count);
        tarotFx.xpGainMult *= addMult(e.xpGainMult, count);
        tarotFx.itemDropChanceMult *=
            addMult(e.itemDropChanceMult, count);
        tarotFx.damageTakenMult *= addMult(e.damageTakenMult, count);
        tarotFx.fireResistMult *= addMult(e.fireResistMult, count);
        tarotFx.magicResistMult *= addMult(e.magicResistMult, count);
        tarotFx.physicalResistMult *= addMult(e.physicalResistMult, count);
        tarotFx.lowHpDamageMult *= addMult(e.lowHpDamageMult, count);
        tarotFx.highHpDamageMult *= addMult(e.highHpDamageMult, count);
        tarotFx.fullHpDamageMult *= addMult(e.fullHpDamageMult, count);
        tarotFx.woundedTargetDamageMult *=
            addMult(e.woundedTargetDamageMult, count);
        tarotFx.aloneDamageMult *= addMult(e.aloneDamageMult, count);
        tarotFx.heavyDamageMult *= addMult(e.heavyDamageMult, count);
        tarotFx.chargeDamageMult *= addMult(e.chargeDamageMult, count);
        tarotFx.precisionMult *= addMult(e.precisionMult, count);
        tarotFx.lowHpDefenseMult *= addMult(e.lowHpDefenseMult, count);
        tarotFx.movingDefenseMult *= addMult(e.movingDefenseMult, count);
        tarotFx.buffDurationMult *= addMult(e.buffDurationMult, count);
        tarotFx.attrMult *= addMult(e.attrMult, count);
        tarotFx.spawnRateMult *= addMult(e.spawnRateMult, count);
        tarotFx.daySpawnMult *= addMult(e.daySpawnMult, count);
        tarotFx.nightSpawnMult *= addMult(e.nightSpawnMult, count);
        tarotFx.enemyHpMult *= addMult(e.enemyHpMult, count);
        tarotFx.eliteChanceMult *= addMult(e.eliteChanceMult, count);
        tarotFx.lightMult *= addMult(e.lightMult, count);
        tarotFx.visionMult *= addMult(e.visionMult, count);
        tarotFx.fogMult *= addMult(e.fogMult, count);
        if (e.bloodMoon) tarotFx.bloodMoon = true;
        // Mago: +N slots a cada 2 cópias (genérico, não linear).
        tarotFx.spellSlots += e.spellSlots * (count / 2);
        // Especiais (somam, não multiplicam).
        if (e.reviveOnce) {
            tarotFx.reviveOnce = true;
            tarotFx.maxRevives += e.maxRevives * count;
            tarotFx.reviveHpPercent =
                std::min(1.f, tarotFx.reviveHpPercent +
                                  e.reviveHpPercent * count);
        }
        tarotFx.damageConversionRate += e.damageConversionRate * count;
        if (e.killStackMax > 0) {
            tarotFx.killStackMax =
                std::max(tarotFx.killStackMax, e.killStackMax);
            tarotFx.killStackBonus += e.killStackBonus * count;
        }
        totalTarotCards_ += count;
        tarotWeight_ +=
            core::weightOf(core::tierOf(arcana)) * count;
    }
    // Derivados que dependem do tarô (resists de fogo/magia, tetos).
    refreshDerived();
}

int Player::effectiveHpMax() const {
    return static_cast<int>(hpMax * computeModifiers().hpMaxMult);
}

bool Player::hurt(int dmg, core::DamageType type) {
    if (dmg <= 0 || hp <= 0 || !hurtIframes.ready()) return false;
    if (!rollIframes.ready()) return false; // rolagem: i-frame do roll
    const int after =
        core::applyResistance(dmg, type, resistances_);
    // Defesa condicional: 9 de Paus (HP<25%), Carro (em movimento).
    float def = tarotFx.defenseMult * curseDefMult_;
    const float hpFrac = effectiveHpMax() > 0
                             ? static_cast<float>(hp) / effectiveHpMax()
                             : 1.f;
    if (hpFrac < 0.25f) def *= tarotFx.lowHpDefenseMult;
    if ((moveLeft || moveRight) && tarotFx.movingDefenseMult != 1.f)
        def *= tarotFx.movingDefenseMult;
    const float taken = (def <= 0.f)
                            ? computeModifiers().damageTakenMult
                            : computeModifiers().damageTakenMult / def;
    const int finalDmg = static_cast<int>(after * taken);
    // Enforcado: fração do recebido vira bônus plano por 10s.
    if (tarotFx.damageConversionRate > 0.f && finalDmg > 0) {
        convBonus_ += finalDmg * tarotFx.damageConversionRate;
        convTimer_ = 10.f;
    }
    // Julgamento: nega a morte (1x por cópia), volta com fração do máx.
    if (hp - finalDmg <= 0 && tarotFx.reviveOnce &&
        tarotRevivesUsed_ < tarotFx.maxRevives) {
        ++tarotRevivesUsed_;
        hp = static_cast<int>(effectiveHpMax() * tarotFx.reviveHpPercent);
        if (hp < 1) hp = 1;
        hurtIframes.trigger(0.6f);
        return true;
    }
    hp -= finalDmg;
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
    r.set(DamageType::Physical,
          (uni - end * 0.005f) * tarotFx.physicalResistMult);
    r.set(DamageType::Frost, uni - end * 0.003f);
    r.set(DamageType::Fire,
          (uni - vit * 0.003f) * tarotFx.fireResistMult);
    r.set(DamageType::Lightning,
          (uni - fth * 0.004f) * tarotFx.magicResistMult);
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
    // Tarô sobrevive à morte (fado da run, não da vida): cartas,
    // peso e revives gastos persistem; só o momento de combate
    // zera (stacks, conversão, vinheta). Só deleteCharacter limpa.
    recomputeTarot();
    killStacks_ = 0;
    killTimer_ = 0.f;
    convBonus_ = 0.f;
    convTimer_ = 0.f;
    tarotRevealActive_ = false;
    fp = fpMax; // respawn renova FP (DS)
    topUpDynamite();
    topUpStarterKit();
    refreshDerived();
    stamina = staminaMax; // respawn renova tudo (DS)
    meleePhase = MeleePhase::Idle;
    meleeCombo = 0;
    meleeTimer = 0.f;
    setFacing8(support::Facing::E);
    targetHandR_ = targetHandL_ = {0.f, 0.f};
    handTargetsLive_ = false; // próximo tick recalcula (repouso)
    hitstopT = 0;
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
};

// Combo light 3-hit: tempos em segundos (tick fixo 1/30).
constexpr MeleeDef kLight[3] = {
    {0.06f, 0.08f, 0.10f,  8,  5.f},
    {0.05f, 0.08f, 0.12f, 10,  6.f},
    {0.10f, 0.10f, 0.22f, 16, 12.f},
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
    anim.play(game::attackClipFor(swingAim), true); // clip do zero
    updateLimbs(); // mão recua no eixo do golpe
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
        anim.gotoFrame(1); // frame do Active (Hitbox viva)
        updateLimbs(); // mão estende no eixo do golpe
    } else if (meleePhase == MeleePhase::Active) {
        meleePhase = MeleePhase::Recovery;
        meleeTimer = d.recovery;
        anim.gotoFrame(2); // Hitbox apaga junto com a fase
        updateLimbs(); // mão assenta
    } else { // Recovery esgotou: volta ao Idle
        meleePhase = MeleePhase::Idle;
        meleeCombo = 0;
        updateLimbs(); // repouso/marcha pela velocidade atual
    }
    return meleePhase;
}

void Player::updateLimbs() {
    const support::PartState* torso =
        body.find(support::BodyPartId::Torso);
    const support::PartState* armR =
        body.find(support::BodyPartId::ArmR);
    const support::PartState* armL =
        body.find(support::BodyPartId::ArmL);
    if (!torso || !armR || !armL) return; // sem boxes: mantém últimos
    const float row = getH() / 40.f;      // mundo por row (kPlayerH)
    // Sem BodySystem os boxes vêm zerados (só attach): ombro cai no
    // AABB (robusto como o rect antigo, que nunca lia boxes).
    const float fw0 = static_cast<float>(facing);
    auto shoulderOf = [&](const support::PartState* arm, float side) {
        if (torso->worldBox.width > 0.f && torso->worldBox.height > 0.f &&
            arm->worldBox.width > 0.f && arm->worldBox.height > 0.f) {
            const float cx =
                arm->worldBox.left + arm->worldBox.width * 0.5f;
            return support::limbShoulder(
                {torso->worldBox.left, torso->worldBox.top},
                {torso->worldBox.width, torso->worldBox.height}, cx);
        }
        return core::Vec2f{getCenterX() + fw0 * side * getW(),
                           getY() + getH() * 0.5f};
    };
    const core::Vec2f shR = shoulderOf(armR, 0.15f);
    const core::Vec2f shL = shoulderOf(armL, -0.15f);
    const float fw = static_cast<float>(facing);
    // Repouso: à frente e abaixo do ombro (braço caído natural).
    auto rest = [&](core::Vec2f sh, float fwdRows) {
        return core::Vec2f{sh.x + fw * fwdRows * row, sh.y + 2.f * row};
    };
    if (inMeleeSwing()) {
        // Snapshot: direita segue a fase no eixo do golpe (recuo -5,
        // impacto +8, assentando +2 rows); esquerda segura o repouso.
        const core::Vec2f aim = support::aimVector(swingAim);
        float reachRows = 2.f;
        if (meleePhase == MeleePhase::Windup)
            reachRows = -5.f;
        else if (meleePhase == MeleePhase::Active)
            reachRows = 8.f;
        targetHandR_ = {shR.x + aim.x * reachRows * row,
                        shR.y + aim.y * reachRows * row};
        targetHandL_ = rest(shL, 2.f);
    } else {
        targetHandR_ = rest(shR, 3.5f);
        targetHandL_ = rest(shL, 2.f);
        // Senoide de marcha no relógio do clip (4 fases: círculo cheio;
        // o bounce em Y vem de walkBobY com a mesma paridade). Vale no
        // ar rápido também (placeholder B.3).
        if (std::fabs(getVx()) > 1.f || std::fabs(getVy()) > 1.f) {
            constexpr float kPi = 3.14159265f;
            const float ph =
                static_cast<float>(loco.frameIndex()) * kPi * 0.5f;
            targetHandR_.x += std::cos(ph) * row * fw;
            targetHandR_.y += std::sin(ph) * row;
            targetHandL_.x += std::cos(ph + kPi) * row * fw;
            targetHandL_.y += std::sin(ph + kPi) * row;
        }
    }
    // Resolve poses (B.4, era no Renderer B.2): alcance calibrado por
    // frame (25% de folga: cotovelo visível, sem dobrar nem esticar).
    // Renderer desenha verbatim; bbox e arma leem a mesma pose.
    auto solve = [&](support::Limb& limb, support::LimbPose& pose,
                     core::Vec2f sh, core::Vec2f hand) {
        const float dx = hand.x - sh.x;
        const float dy = hand.y - sh.y;
        const float dist = std::sqrt(dx * dx + dy * dy);
        const float half = support::limbReach(dist, row) * 0.5f;
        support::Limb l = limb; // calibra sem sujar o canônico 3+3
        l.upper.length = half;
        l.lower.length = half;
        pose = support::solveIK(l, sh, hand, true, facing);
        support::applyPose(limb, pose); // ângulos persistem
    };
    solve(limbR_, poseR_, shR, targetHandR_);
    solve(limbL_, poseL_, shL, targetHandL_);
    handTargetsLive_ = true;
}

namespace {
// Lâmina além da mão em mundo (Fase E): maior eixo do sprite menos a
// origem (guarda), em sprite-px. Soco = punho + avanço (6 rows).
float bladeLengthRows(const Player* p) {
    if (const core::ItemDef* wdef = p->weaponDef()) {
        if (const auto* wd =
                support::WeaponRegistry::instance().find(wdef->id)) {
            return std::max({wd->spriteW - wd->originX,
                             wd->spriteH - wd->originY, 0.f});
        }
    }
    return 6.f;
}
} // namespace

support::SweepArc Player::sweepArc() {
    support::SweepArc arc;
    // Só o Active tem hitbox; sem pose (fase setada à mão), vazio.
    if (meleePhase != MeleePhase::Active || !handTargetsLive_) return arc;
    const float row = getH() / 40.f; // mundo por row (kPlayerH)
    const core::Vec2f o = poseR_.shoulderWorld;
    const core::Vec2f h = poseR_.handWorld;
    const float dx = h.x - o.x;
    const float dy = h.y - o.y;
    const float dist = std::sqrt(dx * dx + dy * dy);
    if (dist <= 1e-6f) return arc;
    const float thick = 2.f * row; // tolerância da lâmina
    arc.origin = o;
    arc.centerAngle = std::atan2(dy, dx);
    arc.halfWidth = support::kSweepHalfWidth;
    arc.rInner = std::max(0.f, dist - thick);
    arc.rOuter = dist + bladeLengthRows(this) * row;
    arc.empty = false;
    return arc;
}

support::SweepArc Player::predictedSweep(support::AimDir aim) {
    support::SweepArc arc;
    const support::PartState* torso =
        body.find(support::BodyPartId::Torso);
    const support::PartState* armR =
        body.find(support::BodyPartId::ArmR);
    if (!torso || !armR) return arc;
    const float row = getH() / 40.f;
    const float cx = armR->worldBox.left + armR->worldBox.width * 0.5f;
    const core::Vec2f o = support::limbShoulder(
        {torso->worldBox.left, torso->worldBox.top},
        {torso->worldBox.width, torso->worldBox.height}, cx);
    // Mesma geometria do ramo Active de updateLimbs (8 rows no eixo).
    const core::Vec2f av = support::aimVector(aim);
    const float dist = 8.f * row;
    const float thick = 2.f * row;
    arc.origin = o;
    arc.centerAngle = std::atan2(av.y, av.x);
    arc.halfWidth = support::kSweepHalfWidth;
    arc.rInner = std::max(0.f, dist - thick);
    arc.rOuter = dist + bladeLengthRows(this) * row;
    arc.empty = false;
    return arc;
}

Player::MeleeBreakdown Player::meleeDamageBreakdown() const {
    return meleeDamageBreakdownVs(1.f, false);
}

Player::MeleeBreakdown Player::meleeDamageBreakdownVs(float targetHpFrac,
                                                      bool alone) const {
    MeleeBreakdown bd;
    bd.base = kLight[meleeCombo].damage;
    if (const core::ItemDef* off = offHandDef()) bd.base += off->damage;
    if (const core::ItemDef* wdef = weaponDef()) {
        // Mundo: +10% em todos os atributos (entradas de scaling).
        const float attrMul = tarotFx.attrMult;
        const auto effAttr = [&](core::Attr a) {
            return static_cast<int>(attrs.get(a) * attrMul);
        };
        bd.strBonus = wdef->damage *
            core::scaleMult(wdef->strScale) *
            core::scaleFactor(effAttr(core::Attr::Strength));
        bd.dexBonus = wdef->damage *
            core::scaleMult(wdef->dexScale) *
            core::scaleFactor(effAttr(core::Attr::Dexterity));
        bd.intBonus = wdef->damage *
            core::scaleMult(wdef->intScale) *
            core::scaleFactor(effAttr(core::Attr::Intelligence));
        bd.faiBonus = wdef->damage *
            core::scaleMult(wdef->faiScale) *
            core::scaleFactor(effAttr(core::Attr::Faith));
        float dmg = static_cast<float>(bd.base) + bd.strBonus +
            bd.dexBonus + bd.intBonus + bd.faiBonus;
        if (attrs.get(core::Attr::Strength) < wdef->strReq ||
            attrs.get(core::Attr::Dexterity) < wdef->dexReq) {
            dmg *= 0.5f;
            bd.halvedByReq = true;
        }
        // Tarô ofensivo (1.0 sem cartas = intacto).
        dmg *= tarotFx.damageMult;
        // Buff elemental (frost_weapon etc.) puxa a via mágica.
        dmg *= (weaponBuffType == core::DamageType::Physical)
                   ? tarotFx.physicalDamageMult
                   : tarotFx.magicDamageMult;
        // Condicionais do próprio HP (Amantes).
        const float hpFrac = effectiveHpMax() > 0
                                 ? static_cast<float>(hp) /
                                       effectiveHpMax()
                                 : 1.f;
        dmg *= (hpFrac < 0.5f) ? tarotFx.lowHpDamageMult
                               : tarotFx.highHpDamageMult;
        // Contexto do alvo, só via MeleeSystem (Justiça/Eremita).
        if (targetHpFrac >= 1.f) dmg *= tarotFx.fullHpDamageMult;
        if (targetHpFrac < 0.25f) dmg *= tarotFx.woundedTargetDamageMult;
        if (alone) dmg *= tarotFx.aloneDamageMult;
        // Finalizador do combo: Estrela pesa, investida empurra.
        if (meleeCombo == 2) {
            dmg *= tarotFx.heavyDamageMult;
            dmg *= tarotFx.chargeDamageMult;
        }
        // Morte: stacks dentro da janela de 30s.
        if (killStacks_ > 0 && tarotFx.killStackBonus != 0.f)
            dmg *= 1.f + tarotFx.killStackBonus * killStacks_;
        // Crítico do fado: sem carta de crítico, sem crítico (jogo base
        // intacto). Com carta, todo 10º swing ×1.5×agregado.
        if (tarotFx.critDamageMult != 1.f && meleeSwingId % 10 == 0)
            dmg *= 1.5f * tarotFx.critDamageMult;
        // Roda: sem precisão, o golpe roça (determinístico, sem RNG).
        if (tarotFx.precisionMult < 1.f) {
            const int period = std::max(
                2, static_cast<int>(1.f / (1.f - tarotFx.precisionMult)));
            if (meleeSwingId % period == 0) dmg *= 0.5f;
        }
        // Enforcado: bônus plano convertido da dor (10s).
        dmg += convBonus_;
        bd.total = static_cast<int>(dmg);
    } else {
        bd.total = bd.base;
    }
    return bd;
}

int Player::meleeDamage() const {
    return meleeDamageBreakdown().total;
}

int Player::meleeDamageVs(float targetHpFrac, bool alone) const {
    return meleeDamageBreakdownVs(targetHpFrac, alone).total;
}

float Player::meleePosture() const { return kLight[meleeCombo].posture; }
