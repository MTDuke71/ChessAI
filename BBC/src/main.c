#include "BitBoard.h"
#include "TimeControl.h"
#include "Random.h"
#include "Hash.h"
#include "Board.h"
#include "Attacks.h"
#include "MagicBitboards.h"
#include "MoveGeneration.h"
#include "Perft.h"
#include "Evaluation.h"
#include "Search.h"

// Function prototypes
// (All function implementations are now in their respective modules)

int main()
{
    // init all
    init_all();
    
    // connect to GUI
    uci_loop();

    return 0;
}
