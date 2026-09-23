/**
 * @file src/support/Enemies/Barks.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define falas do anão e seleção determinística por salt.
 * @details Expõe enum BarkId e singleton BarkRegistry com pick, usado por DwarfAI para alertas, ataque e traição.
 */

#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace support {

// Falas do anão (texto; áudio futuro). pick determinístico por salt.
enum class BarkId : uint8_t {
    Alert,
    Attack,
    Hurt,
    Death,
    Warning1,
    Warning2,
    Warning3,
    Betrayal,
    COUNT
};

class BarkRegistry {
public:
    static BarkRegistry &instance() {
        static BarkRegistry r;
        return r;
    }

    const std::string &pick(BarkId id, uint32_t salt) const {
        auto it = byId_.find(id);
        if (it == byId_.end() || it->second.empty()) return empty_;
        return it->second[salt % it->second.size()];
    }

private:
    BarkRegistry() { init(); }

    void init() {
        byId_[BarkId::Alert] = {"Sai da minha mina!", "Intruso!"};
        byId_[BarkId::Attack] = {"Toma!", "Morre!"};
        byId_[BarkId::Hurt] = {"Ah!", "Ai!"};
        byId_[BarkId::Death] = {"Minha... mina...", "Eu... volto..."};
        byId_[BarkId::Warning1] = {"Ei.", "Ta quente aqui."};
        byId_[BarkId::Warning2] = {"Para.", "Isso e meu."};
        byId_[BarkId::Warning3] = {"ACABOU.", "MORRE."};
        byId_[BarkId::Betrayal] = {"EU CONFIEI EM VOCE!"};
    }

    std::unordered_map<BarkId, std::vector<std::string>> byId_;
    std::string empty_;
};

} // namespace support
