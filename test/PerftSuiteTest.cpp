#include "Perft.h"
#include "Board.h"
#include "MoveGenerator.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

void trim(std::string &s) {
  size_t start = s.find_first_not_of(" \t\r\n");
  size_t end = s.find_last_not_of(" \t\r\n");
  if (start == std::string::npos) {
    s.clear();
  } else {
    s = s.substr(start, end - start + 1);
  }
}

} // namespace

int main(int argc, char *argv[]) {
  int maxPositions = 126;
  if (argc > 1) {
    maxPositions = std::stoi(argv[1]);
  }

  namespace fs = std::filesystem;
  fs::path filePath = fs::path(__FILE__).parent_path() / "perftsuite.epd";
  std::ifstream file(filePath);
  if (!file) {
    std::cerr << "Failed to open " << filePath << std::endl;
    return 1;
  }

  Board board;
  MoveGenerator generator;
  std::string line;
  int index = 0;

  while (index < maxPositions && std::getline(file, line)) {
    trim(line);
    if (line.empty()) continue;
    ++index;

    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string part;
    while (std::getline(ss, part, ';')) {
      trim(part);
      if (!part.empty()) parts.push_back(part);
    }
    if (parts.empty()) continue;

    std::string fen = parts[0];
    if (!board.loadFEN(fen)) {
      std::cerr << "Failed to load FEN for position " << index << std::endl;
      return 1;
    }

    std::vector<uint64_t> expected(7, 0ULL);
    for (size_t i = 1; i < parts.size(); ++i) {
      std::stringstream ps(parts[i]);
      std::string depthToken;
      uint64_t nodes;
      if (!(ps >> depthToken >> nodes)) continue;
      if (depthToken.size() > 1 && depthToken[0] == 'D') {
        int depth = std::stoi(depthToken.substr(1));
        if (depth >= 1 && depth <= 6) {
          expected[depth] = nodes;
        }
      }
    }

    for (int depth = 1; depth <= 6; ++depth) {
      if (expected[depth] == 0) continue;
      uint64_t nodes = perft(board, generator, depth);
      if (nodes != expected[depth]) {
        std::cerr << "Mismatch at position " << index << ", depth " << depth
                  << ": expected " << expected[depth] << ", got " << nodes
                  << std::endl;
        return 1;
      }
    }
  }

  std::cout << "Tested " << index << " positions successfully." << std::endl;
  return 0;
}

