#pragma once

#include "../entity/entity.hpp"
class Player : public Entity
{
    public:

        bool moveDown = false, moveUp = false, moveLeft = false, moveRight = false,runFast = false;
        bool jumping = false;
        float jumpingRecharge = 0.0f;
        

        Player();
        void collide(Entity entity);
        void collide(Component bloco);
        void tick();
};