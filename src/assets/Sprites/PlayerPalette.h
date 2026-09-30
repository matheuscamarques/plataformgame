/**
 * @file src/assets/Sprites/PlayerPalette.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Paleta do jogador: única fonte de verdade char→cor→hitbox.
 * @details Cada letra da arte em PlayerParts.h tem exatamente uma entrada
 * aqui (nome semântico + RGBA + BodyPartId). O renderer (SpriteSet),
 * a hitbox (BodySystem via SpriteFrameRegistry) e os testes leem desta
 * tabela — nunca hard-code cor em outro lugar. Crescer a paleta é
 * adicionar linha aqui; teste de cobertura trava que todo char usado
 * na arte existe aqui (makeSprite engole char desconhecido em silêncio
 * transparente, sem assert).
 *
 * Luz canônica: topo-esquerda (world-space). Sombra = mesma matiz da
 * base ~0.62x (nunca outra matiz — senão lê como sujeira). Rim light
 * (V) 1-2px na borda esquerda/topo (~5% da silhueta, nunca 4px).
 * 'W' segue reservado p/ Weapon (morto na arte do player, mas contrato
 * com arte futura); rim light usa 'V' com nome próprio.
 */

#pragma once

#include <cstddef>

#include "core/sprite_from_ascii.h"

namespace sprites {

// Tier 0 — cores atuais (zero mudança visual; base do spike Tier 1).
inline const core::PaletteEntry kPlayerPalette[] = {
    {'.', {0, 0, 0, 0}, core::BodyPartId::None},     // vazio
    {'K', {30, 20, 20}, core::BodyPartId::None},     // cabelo
    {'F', {230, 180, 140}, core::BodyPartId::Head},  // pele lit
    {'H', {230, 180, 140}, core::BodyPartId::ArmR},  // braço dir lit
    {'G', {230, 180, 140}, core::BodyPartId::ArmL},  // braço esq lit
    {'E', {20, 15, 15}, core::BodyPartId::Head},     // olho/escuro
    {'C', {60, 90, 160}, core::BodyPartId::Torso},   // roupa lit
    {'B', {50, 35, 30}, core::BodyPartId::LegR},     // perna dir lit
    {'L', {50, 35, 30}, core::BodyPartId::LegL},     // perna esq lit
    {'W', {190, 190, 200}, core::BodyPartId::Weapon},// reservado arma
    {'T', {220, 60, 50}, core::BodyPartId::Weapon},  // TNT / arma vermelha
    {'t', {90, 30, 25}, core::BodyPartId::Weapon},   // TNT sombra
    // Tier 1 — sombra lateral + contato + rim (spike Idle E).
// Tier 1 (spike Idle E, commit separado).
};
inline constexpr std::size_t kPlayerPaletteCount = 12;

// Tier 1 (spike Idle E) — sombra lateral + contato + rim. Mesma matiz
// das bases ×0.62; 'o'/'V' sem hitbox (None); minúsculas herdam a
// parte da base (hitbox estável: spans dentro dos spans lit).
//  {'k', {19, 12, 12}, None},      // cabelo em sombra
//  {'f', {143, 112, 87}, Head},    // pele em sombra
//  {'c', {37, 56, 99}, Torso},     // roupa em sombra
//  {'l', {31, 22, 19}, LegL},      // perna esq em sombra
//  {'b', {31, 22, 19}, LegR},      // perna dir em sombra
//  {'g', {143, 112, 87}, ArmL},    // braço esq em sombra
//  {'h', {143, 112, 87}, ArmR},    // braço dir em sombra
//  {'o', {12, 10, 12}, None},      // contato/oclusão
//  {'V', {235, 240, 245}, None},   // rim light neutro
//  {'n', {60, 40, 30}, Head},      // detalhe facial escuro
// Tier 2 (+X Y Z p n...) e Tier 3 (+r w e) só após Tier 1 estável.

} // namespace sprites
