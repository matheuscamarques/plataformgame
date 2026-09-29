/**
 * @file tests/test_animplayer.cpp
 * @brief Teste headless da Fase C: AnimPlayer + clips de ataque.
 * @details Puro, sem GL. Roda com make test em build/tests/test_animplayer.
 */
#include <cassert>
#include <cstdio>
#include "assets/PlayerClips.h"
#include "assets/PlayerSprite.h"
#include "entities/Player/Player.h"
#include "support/Combat/AnimPlayer.h"

int main() {
    using support::AnimClip;
    using support::AnimKeyframe;
    using support::AnimPlayer;
    using support::SpriteFrameId;
    using support::AnimEvent::Hitbox;
    using support::AnimEvent::Shake;
    using support::AnimEvent::Sfx;
    using support::AnimEvent::Step;

    { // TimeDrivenLoop (0.1s/frame: avança, dá a volta, emite borda)
        static constexpr AnimKeyframe k[] = {
            {SpriteFrameId::PlayerWalkA, 0.1f, 0},
            {SpriteFrameId::PlayerWalkB, 0.1f, Sfx},
        };
        constexpr AnimClip clip{"walk", k, 2, true};
        AnimPlayer a;
        a.play(clip);
        assert(a.currentFrame() == SpriteFrameId::PlayerWalkA);
        assert(a.consumeEvents() == 0);
        a.tick(0.05f);
        assert(a.currentFrame() == SpriteFrameId::PlayerWalkA);
        a.tick(0.06f); // 0.11: entra no B + Sfx de borda
        assert(a.currentFrame() == SpriteFrameId::PlayerWalkB);
        assert(a.consumeEvents() == Sfx);
        assert(a.consumeEvents() == 0); // drenou
        a.tick(0.1f);                   // volta ao A (loop)
        assert(a.currentFrame() == SpriteFrameId::PlayerWalkA);
        assert(!a.finished());
    }
    { // NonLoopHoldsLast (sem loop: termina e segura o último)
        static constexpr AnimKeyframe k[] = {
            {SpriteFrameId::PlayerWalkA, 0.1f, 0},
            {SpriteFrameId::PlayerWalkB, 0.1f, 0},
        };
        constexpr AnimClip clip{"once", k, 2, false};
        AnimPlayer a;
        a.play(clip);
        a.tick(10.f);
        assert(a.finished());
        assert(a.currentFrame() == SpriteFrameId::PlayerWalkB);
        a.tick(10.f); // terminado: sem avanço, sem crash
        assert(a.currentFrame() == SpriteFrameId::PlayerWalkB);
    }
    { // SameClipContinues (play repetido não reinicia; force sim)
        static constexpr AnimKeyframe k[] = {
            {SpriteFrameId::PlayerWalkA, 0.1f, 0},
            {SpriteFrameId::PlayerWalkB, 0.1f, 0},
        };
        constexpr AnimClip clip{"walk", k, 2, true};
        AnimPlayer a;
        a.play(clip);
        a.tick(0.15f); // no B
        assert(a.frameIndex() == 1);
        a.play(clip); // mesmo clip: continua
        assert(a.frameIndex() == 1);
        a.play(clip, true); // force: volta ao zero
        assert(a.frameIndex() == 0);
    }
    { // GotoFrameSemReemissao (state-driven: borda 1x por entrada)
        static constexpr AnimKeyframe k[] = {
            {SpriteFrameId::PlayerPunch, 0.f, 0},
            {SpriteFrameId::PlayerPunch, 0.f,
             Hitbox | Shake},
            {SpriteFrameId::PlayerPunch, 0.f, 0},
        };
        constexpr AnimClip clip{"atk", k, 3, false};
        AnimPlayer a;
        a.play(clip, true);
        assert(a.liveEvents() == 0); // frame 0: sem Hitbox
        a.tick(10.f);                // state-driven: tick não avança
        assert(a.frameIndex() == 0);
        a.gotoFrame(1);
        assert(a.liveEvents() == Hitbox); // nível, sem Shake
        assert(a.consumeEvents() == Shake); // borda, sem Hitbox
        a.gotoFrame(1); // mesmo frame: sem re-emissão
        assert(a.consumeEvents() == 0);
        a.gotoFrame(2);
        assert(a.liveEvents() == 0);
        assert(a.finished());
    }
    { // AttackClipForAgrupa (mesma partição do resolvePlayerSprite)
        using support::AimDir;
        assert(&game::attackClipFor(AimDir::E) == &game::attackClipSide());
        assert(&game::attackClipFor(AimDir::W) == &game::attackClipSide());
        assert(&game::attackClipFor(AimDir::N) == &game::attackClipUp());
        assert(&game::attackClipFor(AimDir::NE) == &game::attackClipUp());
        assert(&game::attackClipFor(AimDir::NW) == &game::attackClipUp());
        assert(&game::attackClipFor(AimDir::S) == &game::attackClipDown());
        assert(&game::attackClipFor(AimDir::SE) == &game::attackClipDown());
        assert(&game::attackClipFor(AimDir::SW) == &game::attackClipDown());
        // Hitbox só no frame 1, nos 3 grupos.
        const AnimClip *groups[] = {&game::attackClipSide(),
                                    &game::attackClipUp(),
                                    &game::attackClipDown()};
        for (const AnimClip *c : groups) {
            AnimPlayer a;
            a.play(*c, true);
            assert(a.liveEvents() == 0);
            a.gotoFrame(1);
            assert(a.liveEvents() == Hitbox);
            a.gotoFrame(2);
            assert(a.liveEvents() == 0);
        }
    }
    { // SwooshNaBorda (Fase F: frame 1 emite Sfx 1x, sem repetir)
        AnimPlayer a;
        a.play(game::attackClipSide(), true);
        assert(a.consumeEvents() == 0); // Windup: silencioso
        a.gotoFrame(1);                 // Active: swoosh
        assert(a.consumeEvents() == Sfx);
        assert(a.consumeEvents() == 0); // drenou, sem eco
        a.gotoFrame(2);
        assert(a.consumeEvents() == 0);
    }
    { // SwingTocaClipESincroniza (Player: startSwing→play, fases→goto)
        Player p;
        assert(p.startSwing()); // aim E: clip side
        assert(p.anim.playing());
        assert(p.anim.currentFrame() == SpriteFrameId::PlayerPunch);
        assert(p.anim.liveEvents() == 0); // Windup: sem hitbox
        assert(p.updateMelee(0.10f) == MeleePhase::Active);
        assert(p.anim.liveEvents() == Hitbox);
        assert(p.updateMelee(0.10f) == MeleePhase::Recovery);
        assert(p.anim.liveEvents() == 0);
    }
    { // ClipSegueSwingAim (mira N: frames PunchUp no player)
        Player p;
        p.aimDir = support::AimDir::N;
        assert(p.startSwing());
        assert(p.anim.currentFrame() == SpriteFrameId::PlayerPunchUp);
    }
    { // WalkClipData (A/B/C/D em 0.10s, loop, Step só nos contacts)
        AnimPlayer a;
        a.play(game::walkClip());
        assert(a.currentFrame() == SpriteFrameId::PlayerWalkA);
        assert(a.consumeEvents() == 0); // passing: sem poeira
        a.tick(0.11f);                  // contact B
        assert(a.currentFrame() == SpriteFrameId::PlayerWalkB);
        assert(a.consumeEvents() == Step);
        a.tick(0.10f); // passing C: sem Step
        assert(a.currentFrame() == SpriteFrameId::PlayerWalkC);
        assert(a.consumeEvents() == 0);
        a.tick(0.10f); // contact D
        assert(a.currentFrame() == SpriteFrameId::PlayerWalkD);
        assert(a.consumeEvents() == Step);
        a.tick(0.10f); // loop de volta ao A
        assert(a.currentFrame() == SpriteFrameId::PlayerWalkA);
        assert(!a.finished());
    }
    { // IdleClipData (1 frame parado, sem eventos, sem fim)
        AnimPlayer a;
        a.play(game::idleClip());
        assert(a.currentFrame() == SpriteFrameId::PlayerIdle);
        assert(a.consumeEvents() == 0);
        a.tick(10.f);
        assert(a.currentFrame() == SpriteFrameId::PlayerIdle);
        assert(!a.finished());
    }
    { // ApplyLocoFrame (zona de marcha usa o clip; combate passa)
        using support::SpriteFrameId;
        using game::applyLocoFrame;
        assert(applyLocoFrame(SpriteFrameId::PlayerWalkA,
                              SpriteFrameId::PlayerWalkB) ==
               SpriteFrameId::PlayerWalkB);
        assert(applyLocoFrame(SpriteFrameId::PlayerWalkB,
                              SpriteFrameId::PlayerWalkA) ==
               SpriteFrameId::PlayerWalkA);
        assert(applyLocoFrame(SpriteFrameId::PlayerIdle,
                              SpriteFrameId::PlayerWalkA) ==
               SpriteFrameId::PlayerWalkA);
        assert(applyLocoFrame(SpriteFrameId::PlayerPunch,
                              SpriteFrameId::PlayerWalkA) ==
               SpriteFrameId::PlayerPunch);
        assert(applyLocoFrame(SpriteFrameId::PlayerJump,
                              SpriteFrameId::PlayerIdle) ==
               SpriteFrameId::PlayerJump);
        assert(applyLocoFrame(SpriteFrameId::PlayerHurt,
                              SpriteFrameId::PlayerWalkB) ==
               SpriteFrameId::PlayerHurt);
    }

    std::printf("animplayer test OK\n");
    return 0;
}
