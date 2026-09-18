#pragma once // Heresia usar pragma once

class Camera {
    public:
        float x;
        float y;
        Camera(float x, float y);
        void tick(float playerX, float playerY, float viewW, float viewH);
};

