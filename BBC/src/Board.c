#include "Board.h"

// piece bitboards
U64 bitboards[12];

// occupancy bitboards
U64 occupancies[3];

// side to move
int side;

// enpassant square
int enpassant = no_sq; 

// castling rights
int castle;

// "almost" unique position identifier aka hash key or position key
U64 hash_key;

// positions repetition table
U64 repetition_table[1000];  // 1000 is a number of plies (500 moves) in the entire game

// repetition index
int repetition_index;

// half move counter
int ply;

// print board
void print_board()
{
    // print offset
    printf("\n");

    // loop over board ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop ober board files
        for (int file = 0; file < 8; file++)
        {
            // init square
            int square = rank * 8 + file;
            
            // print ranks
            if (!file)
                printf("  %d ", 8 - rank);
            
            // define piece variable
            int piece = -1;
            
            // loop over all piece bitboards
            for (int bb_piece = P; bb_piece <= k; bb_piece++)
            {
                // if there is a piece on current square
                if (get_bit(bitboards[bb_piece], square))
                    // get piece code
                    piece = bb_piece;
            }
            
            // print different piece set depending on OS
            #ifdef WIN64
                printf(" %c", (piece == -1) ? '.' : ascii_pieces[piece]);
            #else
                printf(" %s", (piece == -1) ? "." : unicode_pieces[piece]);
            #endif
        }
        
        // print new line every rank
        printf("\n");
    }
    
    // print board files
    printf("\n     a b c d e f g h\n\n");
    
    // print side to move
    printf("     Side:     %s\n", !side ? "white" : "black");
    
    // print enpassant square
    printf("     Enpassant:   %s\n", (enpassant != no_sq) ? square_to_coordinates[enpassant] : "no");
    
    // print castling rights
    printf("     Castling:  %c%c%c%c\n\n", (castle & wk) ? 'K' : '-',
                                           (castle & wq) ? 'Q' : '-',
                                           (castle & bk) ? 'k' : '-',
                                           (castle & bq) ? 'q' : '-');
    
    // print hash key
    printf("     Hash key:  %llx\n\n", hash_key);
}

// parse FEN string
void parse_fen(char *fen)
{
    // reset board position (bitboards)
    memset(bitboards, 0ULL, sizeof(bitboards));
    
    // reset occupancies (bitboards)
    memset(occupancies, 0ULL, sizeof(occupancies));
    
    // reset game state variables
    side = 0;
    enpassant = no_sq;
    castle = 0;
    
    // reset repetition index
    repetition_index = 0;
    
    // reset repetition table
    memset(repetition_table, 0ULL, sizeof(repetition_table));
    
    // loop over board ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over board files
        for (int file = 0; file < 8; file++)
        {
            // init current square
            int square = rank * 8 + file;
            
            // match ascii pieces within FEN string
            if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z'))
            {
                // init piece type
                int piece = char_pieces[*fen];
                
                // set piece on corresponding bitboard
                set_bit(bitboards[piece], square);
                
                // increment pointer to FEN string
                fen++;
            }
            
            // match empty square numbers within FEN string
            if (*fen >= '0' && *fen <= '9')
            {
                // init offset (convert char 0 to int 0)
                int offset = *fen - '0';
                
                // define piece variable
                int piece = -1;
                
                // loop over all piece bitboards
                for (int bb_piece = P; bb_piece <= k; bb_piece++)
                {
                    // if there is a piece on current square
                    if (get_bit(bitboards[bb_piece], square))
                        // get piece code
                        piece = bb_piece;
                }
                
                // on empty current square
                if (piece == -1)
                    // decrement file
                    file--;
                
                // adjust file counter
                file += offset;
                
                // increment pointer to FEN string
                fen++;
            }
            
            // match rank separator
            if (*fen == '/')
                // increment pointer to FEN string
                fen++;
        }
    }
    
    // got to parsing side to move (increment pointer to FEN string)
    fen++;
    
    // parse side to move
    (*fen == 'w') ? (side = white) : (side = black);
    
    // go to parsing castling rights
    fen += 2;
    
    // parse castling rights
    while (*fen != ' ')
    {
        switch (*fen)
        {
            case 'K': castle |= wk; break;
            case 'Q': castle |= wq; break;
            case 'k': castle |= bk; break;
            case 'q': castle |= bq; break;
            case '-': break;
        }

        // increment pointer to FEN string
        fen++;
    }
    
    // got to parsing enpassant square (increment pointer to FEN string)
    fen++;
    
    // parse enpassant square
    if (*fen != '-')
    {
        // parse enpassant file & rank
        int file = fen[0] - 'a';
        int rank = 8 - (fen[1] - '0');
        
        // init enpassant square
        enpassant = rank * 8 + file;
    }
    
    // no enpassant square
    else
        enpassant = no_sq;
    
    // loop over white pieces bitboards
    for (int piece = P; piece <= K; piece++)
        // populate white occupancy bitboard
        occupancies[white] |= bitboards[piece];
    
    // loop over black pieces bitboards
    for (int piece = p; piece <= k; piece++)
        // populate white occupancy bitboard
        occupancies[black] |= bitboards[piece];
    
    // init all occupancies
    occupancies[both] |= occupancies[white];
    occupancies[both] |= occupancies[black];
    
    // init hash key
    hash_key = generate_hash_key();
}
