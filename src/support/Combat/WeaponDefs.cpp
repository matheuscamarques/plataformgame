/**
 * @file src/support/Combat/WeaponDefs.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Registra espadas e machados com geometrias de swing distintas.
 * @details Duas geometrias canônicas (swordGeo/axeGeo); itens só mudam
 * o id. Catalisadores usam a do machado (vertical, sem swing).
 * Consumido via WeaponRegistry por BodySystem e render.
 */

#include "WeaponRegistry.h"

using namespace support;

namespace {
// Espada: lâmina horizontal 16x8 com fases de swing.
WeaponDef swordGeo() {
    WeaponDef d;
    d.spriteW = 16;
    d.spriteH = 8;
    d.originX = 5.f;
    d.originY = 5.f;
    d.handOffsetX = 4.f;
    d.handOffsetY = 8.f;
    return d;
}

// Machado/cajado/sino: vertical 8x20, só idle (sem lâmina de swing).
WeaponDef axeGeo() {
    WeaponDef d;
    d.spriteW = 8;
    d.spriteH = 20;
    d.originX = 4.f;
    d.originY = 10.f;
    d.handOffsetX = 4.f;
    d.handOffsetY = 8.f;
    d.hasSwingPhases = false;
    d.trauma = 0.3f; // pesado: chacoalha o dobro da espada
    return d;
}

// Catalisadores: mesma forma do machado, trauma leve (sem impacto).
WeaponDef catalystGeo() {
    WeaponDef d = axeGeo();
    d.trauma = 0.1f;
    return d;
}
} // namespace

REGISTER_WEAPON("sword", swordGeo);
REGISTER_WEAPON("axe", axeGeo);
REGISTER_WEAPON("iron_sword", swordGeo);
REGISTER_WEAPON("iron_axe", axeGeo);
REGISTER_WEAPON("gold_sword", swordGeo);
REGISTER_WEAPON("gold_axe", axeGeo);
REGISTER_WEAPON("diamond_sword", swordGeo);
REGISTER_WEAPON("diamond_axe", axeGeo);
REGISTER_WEAPON("leather_sword", swordGeo);
REGISTER_WEAPON("leather_axe", axeGeo);

// Catalisadores (Fase 3b): verticais como o machado, sem swing.
REGISTER_WEAPON("wooden_staff", catalystGeo);
REGISTER_WEAPON("priest_bell", catalystGeo);
