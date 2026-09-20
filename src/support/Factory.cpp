#include "EnemySystem.h"

#include <SFML/Graphics/Color.hpp>

#include "../defines.h"
#include "BehaviorRegistry.h"
#include "SlimeAI.h"

namespace support {

std::unique_ptr<Enemy> Factory::spawnEnemy(const std::string &kind,
                                            float x, float y) {
    auto ai = BehaviorRegistry::instance().create(kind);
    if (!ai) return nullptr;

    if (kind == "dwarf") return spawnDwarf(std::move(ai), x, y);

    Entity body(SLIME, x, y, 40.0f, 30.0f);
    body.setFillColor(sf::Color(0, 200, 0));

    auto slime = std::make_unique<Enemy>(std::move(body), std::move(ai));
    static auto schema = BodySchema::humanoid(30.0f, 40.0f);
    slime->bodyParts.attach(&schema);
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

// Anão básico: Elite (4 recursos), schema dwarf, corpo marrom.
// Nome de colisão SLIME de propósito (Q2): sem bloqueio até playtest
// dizer se o Elite deve ser parede. Cor vem daqui, não de Tile.
std::unique_ptr<Enemy> Factory::spawnDwarf(std::unique_ptr<Behavior> ai,
                                           float x, float y) {
    Entity body(SLIME, x, y, 36.0f, 44.0f);
    body.setFillColor(sf::Color(139, 90, 43));

    auto dwarf = std::make_unique<Enemy>(std::move(body), std::move(ai));
    static auto schema = BodySchema::dwarf();
    dwarf->bodyParts.attach(&schema);
    dwarf->resources.isTrash = false;
    dwarf->resources.hp = 60;
    dwarf->resources.hpMax = 60;
    dwarf->resources.mana = 0.f;
    dwarf->resources.manaMax = 0.f;
    dwarf->resources.manaRegen = 1.0f;
    dwarf->resources.manaRegenDelay = core::Cooldown(2.0f);
    dwarf->resources.stamina = 40.f;
    dwarf->resources.staminaMax = 40.f;
    dwarf->resources.staminaRegen = 20.f;
    dwarf->resources.staminaRegenDelay = core::Cooldown(0.8f);
    dwarf->resources.posture = 30.f;
    dwarf->resources.postureMax = 30.f;
    dwarf->resources.postureRegen = 15.f;
    dwarf->resources.postureRegenDelay = core::Cooldown(1.2f);
    return dwarf;
}

} // namespace support
