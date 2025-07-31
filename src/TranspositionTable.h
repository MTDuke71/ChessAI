#pragma once
#include <cstdint>
#include <mutex>
#include <vector>

struct TTEntry {
    int depth;
    int value;
    int flag; // 0 exact, 1 lowerbound, -1 upperbound
};

struct TTSlot {
    uint64_t key = 0;
    TTEntry entry{ -1, 0, 0 };
};

class TranspositionTable {
public:
    explicit TranspositionTable(size_t size = DEFAULT_SIZE)
        : table(size) {}

    void store(uint64_t key, const TTEntry& entry) {
        std::lock_guard<std::mutex> lock(mtx);
        auto& slot = table[key % table.size()];
        if (slot.entry.depth <= entry.depth) {
            slot.key = key;
            slot.entry = entry;
        }
    }

    bool probe(uint64_t key, TTEntry& entry) const {
        std::lock_guard<std::mutex> lock(mtx);
        const auto& slot = table[key % table.size()];
        if (slot.entry.depth == -1 || slot.key != key) return false;
        entry = slot.entry;
        return true;
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mtx);
        for (auto& s : table) {
            s.entry.depth = -1;
        }
    }
private:
    static constexpr size_t DEFAULT_SIZE = 1 << 20;
    std::vector<TTSlot> table;
    mutable std::mutex mtx;
};
