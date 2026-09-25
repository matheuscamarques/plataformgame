/**
 * @file src/support/Combat/WeaponDefs.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Registra espadas e machados com geometrias de swing distintas.
 * @details Usa macro REGISTER_WEAPON para sword, axe, iron_sword e iron_axe, consumido via WeaponRegistry por BodySystem e render.
 */

#include "WeaponRegistry.h"

using namespace support;

REGISTER_WEAPON("sword", [] {
    WeaponDef d;
    d.spriteW = 16;
    d.spriteH = 8;
    d.originX = 5.f;
    d.originY = 5.f;
    d.handOffsetX = 4.f;
    d.handOffsetY = 8.f;
    return d;
});

REGISTER_WEAPON("axe", [] {
    WeaponDef d;
    d.spriteW = 8;
    d.spriteH = 20;
    d.originX = 4.f;
    d.originY = 10.f;
    d.handOffsetX = 4.f;
    d.handOffsetY = 8.f;
    d.hasSwingPhases = false; // só idle, sem swing
    return d;
});

// Geometria por item equipado (mesmas do sword/axe acima).
// O render e o BodySystem resolvem pelo defId do Equipment.
REGISTER_WEAPON("iron_sword", [] {
    WeaponDef d;
    d.spriteW = 16;
    d.spriteH = 8;
    d.originX = 5.f;
    d.originY = 5.f;
    d.handOffsetX = 4.f;
    d.handOffsetY = 8.f;
    return d;
});

REGISTER_WEAPON("iron_axe", [] {
    WeaponDef d;
    d.spriteW = 8;
    d.spriteH = 20;
    d.originX = 4.f;
    d.originY = 10.f;
    d.handOffsetX = 4.f;
    d.handOffsetY = 8.f;
    d.hasSwingPhases = false; // só idle, sem swing
    return d;
});

// Variantes por material (mesma geometria da base).
REGISTER_WEAPON("gold_sword", [] {
    WeaponDef d;
    d.spriteW = 16;
    d.spriteH = 8;
    d.originX = 5.f;
    d.originY = 5.f;
    d.handOffsetX = 4.f;
    d.handOffsetY = 8.f;
    return d;
});

REGISTER_WEAPON("gold_axe", [] {
    WeaponDef d;
    d.spriteW = 8;
    d.spriteH = 20;
    d.originX = 4.f;
    d.originY = 10.f;
    d.handOffsetX = 4.f;
    d.handOffsetY = 8.f;
    d.hasSwingPhases = false;
    return d;
});

REGISTER_WEAPON("diamond_sword", [] {
    WeaponDef d;
    d.spriteW = 16;
    d.spriteH = 8;
    d.originX = 5.f;
    d.originY = 5.f;
    d.handOffsetX = 4.f;
    d.handOffsetY = 8.f;
    return d;
});

REGISTER_WEAPON("diamond_axe", [] {
    WeaponDef d;
    d.spriteW = 8;
    d.spriteH = 20;
    d.originX = 4.f;
    d.originY = 10.f;
    d.handOffsetX = 4.f;
    d.handOffsetY = 8.f;
    d.hasSwingPhases = false;
    return d;
});

REGISTER_WEAPON("leather_sword", [] {
    WeaponDef d;
    d.spriteW = 16;
    d.spriteH = 8;
    d.originX = 5.f;
    d.originY = 5.f;
    d.handOffsetX = 4.f;
    d.handOffsetY = 8.f;
    return d;
});

REGISTER_WEAPON("leather_axe", [] {
    WeaponDef d;
    d.spriteW = 8;
    d.spriteH = 20;
    d.originX = 4.f;
    d.originY = 10.f;
    d.handOffsetX = 4.f;
    d.handOffsetY = 8.f;
    d.hasSwingPhases = false;
    return d;
});

// Catalisadores (Fase 3b): verticais como o machado, sem swing.
REGISTER_WEAPON("wooden_staff", [] {
    WeaponDef d;
    d.spriteW = 8;
    d.spriteH = 20;
    d.originX = 4.f;
    d.originY = 10.f;
    d.handOffsetX = 4.f;
    d.handOffsetY = 8.f;
    d.hasSwingPhases = false; // cajado não tem lâmina de swing
    return d;
});

REGISTER_WEAPON("priest_bell", [] {
    WeaponDef d;
    d.spriteW = 8;
    d.spriteH = 20;
    d.originX = 4.f;
    d.originY = 10.f;
    d.handOffsetX = 4.f;
    d.handOffsetY = 8.f;
    d.hasSwingPhases = false; // sino não tem lâmina de swing
    return d;
});
