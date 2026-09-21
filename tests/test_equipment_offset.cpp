#include <cassert>
#include <cmath>
#include <cstdio>

#include "assets/EquipmentLayout.h"
#include "support/Combat/Body.h"

namespace {
bool near(float a, float b) { return std::fabs(a - b) < 0.01f; }

support::Body makeBody() {
    support::Body b;
    support::BodySchema s = support::BodySchema::humanoid(50.f, 50.f);
    b.attach(&s);
    b.rebuild({100.f, 100.f}, 1);
    return b;
}
} // namespace

int main() {
    using namespace support;

    { // OffsetScalesWithWorldScale (mesmo spriteOffset, s=2.5 vs 5.0: dobra)
        Body b = makeBody();
        const PartState *torso = b.find(BodyPartId::Torso);
        assert(torso != nullptr);
        const float ax = torso->worldBox.left + torso->worldBox.width * 0.5f;
        const float ay = torso->worldBox.top + torso->worldBox.height * 0.5f;

        game::PieceDraw pd{nullptr, {0.f, 0.f}, BodyPartId::Torso, {0.f, 4.0f}};
        sf::Vector2f p25 = game::pieceDrawPos(pd, b, 1, 2.5f);
        sf::Vector2f p50 = game::pieceDrawPos(pd, b, 1, 5.0f);
        assert(near(p25.x, ax) && near(p50.x, ax)); // X sem offset: igual
        assert(near(p25.y, ay + 10.f));             // 4.0 * 2.5
        assert(near(p50.y, ay + 20.f));             // 4.0 * 5.0: dobrou
    }
    { // FacingMirrorsOffsetX (facing -1 espelha o X do offset)
        Body b = makeBody();
        game::PieceDraw pd{nullptr, {0.f, 0.f}, BodyPartId::Torso, {2.f, 0.f}};
        sf::Vector2f pr = game::pieceDrawPos(pd, b, 1, 2.5f);
        sf::Vector2f pl = game::pieceDrawPos(pd, b, -1, 2.5f);
        const float ax = b.find(BodyPartId::Torso)->worldBox.left +
                         b.find(BodyPartId::Torso)->worldBox.width * 0.5f;
        assert(near(pr.x, ax + 5.f) && near(pl.x, ax - 5.f));
        assert(near(pr.y, pl.y)); // Y não espelha
    }
    { // MissingPartDrawsAtOffsetOrigin (sem a parte: só offset, sem crash)
        Body b; // sem schema: find retorna null
        game::PieceDraw pd{nullptr, {0.f, 0.f}, BodyPartId::Head, {1.f, 2.f}};
        sf::Vector2f p = game::pieceDrawPos(pd, b, 1, 2.5f);
        assert(near(p.x, 2.5f) && near(p.y, 5.f));
    }

    std::printf("equipment offset test OK\n");
    return 0;
}
