#include "game.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

#include "core/Config.h"
#include "core/Material.h"
#include "entities/Entity.hpp"
#include "entities/Player/Player.h"
#include "world/Stratum.h"
#include "world/World.h"
#include "assets/EquipmentLayout.h"
#include "assets/PlayerSprite.h"
#include "assets/SpriteFrameRegistry.h"
#include "support/Combat/Body.h"
#include "support/Combat/WeaponRegistry.h"
#include "support/Debug/BodyDump.h"
#include "support/Effects/ParticleSystem.h"
#include "support/Effects/ThrowSystem.h"
#include "support/Enemies/EnemySystem.h"
#include "support/Progression/DropSystem.h"

// Renderer: tudo que desenha (render + draws + helpers de char-view).

namespace {
// Cor determinística por char (hash → RGB). '.' = transparente.
sf::Color colorForChar(char c) {
    if (c == '.') return {0, 0, 0, 0};
    const uint32_t h = static_cast<uint32_t>(c) * 2654435761u;
    return sf::Color{
        static_cast<sf::Uint8>(80 + (h & 0x7F)),
        static_cast<sf::Uint8>(80 + ((h >> 7) & 0x7F)),
        static_cast<sf::Uint8>(80 + ((h >> 14) & 0x7F))};
}

// Posição final da peça: game::pieceDrawPos (fonte única, testável).

// Renderiza o ASCII do frame com uma cor por char (F3).
// Ignora texturas: mostra a segmentação semântica direto do dado.
void renderCharView(sf::RenderTarget &target,
                    float px, float py, // canto superior esquerdo no mundo
                    int facing, float scale,
                    const char *const *rows, int w, int h) {
    sf::RectangleShape pixel({scale, scale});
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const char c = rows[y][x];
            if (c == '.') continue;
            pixel.setFillColor(colorForChar(c));
            const float sx = (facing >= 0)
                                 ? px + x * scale
                                 : px + (w - 1 - x) * scale;
            pixel.setPosition(sx, py + y * scale);
            target.draw(pixel);
        }
    }
}
}


void Game::render()
{

    window->clear(sf::Color(135, 206, 235));
    camera.setViewport(viewW_, viewH_);
    camera.follow(player.get()->getX(), player.get()->getY());
    sf::Vector2f camPos = camera.position();
    auto view = window->getDefaultView();
    view.move(camPos.x, camPos.y);
    window->setView(view);
    auto &objects = getWorld()->getPlatforms();

    // Desenha só o visível (+margem), por range de chunks — não pela
    // lista global (chunks modified pinned não encarecem o frame).
    float vx0 = camPos.x - 60.0f, vy0 = camPos.y - 60.0f;
    float vx1 = camPos.x + viewW_ + 60.0f, vy1 = camPos.y + viewH_ + 60.0f;
    // Fundo chapado do estrato do player (1 draw; pop na fronteira
    // marca a transição de propósito).
    {
        const int pty = static_cast<int>(std::floor(player.get()->getY() / core::kBlockSize));
        const support::StratumBg bg =
            support::stratumBg(support::stratumAt(pty));
        sf::RectangleShape bgRect(sf::Vector2f(vx1 - vx0, vy1 - vy0));
        bgRect.setPosition(vx0, vy0);
        bgRect.setFillColor(sf::Color(bg.r, bg.g, bg.b));
        window->draw(bgRect);
    }
    getWorld()->forEachEntityInRect(vx0, vy0, vx1, vy1, [&](Entity *entity) {
        entity->draw(window);
    });

    drawPlayerSprite();
    drawPlayerEquipment(); // elmo, peitoral, perneiras
    drawPlayerWeapon();    // espada por cima
    drawEnemiesSprites();

    // Barks com fade 1.5s acima da cabeça (texto; áudio futuro).
    enemies_->forEach([&](support::Enemy &s) {
        if (s.barkTimer <= 0.f || s.currentBark.empty()) return;
        sf::Text t;
        t.setFont(font);
        t.setString(s.currentBark);
        t.setCharacterSize(14);
        const sf::Uint8 a = static_cast<sf::Uint8>(
            255.f * std::max(0.f, std::min(1.f, s.barkTimer / 1.5f)));
        t.setFillColor(sf::Color(255, 240, 200, a));
        t.setOutlineColor(sf::Color(0, 0, 0, a));
        t.setOutlineThickness(1);
        t.setPosition(s.body.getX() - 20.f, s.body.getY() - 24.f);
        window->draw(t);
    });

    // Throwables visíveis: círculo com cor pelo fuse (verde→vermelho).
    throws_->forEachActive([&](const support::Throwable &t) {
        sf::CircleShape c(3.f);
        c.setOrigin(3.f, 3.f);
        c.setPosition(t.pos);
        if (t.kind == support::ThrowKind::Dynamite) {
            float r = std::clamp(t.fuse / 1.0f, 0.f, 1.f); // 1 cheio → 0 explodindo
            c.setFillColor(sf::Color(
                static_cast<sf::Uint8>(255 - 155 * r),
                static_cast<sf::Uint8>(80 + 120 * r),
                60));
        } else {
            c.setFillColor(sf::Color(200, 180, 60));
        }
        window->draw(c);
    });

    particles_->render(*window);

    // Flash do swing: outline da hitbox só na janela Active.
    if (player.get()->meleePhase == MeleePhase::Active) {
        const sf::FloatRect box = player.get()->meleeHitbox();
        sf::RectangleShape r(sf::Vector2f(box.width, box.height));
        r.setPosition(box.left, box.top);
        r.setFillColor(sf::Color::Transparent);
        r.setOutlineColor(sf::Color::Yellow);
        r.setOutlineThickness(1.f);
        window->draw(r);
    }

    drops_->render(*window);

    // Debug draw das hitboxes por parte (só com overlay ligado).
    if (overlay_.visible()) {
        auto drawParts = [&](const support::Body &b) {
            b.forEach([&](const support::PartState &st, const support::PartDef &) {
                sf::RectangleShape r(sf::Vector2f(st.worldBox.width, st.worldBox.height));
                r.setPosition(st.worldBox.left, st.worldBox.top);
                r.setFillColor(sf::Color::Transparent);
                r.setOutlineColor(sf::Color::Magenta);
                r.setOutlineThickness(1.f);
                window->draw(r);
            });
        };
        drawParts(player.get()->body);
        enemies_->forEach([&](support::Enemy &s) { drawParts(s.bodyParts); });

        // Dump textual a cada 30 frames (não spamma stderr por frame).
        static int dumpCounter = 0;
        if (++dumpCounter % 30 == 0) {
            const auto f = assets::frameData(player.get()->currentFrameId);
            support::dumpBody(std::cerr, player.get()->body,
                              f.rows, f.w, f.h, f.pal, f.palCount);
        }
    }

    // F3: char-view do player (1 char = 1 cor), sem textura.
    if (charView_) {
        const auto f = assets::frameData(player.get()->currentFrameId);
        if (f.rows) {
            const float s = player.get()->getH() / static_cast<float>(f.h);
            const float px = player.get()->getCenterX() - f.w * 0.5f * s;
            const float py = player.get()->getY() + player.get()->getH() - f.h * s;
            renderCharView(*window, px, py, player.get()->facing, s,
                           f.rows, f.w, f.h);
        }
    }

    overlay_.render(*window, font, *getWorld(), *player.get(), objects.size());

    // HUD em espaço de tela (view default): HP, TNT, estrato, morte/pause.
    // Tosco de propósito; HUD bonito é polimento.
    {
        window->setView(window->getDefaultView());
        Player *p = player.get();
        const float ratio = static_cast<float>(p->hp) / static_cast<float>(p->hpMax);

        sf::RectangleShape hpBg(sf::Vector2f(204.f, 20.f));
        hpBg.setPosition(16.f, 16.f);
        hpBg.setFillColor(sf::Color(40, 0, 0));
        window->draw(hpBg);
        sf::RectangleShape hpFg(sf::Vector2f(200.f * ratio, 16.f));
        hpFg.setPosition(18.f, 18.f);
        hpFg.setFillColor(sf::Color(200, 30, 30));
        window->draw(hpFg);

        auto text = [&](const std::string &s, float x, float y, int size = 18) {
            sf::Text t;
            t.setFont(font);
            t.setString(s);
            t.setCharacterSize(size);
            t.setFillColor(sf::Color::White);
            t.setOutlineColor(sf::Color::Black);
            t.setOutlineThickness(1);
            t.setPosition(x, y);
            window->draw(t);
        };
        text("HP " + std::to_string(p->hp) + "/" + std::to_string(p->hpMax), 16.f, 38.f);
        text("TNT:" + std::to_string(p->dynamiteCount) + " J  K melee", 16.f, 62.f);
        text(std::string("Mat: ") + core::materialName(p->loadout.weapon), 16.f, 110.f);
        const int pty = static_cast<int>(std::floor(p->getY() / core::kBlockSize));
        text(std::string(support::stratumName(support::stratumAt(pty)))
             + "  y" + std::to_string(pty), 16.f, 86.f);

        if (run_.isDead()) {
            sf::RectangleShape dim(sf::Vector2f(viewW_, viewH_));
            dim.setFillColor(sf::Color(0, 0, 0, 160));
            window->draw(dim);
            text("VOCE MORREU", viewW_ * 0.5f - 110.f, viewH_ * 0.5f - 40.f, 36);
            text("R para renascer no checkpoint", viewW_ * 0.5f - 170.f, viewH_ * 0.5f + 10.f, 20);
        } else if (run_.isPaused()) {
            text("PAUSADO (ESC)", viewW_ * 0.5f - 110.f, viewH_ * 0.5f - 20.f, 28);
        }
    }

    window->display();
}

void Game::drawPlayerSprite() {
    Player *p = player.get();
    const sf::Texture *tex =
        game::textureForFrame(p->currentFrameId, sprites_, p->meleeTex);
    // Escala p/ altura da entidade (50px), aspecto preservado.
    const float s = p->getH() / static_cast<float>(sprites::kPlayerH);
    sf::Sprite spr;
    spr.setTexture(*tex);
    spr.setOrigin(sprites::kPlayerW * 0.5f, static_cast<float>(sprites::kPlayerH));
    spr.setPosition(p->getCenterX(), p->getY() + p->getH());
    spr.setScale(static_cast<float>(p->facing) * s, s);
    window->draw(spr);
}

void Game::drawPlayerEquipment() {
    Player *p = player.get();
    if (run_.isDead() || !p->loadout.equipped) return;
    const float s = p->getH() / static_cast<float>(sprites::kPlayerH);
    const int f = p->facing;
    const int mHelm = static_cast<int>(p->loadout.helm);
    const int mChest = static_cast<int>(p->loadout.chest);
    const int mLegs = static_cast<int>(p->loadout.legs);

    // Canto superior esquerdo do sprite no mundo (grade 12x20 do player).
    const float spriteLeft = p->getCenterX() - (sprites::kPlayerW * 0.5f) * s;
    const float spriteTop = p->getY() + p->getH() - sprites::kPlayerH * s;

    // Helper que desenha um overlay na grade 12x20 do player.
    // Origin sempre (0,0): com facing<0 o setScale(-s,s) espelha a
    // partir do canto, e equipSpritePos já ancora o canto direito.
    auto drawSprite = [&](const sf::Texture &tex,
                          float spriteX, float spriteY) {
        sf::Sprite spr(tex);
        const sf::Vector2f at = game::equipSpritePos(
            spriteLeft, spriteTop, s, f, spriteX, spriteY);
        spr.setPosition(at);
        spr.setScale(s * f, s);
        window->draw(spr);
    };

    // Posições em coords do sprite 12x20 (mesmas do ASCII do player).
    // Elmo: cobre rows 0-4 do player, cols 0-11.
    drawSprite(sprites_.helm[mHelm], 0.f, 0.f);

    // Peitoral: cobre rows 6-13 (túnica + cinto), cols 0-11.
    drawSprite(sprites_.chest[mChest], 0.f, 6.f);

    // Perneiras: cobre rows 14-16 (parte superior das pernas).
    drawSprite(sprites_.legs[mLegs], 0.f, 14.f);

    // Botas: cobre rows 17-19.
    drawSprite(sprites_.boots[mLegs], 0.f, 17.f);

    // Luvas: 2 draws, uma em cada mão (cols ~1 e ~10).
    const int mGlove = mHelm; // mesmo material do elmo (sem slot próprio)
    drawSprite(sprites_.gloves[mGlove], 0.f, 7.f);
    drawSprite(sprites_.gloves[mGlove], 8.f, 7.f);
}

void Game::drawPlayerWeapon() {
    Player *p = player.get();
    if (run_.isDead() || !p->loadout.equipped) return;
    const int m = static_cast<int>(p->loadout.weapon);
    const float s = p->getH() / static_cast<float>(sprites::kPlayerH);
    // Mão = centro do ArmR (espelho de computeWeaponBbox em BodySystem).
    const auto *arm = p->body.find(support::BodyPartId::ArmR);
    if (!arm) return;
    const float handX = arm->worldBox.left + arm->worldBox.width * 0.5f;
    const float handY = arm->worldBox.top + arm->worldBox.height * 0.5f;
    const sf::Texture *tex = &sprites_.swordIdle[m];
    float originX = 4.f, originY = 20.f;
    // Machado só tem idle: mesma textura em toda fase (dado no registry).
    const support::WeaponDef *wd =
        support::WeaponRegistry::instance().find(p->loadout.weaponId);
    const bool phased = !wd || wd->hasSwingPhases;
    if (!phased) {
        tex = &sprites_.axeIdle[m];
    } else {
        switch (p->meleePhase) {
            case MeleePhase::Windup:
                tex = &sprites_.swordWindup[m];
                break;
            case MeleePhase::Active:
            case MeleePhase::Recovery:
                tex = &sprites_.swordSwing[m];
                originX = 5.f;
                originY = 5.f;
                break;
            default:
                break;
        }
    }
    sf::Sprite spr;
    spr.setTexture(*tex);
    spr.setOrigin(originX, originY);
    spr.setPosition(handX + 4.f * static_cast<float>(p->facing) * s,
                    handY + 8.f);
    spr.setScale(s * static_cast<float>(p->facing), s);
    window->draw(spr);
}

void Game::drawEnemiesSprites() {
    enemies_->forEach([&](support::Enemy &s) {
        // Frame já decidido no tick (currentFrameId); aqui só desenha.
        // Dimensões vêm do registry para não hardcodar por tipo.
        // None (inimigo sem frame) = fallback slime 14x12.
        const auto f = assets::frameData(s.currentFrameId);
        const float fw = f.rows ? static_cast<float>(f.w) : 14.f;
        const float fh = f.rows ? static_cast<float>(f.h) : 12.f;
        const sf::Texture *tex = game::textureForFrame(s.currentFrameId, sprites_);
        const float sc = s.body.getH() / fh;
        sf::Sprite spr;
        spr.setTexture(*tex);
        spr.setOrigin(fw * 0.5f, fh);
        spr.setPosition(s.body.getCenterX(), s.body.getY() + s.body.getH());
        spr.setScale(static_cast<float>(s.body.facing) * sc, sc);
        window->draw(spr);
    });
}
