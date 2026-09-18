#include "./Camera.h"


Camera::Camera(float x, float y){
    this->x = x;
    this->y = y;
}
void Camera::tick(float playerX, float playerY, float viewW, float viewH){
    if(playerX > viewW/2){
        x = playerX - viewW/2;
    }
    if(playerY > viewH/2){
        y = playerY - viewH/2;
    }
}