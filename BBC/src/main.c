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
    
    // set up starting position
    parse_fen(start_position);
    
    // reset nodes counter
    nodes = 0;
    
    // run perft test at depth 6
    printf("BBC Modular Engine - Perft 6 Test\n");
    printf("==================================\n");
    perft_test(6);
    
    return 0;
}
