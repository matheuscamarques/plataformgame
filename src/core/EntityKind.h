/**
 * @file src/core/EntityKind.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Enum tipado de entidades mais IDs legados do jogo.
 * @details Define EntityKind para Player, Slime, Dwarf, TNT e Rock com kId legados, usado por Behavior, SpawnSystem e defines.h.
 */

#pragma once

#include <cstdint>

namespace core {

// Identidade tipada de entidade (novo). Troca os switches em string
// (ai->name() == "DwarfAI") por comparação de enum — rename-safe e O(1).
// Behavior::kind() é a fonte; EnemyArchetype::kind dirige SpawnSystem.
enum class EntityKind : uint8_t { Player, Slime, Dwarf, TNT, Rock, COUNT };
inline constexpr int kEntityKindCount = 5;

// Domínio legado: IDs int de Entity::getName()/Component (defines.h
// morreu: PLAYER=0, SLIME=40...). Migração futura: getName() vira kind().
// Não reutilizar esses números para nada novo.
inline constexpr int kIdPlayer = 0;
inline constexpr int kIdWater = 1;
inline constexpr int kIdColide = 2;
inline constexpr int kIdTreeTrunk = 20;
inline constexpr int kIdTreeLeaf = 21;
inline constexpr int kIdLava = 30;
inline constexpr int kIdSlime = 40;
inline constexpr int kIdBound = 99;

} // namespace core
