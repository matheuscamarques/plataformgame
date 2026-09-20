#include "SkillSystem.h"

#include "EnemySystem.h"
#include "GameContext.h"
#include "Skill.h"

namespace support {
namespace SkillSystem {

bool tryUse(Enemy &self, GameContext &ctx, const std::string &skillId) {
    const SkillDef *def = SkillRegistry::instance().find(skillId);
    if (!def || !def->execute) return false;

    core::Cooldown &cd = self.skillCds[skillId];
    if (!cd.ready()) return false;

    Cost c{def->manaCost, def->staminaCost, def->postureCost};
    if (!self.resources.canPay(c)) return false;

    self.resources.pay(c);
    def->execute(self, ctx);
    cd.trigger(def->cooldown);
    return true;
}

void tick(Enemy &self, float dt) {
    for (auto &kv : self.skillCds) kv.second.tick(dt);
}

void clear(Enemy &self) {
    self.skillCds.clear();
}

} // namespace SkillSystem
} // namespace support
