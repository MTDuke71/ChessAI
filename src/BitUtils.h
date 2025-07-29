#pragma once
#include <cstdint>
#if defined(_MSC_VER)
#include <intrin.h>
#endif

inline int popLSBIndex(uint64_t &bb) {
#if defined(_MSC_VER)
    unsigned long index;
    _BitScanForward64(&index, bb);
    bb &= bb - 1;
    return static_cast<int>(index);
#else
    int index = __builtin_ctzll(bb);
    bb &= bb - 1;
    return index;
#endif
}

inline int lsbIndex(uint64_t bb) {
#if defined(_MSC_VER)
    unsigned long index;
    _BitScanForward64(&index, bb);
    return static_cast<int>(index);
#else
    return __builtin_ctzll(bb);
#endif
}

inline int popcount64(uint64_t bb) {
#if defined(_MSC_VER)
    return static_cast<int>(__popcnt64(bb));
#else
    return __builtin_popcountll(bb);
#endif
}
