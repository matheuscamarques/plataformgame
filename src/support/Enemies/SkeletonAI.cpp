/**
 * @file src/support/Enemies/SkeletonAI.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Registra a IA do esqueleto no BehaviorRegistry.
 * @details Só auto-registro ("skeleton" -> SkeletonAI); a lógica mora na
 * DwarfAI herdada. Chamado por Factory via behaviorKind, sem branch.
 */

#include "SkeletonAI.h"

#include "BehaviorRegistry.h"

namespace support {

SUPPORT_REGISTER_BEHAVIOR("skeleton", SkeletonAI);

} // namespace support
