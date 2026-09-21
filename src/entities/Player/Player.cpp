#include "Player.h"
#include <algorithm>
#include <iostream>
#include "defines.h"
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
    inWater = false; // reset; collide() seta de novo se houver água
    // Snapshot do input p/ mira: o pulo consome moveUp abaixo; sem a
    // cópia, segurar ↑ no ar perde o N após ~5 ticks de subida.
    const bool aimUp = moveUp;
    const bool aimDown = moveDown;
    const bool aimLeft = moveLeft;
    const bool aimRight = moveRight;


    if (moveUp && jumping) {
			setY(getY() - core::kBlockSize * 1.0f / 2);

			jumpingRecharge += core::kBlockSize * 1.0/ 2 ;
			if (jumpingRecharge > core::kBlockSize * 5) {
				jumping = false;
				jumpingRecharge = 0.0f;
				moveUp = false;
			}
	}

    float vxRunSpeed = runFast ? 5.0f : 0.0f;

    // Gravidade com arrasto: acelera até a velocidade terminal.
    // No pulo (teleporte) mantém 9.8 (pulo idêntico ao antigo); fora
    // dele, acumula. Pouso zera no collide(); knockback p/ cima faz
    // arco (soma e cai).
    if (moveUp && jumping) {
        setVy(9.8f);
    } else {
        float vy = getVy() + kGravity;
        if (vy > kTerminalVelocity) vy = kTerminalVelocity;
        setVy(vy);
    }

    if(moveLeft){
        setVx(-9.8f - vxRunSpeed);
    }
    if(moveRight)
        setVx(9.8f + vxRunSpeed);

    if(!moveLeft && !moveRight){
        setVx(0.0f);
    }

    // Mira segue o input todo tick; o swing congela a sua (snapshot).
    aimDir = support::resolveAim(aimUp, aimDown, aimLeft, aimRight, facing);

    // Walk anim (10fps, só no chão): parado volta ao frame 0.
    // jumping=true = no chão (pode pular); false = no ar.
    if ((moveLeft || moveRight) && jumping) {
        walkTimer += 1.0f / 30.0f;
        if (walkTimer >= 0.10f) {
            walkTimer = 0.f;
            walkFrame = (walkFrame + 1) % 4;
        }
    } else {
        walkFrame = 0;
        walkTimer = 0.f;
    }

    // Timer de ataque (frame telegraph).
    if (throwAnimT > 0.f) throwAnimT -= 1.0f / 30.0f;

    // Cooldowns do Player, tickados pelo Player (1 só lugar).
    // Sem o hurtIframes aqui, i-frames nunca expiram e o sprite
    // trava em kPlayerHurt (pick tem hurt como 1ª prioridade).
    hurtIframes.tick(1.0f / 30.0f);
    throwCooldown.tick(1.0f / 30.0f);

    Entity::tick();
}

bool Player::tryThrow(support::ThrowSystem &throws) {
    if (!throwCooldown.ready() || dynamiteCount <= 0) return false;
    // Arco fixo na direção do facing; sem mira manual no MVP.
    sf::Vector2f vel{220.f * static_cast<float>(facing), -320.f};
    if (!throws.throwItem({getCenterX(), getCenterY()}, vel)) return false;
    dynamiteCount--;
    throwCooldown.trigger();
    throwAnimT = kThrowAnimDur;
    return true;
}

void Player::cycleMaterial() {
    if (!loadout.equipped) {
        // Pelado → reequipa no Iron (fecha o ciclo de 5 estados).
        loadout.equipped = true;
        loadout.weapon = loadout.helm = loadout.chest = loadout.legs =
            core::MaterialId::Iron;
        return;
    }
    const int next =
        static_cast<int>(loadout.weapon) + 1;
    if (next >= static_cast<int>(core::MaterialId::COUNT)) {
        loadout.equipped = false; // 5º estado: sem nada, p/ teste
        return;
    }
    loadout.weapon = loadout.helm = loadout.chest = loadout.legs =
        static_cast<core::MaterialId>(next);
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
    dynamiteCount = 999;
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
    if (loadout.equipped) {
        const auto &hb = kAimHitbox[static_cast<int>(swingAim)];
        float ws = 1.f, hs = 1.f;
        if (const auto *wd =
                support::WeaponRegistry::instance().find(loadout.weaponId)) {
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

int Player::meleeDamage() const { return kLight[meleeCombo].damage; }

float Player::meleePosture() const { return kLight[meleeCombo].posture; }
