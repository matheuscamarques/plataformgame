#include "player.h"
#include <iostream>

 Player::Player() :
Entity("player",0,0,50,50)
{
    setFillColor(sf::Color::Red);
    //this->setGravity(9.8f);
}

void Player::collide(Entity bloco)
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
