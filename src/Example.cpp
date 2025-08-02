/* #include <iostream>
#include "Board.h"
#include "Engine.h"

int main() {
    Board board;
    Engine engine;
    std::string best = engine.searchBestMove(board, 2);
    std::cout << "Best move from starting position: " << best << "\n";
    return 0;
} */

#include "OpeningBook.h"
#include <iostream>

int main(int argc, char** argv) {
    std::string file = "books/komodo.bin";
    if (argc > 1) file = argv[1];
    OpeningBook book(file);
    book.print(std::cout);
    return 0;
}
