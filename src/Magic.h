#pragma once
#include <cstdint>
namespace Magic {
    using U64 = uint64_t;
    void init();
    U64 getRookAttacks(int sq, U64 occ);
    U64 getBishopAttacks(int sq, U64 occ);
    U64 getKnightAttacks(int sq);
    U64 getKingAttacks(int sq);
    U64 getQueenAttacks(int sq, U64 occ);
}
