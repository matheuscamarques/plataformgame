/**
 * @file src/core/VecSfml.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Fronteira Vec neutro <-> SFML (permanente, só na borda).
 * @details Conversores fromSf/toSf usados onde gameplay encontra janela/render/input; gameplay nunca inclui SFML direto, incluído por adaptadores de fronteira.
 */

#pragma once

#include <SFML/System/Vector2.hpp>

#include "core/Vec.h"

namespace core {

// Fronteira: só chamar onde o dado cruza p/ SFML (janela, render, input).
inline Vec2f fromSf(const sf::Vector2f &v) { return {v.x, v.y}; }
inline Vec2i fromSf(const sf::Vector2i &v) { return {v.x, v.y}; }
inline sf::Vector2f toSf(const Vec2f &v) { return {v.x, v.y}; }
inline sf::Vector2i toSf(const Vec2i &v) { return {v.x, v.y}; }

} // namespace core
