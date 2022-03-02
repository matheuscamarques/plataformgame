#pragma once
#include "../game/game.h"
#include "../entities/player/player.h"

class Game;
class Camera {
    public:
        float x;
        float y;
        Camera(float x, float y);
        void tick(Player *player, Game *game);
};

