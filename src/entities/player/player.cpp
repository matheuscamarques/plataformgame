#include "player.h"
#include <iostream>
#include "../../defines.h"
#include "../../support/ThrowSystem.h"
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
    // Slime não empurra o player (dano de contato vem na Fase C).
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
    }

    if (getBoundsBottom().intersects(bloco)) {
        setY(bloco.getY() - getH());
        moveDown = false;
        jumping = true;
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
    }

    if (getBoundsBottom().intersects(bloco)) {
        setY(bloco.getY() - getH());
        moveDown = false;
        jumping = true;
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

    if(!moveUp && !moveDown){
        setVy(0.0f);
    }

    if(moveLeft){
        setVx(-9.8f - vxRunSpeed);
    }
    if(moveRight)
        setVx(9.8f + vxRunSpeed);

    if(!moveLeft && !moveRight){
        setVx(0.0f);
    }

    // gravity
    setVy(9.8f);
    Entity::tick();
}

bool Player::tryThrow(support::ThrowSystem &throws) {
    if (!throwCooldown.ready() || dynamiteCount <= 0) return false;
    // Arco fixo na direção do facing; sem mira manual no MVP.
    sf::Vector2f vel{220.f * static_cast<float>(facing), -320.f};
    if (!throws.throwItem({getCenterX(), getCenterY()}, vel)) return false;
    dynamiteCount--;
    throwCooldown.trigger();
    return true;
}
