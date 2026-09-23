/**
 * @file src/core/Pool.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Pool genérico reutilizável sem alocação em caminho quente.
 * @details Gerencia slots ativos com acquire e release e iteração segura, usado por ThrowSystem, efeitos e projéteis.
 */

#pragma once
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace core {

// Pool sem new/delete em hot path. Sem heap por acquire/release.
// Contrato de iteração:
//   SEGURO: release() durante forEachActive (só muda flag).
//   INSEGURO: acquire() durante forEachActive (realloc invalida tudo).
// Usa vector<uint8_t>, nunca vector<bool> (bit-packed é lento em hot path).
template <typename T>
class Pool {
public:
    // capacity > 0: teto rígido (acquire retorna nullptr quando cheio,
    // excesso é descartado). capacity 0: cresce sem limite (evite em jogo).
    explicit Pool(std::size_t capacity = 0)
        : maxSize_(capacity ? capacity : SIZE_MAX) {
        if (capacity) reserve(capacity);
    }

    void reserve(std::size_t capacity) {
        items_.reserve(capacity);
        active_.reserve(capacity);
    }

    T* acquire() {
        for (std::size_t i = 0; i < items_.size(); ++i) {
            if (!active_[i]) {
                active_[i] = 1;
                items_[i] = T{};
                return &items_[i];
            }
        }
        if (items_.size() >= maxSize_) return nullptr; // cheio: descarta
        items_.emplace_back();
        active_.push_back(1);
        return &items_.back();
    }

    void release(T* p) {
        if (!p) return;
        std::size_t idx = static_cast<std::size_t>(p - items_.data());
        if (idx < active_.size()) active_[idx] = 0;
    }

    void releaseAll() {
        std::fill(active_.begin(), active_.end(), uint8_t{0});
    }

    // SEGURO: release() dentro do loop. INSEGURO: acquire() dentro.
    template <typename F>
    void forEachActive(F&& fn) {
        for (std::size_t i = 0; i < items_.size(); ++i)
            if (active_[i]) fn(items_[i]);
    }

    std::size_t capacity()    const { return items_.size(); }
    std::size_t activeCount() const {
        std::size_t n = 0;
        for (auto b : active_) if (b) ++n;
        return n;
    }

private:
    std::vector<T>       items_;
    std::vector<uint8_t> active_;
    std::size_t          maxSize_ = SIZE_MAX;
};

} // namespace core
