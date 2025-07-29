#pragma once
#include <cstdint>
#include <unordered_map>

struct TTEntry {
    int depth;
    int value;
    int flag; // 0 exact, 1 lowerbound, -1 upperbound
};

class TranspositionTable {
public:
    void store(uint64_t key, const TTEntry& entry) { table[key] = entry; }
    bool probe(uint64_t key, TTEntry& entry) const {
        auto it = table.find(key);
        if (it == table.end()) return false;
        entry = it->second;
        return true;
    }
private:
    std::unordered_map<uint64_t, TTEntry> table;
};
