#pragma once
#include <array>
#include <cstdint>

namespace core {

// Atributos estilo Dark Souls (F2). Puro (sem SFML, sem Player).
// 8 atributos base 10; cada ponto comprado +1 nível. Custo DS1 verbatim
// (duas faixas cúbicas); cap 99 por atributo. Derivados (hpMax,
// stamina, scaling) moram nas fases F3/F6 — aqui só moeda e pontos.
enum class Attr : uint8_t {
    Vitality,    // HP máximo
    Attunement,  // espaços de magia (F8) + limiar de status (F7)
    Endurance,   // estamina + carga máxima
    Strength,    // armas pesadas (scaling + req)
    Dexterity,   // armas leves (scaling + req)
    Resistance,  // defesa física + limiar de status (útil, ≠ DS1)
    Intelligence, // feitiçarias (F8) + mitigação elemental
    Faith,        // milagres (F8) + mitigação elemental
    COUNT
};

inline constexpr int kAttrCount = static_cast<int>(Attr::COUNT);

inline const char* attrName(Attr a) {
    static constexpr const char* kNames[] = {
        "Vitalidade", "Conhecimento", "Fortitude", "Força",
        "Destreza", "Resistência", "Inteligência", "Fé",
    };
    static_assert(sizeof(kNames) / sizeof(kNames[0]) ==
                      static_cast<std::size_t>(kAttrCount),
                  "attrName: tabela fora de sincronia com o enum");
    const int i = static_cast<int>(a);
    if (i < 0 || i >= kAttrCount) return "?";
    return kNames[i];
}

// Bônus de arma por atributo, letra S (melhor) a E (pior).
// Multiplicadores DS adaptados; "-" (None) = sem bônus.
enum class ScaleGrade : uint8_t { None, E, D, C, B, A, S, COUNT };

inline constexpr int kScaleGradeCount = static_cast<int>(ScaleGrade::COUNT);

inline float scaleMult(ScaleGrade s) {
    static constexpr float kMults[] = {
        0.f, 0.1f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f, // None,E,D,C,B,A,S
    };
    static_assert(sizeof(kMults) / sizeof(kMults[0]) ==
                      static_cast<std::size_t>(kScaleGradeCount),
                  "scaleMult: tabela fora de sincronia com o enum");
    const int i = static_cast<int>(s);
    if (i < 0 || i >= kScaleGradeCount) return 0.f;
    return kMults[i];
}

inline char scaleLetter(ScaleGrade s) {
    static constexpr char kLetters[] = {
        '-', 'E', 'D', 'C', 'B', 'A', 'S',
    };
    static_assert(sizeof(kLetters) / sizeof(kLetters[0]) ==
                      static_cast<std::size_t>(kScaleGradeCount),
                  "scaleLetter: tabela fora de sincronia com o enum");
    const int i = static_cast<int>(s);
    if (i < 0 || i >= kScaleGradeCount) return '-';
    return kLetters[i];
}

// Fator do atributo no scaling: 0 na base 10, 1.0 aos 30 (soft cap;
// além, +0.25/ponto com a rampa antiga). Base 10 contribui zero = seed
// não muda dano. Rampa íngreme cedo (20 já dá metade) p/ investimento
// baixo ser perceptível; teto igual ao antigo em 40+.
inline float scaleFactor(int attr) {
    if (attr <= 10) return 0.f;
    if (attr >= 40) return 1.f + (attr - 40) * 0.25f / 30.f;
    return (static_cast<float>(attr) - 10.f) / 20.f;
}

struct Attributes {
    static constexpr int kBase = 10;
    static constexpr int kMax = 99;

    // Custo em souls p/ sair do nível atual (fórmula DS1, n = nível atual;
    // ex.: costForLevel(50) = 14535 p/ 50→51).
    static int costForLevel(int level) {
        const double n = level < 1 ? 1.0 : static_cast<double>(level);
        double c;
        if (n <= 12.0)
            c = 0.0068 * n * n * n - 0.06 * n * n + 17.1 * n + 639.0;
        else
            c = 0.02 * n * n * n + 3.06 * n * n + 105.6 * n - 895.0;
        return static_cast<int>(c);
    }

    int get(Attr a) const { return v_[static_cast<int>(a)]; }

    // Escrita direta com clamp (criação de personagem; buy é a via normal).
    void set(Attr a, int v) {
        if (v < 1) v = 1;
        if (v > kMax) v = kMax;
        v_[static_cast<int>(a)] = v;
    }

    int spent() const {
        int total = 0;
        for (int x : v_) total += x - kBase;
        return total;
    }

    int level() const { return 1 + spent(); }

    // Compra 1 ponto com souls (atômico: sem souls ou no cap = false).
    bool buy(Attr a, int& souls) {
        int& slot = v_[static_cast<int>(a)];
        if (slot >= kMax) return false;
        const int cost = costForLevel(level());
        if (souls < cost) return false;
        souls -= cost;
        ++slot;
        return true;
    }

    // Derivados (F3): VIT 10 = 100 (base de gente, não de deus).
    static int maxHP(int vit) {
        if (vit < 1) vit = 1;
        if (vit <= 40) return 80 + vit * 2;
        return 80 + 40 * 2 + (vit - 40) / 2; // soft cap 40
    }
    static int maxStamina(int end) {
        if (end < 1) end = 1;
        return 100 + end * 5;
    }
    static float maxLoad(int end) {
        if (end < 1) end = 1;
        return 60.f + (end - kBase) * 2.f; // END 10 = 60 (atual)
    }
    // Limiar de acúmulo poison/bleed (F7): RES conta 5, ATT conta 2
    // (ATT sem magia ainda serve p/ algo; RES nunca é ponto morto).
    static float statusThreshold(int res, int att) {
        if (res < 1) res = 1;
        if (att < 1) att = 1;
        return 100.f + res * 5.f + att * 2.f; // base 10/10 = 170
    }
    // Espaços de sintonia (F8): limiares DS (12, 18, ...), teto 8.
    static int spellSlots(int att) {
        static constexpr int kMarks[] = {12, 18, 24, 32, 40, 50, 62, 76};
        int slots = 0;
        for (int m : kMarks)
            if (att >= m) ++slots;
        return slots;
    }

private:
    std::array<int, kAttrCount> v_{kBase, kBase, kBase, kBase,
                                   kBase, kBase, kBase, kBase};
};

} // namespace core
