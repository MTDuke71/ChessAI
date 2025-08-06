#pragma once
#include <cstdint>
#include <string>

uint16_t encodeMove(const std::string& move);
std::string decodeMove(uint16_t move);
