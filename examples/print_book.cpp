#include "OpeningBook.h"
#include <iostream>

int main(int argc, char** argv) {
    std::string file = "books/komodo.bin";
    if (argc > 1) file = argv[1];
    OpeningBook book(file);
    book.print(std::cout);
    return 0;
}
