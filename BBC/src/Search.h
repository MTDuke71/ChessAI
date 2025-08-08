#ifndef SEARCH_H
#define SEARCH_H

#include "BitBoard.h"
#include "MoveGeneration.h"
#include "Evaluation.h"
#include "TimeControl.h"
#include "Hash.h"

// Search constants
#define infinity 50000
#define mate_value 49000
#define mate_score 48000
#define max_ply 64

// Hash table constants
#define hash_size 800000
#define no_hash_entry 100000
#define hash_flag_exact 0
#define hash_flag_alpha 1
#define hash_flag_beta 2

// LMR constants
extern const int full_depth_moves;
extern const int reduction_limit;

// MVV LVA table
extern int mvv_lva[12][12];

// Killer moves and history moves
extern int killer_moves[2][max_ply];
extern int history_moves[12][64];

// PV table structures
extern int pv_length[max_ply];
extern int pv_table[max_ply][max_ply];
extern int follow_pv, score_pv;

// Transposition table structure
typedef struct {
    U64 hash_key;
    int depth;
    int flag;
    int score;
} tt;

extern tt hash_table[hash_size];

// Function declarations
void clear_hash_table();
int read_hash_entry(int alpha, int beta, int depth);
void write_hash_entry(int score, int depth, int hash_flag);
void enable_pv_scoring(moves *move_list);
int score_move(int move);
int sort_moves(moves *move_list);
void print_move_scores(moves *move_list);
int is_repetition();
int quiescence(int alpha, int beta);
int negamax(int alpha, int beta, int depth);
void search_position(int depth);

// UCI functions
int parse_move(char *move_string);
void parse_position(char *command);
void parse_go(char *command);
void uci_loop();

// Initialization
void init_all();

#endif
