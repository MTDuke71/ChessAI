#pragma once
#include <cstdint>
#include <string>

class Board {
private:
    uint64_t whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
    uint64_t blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;
    int enPassantSquare;
    bool whiteToMove;
    bool castleWK, castleWQ, castleBK, castleBQ;


public:
    Board();

    enum class Color { None, White, Black };
    Color pieceColorAt(int index) const;

    bool isWhiteToMove() const { return whiteToMove; }
    bool canCastleWK() const { return castleWK; }
    bool canCastleWQ() const { return castleWQ; }
    bool canCastleBK() const { return castleBK; }
    bool canCastleBQ() const { return castleBQ; }
    void setWhiteToMove(bool v) { whiteToMove = v; }
    void setCastleWK(bool v) { castleWK = v; }
    void setCastleWQ(bool v) { castleWQ = v; }
    void setCastleBK(bool v) { castleBK = v; }
    void setCastleBQ(bool v) { castleBQ = v; }

    // Getters for testing and internal logic
    uint64_t getWhitePawns() const { return whitePawns; }
    uint64_t getBlackPawns() const { return blackPawns; }
    uint64_t getWhitePieces() const { return whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing; }
    uint64_t getBlackPieces() const { return blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing; }
    int getEnPassantSquare() const { return enPassantSquare; }

    uint64_t getWhiteKnights() const { return whiteKnights; }
    uint64_t getBlackKnights() const { return blackKnights; }
    uint64_t getWhiteRooks()   const { return whiteRooks;   }
    uint64_t getBlackRooks()   const { return blackRooks;   }
    uint64_t getWhiteBishops() const { return whiteBishops; }
    uint64_t getBlackBishops() const { return blackBishops; }
    uint64_t getWhiteQueens()  const { return whiteQueens;  }
    uint64_t getBlackQueens()  const { return blackQueens;  }
    uint64_t getWhiteKing() const { return whiteKing; }
    uint64_t getBlackKing() const { return blackKing; }

    // Setters for testing
    void setWhitePawns(uint64_t value) { whitePawns = value; }
    void setBlackPawns(uint64_t value) { blackPawns = value; }
    void setWhiteKing(uint64_t value) { whiteKing = value; }
    void setBlackKing(uint64_t value) { blackKing = value; }
    void setWhiteRooks(uint64_t value) { whiteRooks = value; }
    void setBlackRooks(uint64_t value) { blackRooks = value; }
    void setWhiteBishops(uint64_t value) { whiteBishops = value; }
    void setBlackBishops(uint64_t value) { blackBishops = value; }
    void setWhiteQueens(uint64_t value) { whiteQueens = value; }
    void setBlackQueens(uint64_t value) { blackQueens = value; }
    void setWhiteKnights(uint64_t value) { whiteKnights = value; }
    void setBlackKnights(uint64_t value) { blackKnights = value; }
    void setEnPassantSquare(int square) { enPassantSquare = square; }

    void clearBoard();  // Utility function to reset the board state
    void printBoard() const;
    bool loadFEN(const std::string& fen);
    bool isMoveLegal(const std::string& move) const;
    void makeMove(const std::string& move);
private:
    void applyMove(const std::string& move);
};

int algebraicToIndex(const std::string& sq);
