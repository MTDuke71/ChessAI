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

int main()
{
    // init all
    init_all();
    
    // parse starting position
    parse_fen(start_position);
    
    // print board
    print_board();
    
    // generate moves
    moves move_list[1];
    generate_moves(move_list);
    
    printf("Generated %d moves:\n", move_list->count);
    
    // print first few moves
    for (int i = 0; i < (move_list->count < 10 ? move_list->count : 10); i++)
    {
        print_move(move_list->moves[i]);
        printf(" ");
    }
    printf("\n");
    
    // test evaluation
    int eval = evaluate();
    printf("Position evaluation: %d\n", eval);
    
    // test a simple 1-ply search
    printf("Testing 1-ply search...\n");
    search_position(1);

    return 0;
}
