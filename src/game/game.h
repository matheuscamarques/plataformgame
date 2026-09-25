/**
 * @file src/game/game.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara classe central Game com todos os subsistemas.
 * @details Define membros de janela, mundo, jogador, câmera, input, overlay, scheduler, sprites, áudio, luz e UI mais métodos run, tick e render, incluída por todos os módulos.
 */

#pragma once

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include "vector"
#include "core/System.h"
#include "render/Render2D.h"
#include "render/RenderBackend.h"
#include "../support/Camera/Camera.h"
#include "../support/Debug/DebugOverlay.h"
#include "../support/Debug/DebugFeed.h"
#include "../support/Debug/ScreenshotSystem.h"
#include "../support/Input/InputMap.h"
#include "support/Progression/RunManager.h"
#include "assets/Sprites/SpriteSet.h"
#include "core/AudioSystem.h"
#include "core/Bloom.h"
#include "core/DayNightCycle.h"
#include "core/MusicSystem.h"
#include "support/Lighting/LightingSystem.h"
#include "support/Effects/SpellFX.h"
#include "support/UI/HotbarUI.h"
#include "support/UI/InventoryUI.h"

// Só ponteiros no header: definição completa mora no .cpp de cada
// sistema (game.cpp inclui). Valor continua incluído (precisa do tipo).
class Player;

namespace support {
class ChunkLoader;
class ContactDamageSystem;
class DeathSystem;
class DropSystem;
class EnemySystem;
class ExplosionSystem;
class MeleeSystem;
class SpawnSystem;
class StratumManager;
class ParticleSystem;
class ThrowSystem;
class World;
} // namespace support

class Game
{
public:
    Game();
    ~Game(); // definido em game.cpp (ChunkLoader completo só lá)
    static void main();
    void setWindow(sf::RenderWindow *window);
    void start();
    void run();
    support::World* getWorld();
    void setWorld(std::unique_ptr<support::World> world);
    Player* getPlayer();
    // Camada 6 (opt-in, default off): liga/desliga geração assíncrona de
    // chunks. Chamar após setWorld. Sem chamar: 100% síncrono (policy).
    void setAsyncChunks(bool on);
    

private:
    sf::RenderWindow *window; // emprestado: dono é Window
    std::unique_ptr<support::World> world; // dono
    std::unique_ptr<Player> player; // dono
    support::Camera camera; // valor
    support::InputMap input_; // valor
    support::DebugOverlay overlay_; // valor
    core::SystemScheduler scheduler_; // dono dos Systems
    support::EnemySystem *enemies_ = nullptr; // observa; dono é o scheduler
    support::ThrowSystem *throws_ = nullptr; // observa; dono é o scheduler
    support::ExplosionSystem *explodes_ = nullptr; // observa; dono é o scheduler
    support::DeathSystem *deaths_ = nullptr; // observa; dono é o scheduler
    support::MeleeSystem *melee_ = nullptr; // observa; dono é o scheduler
    support::StratumManager *stratum_ = nullptr; // observa; dono é o scheduler
    support::RunManager run_; // valor: gate do tick (morte/pause)
    support::DebugFeed debugFeed_; // valor: números + log (F2/F4)
    support::HotbarUI hotbar_; // valor: 5 primeiros slots (fase 4a)
    support::InventoryUI inventoryUI_; // valor: menu Dark Souls (tabs+menu)
    int activeHotbarSlot_ = 0; // 0..4 via teclas 1-5
    std::string swapToast_; // nome do item trocado (Z/X/C/V)
    float swapToastTime_ = -99.f; // fade 1.5s no HUD
    support::ScreenshotSystem screenshots_; // valor: PNGs de debug (F10-12)
    support::DropSystem *drops_ = nullptr; // observa; dono é o scheduler
    support::ParticleSystem *particles_ = nullptr; // observa; dono é o scheduler
    sprites::SpriteSet sprites_; // dono: build 1x no run (precisa de GL)
    bool spritesBuilt_ = false;
    // Handles do backend p/ frames de inimigo (Fase 3): criados sob
    // demanda por SpriteFrameId, vivos até o fim do run.
    std::unordered_map<int, render::SpriteHandle> backendHandles_;
    // Backend dono das texturas: MEMBRO (não local por frame — handles
    // morrem junto com o backend que os criou; local = sprites sumem).
    std::unique_ptr<render::Render2D> enemyBackend_;
    core::MusicSystem music_; // dono: tracks sintetizadas 1x no run (RAM)
    bool musicBuilt_ = false;
    int lastMusicStratum_ = -1;
    core::AudioSystem audio_; // dono: 26 SFX sintetizados 1x no run (RAM)
    bool sfxBuilt_ = false;
    int lastPlayerLightTileX_ = -1; // cache: raycast só ao trocar de tile
    int lastPlayerLightTileY_ = -1;
    core::DayNightCycle dayNight_; // dono: relógio dia/noite (10 min)
    support::LightingSystem lighting_; // dono: lightmap por frame (GL)
    support::SpellFX spellfx_; // dono: fx de spells (visual puro)
    core::Bloom bloom_; // dono: bright-pass + blur + additive (item 21)
    sf::Texture vignetteTex_; // dono: vinheta 128px, upscale na tela (item 22)
    // Worker de chunks (camada 6): após `world` (destrói antes dele).
    std::unique_ptr<support::ChunkLoader> chunkLoader_;
    bool charView_ = false; // F3: ASCII por char, sem textura
    int tickCount_ = 0; // p/ animação walk do anão
    bool running = false;
    void render();
    void pollEvents(); // loop SFML + edges por frame (run() chama 1x)
    void tick();
    void drawPlayerSprite();
    void drawPlayerEquipment();
    void drawPlayerWeapon();
    void drawEnemiesSprites();

    // Tamanho da viewport. Futuro: AssetManager é dono de font;
    // Game só pede por chave. Não criar AssetManager no B5.
    float viewW_ = 0.0f;
    float viewH_ = 0.0f;
    sf::Font font;
};