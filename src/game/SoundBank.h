/**
 * @file src/game/SoundBank.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define e constrói 26 efeitos sonoros sintetizados.
 * @details Declara enum Sfx mais keyOf e buildSoundBank que sintetiza buffers via Synth e registra no AudioSystem no boot, usada por combate, mundo e UI.
 */

#pragma once
#include "core/AudioSystem.h"
#include "core/Synth.h"
#include <cstdint>
#include <memory>

// Banco de SFX: 26 sons sintetizados em runtime (zero asset).
// Ajuste = 1 número (freq, duração, volume). Roll entra sem call site
// (mecânica não existe ainda) — documentado onde a fiação lista sites.
namespace game {

enum class Sfx : uint8_t {
    // Player
    PlayerJump, PlayerLand, PlayerRoll, PlayerHurt, PlayerDeath,
    // Combate
    MeleeSwing, MeleeHit,
    // Dynamite
    ThrowDyn, DynFuse, Explosion,
    // Mundo
    TileBreak, XpCollect, Checkpoint,
    // Slime
    SlimeBite, SlimeHurt, SlimeDeath,
    // Dwarf
    DwarfAlert, DwarfThrow, DwarfMelee, DwarfHurt, DwarfDeath,
    DwarfWarn1, DwarfWarn2, DwarfWarn3, DwarfBetray,
    // UI
    UiSelect, UiMove, UiConfirm, UiCancel, UiEquip, UiDrop,
    COUNT
};

inline const char* keyOf(Sfx s) {
    switch (s) {
        case Sfx::PlayerJump:    return "player_jump";
        case Sfx::PlayerLand:    return "player_land";
        case Sfx::PlayerRoll:    return "player_roll";
        case Sfx::PlayerHurt:    return "player_hurt";
        case Sfx::PlayerDeath:   return "player_death";
        case Sfx::MeleeSwing:    return "melee_swing";
        case Sfx::MeleeHit:      return "melee_hit";
        case Sfx::ThrowDyn:      return "throw_dyn";
        case Sfx::DynFuse:       return "dyn_fuse";
        case Sfx::Explosion:     return "explosion";
        case Sfx::TileBreak:     return "tile_break";
        case Sfx::XpCollect:     return "xp_collect";
        case Sfx::Checkpoint:    return "checkpoint";
        case Sfx::SlimeBite:     return "slime_bite";
        case Sfx::SlimeHurt:     return "slime_hurt";
        case Sfx::SlimeDeath:    return "slime_death";
        case Sfx::DwarfAlert:    return "dwarf_alert";
        case Sfx::DwarfThrow:    return "dwarf_throw";
        case Sfx::DwarfMelee:    return "dwarf_melee";
        case Sfx::DwarfHurt:     return "dwarf_hurt";
        case Sfx::DwarfDeath:    return "dwarf_death";
        case Sfx::DwarfWarn1:    return "dwarf_warn1";
        case Sfx::DwarfWarn2:    return "dwarf_warn2";
        case Sfx::DwarfWarn3:    return "dwarf_warn3";
        case Sfx::DwarfBetray:   return "dwarf_betray";
        case Sfx::UiSelect:      return "ui_select";
        case Sfx::UiMove:        return "ui_move";
        case Sfx::UiConfirm:     return "ui_confirm";
        case Sfx::UiCancel:      return "ui_cancel";
        case Sfx::UiEquip:       return "ui_equip";
        case Sfx::UiDrop:        return "ui_drop";
        default:                 return "?";
    }
}

inline void buildSoundBank(core::AudioSystem& a) {
    using core::synth;
    using core::Note;
    using core::Wave;
    using core::ADSR;
    using Buf = std::unique_ptr<sf::SoundBuffer>;

    // Lambdas retornam unique_ptr (SoundBuffer não copia): synth com out-param.
    auto sq  = [](float f, float d, float v, ADSR e = {}) -> Buf {
        auto b = std::make_unique<sf::SoundBuffer>();
        synth(d + e.release, {{f, 0.f, d, 1.f}}, Wave::Square, *b, e, v);
        return b;
    };
    auto swp = [](float f0, float f1, float d, float v, ADSR e = {}) -> Buf {
        constexpr int kSteps = 12;
        std::vector<Note> notes;
        for (int i = 0; i < kSteps; ++i) {
            float u = float(i) / (kSteps - 1);
            float f = f0 + (f1 - f0) * u;
            notes.push_back({f, u * d, d / kSteps + 0.01f, 1.f});
        }
        auto b = std::make_unique<sf::SoundBuffer>();
        synth(d + 0.05f, notes, Wave::Square, *b, e, v);
        return b;
    };
    auto nz  = [](float d, float v, ADSR e = {}, uint32_t seed = 1) -> Buf {
        auto b = std::make_unique<sf::SoundBuffer>();
        synth(d + e.release, {{1.f, 0.f, d, 1.f}}, Wave::Noise, *b, e, v, seed);
        return b;
    };
    auto tone = [](const std::vector<Note>& notes, float d, Wave w, ADSR e, float v) -> Buf {
        auto b = std::make_unique<sf::SoundBuffer>();
        synth(d, notes, w, *b, e, v);
        return b;
    };

    // ── Player ──
    // Jump: triangle curto + harmônico sutil (square 280→520 era afiado
    // demais a 2 pulos/s). Se ainda irritar: sq(360, 0.05, 0.20).
    a.registerSound("player_jump",
        tone({{300.f, 0.f, 0.05f, 1.f}, {400.f, 0.02f, 0.05f, 0.5f}},
             0.07f, Wave::Triangle, {0.003f,0.02f,0.4f,0.03f}, 0.22f));
    a.registerSound("player_land",
        nz(0.08f, 0.40f, {0.001f,0.02f,0.2f,0.05f}, 0xA1));
    a.registerSound("player_roll",
        nz(0.18f, 0.30f, {0.01f,0.05f,0.4f,0.10f}, 0xA2));
    a.registerSound("player_hurt",
        sq(180.f, 0.10f, 0.35f, {0.002f,0.03f,0.4f,0.05f}));
    a.registerSound("player_death",
        swp(400.f, 80.f, 0.50f, 0.50f, {0.01f,0.1f,0.6f,0.3f}));

    // ── Combate ──
    a.registerSound("melee_swing",
        nz(0.10f, 0.30f, {0.001f,0.02f,0.1f,0.06f}, 0xA3));
    a.registerSound("melee_hit",
        sq(320.f, 0.06f, 0.45f, {0.001f,0.01f,0.3f,0.03f}));

    // ── Dynamite ──
    a.registerSound("throw_dyn",
        nz(0.10f, 0.25f, {0.01f,0.03f,0.2f,0.05f}, 0xA4));
    a.registerSound("dyn_fuse",
        nz(0.70f, 0.20f, {0.05f,0.1f,0.6f,0.1f}, 0xA5));
    // Explosão em 2 camadas (crack + rumble). Se faltar peso: crackTime
    // 0.12, graves 55→45/28→22. Se faltar variação: duration por tipo.
    {
        auto b = std::make_unique<sf::SoundBuffer>();
        core::explosionSound(*b, 0.7f, 0.08f, 0.75f);
        a.registerSound("explosion", std::move(b));
    }

    // ── Mundo ──
    a.registerSound("tile_break",
        nz(0.10f, 0.35f, {0.001f,0.02f,0.3f,0.05f}, 0xA6));
    a.registerSound("xp_collect",
        tone({{880.f, 0.f, 0.10f, 1.f}}, 0.10f, Wave::Sine,
             {0.005f,0.03f,0.5f,0.05f}, 0.30f));
    a.registerSound("checkpoint",
        sq(440.f, 0.30f, 0.30f, {0.01f,0.05f,0.6f,0.15f}));

    // ── Slime ──
    a.registerSound("slime_bite",
        sq(140.f, 0.08f, 0.30f, {0.005f,0.02f,0.3f,0.04f}));
    a.registerSound("slime_hurt",
        sq(200.f, 0.06f, 0.25f, {0.001f,0.01f,0.2f,0.03f}));
    a.registerSound("slime_death",
        swp(300.f, 100.f, 0.25f, 0.35f, {0.01f,0.05f,0.4f,0.15f}));

    // ── Dwarf ──
    a.registerSound("dwarf_alert",
        sq(160.f, 0.15f, 0.40f, {0.005f,0.04f,0.5f,0.08f}));
    a.registerSound("dwarf_throw",
        sq(180.f, 0.12f, 0.35f, {0.005f,0.03f,0.4f,0.06f}));
    a.registerSound("dwarf_melee",
        nz(0.10f, 0.35f, {0.001f,0.02f,0.3f,0.06f}, 0xA7));
    a.registerSound("dwarf_hurt",
        sq(150.f, 0.08f, 0.30f, {0.002f,0.02f,0.3f,0.04f}));
    a.registerSound("dwarf_death",
        swp(220.f, 60.f, 0.45f, 0.40f, {0.01f,0.05f,0.5f,0.25f}));

    // Warnings — pitch sobe com gravidade
    a.registerSound("dwarf_warn1",
        sq(220.f, 0.20f, 0.30f, {0.01f,0.05f,0.5f,0.10f}));
    a.registerSound("dwarf_warn2",
        sq(280.f, 0.25f, 0.35f, {0.01f,0.05f,0.6f,0.12f}));
    a.registerSound("dwarf_warn3",
        sq(360.f, 0.30f, 0.45f, {0.005f,0.05f,0.7f,0.15f}));
    a.registerSound("dwarf_betray",
        tone({{90.f, 0.f, 0.80f, 0.7f}, {45.f, 0.f, 0.80f, 0.3f}},
             0.80f, Wave::Sine, {0.01f,0.1f,0.7f,0.4f}, 0.60f));

    // ── UI ──
    a.registerSound("ui_select",
        tone({{660.f, 0.f, 0.06f, 1.f}}, 0.06f, Wave::Sine,
             {0.002f,0.01f,0.3f,0.03f}, 0.30f));
    a.registerSound("ui_move",
        tone({{440.f, 0.f, 0.04f, 1.f}}, 0.04f, Wave::Sine,
             {0.002f,0.005f,0.3f,0.02f}, 0.22f));
    a.registerSound("ui_confirm",
        tone({{660.f, 0.f, 0.05f, 1.f}, {880.f, 0.05f, 0.08f, 1.f}},
             0.13f, Wave::Sine,
             {0.002f,0.01f,0.3f,0.03f}, 0.30f));
    a.registerSound("ui_cancel",
        tone({{330.f, 0.f, 0.06f, 1.f}, {220.f, 0.06f, 0.09f, 1.f}},
             0.15f, Wave::Sine,
             {0.002f,0.01f,0.3f,0.04f}, 0.28f));
    a.registerSound("ui_equip",
        tone({{520.f, 0.f, 0.05f, 1.f}, {780.f, 0.05f, 0.06f, 1.f}},
             0.11f, Wave::Square,
             {0.002f,0.008f,0.25f,0.03f}, 0.22f));
    a.registerSound("ui_drop",
        tone({{180.f, 0.f, 0.09f, 1.f}, {120.f, 0.09f, 0.08f, 1.f}},
             0.17f, Wave::Sine,
             {0.003f,0.01f,0.4f,0.05f}, 0.30f));
}

} // namespace game
