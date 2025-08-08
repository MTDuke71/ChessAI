#ifndef RANDOM_H
#define RANDOM_H

#include "BitBoard.h"

// Random number generation
extern unsigned int random_state;

// Function prototypes
unsigned int get_random_U32_number(void);
U64 get_random_U64_number(void);
U64 generate_magic_number(void);

#endif
