#pragma once
#include <array>

namespace MVVLVA {
    enum PieceType { Pawn, Knight, Bishop, Rook, Queen, King, PieceTypeN = 6 };

    constexpr std::array<std::array<int, PieceTypeN>, PieceTypeN> createTable() {
        std::array<std::array<int, PieceTypeN>, PieceTypeN> table{};
        for (int victim = 0; victim < PieceTypeN; ++victim) {
            for (int attacker = 0; attacker < PieceTypeN; ++attacker) {
                table[victim][attacker] = victim * PieceTypeN + (5 - attacker);
            }
        }
        return table;
    }

    inline constexpr auto Table = createTable();
}
