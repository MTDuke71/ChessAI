#ifndef TIMECONTROL_H
#define TIMECONTROL_H

#include "BitBoard.h"

// Time controls variables
extern int quit; // exit from engine flag
extern int movestogo; // UCI "movestogo" command moves counter
extern int movetime; // UCI "movetime" command time counter
extern int time_ms; // UCI "time" command holder (ms)
extern int inc; // UCI "inc" command's time increment holder
extern int starttime; // UCI "starttime" command time holder
extern int stoptime; // UCI "stoptime" command time holder
extern int timeset; // variable to flag time control availability
extern int stopped; // variable to flag when the time is up

// Function prototypes
int get_time_ms(void);
int input_waiting(void);
void read_input(void);
void communicate(void);

#endif
