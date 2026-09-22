#include "game.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <utility>

#include "core/Config.h"
#include "core/Celestial.h"
#include "core/Material.h"
#include "core/RadialTexture.h"
#include "core/Time.h"
#include "entities/Entity.hpp"
#include "entities/Player/Player.h"
#include "world/ChunkKey.h"
#include "world/LightPropagator.h"
#include "world/Stratum.h"
#include "world/TileRenderer.h"
#include "world/World.h"
#include "assets/EquipmentLayout.h"
#include "assets/PlayerSprite.h"
#include "assets/SpriteFrameRegistry.h"
#include "support/Combat/Body.h"
#include "support/Combat/WeaponRegistry.h"
#include "support/Debug/BodyDump.h"
#include "support/Effects/ParticleSystem.h"
#include "support/Effects/ThrowSystem.h"
#include "support/Enemies/DwarfAI.h"
#include "support/Enemies/EnemySystem.h"
#include "support/Enemies/SlimeAI.h"
#include "support/Progression/DropSystem.h"

// Renderer: tudo que desenha (render + draws + helpers de char-view).

namespace {
// Cor por estado do anão p/ label de debug (F5). Cinza = calmo.
std::pair<const char *, sf::Color> dwarfLabel(support::DwarfState st) {
    using support::DwarfState;
    switch (st) {
        case DwarfState::Patrol: return {"PATROL", {160, 160, 160}};
        case DwarfState::Alert: return {"ALERT", {255, 255, 0}};
        case DwarfState::ThrowWindup: return {"THROW", {255, 140, 0}};
        case DwarfState::ThrowRelease: return {"THROW", {255, 140, 0}};
        case DwarfState::Recover: return {"RECOVER", {100, 160, 255}};
        case DwarfState::Melee: return {"MELEE", {255, 60, 60}};
        case DwarfState::Retreat: return {"RETREAT", {200, 100, 255}};
        default: return {"?", {255, 255, 255}};
    }
}

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

    camera.setViewport(viewW_, viewH_);
    camera.follow(player.get()->getX(), player.get()->getY());
    sf::Vector2f camPos = camera.position();
    // Snap do canto superior-esquerdo (não do centro): vale p/ janela
    // par e ímpar — view/2 fracionário não reintroduz a fresta de 1px
    // entre tiles (céu aparecendo na grade) nem no lightmap bilinear.
    camPos.x = std::round(camPos.x - viewW_ * 0.5f) + viewW_ * 0.5f;
    camPos.y = std::round(camPos.y - viewH_ * 0.5f) + viewH_ * 0.5f;
    auto view = window->getDefaultView();
    view.move(camPos.x, camPos.y);
    window->setView(view);
    // View do mundo salva: o HUD troca p/ default depois, e o foco do
    // screenshot precisa converter mundo→pixel nesta view (não na default).
    const sf::View worldView = window->getView();
    screenshots_.setWorldView(worldView);
    auto &objects = getWorld()->getPlatforms();

    // Desenha só o visível (+margem), por range de chunks — não pela
    // lista global (chunks modified pinned não encarecem o frame).
    float vx0 = camPos.x - 60.0f, vy0 = camPos.y - 60.0f;
    float vx1 = camPos.x + viewW_ + 60.0f, vy1 = camPos.y + viewH_ + 60.0f;
    // Céu dinâmico do ciclo dia/noite (superfície) ou preto de caverna.
    // O estrato continua no HUD; o fundo agora é o céu, não o chapado.
    {
        const float playerY  = player.get()->getY();
        const float playerCX = player.get()->getCenterX();
        const float surfY    = getWorld()->surfaceYAt(playerCX);
        window->clear(lighting_.ambientSky(playerY, surfY));
    }
    // Tiles em batch pré-renderizado (camadas 1+5: 1 sprite/chunk);
    // entidades-tile puladas abaixo (já estão no batch — sem double-draw).
    getWorld()->forEachChunkInRect(vx0, vy0, vx1, vy1, [&](support::Chunk *c) {
        if (c->tileDirty) {
            support::TileRenderer::rebuild(*c);
            support::TileRenderer::upload(*c);
        }
        support::TileRenderer::drawLayer(*c, *window);
    });
    getWorld()->forEachEntityInRect(vx0, vy0, vx1, vy1, [&](Entity *entity) {
        if (support::TileRenderer::isTileKind(entity->getName())) return;
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

    // Labels de IA + aggro (canal F5): estado do anão, chase do slime,
    // retângulo de aggro do slime (condição real: |dx|,|dy| < 400),
    // círculo do anão (radial, cfg do tipo).
    if (overlay_.visible() && overlay_.ai()) {
        enemies_->forEach([&](support::Enemy &s) {
            const float cx = s.body.getCenterX();
            const float cy = s.body.getCenterY();
            if (const auto *d =
                    dynamic_cast<const support::DwarfAI *>(s.ai.get())) {
                const auto [label, color] = dwarfLabel(d->state());
                sf::Text t;
                t.setFont(font);
                t.setString(label);
                t.setCharacterSize(12);
                t.setFillColor(color);
                t.setOutlineColor(sf::Color::Black);
                t.setOutlineThickness(1);
                t.setPosition(s.body.getX() - 20.f, s.body.getY() - 40.f);
                window->draw(t);
                const float r = d->aggroRange();
                sf::CircleShape c(r);
                c.setOrigin(r, r);
                c.setPosition(cx, cy);
                c.setFillColor(sf::Color::Transparent);
                c.setOutlineColor(sf::Color(100, 255, 100, 160));
                c.setOutlineThickness(1.f);
                window->draw(c);
            } else if (const auto *sl =
                           dynamic_cast<const support::SlimeAI *>(
                               s.ai.get())) {
                sf::Text t;
                t.setFont(font);
                t.setString(sl->chasing() ? "CHASE" : "PATROL");
                t.setFillColor(sl->chasing() ? sf::Color(255, 80, 80)
                                             : sf::Color(160, 160, 160));
                t.setCharacterSize(12);
                t.setOutlineColor(sf::Color::Black);
                t.setOutlineThickness(1);
                t.setPosition(s.body.getX() - 20.f, s.body.getY() - 40.f);
                window->draw(t);
                sf::RectangleShape r(sf::Vector2f(800.f, 800.f));
                r.setPosition(cx - 400.f, cy - 400.f);
                r.setFillColor(sf::Color::Transparent);
                r.setOutlineColor(sf::Color(255, 100, 100, 160));
                r.setOutlineThickness(1.f);
                window->draw(r);
            }
        });
    }

    // Throwables visíveis: TNT animada por fuse + telegraph do raio.
    // Outros kinds continuam no círculo dourado.
    throws_->forEachActive([&](const support::Throwable &t) {
        if (t.kind != support::ThrowKind::Dynamite) {
            sf::CircleShape c(3.f);
            c.setOrigin(3.f, 3.f);
            c.setPosition(t.pos);
            c.setFillColor(sf::Color(200, 180, 60));
            window->draw(c);
            return;
        }

        // ── 1. Telegraph: aura pulsante até o raio real do dano ──
        if (t.fuse > 0.f) {
            const float fuseRatio = std::clamp(t.fuse / 0.8f, 0.f, 1.f);
            const float pulse     = 0.5f + 0.5f * std::sin(t.fuse * 30.f);
            const float alpha     = 40.f + 80.f * (1.f - fuseRatio);
            const float radius    = t.radius * (0.5f + 0.5f * pulse * (1.f - fuseRatio));

            sf::CircleShape ring(radius);
            ring.setOrigin(radius, radius);
            ring.setPosition(t.pos);
            ring.setFillColor(sf::Color(255, 100, 40,
                                        static_cast<sf::Uint8>(alpha)));
            ring.setOutlineColor(sf::Color(255, 200, 80,
                                           static_cast<sf::Uint8>(alpha * 2)));
            ring.setOutlineThickness(1.f);
            window->draw(ring);
        }

        // ── 2. Sprite da TNT, frame por fuse ──
        int frame = 0;
        if      (t.fuse < 0.33f) frame = 2; // crítico — pavio consumido
        else if (t.fuse < 0.66f) frame = 1; // queimando
        // else: frame 0 (fresco)

        const float scale = 2.5f; // mesmo do player
        sf::Sprite spr(sprites_.tnt[frame]);
        spr.setOrigin(3.f, 4.f); // centro do sprite 6x8
        spr.setPosition(t.pos);
        spr.setScale(scale, scale);

        // Flash branco no último 15% (o "vai explodir agora").
        if (t.fuse < 0.15f) {
            const float flash = 0.5f + 0.5f * std::sin(t.fuse * 60.f);
            spr.setColor(sf::Color(255, 255,
                static_cast<sf::Uint8>(180 + 75 * flash)));
        }
        window->draw(spr);
    });

    particles_->render(*window);

    // Flash do swing: outline da hitbox só na janela Active (canal F2).
    if (player.get()->meleePhase == MeleePhase::Active &&
        overlay_.visible() && overlay_.hitboxes()) {
        const sf::FloatRect box = player.get()->meleeHitbox();
        sf::RectangleShape r(sf::Vector2f(box.width, box.height));
        r.setPosition(box.left, box.top);
        r.setFillColor(sf::Color::Transparent);
        r.setOutlineColor(sf::Color::Yellow);
        r.setOutlineThickness(1.f);
        window->draw(r);
    }

    drops_->render(*window);

    // Debug draw das hitboxes por parte (canal F2; F1 master).
    if (overlay_.visible() && overlay_.hitboxes()) {
        Player *p = player.get();
        auto drawParts = [&](const support::Body &b) {
            b.forEach([&](const support::PartState &st,
                          const support::PartDef &) {
                if (st.fromSchema) return; // parte inexistente neste frame
                sf::RectangleShape r(sf::Vector2f(st.worldBox.width, st.worldBox.height));
                r.setPosition(st.worldBox.left, st.worldBox.top);
                r.setFillColor(sf::Color::Transparent);
                r.setOutlineColor(sf::Color::Magenta);
                r.setOutlineThickness(1.f);
                window->draw(r);
            });
        };
        drawParts(p->body);
        enemies_->forEach([&](support::Enemy &s) { drawParts(s.bodyParts); });

        // Máscara do raycast (canal F7): amarelo = raio alcançou.
        // Parede cortando o amarelo ao meio = oclusão funcionando.
        if (overlay_.lightMask()) {
            const int ptx = static_cast<int>(p->getX() / core::kBlockSize);
            const int pty = static_cast<int>(p->getY() / core::kBlockSize);
            const support::ChunkCoord cc = support::chunkCoordFromWorld(
                ptx, pty, support::Chunk::W);
            if (const support::Chunk *c = getWorld()->findChunk(cc.x, cc.y)) {
                const float cs = static_cast<float>(core::kBlockSize);
                const float ox = c->cx * support::Chunk::W * cs;
                const float oy = c->cy * support::Chunk::H * cs;
                for (int y = 0; y < support::Chunk::H; ++y) {
                    for (int x = 0; x < support::Chunk::W; ++x) {
                        if (!c->isVisible(x, y)) continue;
                        sf::RectangleShape r({cs, cs});
                        r.setPosition(ox + x * cs, oy + y * cs);
                        r.setFillColor(sf::Color(255, 255, 0, 25));
                        r.setOutlineColor(sf::Color(255, 255, 0, 80));
                        r.setOutlineThickness(1.f);
                        window->draw(r);
                    }
                }
            }
        }

        // ── 1. Weapon bbox sempre (ciano): onde a arma está agora,
        // mesmo fora do Active (só existe com arma visível).
        if (p->loadout.equipped) {
            if (const auto *w = p->body.find(support::BodyPartId::Weapon)) {
                if (w->worldBox.width > 0.5f) {
                    sf::RectangleShape r(
                        {w->worldBox.width, w->worldBox.height});
                    r.setPosition(w->worldBox.left, w->worldBox.top);
                    r.setFillColor(sf::Color::Transparent);
                    r.setOutlineColor(sf::Color::Cyan);
                    r.setOutlineThickness(1.f);
                    window->draw(r);
                }
            }
        }

        // ── 2. Preview da hitbox 8-dir (laranja, sólida: SFML não
        // tem tracejado): onde a hitbox cairia se apertasse K agora.
        // Mesma fórmula de Player::meleeHitbox(), mas com a mira
        // atual (fora do swing) ou o snapshot (no swing), sem exigir
        // Active. Tabela duplicada só p/ debug; quem manda em prod é
        // Player::meleeHitbox(). Escala da arma replicada (machado).
        {
            const auto aim = p->inMeleeSwing() ? p->swingAim : p->aimDir;
            struct HB {
                float cx, cy, w, h;
            };
            static const HB kPrev[8] = {
                {20.f, 0.f, 20.f, 14.f}, // E
                {14.f, -14.f, 18.f, 14.f}, // NE
                {0.f, -20.f, 14.f, 20.f}, // N
                {-14.f, -14.f, 18.f, 14.f}, // NW
                {-20.f, 0.f, 20.f, 14.f}, // W
                {-14.f, 14.f, 18.f, 14.f}, // SW
                {0.f, 20.f, 14.f, 20.f}, // S
                {14.f, 14.f, 18.f, 14.f}, // SE
            };
            const auto &hb = kPrev[static_cast<int>(aim)];
            float ws = 1.f, hs = 1.f;
            if (p->loadout.equipped) {
                if (const auto *wd =
                        support::WeaponRegistry::instance().find(
                            p->loadout.weaponId)) {
                    ws = wd->spriteW / 16.f;
                    hs = wd->spriteH / 8.f;
                }
            }
            const float w = hb.w * ws, h = hb.h * hs;
            const float cx = p->getCenterX() + hb.cx;
            const float cy = p->getCenterY() + hb.cy;
            sf::RectangleShape r({w, h});
            r.setPosition(cx - w * 0.5f, cy - h * 0.5f);
            r.setFillColor(sf::Color::Transparent);
            r.setOutlineColor(sf::Color(255, 140, 0)); // laranja
            r.setOutlineThickness(1.f);
            window->draw(r);
        }

        // ── 3. Indicador de mira (linha amarela): centro do player
        // na direção de aimDir.
        {
            const auto v = support::aimVector(p->aimDir);
            const float cx = p->getCenterX();
            const float cy = p->getCenterY();
            sf::Vertex line[] = {
                sf::Vertex({cx, cy}, sf::Color::Yellow),
                sf::Vertex({cx + v.x * 40.f, cy + v.y * 40.f},
                           sf::Color::Yellow),
            };
            window->draw(line, 2, sf::Lines);
        }

        // ── 4. Números de dano flutuantes (feed): sobem e somem em
        // 0.5s. Valida per-part no olho (-16 = head, -5 = arm).
        for (const auto &n : debugFeed_.numbers) {
            const float risen =
                (support::DebugFeed::kNumberTtl - n.ttl) * 80.f;
            const sf::Uint8 a = static_cast<sf::Uint8>(
                255.f * std::max(0.f, std::min(1.f, n.ttl /
                                                         support::DebugFeed::kNumberTtl)));
            sf::Text t;
            t.setFont(font);
            t.setString(n.text);
            t.setCharacterSize(14);
            t.setFillColor(sf::Color(255, 240, 200, a));
            t.setOutlineColor(sf::Color(0, 0, 0, a));
            t.setOutlineThickness(1);
            t.setPosition(n.pos.x - 10.f, n.pos.y - risen);
            window->draw(t);
        }

        // ── 5. Linha Body↔sprite do player: tripwire de offset. Por
        // construção é ~zero (mesma fórmula dos 2 lados); se abrir
        // >2px, alguma conta de desenho divergiu. Vermelho = divergiu.
        {
            Player *pl = player.get();
            const float s =
                pl->getH() / static_cast<float>(sprites::kPlayerH);
            const sf::Vector2f a{pl->getCenterX(), pl->getCenterY()};
            const sf::Vector2f b{pl->getCenterX(),
                                 pl->getY() + pl->getH() - 10.f * s};
            const float dx = a.x - b.x, dy = a.y - b.y;
            const bool diverged =
                (dx * dx + dy * dy) > 4.f; // 2px ao quadrado
            const sf::Color c =
                diverged ? sf::Color::Red : sf::Color::Green;
            sf::Vertex line[] = {sf::Vertex(a, c), sf::Vertex(b, c)};
            window->draw(line, 2, sf::Lines);
        }

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

    // ─── Iluminação por tile (grids × tint do ciclo, antes do HUD) ───
    // Sprites W×H do lightmap com Multiply; tint quente/frio carrega o
    // dia/noite sem relight (grids são topologia baked cheia).
    {
        const sf::Color tint = lighting_.lightTint();
        const float cs = static_cast<float>(core::kBlockSize);
        const float cw = support::Chunk::W * cs;
        const float ch = support::Chunk::H * cs;
        getWorld()->forEachChunkInRect(vx0, vy0, vx1, vy1, [&](support::Chunk *c) {
            if (c->lightDirty)
                support::LightPropagator::updateTexture(
                    *c, getWorld()->findChunk(c->cx - 1, c->cy),
                    getWorld()->findChunk(c->cx + 1, c->cy),
                    getWorld()->findChunk(c->cx, c->cy - 1),
                    getWorld()->findChunk(c->cx, c->cy + 1),
                    getWorld()->findChunk(c->cx - 1, c->cy - 1),
                    getWorld()->findChunk(c->cx + 1, c->cy - 1),
                    getWorld()->findChunk(c->cx - 1, c->cy + 1),
                    getWorld()->findChunk(c->cx + 1, c->cy + 1));
            if (!c->lightmap.getSize().x) return;
            sf::Sprite spr(c->lightmap);
            spr.setPosition(c->cx * cw, c->cy * ch);
            // 2 texels por tile (32×32): mesma pegada 800×800, sem overlap.
            // Overlap com Multiply escureceria a faixa (não-idempotente).
            const float ls = cs / support::LightPropagator::kLightmapScale;
            spr.setScale(ls, ls);
            spr.setColor(tint);
            sf::RenderStates rs;
            rs.blendMode = sf::BlendMultiply;
            window->draw(spr, rs);
        });
    }

    // ─── Astros PÓS-multiply (item 18, fix lua escura) ───
    // Pré-multiply o tint noturno os apagava (lua cinza). Aqui brilham,
    // mas SÓ onde o céu chega: gate por skyLight (sem lua em caverna).
    // View de mundo ainda ativa; tiles já desenhados não importam mais.
    {
        const float h = dayNight_.hour();
        const auto s = dayNight_.sample();
        const float darkness = 1.f - std::clamp(s.sunIntensity + s.moonIntensity,
                                                0.f, 1.f);
        const float bs = static_cast<float>(core::kBlockSize);
        constexpr int W = support::Chunk::W;
        auto skyOpen = [&](float wx, float wy) -> float {            const int tx = static_cast<int>(std::floor(wx / bs));
            const int ty = static_cast<int>(std::floor(wy / bs));
            const int cx = tx >= 0 ? tx / W : -((-tx + W - 1) / W);
            const int cy = ty >= 0 ? ty / W : -((-ty + W - 1) / W);
            const support::Chunk* c = getWorld()->findChunk(cx, cy);
            if (!c) return 0.f;
            const int lx = tx - cx * W, ly = ty - cy * W;
            if (lx < 0 || lx >= W || ly < 0 || ly >= W) return 0.f;
            return c->skyLight[ly * W + lx] / 15.f;
        };
        // Gate por ÁREA (não pixel): o disco tem raio e o centro em céu
        // aberto não basta — bordas invadiriam a rocha e desenhavam por
        // cima (visível descendo/bombando). Mínimo em 9 pontos no raio.
        auto skyOpenArea = [&](float wx, float wy, float r) -> float {
            float g = skyOpen(wx, wy);
            const float o[8][2] = {{r, 0},   {-r, 0},  {0, r},   {0, -r},
                                   {r, -r},  {-r, -r}, {r, r},   {-r, r}};
            for (const auto& d : o) {
                const float v = skyOpen(wx + d[0], wy + d[1]);
                if (v < g) g = v;
                if (g <= 0.01f) break;
            }
            return g;
        };
        auto arcPos = [&](float ang) {
            const float t = ang / 180.f;
            return sf::Vector2f(camPos.x + t * viewW_,
                                camPos.y + viewH_ * (0.08f + 0.30f * (1.f - (2.f * t - 1.f) * (2.f * t - 1.f))));
        };
        // Sol (disco + halo).
        const float sv = core::arcVisibility(core::sunAngle(h));
        if (sv > 0.01f) {
            const sf::Vector2f sp = arcPos(core::sunAngle(h));
            const float g = skyOpenArea(sp.x, sp.y, 30.f);
            if (g > 0.01f) {
                for (int i = 3; i >= 1; --i) {
                    const float r = 30.f * (1.f + i * 0.6f);
                    sf::CircleShape glow(r);
                    glow.setOrigin(r, r);
                    glow.setPosition(sp);
                    glow.setFillColor(sf::Color(255, 240, 180,
                        static_cast<sf::Uint8>(30.f * sv * g / i)));
                    window->draw(glow);
                }
                sf::CircleShape disc(30.f);
                disc.setOrigin(30.f, 30.f);
                disc.setPosition(sp);
                disc.setFillColor(sf::Color(255, 230, 140,
                    static_cast<sf::Uint8>(255.f * sv * g)));
                window->draw(disc);
            }
        }
        // Lua (disco + cratera fixa).
        const float mv = core::arcVisibility(core::moonAngle(h));
        if (mv > 0.01f) {
            const sf::Vector2f mp = arcPos(core::moonAngle(h));
            const float g = skyOpenArea(mp.x, mp.y, 22.f);
            if (g > 0.01f) {
                const sf::Uint8 a = static_cast<sf::Uint8>(255.f * mv * g);
                sf::CircleShape disc(22.f);
                disc.setOrigin(22.f, 22.f);
                disc.setPosition(mp);
                disc.setFillColor(sf::Color(220, 225, 240, a));
                window->draw(disc);
                sf::CircleShape crater(22.f * 0.22f);
                crater.setOrigin(22.f * 0.22f, 22.f * 0.22f);
                crater.setPosition(mp.x - 22.f * 0.25f, mp.y + 22.f * 0.15f);
                crater.setFillColor(sf::Color(180, 185, 200, a));
                window->draw(crater);
            }
        }
        // Estrelas (80 pts, só no escuro, twinkle; gate por estrela: as
        // atrás de rocha somem sozinhas, as do poço aberto ficam).
        if (darkness > 0.4f) {
            const float starAlpha = (darkness - 0.4f) / 0.6f * 200.f;
            sf::VertexArray stars(sf::Points, 80);
            for (int i = 0; i < 80; ++i) {
                const float sx = camPos.x + core::starOffsetX(i) * viewW_;
                const float sy = camPos.y + core::starOffsetY(i) * viewH_;
                const float tw = 0.7f + 0.3f * std::sin(
                    static_cast<float>(i) + core::Time::elapsed() * 0.8f);
                const float g = skyOpen(sx, sy);
                stars[i] = sf::Vertex({sx, sy},
                    sf::Color(255, 255, 240,
                              static_cast<sf::Uint8>(starAlpha * tw * g)));
            }
            window->draw(stars);
        }
    }

    // ─── Emissivos (ADD, após multiply: não são escurecidos) ───
    // Grid carrega a cena (player level 13, TNT item 15); overlays dão o
    // brilho legível: aura sutil do player (master 0.4, sem "ovo") +
    // faísca/blast da TNT com halo (fake bloom, item 17: 2º draw maior
    // e fraco). TNT/blast mantêm alphas próprios independentes.
    lighting_.drawPlayerLight(*window,
                              player.get()->getCenterX(),
                              player.get()->getCenterY());
    throws_->forEachActive([&](const support::Throwable &t) {
        if (t.kind != support::ThrowKind::Dynamite) return;
        if (t.fuse <= 0.f) return;
        const auto gp =
            support::tntGlowParams(t.fuse, core::Time::elapsed());
        lighting_.drawRadial(*window, t.pos, gp.radius,
                             sf::Color(
                                 static_cast<sf::Uint8>(gp.r),
                                 static_cast<sf::Uint8>(gp.g),
                                 static_cast<sf::Uint8>(gp.b),
                                 static_cast<sf::Uint8>(gp.a)));
        // Halo: mesma cor, ~2.2× o raio, 1/4 do alpha.
        lighting_.drawRadial(*window, t.pos, gp.radius * 2.2f,
                             sf::Color(
                                 static_cast<sf::Uint8>(gp.r),
                                 static_cast<sf::Uint8>(gp.g),
                                 static_cast<sf::Uint8>(gp.b),
                                 static_cast<sf::Uint8>(gp.a * 0.25f)));
    });
    throws_->renderBlasts(*window,
        [&](sf::Vector2f p, float r, sf::Color c) {
            lighting_.drawRadial(*window, p, r, c);
            // Halo do blast: mesma cor, ~2.2× o raio, 1/4 do alpha.
            lighting_.drawRadial(*window, p, r * 2.2f,
                                 sf::Color(c.r, c.g, c.b,
                                           static_cast<sf::Uint8>(c.a * 0.25f)));
        });

    overlay_.render(*window, font, *getWorld(), *player.get(), objects.size());

    // Bloom (item 21): captura a cena (sem HUD) e soma o brilho.
    // Sem shader/GL: no-op, cena intacta. HUD vem depois (não brilha).
    bloom_.endScene(*window);

    // Vinheta (item 22): após o bloom, ANTES do HUD (HUD legível).
    // Alpha máx 50 (sutil): 70 ainda pesava nos cantos. View default ativa.
    {
        if (vignetteTex_.getSize().x == 0)
            vignetteTex_.loadFromImage(core::makeVignetteImage(128));
        sf::Sprite v(vignetteTex_);
        v.setPosition(0.f, 0.f);
        v.setScale(viewW_ / 128.f, viewH_ / 128.f);
        v.setColor(sf::Color(255, 255, 255, 50));
        window->draw(v);
    }

    // HUD em espaço de tela (view default): HP, TNT, estrato, morte/pause.
    // Tosco de propósito; HUD bonito é polimento.
    {
        window->setView(window->getDefaultView());
        // Log de eventos (canal F4): últimas linhas do feed, topo-right.
        if (overlay_.visible() && overlay_.events()) {
            int row = 0;
            for (const auto &line : debugFeed_.log) {
                sf::Text t;
                t.setFont(font);
                t.setString(line);
                t.setCharacterSize(14);
                t.setFillColor(sf::Color(200, 255, 200));
                t.setOutlineColor(sf::Color::Black);
                t.setOutlineThickness(1);
                t.setPosition(viewW_ - 380.f, 16.f + row * 20.f);
                window->draw(t);
                ++row;
            }
        }
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
        text("TNT:" + std::to_string(p->inventory.count("dynamite")) + " J  K melee", 16.f, 62.f);
        text(std::string("Mat: ") + (p->loadout.equipped
                                          ? core::materialName(p->loadout.weapon)
                                          : "--"),
             16.f, 110.f);
        const int pty = static_cast<int>(std::floor(p->getY() / core::kBlockSize));
        text(std::string(support::stratumName(support::stratumAt(pty)))
             + "  y" + std::to_string(pty), 16.f, 86.f);
        {
            const float h = dayNight_.hour();
            const int hh = static_cast<int>(h);
            const int mm = static_cast<int>((h - hh) * 60);
            text(std::string("Hora ") + std::to_string(hh) + ":"
                     + (mm < 10 ? "0" : "") + std::to_string(mm),
                 16.f, 134.f);
        }

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

    // Hotbar 4a: após o HUD (view default ativa), antes do screenshot.
    hotbar_.render(*window, player.get()->inventory, activeHotbarSlot_,
                   viewW_, viewH_, font);

    // Menu Dark Souls: por cima da hotbar (view default ativa).
    // Dependências já injetadas no tick (App); aqui só desenha.
    inventoryUI_.render(*window, font, viewW_, viewH_);

    // Screenshot auto (F11): aqui, após todos os draws e antes do
    // display — o framebuffer contém exatamente este frame. No tick,
    // a captura sairia 1 frame atrasada (sem a yellow box do Active).
    // A conversão mundo→pixel usa a worldView armazenada.
    if (player.get()->meleePhase == MeleePhase::Active) {
        screenshots_.setFocus({player.get()->getCenterX(),
                               player.get()->getCenterY()});
        screenshots_.maybeCaptureMelee(player.get()->meleeSwingId);
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
    const int mGlove = mHelm; // mesmo material do elmo (sem slot próprio)

    // Peça full-width (12px) centralizada no centro-x da parte âncora.
    // Offsets em rows do sprite (nunca world): a âncora segue o Body,
    // que o BodySystem recalcula por frame via rebuildFromSprite —
    // o headOffsetRows morreu aqui (valia só p/ PunchUp).
    // Origin (0,0) + facing<0 desenha p/ esquerda: x ancora o canto
    // direito (mesma regra de equipSpritePos).
    auto drawFullWidth = [&](const sf::Texture &tex, int texW,
                             support::BodyPartId anchor, float fracY,
                             float offRows) {
        const auto *part = p->body.find(anchor);
        if (!part || part->fromSchema) return; // âncora ausente: não desenhar
        const float cx = part->worldBox.left + part->worldBox.width * 0.5f;
        const float y =
            part->worldBox.top + part->worldBox.height * fracY + offRows * s;
        sf::Sprite spr(tex);
        spr.setPosition(cx - static_cast<float>(f * texW) * 0.5f * s, y);
        spr.setScale(s * static_cast<float>(f), s);
        window->draw(spr);
    };

    // Luva centrada na mão; some se o braço está oculto no frame
    // (fromSchema: walkA/walkB/throw/punch mostram 1 braço só).
    auto drawGlove = [&](support::BodyPartId arm) {
        const auto *part = p->body.find(arm);
        if (!part || part->fromSchema) return;
        sf::Sprite spr(sprites_.gloves[mGlove]);
        spr.setOrigin(sprites::kGloveW * 0.5f,
                      static_cast<float>(sprites::kGloveH) * 0.5f);
        spr.setPosition(part->worldBox.left + part->worldBox.width * 0.5f,
                        part->worldBox.top + part->worldBox.height * 0.5f);
        spr.setScale(s * static_cast<float>(f), s);
        window->draw(spr);
    };

    // Elmo 12x5: topo 2 rows acima do topo da cabeça (idle: rows 0-4).
    drawFullWidth(sprites_.helm[mHelm], sprites::kHelmW,
                  support::BodyPartId::Head, 0.f, -2.f);
    // Peitoral 12x8: topo no topo do torso (idle: rows 6-13).
    drawFullWidth(sprites_.chest[mChest], sprites::kChestW,
                  support::BodyPartId::Torso, 0.f, 0.f);
    // Perneiras 12x6: topo 2 rows acima da base do torso (idle: rows
    // 14-19, sobrepõe a coxa sob a túnica, como no layout antigo).
    drawFullWidth(sprites_.legs[mLegs], sprites::kLegsW,
                  support::BodyPartId::Torso, 1.f, -2.f);
    // Botas 12x3: topo 1 row abaixo da base do torso (idle: rows 17-19).
    drawFullWidth(sprites_.boots[mLegs], sprites::kBootsW,
                  support::BodyPartId::Torso, 1.f, 1.f);

    drawGlove(support::BodyPartId::ArmL);
    drawGlove(support::BodyPartId::ArmR);
}

void Game::drawPlayerWeapon() {
    Player *p = player.get();
    if (run_.isDead() || !p->loadout.equipped) return;
    const int m = static_cast<int>(p->loadout.weapon);
    const float s = p->getH() / static_cast<float>(sprites::kPlayerH);
    // Mão = base do ArmR (onde o pixel de pele termina, row 9 no idle).
    // Espelho de computeWeaponBbox em BodySystem — mudar um sem o outro
    // desalinha desenho e hitbox.
    const auto *arm = p->body.find(support::BodyPartId::ArmR);
    if (!arm) return;
    const float handX = arm->worldBox.left + arm->worldBox.width * 0.5f;
    const float handY = arm->worldBox.top + arm->worldBox.height;
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
    // Direção pela rotação, não pelo flip: flip + rotação espelharia
    // errado em NW/SW. Fora do swing segue o input (aimDir); no swing,
    // o snapshot — swingAim idle congela no último golpe (lâmina
    // invertida para sempre). Tabela calibrada p/ sprite horizontal
    // (swing, lâmina p/ direita); sprite vertical (idle/windup, lâmina
    // em cima) ganha +90°. Windup é aproximado (lâmina na diagonal).
    const bool verticalSprite = (p->meleePhase == MeleePhase::Idle ||
                                 p->meleePhase == MeleePhase::Windup);
    float angle = 0.f;
    using support::AimDir;
    switch (p->effectiveAim()) {
        case AimDir::E: angle = 0.f; break;
        case AimDir::NE: angle = -45.f; break;
        case AimDir::N: angle = -90.f; break;
        case AimDir::NW: angle = -135.f; break;
        case AimDir::W: angle = 180.f; break;
        case AimDir::SW: angle = 135.f; break;
        case AimDir::S: angle = 90.f; break;
        case AimDir::SE: angle = 45.f; break;
        default: break;
    }
    if (verticalSprite) angle += 90.f;
    sf::Sprite spr;
    spr.setTexture(*tex);
    spr.setOrigin(originX, originY);
    spr.setPosition(handX, handY);
    spr.setScale(s, s);
    spr.setRotation(angle);
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
