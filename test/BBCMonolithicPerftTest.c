#include <stdio.h>
#include <time.h>

// Forward declarations from BBC
typedef unsigned long long U64;
extern U64 nodes;
extern char* start_position;

// Function declarations
void init_all();
void parse_fen(char* fen);
void print_board();
void perft_driver(int depth);

int main() {
    printf("BBC Monolithic Engine Perft Test (Depth 7)\n");
    printf("==========================================\n\n");
    
    printf("This test requires the BBC.exe to be run separately.\n");
    printf("The BBC monolithic engine includes its own main function,\n");
    printf("so we cannot directly link with it in a test program.\n\n");
    
    printf("To test BBC monolithic performance at depth 7:\n");
    printf("1. Run: .\\BBC.exe\n");
    printf("2. Enter: perft 7\n");
    printf("3. Compare with BBC_modular performance\n\n");
    
    printf("Expected results for starting position:\n");
    printf("Depth 1: 20 nodes\n");
    printf("Depth 2: 400 nodes\n");
    printf("Depth 3: 8,902 nodes\n");
    printf("Depth 4: 197,281 nodes\n");
    printf("Depth 5: 4,865,609 nodes\n");
    printf("Depth 6: 119,060,324 nodes\n");
    printf("Depth 7: 3,195,901,860 nodes\n\n");
    
    return 0;
}
