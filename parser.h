#include <stdio.h>

#include "utils.h"
#include "bfun.h"

// if using more than 6 digits change str_buf in parser.c
// you shouldn't be doing that anyway - use a real VLSI tool fercrissake
#define MAX_FUNCTIONS 100 

void parse();
void do_quit();
void do_and(int dest, int m, int n);
void do_or(int dest, int m, int n);
void do_not(int dest, int n);
void do_write(int n);
void do_read(int n);
