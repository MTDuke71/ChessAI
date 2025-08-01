#include "Engine.h"
#include <cassert>
#include <iostream>

int main() {
    Engine engine;
    size_t defaultSize = engine.getHashSize();
    engine.setHashSizeMB(2); // 2 MB
    size_t newSize = engine.getHashSize();
    assert(newSize != defaultSize);
    std::cout << "Hash table resized from " << defaultSize
              << " to " << newSize << " entries\n";
    return 0;
}
