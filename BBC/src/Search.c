#include "Search.h"
#include "Perft.h"

// most valuable victim & less valuable attacker

/*
                          
    (Victims) Pawn Knight Bishop   Rook  Queen   King
  (Attackers)
        Pawn   105    205    305    405    505    605
      Knight   104    204    304    404    504    604
      Bishop   103    203    303    403    503    603
        Rook   102    202    302    402    502    602
       Queen   101    201    301    401    501    601
        King   100    200    300    400    500    600

*/

// MVV LVA [attacker][victim]
int mvv_lva[12][12] = {
 	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,

	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600
};

// killer moves [id][ply]
int killer_moves[2][max_ply];

// history moves [piece][square]
int history_moves[12][64];

// PV length [ply]
int pv_length[max_ply];

/*
      ================================
            Triangular PV table
      --------------------------------
        PV line: e2e4 e7e5 g1f3 b8c6
      ================================

           0    1    2    3    4    5
      
      0    m1   m2   m3   m4   m5   m6
      
      1    0    m2   m3   m4   m5   m6 
      
      2    0    0    m3   m4   m5   m6
      
      3    0    0    0    m4   m5   m6
       
      4    0    0    0    0    m5   m6
      
      5    0    0    0    0    0    m6
*/

// PV table [ply][ply]
int pv_table[max_ply][max_ply];

// follow PV & score PV move
int follow_pv, score_pv;

// define TT instance
tt hash_table[hash_size];

// full depth moves counter
const int full_depth_moves = 4;

// depth limit to consider reduction
const int reduction_limit = 3;

// clear TT (hash table)
void clear_hash_table()
{
    // loop over TT elements
    for (int index = 0; index < hash_size; index++)
    {
        // reset TT inner fields
        hash_table[index].hash_key = 0;
        hash_table[index].depth = 0;
        hash_table[index].flag = 0;
        hash_table[index].score = 0;
    }
}

// read hash entry data
int read_hash_entry(int alpha, int beta, int depth)
{
    // create a TT instance pointer to particular hash entry storing
    // the scoring data for the current board position if available
    tt *hash_entry = &hash_table[hash_key % hash_size];
    
    // make sure we're dealing with the exact position we need
    if (hash_entry->hash_key == hash_key)
    {
        // make sure that we match the exact depth our search is now at
        if (hash_entry->depth >= depth)
        {
            // extract stored score from TT entry
            int score = hash_entry->score;
            
            // retrieve score independent from the actual path
            // from root node (position) to current node (position)
            if (score < -mate_score) score += ply;
            if (score > mate_score) score -= ply;
        
            // match the exact (PV node) score 
            if (hash_entry->flag == hash_flag_exact)
                // return exact (PV node) score
                return score;
            
            // match alpha (fail-low node) score
            if ((hash_entry->flag == hash_flag_alpha) &&
                (score <= alpha))
                // return alpha (fail-low node) score
                return alpha;
            
            // match beta (fail-high node) score
            if ((hash_entry->flag == hash_flag_beta) &&
                (score >= beta))
                // return beta (fail-high node) score
                return beta;
        }
    }
    
    // if hash entry doesn't exist
    return no_hash_entry;
}

// write hash entry data
void write_hash_entry(int score, int depth, int hash_flag)
{
    // create a TT instance pointer to particular hash entry storing
    // the scoring data for the current board position if available
    tt *hash_entry = &hash_table[hash_key % hash_size];

    // store score independent from the actual path
    // from root node (position) to current node (position)
    if (score < -mate_score) score -= ply;
    if (score > mate_score) score += ply;

    // write hash entry data 
    hash_entry->hash_key = hash_key;
    hash_entry->score = score;
    hash_entry->flag = hash_flag;
    hash_entry->depth = depth;
}

// enable PV move scoring
void enable_pv_scoring(moves *move_list)
{
    // disable following PV
    follow_pv = 0;
    
    // loop over the moves within a move list
    for (int count = 0; count < move_list->count; count++)
    {
        // make sure we hit PV move
        if (pv_table[0][ply] == move_list->moves[count])
        {
            // enable move scoring
            score_pv = 1;
            
            // enable following PV
            follow_pv = 1;
        }
    }
}

/*  =======================
         Move ordering
    =======================
    
    1. PV move
    2. Captures in MVV/LVA
    3. 1st killer move
    4. 2nd killer move
    5. History moves
    6. Unsorted moves
*/

// score moves
int score_move(int move)
{
    // if PV move scoring is allowed
    if (score_pv)
    {
        // make sure we are dealing with PV move
        if (pv_table[0][ply] == move)
        {
            // disable score PV flag
            score_pv = 0;
            
            // give PV move the highest score to search it first
            return 20000;
        }
    }
    
    // score capture move
    if (get_move_capture(move))
    {
        // init target piece
        int target_piece = P;
        
        // pick up bitboard piece index ranges depending on side
        int start_piece, end_piece;
        
        // pick up side to move
        if (side == white) { start_piece = p; end_piece = k; }
        else { start_piece = P; end_piece = K; }
        
        // loop over bitboards opposite to the current side to move
        for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++)
        {
            // if there's a piece on the target square
            if (get_bit(bitboards[bb_piece], get_move_target(move)))
            {
                // remove it from corresponding bitboard
                target_piece = bb_piece;
                break;
            }
        }
                
        // score move by MVV LVA lookup [source piece][target piece]
        return mvv_lva[get_move_piece(move)][target_piece] + 10000;
    }
    
    // score quiet move
    else
    {
        // score 1st killer move
        if (killer_moves[0][ply] == move)
            return 9000;
        
        // score 2nd killer move
        else if (killer_moves[1][ply] == move)
            return 8000;
        
        // score history move
        else
            return history_moves[get_move_piece(move)][get_move_target(move)];
    }
    
    return 0;
}

// sort moves in descending order
int sort_moves(moves *move_list)
{
    // move scores
    int move_scores[move_list->count];
    
    // score all the moves within a move list
    for (int count = 0; count < move_list->count; count++)
        // score move
        move_scores[count] = score_move(move_list->moves[count]);
    
    // loop over current move within a move list
    for (int current_move = 0; current_move < move_list->count; current_move++)
    {
        // loop over next move within a move list
        for (int next_move = current_move + 1; next_move < move_list->count; next_move++)
        {
            // compare current and next move scores
            if (move_scores[current_move] < move_scores[next_move])
            {
                // swap scores
                int temp_score = move_scores[current_move];
                move_scores[current_move] = move_scores[next_move];
                move_scores[next_move] = temp_score;
                
                // swap moves
                int temp_move = move_list->moves[current_move];
                move_list->moves[current_move] = move_list->moves[next_move];
                move_list->moves[next_move] = temp_move;
            }
        }
    }
    
    return 0;
}

// print move scores
void print_move_scores(moves *move_list)
{
    printf("     Move scores:\n\n");
        
    // loop over moves within a move list
    for (int count = 0; count < move_list->count; count++)
    {
        printf("     move: ");
        print_move(move_list->moves[count]);
        printf(" score: %d\n", score_move(move_list->moves[count]));
    }
}

// position repetition detection
int is_repetition()
{
    // loop over repetition indicies range
    for (int index = 0; index < repetition_index; index++)
        // if we found the hash key same with a current
        if (repetition_table[index] == hash_key)
            // we found a repetition
            return 1;
    
    // if no repetition found
    return 0;
}

// quiescence search
int quiescence(int alpha, int beta)
{
    // every 2047 nodes
    if((nodes & 2047 ) == 0)
        // "listen" to the GUI/user input
		communicate();
	
    // increment nodes count
    nodes++;

    // we are too deep, hence there's an overflow of arrays relying on max ply constant
    if (ply > max_ply - 1)
        // evaluate position
        return evaluate();

    // evaluate position
    int evaluation = evaluate();
    
    // fail-hard beta cutoff
    if (evaluation >= beta)
    {
        // node (position) fails high
        return beta;
    }
    
    // found a better move
    if (evaluation > alpha)
    {
        // PV node (position)
        alpha = evaluation;
    }
    
    // create move list instance
    moves move_list[1];
    
    // generate moves
    generate_moves(move_list);
    
    // sort moves
    sort_moves(move_list);
    
    // loop over moves within a movelist
    for (int count = 0; count < move_list->count; count++)
    {
        // preserve board state
        copy_board();
        
        // increment ply
        ply++;
        
        // increment repetition index & store hash key
        repetition_index++;
        repetition_table[repetition_index] = hash_key;

        
        // make sure to make only legal moves
        if (make_move(move_list->moves[count], only_captures) == 0)
        {
            // decrement ply
            ply--;
            
            // decrement repetition index
            repetition_index--;
            
            // skip to next move
            continue;
        }

        // score current move
        int score = -quiescence(-beta, -alpha);
        
        // decrement ply
        ply--;
        
        // decrement repetition index
        repetition_index--;

        // take move back
        take_back();
        
        // reutrn 0 if time is up
        if(stopped == 1) return 0;
        
        // found a better move
        if (score > alpha)
        {
            // PV node (position)
            alpha = score;
            
            // fail-hard beta cutoff
            if (score >= beta)
            {
                // node (position) fails high
                return beta;
            }
        }
    }
    
    // node (position) fails low
    return alpha;
}

// negamax alpha beta search
int negamax(int alpha, int beta, int depth)
{
    // variable to store current move's score (from the static evaluation perspective)
    int score;
    
    // define hash flag
    int hash_flag = hash_flag_alpha;
    
    // if position repetition occurs
    if (ply && is_repetition())
        // return draw score
        return 0;
    
    // a hack by Pedro Castro to figure out whether the current node is PV node or not 
    int pv_node = beta - alpha > 1;
    
    // read hash entry if we're not in a root ply and hash entry is available
    // and current node is not a PV node
    if (ply && (score = read_hash_entry(alpha, beta, depth)) != no_hash_entry && pv_node == 0)
        // if the move has already been searched (hence has a value)
        // we just return the score for this move without searching it
        return score;
        
    // every 2047 nodes
    if((nodes & 2047 ) == 0)
        // "listen" to the GUI/user input
		communicate();

    // init PV length
    pv_length[ply] = ply;

    // recursion escapre condition
    if (depth == 0)
        // run quiescence search
        return quiescence(alpha, beta);
    
    // we are too deep, hence there's an overflow of arrays relying on max ply constant
    if (ply > max_ply - 1)
        // evaluate position
        return evaluate();
    
    // increment nodes count
    nodes++;
    
    // is king in check
    int in_check = is_square_attacked((side == white) ? get_ls1b_index(bitboards[K]) : 
                                                        get_ls1b_index(bitboards[k]),
                                                        side ^ 1);
    
    // increase search depth if the king has been exposed into a check
    if (in_check) depth++;
    
    // legal moves counter
    int legal_moves = 0;
    
    // null move pruning
    if (depth >= 3 && in_check == 0 && ply)
    {
        // preserve board state
        copy_board();
        
        // increment ply
        ply++;
        
        // increment repetition index & store hash key
        repetition_index++;
        repetition_table[repetition_index] = hash_key;

        
        // hash enpassant if available
        if (enpassant != no_sq) hash_key ^= enpassant_keys[enpassant];
        
        // reset enpassant capture square
        enpassant = no_sq;
        
        // switch the side, literally giving opponent an extra move to make
        side ^= 1;
        
        // hash the side
        hash_key ^= side_key;
                
        /* search moves with reduced depth to find beta cutoffs
           depth - 1 - R where R is a reduction limit */
        score = -negamax(-beta, -beta + 1, depth - 1 - 2);

        // decrement ply
        ply--;
        
        // decrement repetition index
        repetition_index--;
            
        // restore board state
        take_back();

        // reutrn 0 if time is up
        if(stopped == 1) return 0;

        // fail-hard beta cutoff
        if (score >= beta)
            // node (position) fails high
            return beta;
    }
    
    // create move list instance
    moves move_list[1];
    
    // generate moves
    generate_moves(move_list);
    
    // if we are now following PV line
    if (follow_pv)
        // enable PV move scoring
        enable_pv_scoring(move_list);
    
    // sort moves
    sort_moves(move_list);
    
    // number of moves searched in a move list
    int moves_searched = 0;
    
    // loop over moves within a movelist
    for (int count = 0; count < move_list->count; count++)
    {
        // preserve board state
        copy_board();
        
        // increment ply
        ply++;
        
        // increment repetition index & store hash key
        repetition_index++;
        repetition_table[repetition_index] = hash_key;
        
        // make sure to make only legal moves
        if (make_move(move_list->moves[count], all_moves) == 0)
        {
            // decrement ply
            ply--;
            
            // decrement repetition index
            repetition_index--;
            
            // skip to next move
            continue;
        }
        
        // increment legal moves
        legal_moves++;
        
        // full depth search
        if (moves_searched == 0)
            // do normal alpha beta search
            score = -negamax(-beta, -alpha, depth - 1);
        
        // late move reduction (LMR)
        else
        {
            // condition to consider LMR
            if(
                moves_searched >= full_depth_moves &&
                depth >= reduction_limit &&
                in_check == 0 && 
                get_move_capture(move_list->moves[count]) == 0 &&
                get_move_promoted(move_list->moves[count]) == 0
              )
                // search current move with reduced depth:
                score = -negamax(-alpha - 1, -alpha, depth - 2);
            
            // hack to ensure that full-depth search is done
            else score = alpha + 1;
            
            // principle variation search PVS
            if(score > alpha)
            {
             /* Once you've found a move with a score that is between alpha and beta,
                the rest of the moves are searched with the goal of proving that they are all bad.
                It's possible to do this a bit faster than a search that worries that one
                of the remaining moves might be good. */
                score = -negamax(-alpha - 1, -alpha, depth-1);
            
             /* If the algorithm finds out that it was wrong, and that one of the
                subsequent moves was better than the first PV move, it has to search again,
                in the normal alpha-beta manner.  This happens sometimes, and it's a waste of time,
                but generally not often enough to counteract the savings gained from doing the
                "bad move proof" search referred to earlier. */
                if((score > alpha) && (score < beta))
                 /* re-search the move that has failed to be proved to be bad
                    with normal alpha beta score bounds*/
                    score = -negamax(-beta, -alpha, depth-1);
            }
        }
        
        // decrement ply
        ply--;
        
        // decrement repetition index
        repetition_index--;

        // take move back
        take_back();
        
        // reutrn 0 if time is up
        if(stopped == 1) return 0;
        
        // increment the counter of moves searched so far
        moves_searched++;
        
        // found a better move
        if (score > alpha)
        {
            // switch hash flag from storing score for fail-low node
            // to the one storing score for PV node
            hash_flag = hash_flag_exact;
        
            // on quiet moves
            if (get_move_capture(move_list->moves[count]) == 0)
                // store history moves
                history_moves[get_move_piece(move_list->moves[count])][get_move_target(move_list->moves[count])] += depth;
            
            // PV node (position)
            alpha = score;
            
            // write PV move
            pv_table[ply][ply] = move_list->moves[count];
            
            // loop over the next ply
            for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++)
                // copy move from deeper ply into a current ply's line
                pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
            
            // adjust PV length
            pv_length[ply] = pv_length[ply + 1];
            
            // fail-hard beta cutoff
            if (score >= beta)
            {
                // store hash entry with the score equal to beta
                write_hash_entry(beta, depth, hash_flag_beta);
            
                // on quiet moves
                if (get_move_capture(move_list->moves[count]) == 0)
                {
                    // store killer moves
                    killer_moves[1][ply] = killer_moves[0][ply];
                    killer_moves[0][ply] = move_list->moves[count];
                }
                
                // node (position) fails high
                return beta;
            }            
        }
    }
    
    // we don't have any legal moves to make in the current postion
    if (legal_moves == 0)
    {
        // king is in check
        if (in_check)
            // return mating score (assuming closest distance to mating position)
            return -mate_value + ply;
        
        // king is not in check
        else
            // return stalemate score
            return 0;
    }
    
    // store hash entry with the score equal to alpha
    write_hash_entry(alpha, depth, hash_flag);
    
    // node (position) fails low
    return alpha;
}

// search position for the best move
void search_position(int depth)
{
    // define best score variable
    int score = 0;
    
    // reset nodes counter
    nodes = 0;
    
    // reset "time is up" flag
    stopped = 0;
    
    // reset follow PV flags
    follow_pv = 0;
    score_pv = 0;
    
    // clear helper data structures for search
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_moves, 0, sizeof(history_moves));
    memset(pv_table, 0, sizeof(pv_table));
    memset(pv_length, 0, sizeof(pv_length));
    
    // define initial alpha beta bounds
    int alpha = -infinity;
    int beta = infinity;
 
    // iterative deepening
    for (int current_depth = 1; current_depth <= depth; current_depth++)
    {
        // if time is up
        if(stopped == 1)
			// stop calculating and return best move so far 
			break;
		
        // enable follow PV flag
        follow_pv = 1;
        
        // find best move within a given position
        score = negamax(alpha, beta, current_depth);
 
        // we fell outside the window, so try again with a full-width window (and the same depth)
        if ((score <= alpha) || (score >= beta)) {
            alpha = -infinity;    
            beta = infinity;      
            continue;
        }
        
        // set up the window for the next iteration
        alpha = score - 50;
        beta = score + 50;
        
        // print search info
        if (score > -mate_value && score < -mate_score)
            printf("info score mate %d depth %d nodes %lld time %d pv ", -(score + mate_value) / 2 - 1, current_depth, nodes, get_time_ms() - starttime);
        
        else if (score > mate_score && score < mate_value)
            printf("info score mate %d depth %d nodes %lld time %d pv ", (mate_value - score) / 2 + 1, current_depth, nodes, get_time_ms() - starttime);   
        
        else
            printf("info score cp %d depth %d nodes %lld time %d pv ", score, current_depth, nodes, get_time_ms() - starttime);
        
        // loop over the moves within a PV line
        for (int count = 0; count < pv_length[0]; count++)
        {
            // print PV move
            print_move(pv_table[0][count]);
            printf(" ");
        }
        
        // print new line
        printf("\n");
    }

    // print best move
    printf("bestmove ");
    print_move(pv_table[0][0]);
    printf("\n");
}

// parse user/GUI move string input (e.g. "e7e8q")
int parse_move(char *move_string)
{
    // create move list instance
    moves move_list[1];
    
    // generate moves
    generate_moves(move_list);
    
    // parse source square
    int source_square = (move_string[0] - 'a') + (8 - (move_string[1] - '0')) * 8;
    
    // parse target square
    int target_square = (move_string[2] - 'a') + (8 - (move_string[3] - '0')) * 8;
    
    // loop over the moves within a move list
    for (int move_count = 0; move_count < move_list->count; move_count++)
    {
        // init move
        int move = move_list->moves[move_count];
        
        // make sure source & target squares are available within the generated move
        if (source_square == get_move_source(move) && target_square == get_move_target(move))
        {
            // init promoted piece
            int promoted_piece = get_move_promoted(move);
            
            // promoted piece is available
            if (promoted_piece)
            {
                // promoted to queen
                if ((promoted_piece == Q || promoted_piece == q) && move_string[4] == 'q')
                    // return legal move
                    return move;
                
                // promoted to rook
                else if ((promoted_piece == R || promoted_piece == r) && move_string[4] == 'r')
                    // return legal move
                    return move;
                
                // promoted to bishop
                else if ((promoted_piece == B || promoted_piece == b) && move_string[4] == 'b')
                    // return legal move
                    return move;
                
                // promoted to knight
                else if ((promoted_piece == N || promoted_piece == n) && move_string[4] == 'n')
                    // return legal move
                    return move;
                
                // continue the loop on possible wrong promotions (e.g. "e7e8f")
                continue;
            }
            
            // return legal move
            return move;
        }
    }
    
    // return illegal move
    return 0;
}

// parse UCI "position" command
void parse_position(char *command)
{
    // shift pointer to the right where next token begins
    command += 9;
    
    // init pointer to the current character in the command string
    char *current_char = command;
    
    // parse UCI "startpos" command
    if (strncmp(command, "startpos", 8) == 0)
        // init chess board with start position
        parse_fen(start_position);
    
    // parse UCI "fen" command 
    else
    {
        // make sure "fen" command is available within command string
        current_char = strstr(command, "fen");
        
        // if no "fen" command is available within command string
        if (current_char == NULL)
            // init chess board with start position
            parse_fen(start_position);
            
        // found "fen" substring
        else
        {
            // shift pointer to the right where next token begins
            current_char += 4;
            
            // init chess board with position from FEN string
            parse_fen(current_char);
        }
    }
    
    // parse moves after position
    current_char = strstr(command, "moves");
    
    // moves available
    if (current_char != NULL)
    {
        // shift pointer to the right where next token begins
        current_char += 6;
        
        // loop over moves within a move string
        while(*current_char)
        {
            // parse next move
            int move = parse_move(current_char);
            
            // if no more moves
            if (move == 0)
                // break out of the loop
                break;
            
            // increment repetition index
            repetition_index++;
            
            // wtire hash key into a repetition table
            repetition_table[repetition_index] = hash_key;
            
            // make move on the chess board
            make_move(move, all_moves);
            
            // move current character mointer to the end of current move
            while (*current_char && *current_char != ' ') current_char++;
            
            // go to the next move
            current_char++;
        }        
    }
    
    // print board
    print_board();
}

// parse UCI command "go"
void parse_go(char *command)
{
    // init parameters
    int depth = -1;

    // init argument
    char *argument = NULL;

    // infinite search
    if ((argument = strstr(command,"infinite"))) {}

    // match UCI "binc" command
    if ((argument = strstr(command,"binc")) && side == black)
        // parse black time increment
        inc = atoi(argument + 5);

    // match UCI "winc" command
    if ((argument = strstr(command,"winc")) && side == white)
        // parse white time increment
        inc = atoi(argument + 5);

    // match UCI "wtime" command
    if ((argument = strstr(command,"wtime")) && side == white)
        // parse white time limit
        time_ms = atoi(argument + 6);

    // match UCI "btime" command
    if ((argument = strstr(command,"btime")) && side == black)
        // parse black time limit
        time_ms = atoi(argument + 6);

    // match UCI "movestogo" command
    if ((argument = strstr(command,"movestogo")))
        // parse number of moves to go
        movestogo = atoi(argument + 10);

    // match UCI "movetime" command
    if ((argument = strstr(command,"movetime")))
        // parse amount of time allowed to spend to make a move
        movetime = atoi(argument + 9);

    // match UCI "depth" command
    if ((argument = strstr(command,"depth")))
        // parse search depth
        depth = atoi(argument + 6);

    // if move time is not available
    if(movetime != -1)
    {
        // set time equal to move time
        time_ms = movetime;

        // set moves to go to 1
        movestogo = 1;
    }

    // init start time
    starttime = get_time_ms();

    // init search depth
    depth = depth;

    // if time control is available
    if(time_ms != -1)
    {
        // flag we're playing with time control
        timeset = 1;

        // set up timing
        time_ms /= movestogo;
        
        // "illegal" (empty) move bug fix
        if (time_ms > 1500) time_ms -= 50;
        
        // init stoptime
        stoptime = starttime + time_ms + inc;
    }

    // if depth is not available
    if(depth == -1)
        // set depth to 64 plies (takes ages to complete...)
        depth = 64;

    // print debug info
    printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
    time_ms, starttime, stoptime, depth, timeset);

    // search position
    search_position(depth);
}

// main UCI loop
void uci_loop()
{
    // reset STDIN & STDOUT buffers
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    
    // define user / GUI input buffer
    char input[2000];
    
    // print engine info
    printf("id name BBC\n");
    printf("id name Code Monkey King\n");
    printf("uciok\n");
    
    // main loop
    while (1)
    {
        // reset user /GUI input
        memset(input, 0, sizeof(input));
        
        // make sure output reaches the GUI
        fflush(stdout);
        
        // get user / GUI input
        if (!fgets(input, 2000, stdin))
            // continue the loop
            continue;
        
        // make sure input is available
        if (input[0] == '\n')
            // continue the loop
            continue;
        
        // parse UCI "isready" command
        if (strncmp(input, "isready", 7) == 0)
        {
            printf("readyok\n");
            continue;
        }
        
        // parse UCI "position" command
        else if (strncmp(input, "position", 8) == 0)
        {
            // call parse position function
            parse_position(input);
        
            // clear hash table
            clear_hash_table();
        }
        // parse UCI "ucinewgame" command
        else if (strncmp(input, "ucinewgame", 10) == 0)
        {
            // call parse position function
            parse_position("position startpos");
            
            // clear hash table
            clear_hash_table();
        }
        // parse UCI "go" command
        else if (strncmp(input, "go", 2) == 0)
            // call parse go function
            parse_go(input);
        
        // parse UCI "quit" command
        else if (strncmp(input, "quit", 4) == 0)
            // quit from the chess engine program execution
            break;
        
        // parse UCI "uci" command
        else if (strncmp(input, "uci", 3) == 0)
        {
            // print engine info
            printf("id name BBC\n");
            printf("id name Code Monkey King\n");
            printf("uciok\n");
        }
    }
}

// init all variables
void init_all()
{
    // init leaper pieces attacks
    init_leapers_attacks();
    
    // init slider pieces attacks
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
    
    // init random keys for hashing purposes
    init_random_keys();
    
    // clear hash table
    clear_hash_table();
    
    // init evaluation masks
    init_evaluation_masks();
}
