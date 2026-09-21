#include <cassert>
#include <cstdio>
#include <string>

#include "core/AudioSystem.h"
#include "game/SoundBank.h"

// SFX: cobertura enum↔key + bank com 26 entradas. Sem device OpenAL em
// headless: buffers podem estar vazios — assert só presença, nunca
// conteúdo; play() nunca é chamado aqui.
int main() {
    using game::Sfx;

    { // KeysCoverEnum (todo Sfx < COUNT tem key real, sem "?")
        for (int i = 0; i < static_cast<int>(Sfx::COUNT); ++i) {
            const char* k = game::keyOf(static_cast<Sfx>(i));
            assert(k != nullptr && std::string(k) != "?");
        }
        assert(static_cast<int>(Sfx::COUNT) == 26);
    }
    { // BankHas26 (build registra 1 buffer por key)
        core::AudioSystem a;
        game::buildSoundBank(a);
        assert(a.soundCount() == 26u);
        assert(a.has("player_jump"));
        assert(a.has("melee_hit"));
        assert(a.has("explosion"));
        assert(a.has("dwarf_betray"));
        assert(a.has("ui_select"));
        assert(!a.has("nao_existe"));
    }
    { // MissingKeyIsNoOp (play sem registro não crasha, sem device)
        core::AudioSystem a; // sem sons, sem play
        a.tick();
        assert(a.soundCount() == 0u);
    }

    std::printf("sfx test OK\n");
    return 0;
}
