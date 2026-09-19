#pragma once

#include "../entity/entity.hpp"
#include "../../support/Body.h"
class Player : public Entity
{
    public:

        bool moveDown = false, moveUp = false, moveLeft = false, moveRight = false,runFast = false;
        bool jumping = false;
        float jumpingRecharge = 0.0f;

        support::Body body; // hitboxes por parte (rebuild via BodySystem)

        Player();
        void collide(Entity entity);
        void collide(Component bloco);
        void tick();
};