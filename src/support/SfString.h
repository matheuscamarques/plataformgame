/**
 * @file src/support/SfString.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Conversão UTF-8 para sf::String (fronteira de texto).
 * @details sf::Text::setString(std::string) interpreta bytes como Latin-1 e quebra acentos (tofu); todo texto da UI passa por utf8(), incluído por quem desenha texto.
 */

#pragma once

#include <cstring>
#include <string>

#include <SFML/System/String.hpp>

namespace support {

// std::string UTF-8 (literais PT do código) -> sf::String correta.
// Sem isso, "Força" vira 2 quadradinhos (cada byte vira glyph ausente).
inline sf::String utf8(const std::string &s) {
    return sf::String::fromUtf8(s.begin(), s.end());
}

inline sf::String utf8(const char *s) {
    return sf::String::fromUtf8(s, s + std::strlen(s));
}

} // namespace support
