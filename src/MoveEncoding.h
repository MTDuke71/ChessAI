#pragma once
#include <cstdint>
#include <string>

uint16_t encodeMove(const std::string& move);
uint16_t encodeMove(const std::string& move, bool isWhiteToMove);
std::string decodeMove(uint16_t move);

// Helpers to work with encoded moves without converting to strings
inline int moveFrom(uint16_t move) { return (move >> 6) & 0x3F; }
inline int moveTo(uint16_t move) { return move & 0x3F; }
inline int movePromotion(uint16_t move) { return (move >> 12) & 0x3; }
inline int moveSpecial(uint16_t move) { return (move >> 14) & 0x3; }
