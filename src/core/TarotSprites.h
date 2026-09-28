/**
 * @file src/core/TarotSprites.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Sprites ASCII das 78 cartas de tarô (22 Maiores + 4 naipes × 4 ranks).
 * @details Cada sprite é 12×20. Maiores têm símbolo próprio; Menores são
 * parametrizados por naipe (Wands/Cups/Swords/Pentacles) e rank visual
 * (Ás, Número, Pajem, Corte). spriteFor() mapeia TarotArcana → sprite.
 * Puro core (sem GL, sem support): testável headless via toSpriteData.
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "core/SpriteData.h"
#include "core/TarotCard.h"

namespace core {
namespace tarot_sprites {

using Pal = SpritePalEntry;

// ═══════════════════════════════════════════════════════════════════
// PALETA MESTRA (pergaminho + tinta + dourado + naipes)
// ═══════════════════════════════════════════════════════════════════

inline const Pal kFramePal[] = {
    {'.', 0x00000000, BodyPartId::None},
    {'K', 0x1A1A2EFF, BodyPartId::None},     // contorno
    {'P', 0xF5E6C8FF, BodyPartId::Torso},    // pergaminho
    {'G', 0xC9A227FF, BodyPartId::Head},     // dourado
    {'g', 0x7A5F1AFF, BodyPartId::Head},     // dourado escuro
    {'W', 0xFFFFFFFF, BodyPartId::Head},     // branco
    {'R', 0xD1483CFF, BodyPartId::Head},     // vermelho
    {'r', 0x8B2A22FF, BodyPartId::Head},     // vermelho escuro
    {'B', 0x4A7AB8FF, BodyPartId::Head},     // azul
    {'b', 0x2A4A7AFF, BodyPartId::Head},     // azul escuro
    {'Y', 0xFFE68AFF, BodyPartId::Head},     // amarelo
    {'y', 0xBFA020FF, BodyPartId::Head},     // amarelo escuro
    {'V', 0x6A4A8AFF, BodyPartId::Head},     // violeta
    {'v', 0x3A2A5AFF, BodyPartId::Head},     // violeta escuro
    {'S', 0xC0C8D0FF, BodyPartId::Head},     // aço claro
    {'s', 0x707880FF, BodyPartId::Head},     // aço escuro
    {'F', 0xE07830FF, BodyPartId::Head},     // chama
    {'f', 0x904020FF, BodyPartId::Head},     // chama escura
    {'E', 0x7A5A3AFF, BodyPartId::Head},     // terra
    {'e', 0x4A3A2AFF, BodyPartId::Head},     // terra escura
    {'C', 0x4AD0E0FF, BodyPartId::Head},     // ciano
    {'c', 0x2080A0FF, BodyPartId::Head},     // ciano escuro
    {'N', 0x0A0A1AFF, BodyPartId::None},     // preto
    {'n', 0x2A2A3AFF, BodyPartId::None},     // cinza escuro
};
inline constexpr std::size_t kFramePalCount = 24;

// ═══════════════════════════════════════════════════════════════════
// 22 ARCANOS MAIORES (12×20 cada)
// ═══════════════════════════════════════════════════════════════════

// 0 — O Louco (penhasco + pena + sol)
inline const char* const kFoolSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPPGPPPPK",
    "KPPPPGGGPPPK",
    "KPPPPPGPPPPK",
    "KPPPPPPPPPPK",
    "KPPWPPPPPPPK",
    "KPPWWPPPPPPK",
    "KPPWWWPPPPPK",
    "KPPWWWWPPPPK",
    "KPPWWWPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPnnPPPPK",
    "KPPPnnnnPPPK",
    "KPPnnnnnnPPK",
    "KPnnnnnnnnPK",
    "KnnnnnnnnnnK",
    "KnnnnnnnnnnK",
    "KnnnnnnnnnnK",
    "KKKKKKKKKKKK",
};

// I — O Mago (infinito + cajado)
inline const char* const kMagicianSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPRRPPPPK",
    "KPPPRRRRPPPK",
    "KPPRRPPRRPPK",
    "KPPRRPPRRPPK",
    "KPPPRRRRPPPK",
    "KPPPPRRPPPPK",
    "KPPPPGGPPPPK",
    "KPPPPPWPPPPK",
    "KPPPPPWPPPPK",
    "KPPPPWWWPPPK",
    "KPPPPWWWPPPK",
    "KPPPPPWPPPPK",
    "KPPPPPWPPPPK",
    "KPPPPWGWPPPK",
    "KPPPPWGWPPPK",
    "KPPPPPPPWWPK",
    "KPPPPPPPPPWK",
    "KKKKKKKKKKKK",
};

// II — A Sacerdotisa (lua crescente + véu)
inline const char* const kHighPriestessSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPPWPPPPK",
    "KPPPPWWWPPPK",
    "KPPPWPPWPPPK",
    "KPPPWPPPPPPK",
    "KPPPWPPPPPPK",
    "KPPPWPPPPPPK",
    "KPPPBPPPPPPK",
    "KPPBBBBBBPPK",
    "KPBBBBBBBBPK",
    "KPPBBBBBBPPK",
    "KPPPBPPPBPPK",
    "KPPPBPPPBPPK",
    "KPPPBPPPBPPK",
    "KPPPBPPPBPPK",
    "KPPPBPPPBPPK",
    "KPPPBPPPBPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// III — A Imperatriz (coroa + trigo)
inline const char* const kEmpressSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPGGGGGGPPK",
    "KPPGYGYGYGPK",
    "KPPPPGPGPPPK",
    "KPPPPPWPPPPK",
    "KPPPPGPPPPPK",
    "KPPPGGPPPPPK",
    "KPPPGPGPPPPK",
    "KPPPGPGPPPPK",
    "KPPPGPGPPPPK",
    "KPPGGPGGPPPK",
    "KPPPPGPPPPPK",
    "KPPPPGPPPPPK",
    "KPPPPGPPPPPK",
    "KPPPPGPPPPPK",
    "KPPPPGPPPPPK",
    "KPPPPGPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// IV — O Imperador (trono + carneiro)
inline const char* const kEmperorSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPGPPPPPGPK",
    "KPPPPGPGPPPK",
    "KPPPPRPRPPPK",
    "KPPPPRPRPPPK",
    "KPGGGGGGGGPK",
    "KPGPPPPPPGPK",
    "KPGPRRRRPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGGGGGGGGPK",
    "KPPGPPPPGPPK",
    "KPPGPPPPGPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// V — O Hierofante (chave tripla + trono)
inline const char* const kHierophantSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPPGPPPPK",
    "KPPPPGGGPPPK",
    "KPPPPPGPPPPK",
    "KPPPPPGPPPPK",
    "KPPPPGGGPPPK",
    "KPPPPPGPPPPK",
    "KPPPPPGPPPPK",
    "KPPPPGGGPPPK",
    "KPPPPPGPPPPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGGGGGGGGPK",
    "KPPGPPPPGPPK",
    "KPPGPPPPGPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// VI — Os Amantes (dois corações + sol)
inline const char* const kLoversSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPPGPPPPK",
    "KPPPPGGGPPPK",
    "KPPPPPGPPPPK",
    "KPPPPPPPPPPK",
    "KPPPRPPRPPPK",
    "KPPRRRRRRPPK",
    "KPRRRRRRRRPK",
    "KPRRRRRRRRPK",
    "KPPRRRRRRPPK",
    "KPPPRRRRPPPK",
    "KPPPPRRPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPGPPPPPK",
    "KPPPPGGGPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// VII — O Carro (roda + armadura)
inline const char* const kChariotSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPSSPPPPK",
    "KPPPSSSSPPPK",
    "KPPPSSSSPPPK",
    "KPPPSWWSPPPK",
    "KPPSSSSSSPPK",
    "KPSSSSSSSSPK",
    "KPPPPPPPPPPK",
    "KPPGPPPPPGPK",
    "KPGWGPPGWGPK",
    "KPGWGPPGWGPK",
    "KPPGPPPPPGPK",
    "KPPPPPPPPPPK",
    "KPPBPPPPPBPK",
    "KPBBBBBBBBPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// VIII — A Força (infinito + leão)
inline const char* const kStrengthSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPRRRRPPPK",
    "KPPRRRRRRPPK",
    "KPRRPPPPRRPK",
    "KPRRPPPPRRPK",
    "KPPRRRRRRPPK",
    "KPPPRRRRPPPK",
    "KPPPPPPPPPPK",
    "KPPGPPPPPGPK",
    "KPGGGGGGGGPK",
    "KPGPPPPPPGPK",
    "KPGPYPPYPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGGGGGGGGPK",
    "KPPGGGGGGPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// IX — O Eremita (lanterna)
inline const char* const kHermitSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPGPPPPPPK",
    "KPPGPPPPPPGK",
    "KPGPPPPPPPGK",
    "KPGYYYYYYPGK",
    "KPGYYYYYYPGK",
    "KPGYWWWWYPGK",
    "KPGYWNNWYPGK",
    "KPGYWNNWYPGK",
    "KPGYWWWWYPGK",
    "KPGYYYYYYPGK",
    "KPGYYYYYYPGK",
    "KPGPPPPPPPGK",
    "KPPGPPPPPGPK",
    "KPPPGPPPPGPK",
    "KPPPPGGGGPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// X — A Roda da Fortuna (roda com 4 símbolos)
inline const char* const kFortuneSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPVVPPPPK",
    "KPPPVVVVPPPK",
    "KPPVVRRVVPPK",
    "KPVVRRRRVVPK",
    "KPVRRGGRRVPK",
    "KPVRGWWGRVPK",
    "KPVRGWWGRVPK",
    "KPVRRGGRRVPK",
    "KPVVRRRRVVPK",
    "KPPVVRRVVPPK",
    "KPPPVVVVPPPK",
    "KPPPPVVPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// XI — A Justiça (balança + espada)
inline const char* const kJusticeSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPPSPPPPK",
    "KPPPPPSPPPPK",
    "KPPPPPSPPPPK",
    "KPPPPPSPPPPK",
    "KPPPPSSSPPPK",
    "KPPPPPSPPPPK",
    "KPGGGPPGGGPK",
    "KPGPGPPGPGPK",
    "KPGPGPPGPGPK",
    "KPGPGPPGPGPK",
    "KPPGPGGPGPPK",
    "KPPPGPGPGPPK",
    "KPPPPGPGPPPK",
    "KPPPPPGPPPPK",
    "KPPPPPGPPPPK",
    "KPPPPPGPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// XII — O Enforcado (figura suspensa)
inline const char* const kHangedManSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPGPPPPPPPK",
    "KPPGPPPPPPPK",
    "KPPGPPPPPPPK",
    "KPPGPPPPPPPK",
    "KPPGGPPPPPPK",
    "KPPPGGPPPPPK",
    "KPPPPGGPPPPK",
    "KPPPPPWPPPPK",
    "KPPPPWWWPPPK",
    "KPPPPWWWPPPK",
    "KPPPPPWPPPPK",
    "KPPPPWPPWPPK",
    "KPPWWPPPPWWK",
    "KPPWPPPPPPWK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// XIII — A Morte (ceifador + foice)
inline const char* const kDeathSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPNNNNNPPPK",
    "KPNNNNNNNPPK",
    "KPNNWWWNNPPK",
    "KPNWNWNWNPPK",
    "KPNWWWWWNPPK",
    "KPNNWWWNNPPK",
    "KPPNNNNNPPPK",
    "KPPPPNPPPPPK",
    "KPPPPNPPPPPK",
    "KSSSSNSSSSPK",
    "KPPPPNPPPPPK",
    "KPPPPNPPPPPK",
    "KPPPPNPPPPPK",
    "KPPPPNPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// XIV — A Temperança (duas taças + anjo)
inline const char* const kTemperanceSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPWWWPPPK",
    "KPPPWWWWWPPK",
    "KPPPPWWWPPPK",
    "KPPWWWWWWPPK",
    "KPWWWWWWWWPK",
    "KPPPPPPPPPPK",
    "KPPBPPPPPBPK",
    "KPBBBPPBBBPK",
    "KPBBBPPBBBPK",
    "KPPBPPPPPBPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// XV — O Diabo (chifres + corrente)
inline const char* const kDevilSprite[] = {
    "KKKKKKKKKKKK",
    "KPRPPPPPPPRK",
    "KRRPPPPPPRRK",
    "KPRPPPPPPPPK",
    "KPPPRRRRRPPK",
    "KPPRRRRRRPPK",
    "KPRRYRRYRRPK",
    "KPRRRRRRRRPK",
    "KPPRRRRRRPPK",
    "KPPPRRRRPPPK",
    "KPPPPPPPPPPK",
    "KPSSSSSSSSPK",
    "KPSPSSPSSPPK",
    "KPSSSSSSSSPK",
    "KPSPSSPSSPPK",
    "KPSSSSSSSSPK",
    "KPSPSSPSSPPK",
    "KPSSSSSSSSPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// XVI — A Torre (raio)
inline const char* const kTowerSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPYPPPPPK",
    "KPPPYYYPPPPK",
    "KPPYYPPYYPPK",
    "KPYYPPPPYYPK",
    "KYYPPPPPPYPK",
    "KPPPPPPPPPPK",
    "KPPnnnnnnPPK",
    "KPnnPPPPnnPK",
    "KPnPPPPPPnPK",
    "KPnPPPPPPnPK",
    "KPnPPPPPPnPK",
    "KPnPYPPYPnPK",
    "KPnPPPPPPnPK",
    "KPnPPPPPPnPK",
    "KPnPPPPPPnPK",
    "KPnPPPPPPnPK",
    "KPnnnnnnnnPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// XVII — A Estrela (estrela 8 pontas + jarro)
inline const char* const kStarSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPCSPPPK",
    "KPPPPPCPPPPK",
    "KPPPPCCCPPPK",
    "KPCCCCCCCCPK",
    "KPPPPPCPPPPK",
    "KPPPPPCSPPPK",
    "KPPPPPPPPPPK",
    "KPPBPPPPPBPK",
    "KPBBBPPBBBPK",
    "KPBBBPPBBBPK",
    "KPPBPPPPPBPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPWWWWWWPPK",
    "KPWWWWWWWWPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// XVIII — A Lua (lua + lobo)
inline const char* const kMoonSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPWWWPPPK",
    "KPPPWWWWWPPK",
    "KPPWWPPPWPPK",
    "KPPWWPPWWPPK",
    "KPPWWWWWWPPK",
    "KPPPWWWWWPPK",
    "KPPPPWWWPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPnnnnnnPPK",
    "KPnnnnnnnnPK",
    "KPnWnnnnWnPK",
    "KPnnnnnnnnPK",
    "KPnnnnnnnnPK",
    "KPPnnnnnnPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// XIX — O Sol (sol radiante)
inline const char* const kSunSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPYPPPYPPK",
    "KPYPPYYPPYPK",
    "KPPPYYYYYPPK",
    "KPPYYYYYYPPK",
    "KPYYYYYYYYPK",
    "KPYYYWWWYYPK",
    "KPYYWWWWWYPK",
    "KPYYWWWWWYPK",
    "KPYYYWWWYYPK",
    "KPYYYYYYYYPK",
    "KPPYYYYYYPPK",
    "KPPPYYYYYPPK",
    "KPYPPYYPPYPK",
    "KPPPYPPPYPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// XX — O Julgamento (trombeta + anjo)
inline const char* const kJudgmentSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPWWWPPPK",
    "KPPPWWWWWPPK",
    "KPPPPWWWPPPK",
    "KPPPPPPPPPPK",
    "KPPPPGGGPPPK",
    "KPPGGPPPPGPK",
    "KPGGPPPPPGPK",
    "KGPPPPPPPGPK",
    "KGPPPPPPPGPK",
    "KGPPPPPPPGPK",
    "KGGPPPPPGGPK",
    "KPPGGGGGPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// XXI — O Mundo (coroa de louros + figura)
inline const char* const kWorldSprite[] = {
    "KKKKKKKKKKKK",
    "KPPGGGGGGPPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPWWPPGPK",
    "KPGPWWWWPGPK",
    "KPGPPWWPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPPGGGGGGPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// ═══════════════════════════════════════════════════════════════════
// 16 SPRITES DOS MENORES (4 naipes × 4 ranks)
// ═══════════════════════════════════════════════════════════════════

// ─── WANDS (Fogo) — bastão com chama ───
inline const char* const kWandsAceSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPPFRPPPK",
    "KPPPPFRRPPPK",
    "KPPPPFRFRPPK",
    "KPPPPRFRPPPK",
    "KPPPPPEPPPPK",
    "KPPPPPEPPPPK",
    "KPPPPPEPPPPK",
    "KPPPPPEPPPPK",
    "KPPPPPEPPPPK",
    "KPPPPPEPPPPK",
    "KPPPPPEPPPPK",
    "KPPPPPEPPPPK",
    "KPPPPPEPPPPK",
    "KPPPPPEPPPPK",
    "KPPPPPEPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

inline const char* const kWandsNumberSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPFRRPPPK",
    "KPPPPRFRPPPK",
    "KPPPPEPPPPPK",
    "KPPPPEPPPPPK",
    "KPPPPEPPPPPK",
    "KPPGPPPPPGPK",
    "KPPGPPPPPGPK",
    "KPPGPPPPPGPK",
    "KPPPPEPPPPPK",
    "KPPPPEPPPPPK",
    "KPPPPEPPPPPK",
    "KPPPPEPPPPPK",
    "KPPPPEPPPPPK",
    "KPPGPPPPPGPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

inline const char* const kWandsPageSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPFFPPPPK",
    "KPPPFFFFPPPK",
    "KPPPFWPFPPPK",
    "KPPPFFFFPPPK",
    "KPPPPPPPPPPK",
    "KPPPPEEPPPPK",
    "KPPPEEEPPPPK",
    "KPPPEPPEPPPK",
    "KPPPEPPEPPPK",
    "KPPPPEPPPPPK",
    "KPPPPEPPPPPK",
    "KPPPPEPPPPPK",
    "KPPPPEPPPPPK",
    "KPPPEEEEPPPK",
    "KPPPPPPPPPPK",
    "KPPPPFRRPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

inline const char* const kWandsCourtSprite[] = {
    "KKKKKKKKKKKK",
    "KPPGPPPPPGPK",
    "KPPGGGGGGGPK",
    "KPPPGGGGGPPK",
    "KPPPFFWFFPPK",
    "KPPPFFWFFPPK",
    "KPPPPFFPPPPK",
    "KPPGGPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPGPPPPPPGPK",
    "KPPGPPPPPGPK",
    "KPPPPFRRPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

// ─── CUPS (Água) — cálice com líquido ───
inline const char* const kCupsAceSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPGGGGGGPPK",
    "KPGPPPPPPGPK",
    "KPGPBBBBPGPK",
    "KPGPBBBBPGPK",
    "KPGPBBBBPGPK",
    "KPPGPPPPGPPK",
    "KPPPGPPGPPPK",
    "KPPPPGGPPPPK",
    "KPPPPPGPPPPK",
    "KPPPPPGPPPPK",
    "KPPPPPGPPPPK",
    "KPPPPGGGPPPK",
    "KPPPGGGGPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

inline const char* const kCupsNumberSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPGGGGGPPPK",
    "KPGPPPPPGPPK",
    "KPGPBBPGPPPK",
    "KPGPBBPGPPPK",
    "KPPGPPPGPPPK",
    "KPPPGPGPPPPK",
    "KPPPPGPPPPPK",
    "KPPPPGPPPPPK",
    "KPPPGGPPPPPK",
    "KPPPPPPPPPPK",
    "KPPGPPPPPGPK",
    "KPPGPPPPPGPK",
    "KPPPPPPPPPPK",
    "KPPGPPPPPGPK",
    "KPPGPPPPPGPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

inline const char* const kCupsPageSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPWWPPPPK",
    "KPPPWWWWPPPK",
    "KPPPWBWBWPPK",
    "KPPPWWWWPPPK",
    "KPPPPWWPPPPK",
    "KPPPPPPPPPPK",
    "KPPPBPPPBPPK",
    "KPPPBBBBBPPK",
    "KPPPBBBBBPPK",
    "KPPPBBBBBPPK",
    "KPPPBPPPBPPK",
    "KPPPBPPPBPPK",
    "KPPPBPPPBPPK",
    "KPPPBPPPBPPK",
    "KPPPPPPPPPPK",
    "KPPGGGGGPPPK",
    "KPGPBBPGPPPK",
    "KKKKKKKKKKKK",
};

inline const char* const kCupsCourtSprite[] = {
    "KKKKKKKKKKKK",
    "KPPGPPPPPGPK",
    "KPPGGGGGGGPK",
    "KPPPGGGGGPPK",
    "KPPPWWWWWPPK",
    "KPPPWBWBWPPK",
    "KPPPWWWWWPPK",
    "KPPBBPPPPBPK",
    "KPBBBBBBBBPK",
    "KPBBBBBBBBPK",
    "KPBBBBBBBBPK",
    "KPBBBBBBBBPK",
    "KPBBBBBBBBPK",
    "KPBBBBBBBBPK",
    "KPBBBBBBBBPK",
    "KPBBBBBBBBPK",
    "KPPBBBBBBPPK",
    "KPPGGGGGPPPK",
    "KPGPBBPGPPPK",
    "KKKKKKKKKKKK",
};

// ─── SWORDS (Ar) — espada vertical ───
inline const char* const kSwordsAceSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPSPPPPK",
    "KPPPPSSPPPPK",
    "KPPPPSPPPPPK",
    "KPPPPSPPPPPK",
    "KPPPPSPPPPPK",
    "KPPPPSPPPPPK",
    "KPPPPSPPPPPK",
    "KPPPPSPPPPPK",
    "KPPGGGGGGGPK",
    "KGGGGGGGGGPK",
    "KPPGGGGGGGPK",
    "KPPPPEPPPPPK",
    "KPPPPEPPPPPK",
    "KPPPPEPPPPPK",
    "KPPPPEPPPPPK",
    "KPPPPEPPPPPK",
    "KPPPGGGPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

inline const char* const kSwordsNumberSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPPSPPPPK",
    "KPPPPSSPPPPK",
    "KPPPPSPPPPPK",
    "KPPPPSPPPPPK",
    "KPPPPSPPPPPK",
    "KPPGGGGGGGPK",
    "KGGGGGGGGGPK",
    "KPPPEPPPPPPK",
    "KPPPEPPPPPPK",
    "KPPPEPPPPPPK",
    "KPPPEPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPGPPPPPGPK",
    "KPPGPPPPPGPK",
    "KPPPPPPPPPPK",
    "KPPGPPPPPGPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

inline const char* const kSwordsPageSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPSWWPPPPK",
    "KPPPSSSSPPPK",
    "KPPPSWSWPPPK",
    "KPPPSSSSPPPK",
    "KPPPPSSPPPPK",
    "KPPPPPPPPPPK",
    "KPPSSSSSSPPK",
    "KPSSSSSSSSPK",
    "KPSSSSSSSSPK",
    "KPSSSSSSSSPK",
    "KPSSSSSSSSPK",
    "KPPSSSSSSPPK",
    "KPPPPSSPPPPK",
    "KPPPPSSPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPSPPPPK",
    "KPPGGGGGGPPK",
    "KKKKKKKKKKKK",
};

inline const char* const kSwordsCourtSprite[] = {
    "KKKKKKKKKKKK",
    "KPPGPPPPPGPK",
    "KPPGGGGGGGPK",
    "KPPPGGGGGPPK",
    "KPPPSSWSSPPK",
    "KPPPSWSWSPPK",
    "KPPPSSSSSPPK",
    "KPPSSSSSSPPK",
    "KPSSSSSSSSPK",
    "KPSSSSSSSSPK",
    "KPSSSSSSSSPK",
    "KPSSSSSSSSPK",
    "KPSSSSSSSSPK",
    "KPSSSSSSSSPK",
    "KPSSSSSSSSPK",
    "KPSSSSSSSSPK",
    "KPPSSSSSSPPK",
    "KPPPPPSPPPPK",
    "KPPGGGGGGPPK",
    "KKKKKKKKKKKK",
};

// ─── PENTACLES (Terra) — moeda/pentagrama ───
inline const char* const kPentaclesAceSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPGGGPPPK",
    "KPPPGGGGGPPK",
    "KPPGGGGGGGPK",
    "KPPGGWGWGGPK",
    "KPPGGWGWGGPK",
    "KPGGWWWWGPPK",
    "KPGGWGWGGGPK",
    "KPPGGWGWGGPK",
    "KPPGGGGGGGPK",
    "KPPPGGGGGPPK",
    "KPPPPGGGPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

inline const char* const kPentaclesNumberSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPGGGPPPK",
    "KPPPGGGGGPPK",
    "KPPGGWGWGGPK",
    "KPPGGWGWGGPK",
    "KPGGWWWWGPPK",
    "KPPGGWGWGGPK",
    "KPPGGGGGGGPK",
    "KPPPGGGGGPPK",
    "KPPPPGGGPPPK",
    "KPPPPPPPPPPK",
    "KPPGPPPPPGPK",
    "KPPGPPPPPGPK",
    "KPPPPPPPPPPK",
    "KPPGPPPPPGPK",
    "KPPGPPPPPGPK",
    "KPPPPPPPPPPK",
    "KPPPPPPPPPPK",
    "KKKKKKKKKKKK",
};

inline const char* const kPentaclesPageSprite[] = {
    "KKKKKKKKKKKK",
    "KPPPPPPPPPPK",
    "KPPPPEEPPPPK",
    "KPPPEEEEPPPK",
    "KPPPEWEWPPPK",
    "KPPPEEEEPPPK",
    "KPPPPEEPPPPK",
    "KPPPPPPPPPPK",
    "KPPEEEEEPPPK",
    "KPEEEEEEEEPK",
    "KPEEEEEEEEPK",
    "KPEEEEEEEEPK",
    "KPEEEEEEEEPK",
    "KPEEEEEEEEPK",
    "KPPEEEEEPPPK",
    "KPPPPEEEPPPK",
    "KPPPPPPPPPPK",
    "KPPPPGGGPPPK",
    "KPPPGGGGGPPK",
    "KKKKKKKKKKKK",
};

inline const char* const kPentaclesCourtSprite[] = {
    "KKKKKKKKKKKK",
    "KPPGPPPPPGPK",
    "KPPGGGGGGGPK",
    "KPPPGGGGGPPK",
    "KPPPEEWEEPPK",
    "KPPPEWEWEPPK",
    "KPPPEEEEEPPK",
    "KPPEEEEEPPPK",
    "KPEEEEEEEEPK",
    "KPEEEEEEEEPK",
    "KPEEEEEEEEPK",
    "KPEEEEEEEEPK",
    "KPEEEEEEEEPK",
    "KPEEEEEEEEPK",
    "KPEEEEEEEEPK",
    "KPEEEEEEEEPK",
    "KPPEEEEEPPPK",
    "KPPPPGGGPPPK",
    "KPPPGGGGGPPK",
    "KKKKKKKKKKKK",
};

// ═══════════════════════════════════════════════════════════════════
// MAPEAMENTO spriteFor(TarotArcana)
// ═══════════════════════════════════════════════════════════════════

struct SpriteRef {
    const char* const* rows;
    int w = 12;
    int h = 20;
    const Pal* pal;
    std::size_t palCount;
};

// Todos os sprites usam a mesma paleta mestra (kFramePal).
inline SpriteRef makeRef(const char* const* rows) {
    return {rows, 12, 20, kFramePal, kFramePalCount};
}

// 22 Maiores — tabela direta por índice.
inline const char* const* majorSprite(TarotArcana a) {
    switch (a) {
        case TarotArcana::Fool:           return kFoolSprite;
        case TarotArcana::Magician:       return kMagicianSprite;
        case TarotArcana::HighPriestess:  return kHighPriestessSprite;
        case TarotArcana::Empress:        return kEmpressSprite;
        case TarotArcana::Emperor:        return kEmperorSprite;
        case TarotArcana::Hierophant:     return kHierophantSprite;
        case TarotArcana::Lovers:         return kLoversSprite;
        case TarotArcana::Chariot:        return kChariotSprite;
        case TarotArcana::Strength:       return kStrengthSprite;
        case TarotArcana::Hermit:         return kHermitSprite;
        case TarotArcana::WheelOfFortune: return kFortuneSprite;
        case TarotArcana::Justice:        return kJusticeSprite;
        case TarotArcana::HangedMan:      return kHangedManSprite;
        case TarotArcana::Death:          return kDeathSprite;
        case TarotArcana::Temperance:     return kTemperanceSprite;
        case TarotArcana::Devil:          return kDevilSprite;
        case TarotArcana::Tower:          return kTowerSprite;
        case TarotArcana::Star:           return kStarSprite;
        case TarotArcana::Moon:           return kMoonSprite;
        case TarotArcana::Sun:            return kSunSprite;
        case TarotArcana::Judgment:       return kJudgmentSprite;
        case TarotArcana::World:          return kWorldSprite;
        default: return nullptr;
    }
}

// 4 naipes × 4 ranks visuais.
inline const char* const* minorSprite(int suit, int pos) {
    // pos: 0=Ás, 1-9=Número (2-10), 10=Pajem, 11-13=Corte
    const int rank = (pos == 0) ? 0
                   : (pos <= 9)  ? 1
                   : (pos == 10) ? 2
                   : 3;
    // suits: 0=Wands, 1=Cups, 2=Swords, 3=Pentacles
    static const char* const* table[4][4] = {
        {kWandsAceSprite, kWandsNumberSprite, kWandsPageSprite,
         kWandsCourtSprite},
        {kCupsAceSprite, kCupsNumberSprite, kCupsPageSprite,
         kCupsCourtSprite},
        {kSwordsAceSprite, kSwordsNumberSprite, kSwordsPageSprite,
         kSwordsCourtSprite},
        {kPentaclesAceSprite, kPentaclesNumberSprite, kPentaclesPageSprite,
         kPentaclesCourtSprite},
    };
    if (suit < 0 || suit > 3 || rank < 0 || rank > 3) return nullptr;
    return table[suit][rank];
}

// Ponto de entrada: mapeia qualquer TarotArcana para o sprite.
inline SpriteRef spriteFor(TarotArcana a) {
    if (static_cast<int>(a) <= static_cast<int>(TarotArcana::World)) {
        return makeRef(majorSprite(a));
    }
    const int idx =
        static_cast<int>(a) - static_cast<int>(TarotArcana::WandsAce);
    return makeRef(minorSprite(idx / 14, idx % 14));
}

} // namespace tarot_sprites
} // namespace core
