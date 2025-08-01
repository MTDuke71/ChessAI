#pragma once
#include <cstdint>
#include <atomic>
#include <vector>

struct TTEntry {
    int depth;
    int value;
    int flag; // 0 exact, 1 lowerbound, -1 upperbound
};

struct TTSlot {
    std::atomic<uint64_t> key{0};
    std::atomic<int> depth{-1};
    std::atomic<int> value{0};
    std::atomic<int> flag{0};
};

class TranspositionTable {
public:
    explicit TranspositionTable(size_t size = DEFAULT_SIZE)
        : table(size), usedSlots(0) {}

    size_t size() const { return table.size(); }

    size_t used() const { return usedSlots.load(std::memory_order_relaxed); }

    void store(uint64_t key, const TTEntry& entry) {
        auto& slot = table[key % table.size()];
        int curDepth = slot.depth.load(std::memory_order_relaxed);
        if (curDepth <= entry.depth) {
            if (curDepth == -1) {
                usedSlots.fetch_add(1, std::memory_order_relaxed);
            }
            slot.key.store(key, std::memory_order_relaxed);
            slot.depth.store(entry.depth, std::memory_order_relaxed);
            slot.value.store(entry.value, std::memory_order_relaxed);
            slot.flag.store(entry.flag, std::memory_order_relaxed);
        }
    }

    bool probe(uint64_t key, TTEntry& entry) const {
        const auto& slot = table[key % table.size()];
        if (slot.depth.load(std::memory_order_relaxed) == -1 ||
            slot.key.load(std::memory_order_relaxed) != key) return false;
        entry.depth = slot.depth.load(std::memory_order_relaxed);
        entry.value = slot.value.load(std::memory_order_relaxed);
        entry.flag = slot.flag.load(std::memory_order_relaxed);
        return true;
    }

    void clear() {
        for (auto& s : table) {
            s.depth.store(-1, std::memory_order_relaxed);
        }
        usedSlots.store(0, std::memory_order_relaxed);
    }
private:
    static constexpr size_t DEFAULT_SIZE = 1 << 20;
    std::vector<TTSlot> table;
    std::atomic<size_t> usedSlots;
};
