/**
 * @file src/world/ChunkLoader.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Geração assíncrona opcional de chunks fora da main thread.
 * @details Declara Builder, request, tryTake e controle start e stop com GL restrito à main, incluído por ChunkManager e World como opt-in.
 */

#pragma once

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <thread>
#include <utility>

namespace support {

struct Chunk;

// Geração de chunks em worker thread (camada 6, opt-in; default: sync).
// O worker executa SÓ construção nua (tiles + entidades, CPU puro):
// relight, mapa/LRU e textura ficam na main thread (GL nunca sai dela).
// Desligado por padrão: sem start(), request/tryTake são no-ops vazios.
class ChunkLoader {
public:
    // (cx, cy) -> chunk nu. Deve ser thread-safe (só estado por-chunk).
    using Builder = std::function<std::unique_ptr<Chunk>(int cx, int cy)>;

    explicit ChunkLoader(Builder build);
    ~ChunkLoader(); // para + join (nunca com worker rodando solto)

    ChunkLoader(const ChunkLoader&) = delete;
    ChunkLoader& operator=(const ChunkLoader&) = delete;

    void start(); // idempotente
    void stop();  // idempotente, join
    bool running() const;

    // Pede geração (dedup: mesmo chunk 1× por vez). Sem start: ignora.
    void request(int cx, int cy);
    // Retira um pronto (nullptr se vazio). Main thread consome com budget.
    std::unique_ptr<Chunk> tryTake();
    std::size_t pending() const;

private:
    void loop();

    Builder build_;
    std::thread worker_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_ = false;
    bool started_ = false;
    std::set<std::pair<int, int>> queued_; // dedup (pedido ou em voo)
    std::queue<std::pair<int, int>> pending_;
    std::queue<std::unique_ptr<Chunk>> ready_;
};

} // namespace support
