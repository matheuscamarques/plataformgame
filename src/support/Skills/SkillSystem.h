#pragma once

#include <string>

namespace support {

struct Enemy;
struct GameContext;

// Funções livres sobre Enemy::skillCds (opção 3b): sem classe, sem mapa
// externo, sem dangling. Cooldowns morrem com o Enemy.
namespace SkillSystem {

bool tryUse(Enemy &self, GameContext &ctx, const std::string &skillId);
void tick(Enemy &self, float dt);
void clear(Enemy &self);

} // namespace SkillSystem

} // namespace support
