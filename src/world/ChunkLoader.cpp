/**
 * @file src/world/ChunkLoader.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Executa fila de geração de chunks em thread auxiliar.
 * @details Implementa start, stop, request com dedup e tryTake com filas mutexadas, roda Builder puro em loop, consumido por ChunkManager updateAsync e World.
 */

#include "world/ChunkLoader.h"
#include "world/Chunk.h" // completo aqui: fila/move/destrói unique_ptr

namespace support {

ChunkLoader::ChunkLoader(Builder build) : build_(std::move(build)) {}

ChunkLoader::~ChunkLoader() { stop(); }

void ChunkLoader::start() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (started_) return;
    started_ = true;
    stop_ = false;
    worker_ = std::thread(&ChunkLoader::loop, this);
}

void ChunkLoader::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!started_) return;
        stop_ = true;
    }
    cv_.notify_all();
    if (worker_.joinable()) worker_.join();
    std::lock_guard<std::mutex> lock(mutex_);
    started_ = false;
}

bool ChunkLoader::running() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return started_ && !stop_;
}

void ChunkLoader::request(int cx, int cy) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!started_ || stop_) return;
    const auto key = std::make_pair(cx, cy);
    if (!queued_.insert(key).second) return; // já pedido/em voo
    pending_.push(key);
    cv_.notify_one();
}

std::unique_ptr<Chunk> ChunkLoader::tryTake() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (ready_.empty()) return nullptr;
    auto c = std::move(ready_.front());
    ready_.pop();
    return c;
}

std::size_t ChunkLoader::pending() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pending_.size() + ready_.size();
}

void ChunkLoader::loop() {
    for (;;) {
        std::pair<int, int> job{0, 0};
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [&] { return stop_ || !pending_.empty(); });
            if (stop_) return;
            job = pending_.front();
            pending_.pop();
        }
        // Fora do lock: construção nua (só estado por-chunk).
        auto c = build_(job.first, job.second);
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stop_) return; // descarta: shutting down
            queued_.erase(job);
            if (c) ready_.push(std::move(c));
        }
    }
}

} // namespace support
