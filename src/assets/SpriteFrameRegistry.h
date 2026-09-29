/**
 * @file src/assets/SpriteFrameRegistry.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara estrutura e acesso aos dados de frames de sprite.
 * @details Define struct SpriteFrameData com rows, tamanho e paleta mais função frameData, incluída por quem precisa converter id em pixels sem arrastar Sprites.
 */

#pragma once
#include <cstddef>

#include "assets/Sprites/PlayerParts.h"
#include "core/sprite_from_ascii.h"
#include "support/Combat/Facing.h"
#include "support/Combat/SpriteFrame.h"

namespace assets {

// Dado de frame por id. Corpo no .cpp (único TU que arrasta Sprites.h).
struct SpriteFrameData {
    const char* const* rows = nullptr;
    int w = 0, h = 0;
    const core::PaletteEntry* pal = nullptr;
    std::size_t palCount = 0;
};

SpriteFrameData frameData(support::SpriteFrameId id);

// Frame do player por (pose, direção de arte): mesma fonte do build
// (posePartsFor) com cache composto. frameData() acima serve E
// (legado/testes); BodySystem usa este p/ hitboxes direcionais.
SpriteFrameData playerFrameData(sprites::PlayerPose pose,
                                support::Facing artDir);

// Frames estáticos (tudo menos player/None/COUNT): fonte única dos
// dados acima e do SpriteSet::build (dimensões/paleta não divergem).
// Player fica de fora (composto em runtime, por pose).
struct StaticFrameEntry {
    support::SpriteFrameId id = support::SpriteFrameId::None;
    const char* const* rows = nullptr;
    int w = 0, h = 0;
    const core::PaletteEntry* pal = nullptr;
    std::size_t palCount = 0;
};

// Tabela + tamanho (ponteiro p/ array estático; sem cópia).
const StaticFrameEntry* staticFrameTable(int* outCount);

} // namespace assets
