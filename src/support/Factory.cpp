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
    return std::make_unique<Slime>(std::move(body), std::move(ai));
}

} // namespace support
