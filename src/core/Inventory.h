#pragma once
#include <array>

#include "core/Item.h"

namespace core {

// Inventário slot-based com stacking (Terraria/Minecraft, não Tetris).
// Puro (sem SFML, sem Player): headless-safe. Capacidade fixa;
// add() retorna a sobra em vez de descartar.
class Inventory {
public:
    static constexpr int kCapacity = 40; // 5 fileiras × 8 colunas

    // Adiciona: preenche stacks existentes, depois slots vazios.
    // Retorna quanto SOBROU (0 = coube tudo). Def desconhecido = recusa.
    int add(const Item& item);

    // Remove quantity (tudo ou nada: verifica antes — atômico).
    bool remove(const std::string& defId, int quantity = 1);
    int  count(const std::string& defId) const;

    Item&       slot(int index)             { return slots_[index]; }
    const Item& slot(int index) const       { return slots_[index]; }
    bool        isEmpty(int index) const    { return slots_[index].isEmpty(); }
    int         usedSlots() const;

    void swap(int a, int b);

    // Reordena por tipo, depois por id (Arrange). Determinístico.
    void sort();

    int  gold() const { return gold_; }
    void addGold(int amount) { gold_ += amount; }

private:
    std::array<Item, kCapacity> slots_;
    int gold_ = 0;
};

} // namespace core
