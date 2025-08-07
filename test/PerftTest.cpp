#include "Perft.h"
#include "Board.h"
#include "MoveGenerator.h"
#include <cassert>
#include <iostream>

void testInitialPerft() {
  Board board;
  MoveGenerator gen;
  bool loaded =
      board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  assert(loaded);

  uint64_t n1 = perft(board, gen, 1);
  std::cout << "Perft(1) = " << n1 << std::endl;
  assert(n1 == 20);

  uint64_t n2 = perft(board, gen, 2);
  std::cout << "Perft(2) = " << n2 << std::endl;
  assert(n2 == 400);

  uint64_t n3 = perft(board, gen, 3);
  std::cout << "Perft(3) = " << n3 << std::endl;
  assert(n3 == 8902);
}

void testEnPassantPerft() {
  Board board;
  MoveGenerator gen;
  bool loaded = board.loadFEN(
      "rnbqkbnr/2pppppp/p7/Pp6/8/8/1PPPPPPP/RNBQKBNR w KQkq b6 0 3");
  assert(loaded);

  uint64_t n1 = perft(board, gen, 1);
  std::cout << "En passant Perft(1) = " << n1 << std::endl;
  assert(n1 == 22);
}

int main() {
  testInitialPerft();
  testEnPassantPerft();
  std::cout << "\nPerft tests passed!" << std::endl;
  return 0;
}
