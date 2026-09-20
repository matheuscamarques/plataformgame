#include "Player.h"
#include <iostream>
#include "defines.h"
#include "support/Effects/ThrowSystem.h"

namespace {
// Queda livre: acelera 2px/tick² até 25px/tick (750px/s, ~2.5x os 9.8
// fixos de antes). Terminal < 50px do tile: sem tunelamento.
constexpr float kGravity = 2.0f;
constexpr float kTerminalVelocity = 25.0f;
}
 Player::Player() :
Entity(PLAYER,0,0,50,50)
{
    setFillColor(sf::Color::Red);
    static auto schema = support::BodySchema::humanoid(50.f, 50.f);
    body.attach(&schema);
    //this->setGravity(9.8f);
}

void Player::collide(Entity bloco)
{
    if(
        bloco.getName() == WATER
    ){
        jumping = true;
        return;
    }
    // Lava e deco não colidem como parede (dano vem na Fase C).
    if(
        bloco.getName() == LAVA
    ){
        return;
    }
    // Enemy não empurra o player (dano de contato vem na Fase C).
    if(
        bloco.getName() == SLIME
    ){
        return;
    }
    if(
        bloco.getName() == TREE_TRUNK || bloco.getName() == TREE_LEAF
    ){
        return;
    }
    if (getBoundsTop().intersects(bloco)
    ) {
        setY(bloco.getY() + getH());
        if (getVy() < 0.f) setVy(0.f); // bonk: teto zera subida (senão gruda)
    }

    if (getBoundsBottom().intersects(bloco)) {
        setY(bloco.getY() - getH());
        moveDown = false;
        jumping = true;
        setVy(0.f); // pouso mata a queda (gravidade reacumula se sair)
    } else if(jumping){
         moveDown = true;
    }

    if (getBoundsRight().intersects(bloco)) {
        setX(bloco.getX() - getW());
    }

    if (getBoundsLeft().intersects(bloco)) {
        setX(bloco.getX() + getW());
    }

}

void Player::collide(Component bloco)
{
    if (getBoundsTop().intersects(bloco)
            ) {
        setY(bloco.getY() + getH());
        if (getVy() < 0.f) setVy(0.f); // bonk: teto zera subida (senão gruda)
    }

    if (getBoundsBottom().intersects(bloco)) {
        setY(bloco.getY() - getH());
        moveDown = false;
        jumping = true;
        setVy(0.f); // pouso mata a queda (gravidade reacumula se sair)
    } else if(jumping){
         moveDown = true;
    }

    if (getBoundsRight().intersects(bloco)) {
        setX(bloco.getX() - getW());
    }

    if (getBoundsLeft().intersects(bloco)) {
        setX(bloco.getX() + getW());
    }

}

void Player::tick() {


    if (moveUp && jumping) {
        // 50 is block tile tam
			setY(getY() - 50 * 1.0f / 2);

			jumpingRecharge += 50 * 1.0/ 2 ;
			if (jumpingRecharge > 50 * 5) {
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

    // Timers de ataque (frames telegraph).
    if (meleeAnimT > 0.f) meleeAnimT -= 1.0f / 30.0f;
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
    meleeAnimT = kMeleeAnimDur;
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
    const MeleeDef &d = kLight[meleeCombo];
    const float cx = getCenterX() + static_cast<float>(facing) * (getW() * 0.5f + d.hx * 0.5f);
    const float cy = getCenterY();
    return sf::FloatRect{cx - d.hx * 0.5f, cy - d.hy * 0.5f, d.hx, d.hy};
}

int Player::meleeDamage() const { return kLight[meleeCombo].damage; }

float Player::meleePosture() const { return kLight[meleeCombo].posture; }
