#include "EnemySystem.h"

#include <SFML/Graphics/Color.hpp>

#include "../defines.h"
#include "BehaviorRegistry.h"
#include "SlimeAI.h"

namespace support {

std::unique_ptr<Slime> Factory::spawnEnemy(const std::string &kind,
                                           float x, float y) {
    auto ai = BehaviorRegistry::instance().create(kind);
    if (!ai) return nullptr;

    Entity body(SLIME, x, y, 40.0f, 30.0f);
    body.setFillColor(sf::Color(0, 200, 0));

    auto slime = std::make_unique<Slime>(std::move(body), std::move(ai));
    // S2: recursos default de trash (HP + postura; mana/stamina ignorados).
    slime->resources.isTrash = true;
    slime->resources.hp = 30;
    slime->resources.hpMax = 30;
    slime->resources.posture = 20.0f;
    slime->resources.postureMax = 20.0f;
    slime->resources.postureRegen = 10.0f;
    slime->resources.postureRegenDelay = core::Cooldown(1.0f);
    return slime;
}

} // namespace support
