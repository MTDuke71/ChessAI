#pragma once
#include <cstdint>
#include <unordered_map>
#include <mutex>

struct TTEntry {
    int depth;
    int value;
    int flag; // 0 exact, 1 lowerbound, -1 upperbound
};

class TranspositionTable {
public:
    void store(uint64_t key, const TTEntry& entry) {
        std::lock_guard<std::mutex> lock(mtx);
        table[key] = entry;
    }
    bool probe(uint64_t key, TTEntry& entry) const {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = table.find(key);
        if (it == table.end()) return false;
        entry = it->second;
        return true;
    }
    void clear() {
        std::lock_guard<std::mutex> lock(mtx);
        table.clear();
    }
private:
    std::unordered_map<uint64_t, TTEntry> table;
    mutable std::mutex mtx;
};
