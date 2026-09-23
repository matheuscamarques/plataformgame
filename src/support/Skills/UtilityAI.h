/**
 * @file src/support/Skills/UtilityAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara escolha determinística da melhor skill utilizável agora.
 * @details Define classe UtilityAI com choose estático, incluída por DwarfAI e alimentada por SkillRegistry via GameContext.
 */

#pragma once
#include <string>
#include <vector>

namespace support {

struct Enemy;
struct GameContext;
struct SkillDef;

// Escolhe skill por score (determinístico, sem RNG). Gates eliminam;
// score ordena. UtilityAI não executa — SkillSystem::tryUse executa.
class UtilityAI {
public:
    // Melhor skill usável AGORA entre candidates, ou nullptr.
    // Toma Enemy& não-const porque getters legados de Entity não são
    // const (não muta nada — só lê posição/recursos).
    static const SkillDef *choose(Enemy &self,
                                  const GameContext &ctx,
                                  const std::vector<std::string> &candidates);
};

} // namespace support
