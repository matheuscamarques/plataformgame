/**
 * @file src/game/CharacterCreationScreen.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementa navegação, digitação e desenho da criação.
 * @details Cursor em Nome/classes/COMEÇAR; texto só edita o Nome;
 * preview à direita mostra attrs/equip/magias da selecionada.
 * Render centralizado como o menu, chamado por App.
 */

#include "game/CharacterCreationScreen.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>

#include "core/ItemDef.h"
#include "support/SfString.h"

namespace game {
namespace creation {

void CharacterCreationScreen::reset() {
    cursor_ = kNameRow;
    name_.clear();
    nameWarn_ = false;
    selected_ = core::PlayerClass::Knight;
    pending_ = Action::None;
    rebuildPreview();
}

CharacterCreationScreen::CharacterCreationScreen() { reset(); }

void CharacterCreationScreen::handleInput(support::InputMap &in) {
    // Teclas FÍSICAS (setas/Enter/Esc): letras digitadas (WASD, U...)
    // nunca movem o cursor nem confirmam — TextEntered cuida do nome.
    // Consome a Action equivalente junto (S também marca Action::Down:
    // sem isso o edge vaza p/ o próximo consumidor).
    using K = sf::Keyboard;
    using A = support::Action;
    if (in.pressedKey(K::Up)) {
        in.consumeKey(K::Up);
        in.consume(A::Up);
        cursor_ = (cursor_ + kRowCount - 1) % kRowCount;
    }
    if (in.pressedKey(K::Down)) {
        in.consumeKey(K::Down);
        in.consume(A::Down);
        cursor_ = (cursor_ + 1) % kRowCount;
    }
    if (in.pressedKey(K::Return)) {
        in.consumeKey(K::Return);
        in.consume(A::UseItem);
        confirm();
    }
    // Espaço confirma fora do Nome (no Nome, digita espaço via texto).
    if (cursor_ != kNameRow && in.pressedKey(K::Space)) {
        in.consumeKey(K::Space);
        in.consume(A::Jump);
        in.consume(A::RunFast);
        confirm();
    }
    if (in.pressedKey(K::Escape)) {
        in.consumeKey(K::Escape);
        in.consume(A::Pause);
        pending_ = game::creation::Action::Back;
    }
    // Nenhum edge sobrevive ao frame (letras digitadas não vazam).
    in.clearEdges();
}

void CharacterCreationScreen::confirm() {
    if (cursor_ == kNameRow || (cursor_ >= kClassFirst &&
                                cursor_ < kStartRow)) {
        if (cursor_ >= kClassFirst) {
            selected_ =
                static_cast<core::PlayerClass>(cursor_ - kClassFirst);
            rebuildPreview();
        }
        cursor_ = kStartRow; // confirma avança p/ COMEÇAR
    } else if (cursor_ == kStartRow) {
        if (name_.empty()) {
            nameWarn_ = true; // sem nome, sem jogo
            return;
        }
        pending_ = game::creation::Action::Done;
    }
}

void CharacterCreationScreen::handleText(std::uint32_t unicode) {
    // ASCII visível primeiro: Enter/Tab/controle (unicode < 32) que o
    // SFML entrega via TextEntered junto do KeyPressed NÃO podem mexer
    // no cursor nem no nome (senão o Enter arrasta p/ o Nome antes do
    // confirm() e a classe/COMEÇAR nunca são alcançados).
    if (unicode < 32 || unicode > 126) return;
    if (cursor_ != kNameRow) cursor_ = kNameRow; // digitar foca o Nome
    if (name_.size() >= kMaxName) return;
    name_.push_back(static_cast<char>(unicode));
    nameWarn_ = false; // digitou: some o aviso
}

void CharacterCreationScreen::handleBackspace() {
    if (cursor_ != kNameRow) return;
    if (!name_.empty()) name_.pop_back();
}

Action CharacterCreationScreen::consumeAction() {
    const Action a = pending_;
    pending_ = Action::None;
    return a;
}

void CharacterCreationScreen::render(sf::RenderTarget &target,
                                     const sf::Font &font, float sw,
                                     float sh) const {
    target.setView(sf::View(sf::FloatRect(0.f, 0.f, sw, sh)));
    sf::RectangleShape bg({sw, sh});
    bg.setFillColor(sf::Color(8, 8, 14));
    target.draw(bg);

    auto textAt = [&](const std::string &s, float x, float y, int size,
                      sf::Color c) {
        sf::Text t;
        t.setFont(font);
        t.setString(support::utf8(s));
        t.setCharacterSize(static_cast<unsigned>(size));
        t.setFillColor(c);
        t.setOutlineColor(sf::Color::Black);
        t.setOutlineThickness(1);
        t.setPosition(x, y);
        target.draw(t);
    };
    auto centerText = [&](const std::string &s, float y, int size,
                          sf::Color c) {
        sf::Text t;
        t.setFont(font);
        t.setString(support::utf8(s));
        t.setCharacterSize(static_cast<unsigned>(size));
        t.setFillColor(c);
        t.setOutlineColor(sf::Color::Black);
        t.setOutlineThickness(1);
        const auto b = t.getLocalBounds();
        t.setPosition(sw * 0.5f - (b.left + b.width * 0.5f), y);
        target.draw(t);
    };
    centerText("NOVO PERSONAGEM", sh * 0.5f - 220.f, 32,
               sf::Color(200, 180, 120));

    // Coluna esquerda: Nome, classes, COMEÇAR.
    const float xL = sw * 0.5f - 260.f;
    float y = sh * 0.5f - 150.f;
    {
        const bool sel = (cursor_ == kNameRow);
        std::string s =
            std::string(sel ? "> " : "  ") + "Nome: " + name_;
        if (sel && (name_.size() < kMaxName)) s += "_";
        textAt(s, xL, y, 20,
               sel ? sf::Color(255, 220, 100)
                   : sf::Color(170, 170, 180));
        y += 34.f;
        if (nameWarn_) {
            textAt("Dê um nome (obrigatório)", xL + 20.f, y - 8.f, 13,
                   sf::Color(240, 120, 120));
            y += 18.f;
        }
    }
    for (int i = 0; i < kClassCount; ++i) {
        const bool sel = (cursor_ == kClassFirst + i);
        const bool marked =
            (selected_ ==
             static_cast<core::PlayerClass>(i));
        std::string s = std::string(sel ? "> " : "  ") +
                        core::className(static_cast<core::PlayerClass>(i));
        if (marked) s += "  [x]";
        textAt(s, xL, y, 20,
               sel ? sf::Color(255, 220, 100)
                   : sf::Color(170, 170, 180));
        y += 30.f;
    }
    {
        const bool sel = (cursor_ == kStartRow);
        textAt(std::string(sel ? "> " : "  ") + "COMEÇAR", xL, y + 6.f,
               22,
               sel ? sf::Color(140, 255, 140)
                   : sf::Color(170, 170, 180));
    }

    // Coluna direita: preview da selecionada (attrs + kit + flavor).
    const float xR = sw * 0.5f + 40.f;
    float yr = sh * 0.5f - 150.f;
    const core::ClassDef def = core::classDef(selected_);
    textAt(def.flavor, xR, yr, 13, sf::Color(150, 150, 150));
    yr += 30.f;
    for (int i = 0; i < core::kAttrCount; ++i) {
        const auto a = static_cast<core::Attr>(i);
        textAt(std::string(core::attrName(a)) + ": " +
                   std::to_string(def.base[i]),
               xR, yr, 15, sf::Color(200, 200, 200));
        yr += 24.f;
    }
    yr += 6.f;
    std::string kit;
    for (const auto &id : def.equipment) {
        if (const auto *d = core::ItemRegistry::instance().find(id)) {
            if (!kit.empty()) kit += ", ";
            kit += d->name;
        }
    }
    for (const auto &sp : def.spells) {
        if (const auto *d = core::ItemRegistry::instance().find(sp)) {
            if (!kit.empty()) kit += ", ";
            kit += d->name;
        }
    }
    if (kit.empty()) kit = "(nada)";
    textAt("Kit: " + kit, xR, yr, 14, sf::Color(150, 200, 255));

    centerText("Setas navegam - letras digitam - Enter/Espaço confirma",
               sh - 60.f, 13, sf::Color(120, 120, 130));
}

} // namespace creation
} // namespace game
