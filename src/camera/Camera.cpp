#include "./Camera.h"


Camera::Camera(float x, float y){
    this->x = x;
    this->y = y;
}
void Camera::tick(Player *player, Game *game){
    if(player->getX() > game->width/2){
        x = player->getX() - game->getW()/2;
    }
    if(player->getY() > game->height/2){
        y = player->getY() - game->getH()/2;
    }
}