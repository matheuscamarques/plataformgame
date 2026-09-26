/**
 * @file src/support/Enemies/PackAI.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Registra as IAs do pack (6 dados + 2 voadores) no BehaviorRegistry.
 * @details Um arquivo p/ os 8 registros (subclasses mínimas não merecem .cpp próprio); Factory cria por behaviorKind sem branch, incluído no build por glob.
 */

#include "BehaviorRegistry.h"
#include "BurstAI.h"
#include "DemonEyeAI.h"
#include "ElementalAI.h"
#include "HarpyAI.h"
#include "HollowAI.h"
#include "ImpAI.h"
#include "RatAI.h"
#include "UndeadAI.h"

namespace support {

SUPPORT_REGISTER_BEHAVIOR("hollow", HollowAI);
SUPPORT_REGISTER_BEHAVIOR("rat", RatAI);
SUPPORT_REGISTER_BEHAVIOR("burst", BurstAI);
SUPPORT_REGISTER_BEHAVIOR("imp", ImpAI);
SUPPORT_REGISTER_BEHAVIOR("elemental", ElementalAI);
SUPPORT_REGISTER_BEHAVIOR("undead", UndeadAI);
SUPPORT_REGISTER_BEHAVIOR("harpy", HarpyAI);
SUPPORT_REGISTER_BEHAVIOR("eye", DemonEyeAI);

} // namespace support
