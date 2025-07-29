#pragma once
#include <cstdint>
namespace Magic {
    using U64 = uint64_t;
    void init();
    U64 getRookAttacks(int sq, U64 occ);
    U64 getBishopAttacks(int sq, U64 occ);
}
