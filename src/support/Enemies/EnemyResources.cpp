#include "EnemyResources.h"

#include <algorithm>

namespace support {

bool EnemyResources::canPay(const Cost& c) const {
    if (isTrash) {
        return posture >= c.posture;
    }
    return mana    >= c.mana
        && stamina >= c.stamina
        && posture >= c.posture;
}

void EnemyResources::pay(const Cost& c) {
    if (isTrash) {
        posture = std::max(0.f, posture - c.posture);
        if (c.posture > 0.f) postureRegenDelay.trigger();
    } else {
        if (c.mana    > 0.f) { mana    = std::max(0.f, mana    - c.mana);
                               manaRegenDelay.trigger();    }
        if (c.stamina > 0.f) { stamina = std::max(0.f, stamina - c.stamina);
                               staminaRegenDelay.trigger(); }
        if (c.posture > 0.f) { posture = std::max(0.f, posture - c.posture);
                               postureRegenDelay.trigger(); }
    }
    checkStagger();
}

void EnemyResources::tick(float dt) {
    manaRegenDelay.tick(dt);
    staminaRegenDelay.tick(dt);
    postureRegenDelay.tick(dt);

    if (staggerTimer > 0.f) {
        staggerTimer -= dt;
        if (staggerTimer < 0.f) staggerTimer = 0.f;
    }

    if (!isTrash) {
        if (manaRegenDelay.ready() && mana < manaMax)
            mana = std::min(manaMax, mana + manaRegen * dt);
        if (staminaRegenDelay.ready() && stamina < staminaMax)
            stamina = std::min(staminaMax, stamina + staminaRegen * dt);
    }
    if (postureRegenDelay.ready() && posture < postureMax)
        posture = std::min(postureMax, posture + postureRegen * dt);
}

int EnemyResources::takeDamage(int amount) {
    if (hp <= 0)    return 0;
    if (amount <= 0) return 0;
    int applied = std::min(amount, hp);
    hp -= applied;
    return applied;
}

void EnemyResources::damagePosture(float amount) {
    if (amount <= 0.f) return;
    posture = std::max(0.f, posture - amount);
    postureRegenDelay.trigger();
    checkStagger();
}

void EnemyResources::checkStagger() {
    // Só dispara stagger quando a postura zera E ainda não está em stagger.
    // Não chame isso em tick — evita re-stagger infinito enquanto regen não volta.
    if (posture <= 0.f && staggerTimer <= 0.f) {
        posture      = 0.f;
        staggerTimer = staggerDuration;
    }
}

} // namespace support
