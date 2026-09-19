#pragma once

#include "../entity/entity.hpp"
#include "../../support/Body.h"
#include "../../core/Cooldown.h"

namespace support { class ThrowSystem; }

class Player : public Entity
{
    public:

        bool moveDown = false, moveUp = false, moveLeft = false, moveRight = false,runFast = false;
        bool jumping = false;
        float jumpingRecharge = 0.0f;

        support::Body body; // hitboxes por parte (rebuild via BodySystem)

        // S6: verbo de arremesso. Cooldown tickado no Game::tick (1/30 fixo);
        // lógica aqui para ser testável sem Game/janela.
        core::Cooldown throwCooldown{0.5f};
        int dynamiteCount = 5;

        Player();
        void collide(Entity entity);
        void collide(Component bloco);
        void tick();

        // Tenta arremessar na direção do facing com arco fixo.
        // Retorna false sem efeito se cooldown/inventário/pool bloquearem.
        bool tryThrow(support::ThrowSystem &throws);
};